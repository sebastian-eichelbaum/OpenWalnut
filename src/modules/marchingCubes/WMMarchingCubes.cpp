//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV@Uni-Leipzig and CNCF@MPI-CBS
// For more information see http://www.openwalnut.org/copying
//
// This file is part of OpenWalnut.
//
// OpenWalnut is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// OpenWalnut is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with OpenWalnut. If not, see <http://www.gnu.org/licenses/>.
//
//---------------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <cmath>


#include "WMMarchingCubes.h"
#include "iso_surface.xpm"
#include "marchingCubesCaseTables.h"
#include "../../common/datastructures/WTriangleMesh.h"
#include "../../common/WLimits.h"

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/StateSet>
#include <osg/StateAttribute>
#include <osg/PolygonMode>
#include <osg/LightModel>
#include <osgDB/WriteFile>

#include "../../common/WProgress.h"
#include "../../common/WPreferences.h"
#include "../../math/WVector3D.h"
#include "../../dataHandler/WDataHandler.h"
#include "../../dataHandler/WSubject.h"
#include "../../dataHandler/WGridRegular3D.h"
#include "../../dataHandler/WDataTexture3D.h"
#include "../../kernel/WKernel.h"
#include "../../graphicsEngine/WShader.h"

#include "../data/WMData.h"


WMMarchingCubes::WMMarchingCubes():
    WModule(),
    m_recompute( boost::shared_ptr< WCondition >( new WCondition() ) ),
    m_nCellsX( 0 ),
    m_nCellsY( 0 ),
    m_nCellsZ( 0 ),
    m_fCellLengthX( 1 ),
    m_fCellLengthY( 1 ),
    m_fCellLengthZ( 1 ),
    m_tIsoLevel( 100 ),
    m_dataSet(),
    m_shaderUseLighting( false ),
    m_shaderUseTransparency( false ),
    m_moduleNode( new WGEGroupNode() ),
    m_surfaceGeode( 0 )
{
    // WARNING: initializing connectors inside the constructor will lead to an exception.
    // Implement WModule::initializeConnectors instead.
}

WMMarchingCubes::~WMMarchingCubes()
{
    // cleanup
    removeConnectors();
}

boost::shared_ptr< WModule > WMMarchingCubes::factory() const
{
    return boost::shared_ptr< WModule >( new WMMarchingCubes() );
}

const char** WMMarchingCubes::getXPMIcon() const
{
    return iso_surface_xpm;
}

const std::string WMMarchingCubes::getName() const
{
    return "Isosurface";
}

const std::string WMMarchingCubes::getDescription() const
{
    return "This module implement the marching cubes"
" algorithm with a consistent triangulation. It allows to compute isosurfaces"
" for a given isovalue on data given on a grid only consisting of cubes. It yields"
" the surface as triangle soup.";
}

void WMMarchingCubes::moduleMain()
{
    // use the m_input "data changed" flag
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_recompute );

    // signal ready state
    ready();

    // now, to watch changing/new textures use WSubject's change condition
    boost::signals2::connection con = WDataHandler::getDefaultSubject()->getChangeCondition()->subscribeSignal(
            boost::bind( &WMMarchingCubes::notifyTextureChange, this )
    );

    // loop until the module container requests the module to quit
    while ( !m_shutdownFlag() )
    {
        // acquire data from the input connector
        m_dataSet = m_input->getData();
        if ( !m_dataSet.get() )
        {
            // OK, the output has not yet sent data
            // NOTE: see comment at the end of this while loop for m_moduleState
            debugLog() << "Waiting for data ...";
            m_moduleState.wait();
            continue;
        }

        // update ISO surface
        debugLog() << "Computing surface ...";

        generateSurfacePre( m_isoValueProp->get() );

        // TODO(wiebel): MC remove this from here
        //    renderMesh( load( "/tmp/isosurfaceTestMesh.vtk" ) );

        debugLog() << "Rendering surface ...";

        // settings for normal isosurface
        m_shaderUseLighting = true;
        m_shaderUseTransparency = true;

        renderSurface();
        debugLog() << "Done!";

        // this waits for m_moduleState to fire. By default, this is only the m_shutdownFlag condition.
        // NOTE: you can add your own conditions to m_moduleState using m_moduleState.add( ... )
        m_moduleState.wait();
    }
}

void WMMarchingCubes::connectors()
{
    // initialize connectors
    m_input = boost::shared_ptr< WModuleInputData < WDataSetSingle  > >(
        new WModuleInputData< WDataSetSingle >( shared_from_this(),
                                                               "in", "Dataset to compute isosurface for." )
        );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_input );

    // call WModules initialization
    WModule::connectors();
}

void WMMarchingCubes::properties()
{
    m_isoValueProp = m_properties2->addProperty( "Iso Value", "The surface will show the area that has this value.", 100., m_recompute );
    m_isoValueProp->setMin( wlimits::MIN_DOUBLE );
    m_isoValueProp->setMax( wlimits::MAX_DOUBLE );
    {
        // If set in config file use standard isovalue from config file
        double tmpIsoValue;
        if( WPreferences::getPreference( "modules.MC.isoValue", &tmpIsoValue ) )
        {
            m_isoValueProp->set( tmpIsoValue );
        }
    }

    m_opacityProp = m_properties2->addProperty( "Opacity %", "Opaqueness of surface.", 100 );
    m_opacityProp->setMin( 0 );
    m_opacityProp->setMax( 100 );

    m_useTextureProp = m_properties2->addProperty( "Use Texture", "Use texturing of the surface?", false );
}

void WMMarchingCubes::generateSurfacePre( double isoValue )
{
    debugLog() << "Isovalue: " << isoValue;
    switch( (*m_dataSet).getValueSet()->getDataType() )
    {
        case W_DT_UNSIGNED_CHAR:
        {
            boost::shared_ptr< WValueSet< unsigned char > > vals;
            vals =  boost::shared_dynamic_cast< WValueSet< unsigned char > >( ( *m_dataSet ).getValueSet() );
            assert( vals );
            generateSurface( ( *m_dataSet ).getGrid(), vals, isoValue );
            break;
        }
        case W_DT_INT16:
        {
            boost::shared_ptr< WValueSet< int16_t > > vals;
            vals =  boost::shared_dynamic_cast< WValueSet< int16_t > >( ( *m_dataSet ).getValueSet() );
            assert( vals );
            generateSurface( ( *m_dataSet ).getGrid(), vals, isoValue );
            break;
        }
        case W_DT_SIGNED_INT:
        {
            boost::shared_ptr< WValueSet< int32_t > > vals;
            vals =  boost::shared_dynamic_cast< WValueSet< int32_t > >( ( *m_dataSet ).getValueSet() );
            assert( vals );
            generateSurface( ( *m_dataSet ).getGrid(), vals, isoValue );
            break;
        }
        case W_DT_FLOAT:
        {
            boost::shared_ptr< WValueSet< float > > vals;
            vals =  boost::shared_dynamic_cast< WValueSet< float > >( ( *m_dataSet ).getValueSet() );
            assert( vals );
            generateSurface( ( *m_dataSet ).getGrid(), vals, isoValue );
            break;
        }
        case W_DT_DOUBLE:
        {
            boost::shared_ptr< WValueSet< double > > vals;
            vals =  boost::shared_dynamic_cast< WValueSet< double > >( ( *m_dataSet ).getValueSet() );
            assert( vals );
            generateSurface( ( *m_dataSet ).getGrid(), vals, isoValue );
            break;
        }
        default:
            assert( false && "Unknow data type in MarchingCubes module" );
    }
}


template< typename T > void WMMarchingCubes::generateSurface( boost::shared_ptr< WGrid > inGrid,
                                                              boost::shared_ptr< WValueSet< T > > vals,
                                                              double isoValue )
{
    assert( vals );

    m_idToVertices.clear();
    m_trivecTriangles.clear();

    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( inGrid );
    m_grid = grid;
    assert( grid );

    m_fCellLengthX = grid->getOffsetX();
    m_fCellLengthY = grid->getOffsetY();
    m_fCellLengthZ = grid->getOffsetZ();

    m_nCellsX = grid->getNbCoordsX() - 1;
    m_nCellsY = grid->getNbCoordsY() - 1;
    m_nCellsZ = grid->getNbCoordsZ() - 1;

    m_tIsoLevel = isoValue;

    unsigned int nX = m_nCellsX + 1;
    unsigned int nY = m_nCellsY + 1;


    unsigned int nPointsInSlice = nX * nY;

    boost::shared_ptr< WProgress > progress = boost::shared_ptr< WProgress >( new WProgress( "Marching Cubes", m_nCellsZ ) );
    m_progress->addSubProgress( progress );
    // Generate isosurface.
    for( unsigned int z = 0; z < m_nCellsZ; z++ )
    {
        ++*progress;
        for( unsigned int y = 0; y < m_nCellsY; y++ )
        {
            for( unsigned int x = 0; x < m_nCellsX; x++ )
            {
                // Calculate table lookup index from those
                // vertices which are below the isolevel.
                unsigned int tableIndex = 0;
                if( vals->getScalar( z * nPointsInSlice + y * nX + x ) < m_tIsoLevel )
                    tableIndex |= 1;
                if( vals->getScalar( z * nPointsInSlice + ( y + 1 ) * nX + x ) < m_tIsoLevel )
                    tableIndex |= 2;
                if( vals->getScalar( z * nPointsInSlice + ( y + 1 ) * nX + ( x + 1 ) ) < m_tIsoLevel )
                    tableIndex |= 4;
                if( vals->getScalar( z * nPointsInSlice + y * nX + ( x + 1 ) ) < m_tIsoLevel )
                    tableIndex |= 8;
                if( vals->getScalar( ( z + 1 ) * nPointsInSlice + y * nX + x ) < m_tIsoLevel )
                    tableIndex |= 16;
                if( vals->getScalar( ( z + 1 ) * nPointsInSlice + ( y + 1 ) * nX + x ) < m_tIsoLevel )
                    tableIndex |= 32;
                if( vals->getScalar( ( z + 1 ) * nPointsInSlice + ( y + 1 ) * nX + ( x + 1 ) ) < m_tIsoLevel )
                    tableIndex |= 64;
                if( vals->getScalar( ( z + 1 ) * nPointsInSlice + y * nX + ( x + 1 ) ) < m_tIsoLevel )
                    tableIndex |= 128;

                // Now create a triangulation of the isosurface in this
                // cell.
                if( wMarchingCubesCaseTables::edgeTable[tableIndex] != 0 )
                {
                    if( wMarchingCubesCaseTables::edgeTable[tableIndex] & 8 )
                    {
                        WPointXYZId pt = calculateIntersection( vals, x, y, z, 3 );
                        unsigned int id = getEdgeID( x, y, z, 3 );
                        m_idToVertices.insert( ID2WPointXYZId::value_type( id, pt ) );
                    }
                    if( wMarchingCubesCaseTables::edgeTable[tableIndex] & 1 )
                    {
                        WPointXYZId pt = calculateIntersection( vals, x, y, z, 0 );
                        unsigned int id = getEdgeID( x, y, z, 0 );
                        m_idToVertices.insert( ID2WPointXYZId::value_type( id, pt ) );
                    }
                    if( wMarchingCubesCaseTables::edgeTable[tableIndex] & 256 )
                    {
                        WPointXYZId pt = calculateIntersection( vals, x, y, z, 8 );
                        unsigned int id = getEdgeID( x, y, z, 8 );
                        m_idToVertices.insert( ID2WPointXYZId::value_type( id, pt ) );
                    }

                    if( x == m_nCellsX - 1 )
                    {
                        if( wMarchingCubesCaseTables::edgeTable[tableIndex] & 4 )
                        {
                            WPointXYZId pt = calculateIntersection( vals, x, y, z, 2 );
                            unsigned int id = getEdgeID( x, y, z, 2 );
                            m_idToVertices.insert( ID2WPointXYZId::value_type( id, pt ) );
                        }
                        if( wMarchingCubesCaseTables::edgeTable[tableIndex] & 2048 )
                        {
                            WPointXYZId pt = calculateIntersection( vals, x, y, z, 11 );
                            unsigned int id = getEdgeID( x, y, z, 11 );
                            m_idToVertices.insert( ID2WPointXYZId::value_type( id, pt ) );
                        }
                    }
                    if( y == m_nCellsY - 1 )
                    {
                        if( wMarchingCubesCaseTables::edgeTable[tableIndex] & 2 )
                        {
                            WPointXYZId pt = calculateIntersection( vals, x, y, z, 1 );
                            unsigned int id = getEdgeID( x, y, z, 1 );
                            m_idToVertices.insert( ID2WPointXYZId::value_type( id, pt ) );
                        }
                        if( wMarchingCubesCaseTables::edgeTable[tableIndex] & 512 )
                        {
                            WPointXYZId pt = calculateIntersection( vals, x, y, z, 9 );
                            unsigned int id = getEdgeID( x, y, z, 9 );
                            m_idToVertices.insert( ID2WPointXYZId::value_type( id, pt ) );
                        }
                    }
                    if( z == m_nCellsZ - 1 )
                    {
                        if( wMarchingCubesCaseTables::edgeTable[tableIndex] & 16 )
                        {
                            WPointXYZId pt = calculateIntersection( vals, x, y, z, 4 );
                            unsigned int id = getEdgeID( x, y, z, 4 );
                            m_idToVertices.insert( ID2WPointXYZId::value_type( id, pt ) );
                        }
                        if( wMarchingCubesCaseTables::edgeTable[tableIndex] & 128 )
                        {
                            WPointXYZId pt = calculateIntersection( vals, x, y, z, 7 );
                            unsigned int id = getEdgeID( x, y, z, 7 );
                            m_idToVertices.insert( ID2WPointXYZId::value_type( id, pt ) );
                        }
                    }
                    if( ( x == m_nCellsX - 1 ) && ( y == m_nCellsY - 1 ) )
                        if( wMarchingCubesCaseTables::edgeTable[tableIndex] & 1024 )
                        {
                            WPointXYZId pt = calculateIntersection( vals, x, y, z, 10 );
                            unsigned int id = getEdgeID( x, y, z, 10 );
                            m_idToVertices.insert( ID2WPointXYZId::value_type( id, pt ) );
                        }
                    if( ( x == m_nCellsX - 1 ) && ( z == m_nCellsZ - 1 ) )
                        if( wMarchingCubesCaseTables::edgeTable[tableIndex] & 64 )
                        {
                            WPointXYZId pt = calculateIntersection( vals, x, y, z, 6 );
                            unsigned int id = getEdgeID( x, y, z, 6 );
                            m_idToVertices.insert( ID2WPointXYZId::value_type( id, pt ) );
                        }
                    if( ( y == m_nCellsY - 1 ) && ( z == m_nCellsZ - 1 ) )
                        if( wMarchingCubesCaseTables::edgeTable[tableIndex] & 32 )
                        {
                            WPointXYZId pt = calculateIntersection( vals, x, y, z, 5 );
                            unsigned int id = getEdgeID( x, y, z, 5 );
                            m_idToVertices.insert( ID2WPointXYZId::value_type( id, pt ) );
                        }

                    for( int i = 0; wMarchingCubesCaseTables::triTable[tableIndex][i] != -1; i += 3 )
                    {
                        WMCTriangle triangle;
                        unsigned int pointID0, pointID1, pointID2;
                        pointID0 = getEdgeID( x, y, z, wMarchingCubesCaseTables::triTable[tableIndex][i] );
                        pointID1 = getEdgeID( x, y, z, wMarchingCubesCaseTables::triTable[tableIndex][i + 1] );
                        pointID2 = getEdgeID( x, y, z, wMarchingCubesCaseTables::triTable[tableIndex][i + 2] );
                        triangle.pointID[0] = pointID0;
                        triangle.pointID[1] = pointID1;
                        triangle.pointID[2] = pointID2;
                        m_trivecTriangles.push_back( triangle );
                    }
                }
            }
        }
    }
    progress->finish();
}

template< typename T > WPointXYZId WMMarchingCubes::calculateIntersection( boost::shared_ptr< WValueSet< T > > vals,
                                                                           unsigned int nX, unsigned int nY, unsigned int nZ,
                                                                           unsigned int nEdgeNo )
{
    double x1;
    double y1;
    double z1;

    double x2;
    double y2;
    double z2;

    unsigned int v1x = nX;
    unsigned int v1y = nY;
    unsigned int v1z = nZ;

    unsigned int v2x = nX;
    unsigned int v2y = nY;
    unsigned int v2z = nZ;

    switch ( nEdgeNo )
    {
        case 0:
            v2y += 1;
            break;
        case 1:
            v1y += 1;
            v2x += 1;
            v2y += 1;
            break;
        case 2:
            v1x += 1;
            v1y += 1;
            v2x += 1;
            break;
        case 3:
            v1x += 1;
            break;
        case 4:
            v1z += 1;
            v2y += 1;
            v2z += 1;
            break;
        case 5:
            v1y += 1;
            v1z += 1;
            v2x += 1;
            v2y += 1;
            v2z += 1;
            break;
        case 6:
            v1x += 1;
            v1y += 1;
            v1z += 1;
            v2x += 1;
            v2z += 1;
            break;
        case 7:
            v1x += 1;
            v1z += 1;
            v2z += 1;
            break;
        case 8:
            v2z += 1;
            break;
        case 9:
            v1y += 1;
            v2y += 1;
            v2z += 1;
            break;
        case 10:
            v1x += 1;
            v1y += 1;
            v2x += 1;
            v2y += 1;
            v2z += 1;
            break;
        case 11:
            v1x += 1;
            v2x += 1;
            v2z += 1;
            break;
    }

    x1 = v1x * m_fCellLengthX;
    y1 = v1y * m_fCellLengthY;
    z1 = v1z * m_fCellLengthZ;
    x2 = v2x * m_fCellLengthX;
    y2 = v2y * m_fCellLengthY;
    z2 = v2z * m_fCellLengthZ;

    unsigned int nPointsInSlice = ( m_nCellsX + 1 ) * ( m_nCellsY + 1 );
    double val1 = vals->getScalar( v1z * nPointsInSlice + v1y * ( m_nCellsX + 1 ) + v1x );
    double val2 = vals->getScalar( v2z * nPointsInSlice + v2y * ( m_nCellsX + 1 ) + v2x );

    WPointXYZId intersection = interpolate( x1, y1, z1, x2, y2, z2, val1, val2 );
    intersection.newID = 0;
    return intersection;
}

WPointXYZId WMMarchingCubes::interpolate( double fX1, double fY1, double fZ1, double fX2, double fY2, double fZ2,
                                             double tVal1, double tVal2 )
{
    WPointXYZId interpolation;
    double mu;

    mu = static_cast<double>( ( m_tIsoLevel - tVal1 ) ) / ( tVal2 - tVal1 );
    interpolation.x = fX1 + mu * ( fX2 - fX1 );
    interpolation.y = fY1 + mu * ( fY2 - fY1 );
    interpolation.z = fZ1 + mu * ( fZ2 - fZ1 );
    interpolation.newID = 0;

    return interpolation;
}

int WMMarchingCubes::getEdgeID( unsigned int nX, unsigned int nY, unsigned int nZ, unsigned int nEdgeNo )
{
    switch ( nEdgeNo )
    {
        case 0:
            return 3 * getVertexID( nX, nY, nZ ) + 1;
        case 1:
            return 3 * getVertexID( nX, nY + 1, nZ );
        case 2:
            return 3 * getVertexID( nX + 1, nY, nZ ) + 1;
        case 3:
            return 3 * getVertexID( nX, nY, nZ );
        case 4:
            return 3 * getVertexID( nX, nY, nZ + 1 ) + 1;
        case 5:
            return 3 * getVertexID( nX, nY + 1, nZ + 1 );
        case 6:
            return 3 * getVertexID( nX + 1, nY, nZ + 1 ) + 1;
        case 7:
            return 3 * getVertexID( nX, nY, nZ + 1 );
        case 8:
            return 3 * getVertexID( nX, nY, nZ ) + 2;
        case 9:
            return 3 * getVertexID( nX, nY + 1, nZ ) + 2;
        case 10:
            return 3 * getVertexID( nX + 1, nY + 1, nZ ) + 2;
        case 11:
            return 3 * getVertexID( nX + 1, nY, nZ ) + 2;
        default:
            // Invalid edge no.
            return -1;
    }
}

unsigned int WMMarchingCubes::getVertexID( unsigned int nX, unsigned int nY, unsigned int nZ )
{
    return nZ * ( m_nCellsY + 1 ) * ( m_nCellsX + 1) + nY * ( m_nCellsX + 1 ) + nX;
}

void WMMarchingCubes::renderSurface()
{
    unsigned int nextID = 0;
    WTriangleMesh triMesh;
    triMesh.clearMesh();
    triMesh.resizeVertices( m_idToVertices.size() );
    triMesh.resizeTriangles( m_trivecTriangles.size() );

    // TODO(wiebel): MC what is this for?
    float xOff = 0.5f;
    float yOff = 0.5f;
    float zOff = 0.5f;

    // Rename vertices.
    ID2WPointXYZId::iterator mapIterator = m_idToVertices.begin();
    while ( mapIterator != m_idToVertices.end() )
    {
        ( *mapIterator ).second.newID = nextID;
        triMesh.fastAddVert( wmath::WPosition( ( *mapIterator ).second.x + xOff,
                                               ( *mapIterator ).second.y + yOff,
                                               ( *mapIterator ).second.z + zOff ) );
        nextID++;
        mapIterator++;
    }

    // Now rename triangles.
    WMCTriangleVECTOR::iterator vecIterator = m_trivecTriangles.begin();
    while ( vecIterator != m_trivecTriangles.end() )
    {
        for( unsigned int i = 0; i < 3; i++ )
        {
            unsigned int newID = m_idToVertices[( *vecIterator ).pointID[i]].newID;
            ( *vecIterator ).pointID[i] = newID;
        }
        triMesh.fastAddTriangle( ( *vecIterator ).pointID[0], ( *vecIterator ).pointID[1],
                                 ( *vecIterator ).pointID[2] );
        vecIterator++;
    }

    renderMesh( &triMesh );

    // TODO(wiebel): MC make the filename set automatically
//     bool saved = save( "/tmp/isosurfaceTestMesh.vtk", triMesh );
//     if( saved )
//     {
//         WLogger::getLogger()->addLogMessage( "File opened", "Marching Cubes", LL_DEBUG );
//     }
}


void WMMarchingCubes::renderMesh( WTriangleMesh* mesh )
{
//    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()
    m_moduleNode->remove( m_surfaceGeode );
    osg::Geometry* surfaceGeometry = new osg::Geometry();
    m_surfaceGeode = osg::ref_ptr< osg::Geode >( new osg::Geode );

    osg::Vec3Array* vertices = new osg::Vec3Array;
    for( size_t i = 0; i < mesh->getNumVertices(); ++i )
    {
        wmath::WPosition vertPos;
        vertPos = mesh->getVertex( i );
        vertices->push_back( osg::Vec3( vertPos[0], vertPos[1], vertPos[2] ) );
    }
    surfaceGeometry->setVertexArray( vertices );

    osg::DrawElementsUInt* surfaceElement;

    surfaceElement = new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES, 0 );
    for( unsigned int triId = 0; triId < mesh->getNumTriangles(); ++triId )
    {
        for( unsigned int vertId = 0; vertId < 3; ++vertId )
        {
            surfaceElement->push_back( mesh->getTriangleVertexId( triId, vertId ) );
        }
    }
    surfaceGeometry->addPrimitiveSet( surfaceElement );

    // ------------------------------------------------
    // normals
    osg::ref_ptr< osg::Vec3Array> normals( new osg::Vec3Array() );

    mesh->computeVertNormals(); // time consuming
    for( unsigned int vertId = 0; vertId < mesh->getNumVertices(); ++vertId )
    {
        wmath::WVector3D tmpNormal = mesh->getVertexNormal( vertId );
        normals->push_back( osg::Vec3( tmpNormal[0], tmpNormal[1], tmpNormal[2] ) );
    }
    surfaceGeometry->setNormalArray( normals.get() );
    surfaceGeometry->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );

    m_surfaceGeode->addDrawable( surfaceGeometry );
    osg::StateSet* state = m_surfaceGeode->getOrCreateStateSet();

    // ------------------------------------------------
    // colors
    osg::Vec4Array* colors = new osg::Vec4Array;

    colors->push_back( osg::Vec4( .9f, .9f, 0.9f, 1.0f ) );
    surfaceGeometry->setColorArray( colors );
    surfaceGeometry->setColorBinding( osg::Geometry::BIND_OVERALL );

    osg::ref_ptr<osg::LightModel> lightModel = new osg::LightModel();
    lightModel->setTwoSided( true );
    state->setAttributeAndModes( lightModel.get(), osg::StateAttribute::ON );
    state->setMode(  GL_BLEND, osg::StateAttribute::ON  );

    // ------------------------------------------------
    // Shader stuff

    // TODO(wiebel): fix texture coords.
    double xext = m_grid->getOffsetX() * m_grid->getNbCoordsX();
    double yext = m_grid->getOffsetY() * m_grid->getNbCoordsY();
    double zext = m_grid->getOffsetZ() * m_grid->getNbCoordsZ();

    osg::Vec3Array* texCoords = new osg::Vec3Array;
    for( size_t i = 0; i < mesh->getNumVertices(); ++i )
    {
        wmath::WPosition vertPos;
        vertPos = mesh->getVertex( i );
        texCoords->push_back( osg::Vec3( vertPos[0]/xext, vertPos[1]/yext, vertPos[2]/zext ) );
    }
    surfaceGeometry->setTexCoordArray( 0, texCoords );

    m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type0", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type1", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type2", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type3", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type4", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type5", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type6", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type7", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type8", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type9", 0 ) ) );

    m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha0", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha1", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha2", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha3", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha4", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha5", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha6", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha7", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha8", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha9", 1.0f ) ) );

    m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold0", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold1", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold2", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold3", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold4", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold5", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold6", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold7", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold8", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold9", 0.0f ) ) );

    m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex0", 0 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex1", 1 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex2", 2 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex3", 3 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex4", 4 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex5", 5 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex6", 6 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex7", 7 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex8", 8 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex9", 9 ) ) );

    for ( int i = 0; i < 10; ++i )
    {
        state->addUniform( m_typeUniforms[i] );
        state->addUniform( m_thresholdUniforms[i] );
        state->addUniform( m_alphaUniforms[i] );
        state->addUniform( m_samplerUniforms[i] );
    }

    state->addUniform( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useTexture", m_properties->getValue< bool >( "Use Texture" ) ) ) );
    state->addUniform( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useLighting", m_shaderUseLighting ) ) );
    if( m_shaderUseTransparency )
    {
        state->addUniform( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "opacity", m_opacityProp->get( true ) ) ) );
    }
    else
    {
        state->addUniform( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "opacity", 100 ) ) );
    }

    // NOTE: the following code should not be necessary. The update callback does this job just before the mesh is rendered
    // initially. Just set the texture changed flag to true. If this however might be needed use WSubject::getDataTextures.
    m_textureChanged = true;
    //std::vector< boost::shared_ptr< WDataSet > > dsl = WKernel::getRunningKernel()->getGui()->getDataSetList( 0, true );*/
    //if ( dsl.size() > 0 )
    //{
        //for ( int i = 0; i < 10; ++i )
        //{
            //m_typeUniforms[i]->set( 0 );
        //}

        //int c = 0;
        //for ( size_t i = 0; i < dsl.size(); ++i )
        //{
            //osg::ref_ptr<osg::Texture3D> texture3D = dsl[i]->getTexture()->getTexture();

            //state->setTextureAttributeAndModes( c, texture3D, osg::StateAttribute::ON );

            //float t = dsl[i]->getTexture()->getThreshold()/ 100.0;
            //float a = dsl[i]->getTexture()->getAlpha();

            //m_typeUniforms[c]->set( boost::shared_dynamic_cast<WDataSetSingle>( dsl[i] )->getValueSet()->getDataType() );
            //m_thresholdUniforms[c]->set( t );
            //m_alphaUniforms[c]->set( a );

            //++c;
        //}
    //}

    m_shader = osg::ref_ptr< WShader > ( new WShader( "surface" ) );
    m_shader->apply( m_surfaceGeode );

    m_moduleNode->insert( m_surfaceGeode );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_moduleNode );

    m_moduleNode->addUpdateCallback( new SurfaceNodeCallback( boost::shared_dynamic_cast< WMMarchingCubes >( shared_from_this() ) ) );

    //osgDB::writeNodeFile( *m_surfaceGeode, "/tmp/saved.osg" ); //for debugging
}

// TODO(wiebel): move this somewhere, where more classes can use it
int myIsfinite( double number )
{
#if defined( __linux__ ) || defined( __APPLE__ )
    // C99 defines isfinite() as a macro.
    return std::isfinite(number);
#elif defined( _WIN32 )
    // Microsoft Visual C++ and Borland C++ Builder use _finite().
    return _finite(number);
#else
    assert( 0 && "isfinite not provided on this platform or platform not known" );
#endif
}

void WMMarchingCubes::notifyTextureChange()
{
    m_textureChanged = true;
}

// TODO(wiebel): MC move this to a separate module in the future
bool WMMarchingCubes::save( std::string fileName, const WTriangleMesh& triMesh ) const
{
    if( triMesh.getNumVertices() == 0 )
    {
        WLogger::getLogger()->addLogMessage( "Will not write file that contains 0 vertices.", "Marching Cubes", LL_ERROR );
        return false;
    }

    if( triMesh.getNumTriangles() == 0 )
    {
        WLogger::getLogger()->addLogMessage( "Will not write file that contains 0 triangles.", "Marching Cubes", LL_ERROR );
        return false;
    }

    const char* c_file = fileName.c_str();
    std::ofstream dataFile( c_file );

    if ( dataFile )
    {
        WLogger::getLogger()->addLogMessage( "opening file", "Marching Cubes", LL_DEBUG );
    }
    else
    {
        WLogger::getLogger()->addLogMessage( "open file failed" + fileName , "Marching Cubes", LL_ERROR );
        return false;
    }

    dataFile.precision( 16 );

    WLogger::getLogger()->addLogMessage( "start writing file", "Marching Cubes", LL_DEBUG );
    dataFile << ( "# vtk DataFile Version 2.0\n" );
    dataFile << ( "generated using OpenWalnut\n" );
    dataFile << ( "ASCII\n" );
    dataFile << ( "DATASET UNSTRUCTURED_GRID\n" );

    wmath::WPosition point;
    dataFile << "POINTS " << triMesh.getNumVertices() << " float\n";
    for ( unsigned int i = 0; i < triMesh.getNumVertices(); ++i )
    {
        point = triMesh.getVertex( i );
        if( !( myIsfinite( point[0] ) && myIsfinite( point[1] ) && myIsfinite( point[2] ) ) )
        {
            WLogger::getLogger()->addLogMessage( "Will not write file from data that contains NAN or INF.", "Marching Cubes", LL_ERROR );
            return false;
        }
        dataFile << point[0] << " " << point[1] << " " << point[2] << "\n";
    }

    dataFile << "CELLS " << triMesh.getNumTriangles() << " " << triMesh.getNumTriangles() * 4 << "\n";
    for ( unsigned int i = 0; i < triMesh.getNumTriangles(); ++i )
    {
        dataFile << "3 " << triMesh.getTriangleVertexId( i, 0 ) << " "
                 <<  triMesh.getTriangleVertexId( i, 1 ) << " "
                 <<  triMesh.getTriangleVertexId( i, 2 ) << "\n";
    }
    dataFile << "CELL_TYPES "<< triMesh.getNumTriangles() <<"\n";
    for ( unsigned int i = 0; i < triMesh.getNumTriangles(); ++i )
    {
        dataFile << "5\n";
    }
    dataFile << "POINT_DATA " << triMesh.getNumVertices() << "\n";
    dataFile << "SCALARS scalars float\n";
    dataFile << "LOOKUP_TABLE default\n";
    for ( unsigned int i = 0; i < triMesh.getNumVertices(); ++i )
    {
        dataFile << "0\n";
    }
    dataFile.close();
    WLogger::getLogger()->addLogMessage( "saving done", "Marching Cubes", LL_DEBUG );
    return true;
}

WTriangleMesh WMMarchingCubes::load( std::string fileName )
{
    WTriangleMesh triMesh;
    triMesh.clearMesh();

    namespace su = string_utils;

    std::ifstream ifs;
    ifs.open( fileName.c_str(), std::ifstream::in );
    if( !ifs || ifs.bad() )
    {
        WLogger::getLogger()->addLogMessage( "Try load broken file '" + fileName + "'", "Marching Cubes", LL_ERROR );
        throw std::runtime_error( "Problem during reading file. Probably file not found." );
    }
    std::string line;

    // ------ HEADER -------
    std::vector< std::string > header;
    for( int i = 0; i < 4; ++i )  // strip first four lines
    {
        std::getline( ifs, line );
        if( !ifs.good() )
        {
            WLogger::getLogger()->addLogMessage( "Unexpected end of file: " + fileName, "Marching Cubes", LL_ERROR );
        }
        header.push_back( line );
    }
    if( header.at(0) != "# vtk DataFile Version 2.0" )
    {
        WLogger::getLogger()->addLogMessage( "Unsupported format version string: " + header.at( 0 ), "Marching Cubes", LL_WARNING );
    }

    if( su::toUpper( su::trim( header.at( 2 ) ) ) != "ASCII" )
    {
        WLogger::getLogger()->addLogMessage( "Only ASCII files supported, not " + header.at( 2 ), "Marching Cubes", LL_ERROR );
    }

    if(  su::tokenize( header.at( 3 ) ).size() < 2 ||
         su::toUpper( su::tokenize( header.at( 3 ) )[1] ) != "UNSTRUCTURED_GRID" )
    {
        WLogger::getLogger()->addLogMessage( "Invalid VTK DATASET type: " + su::tokenize( header.back() )[1], "Marching Cubes", LL_ERROR );
    }

    // ------ POINTS -------

    std::getline( ifs, line );
    size_t numPoints = 0;
    std::vector< std::string > tokens = su::tokenize( line );
    if( tokens.size() != 3 || su::toLower( tokens.at( 2 ) ) != "float" || su::toLower( tokens.at( 0 ) ) != "points"  )
    {
        WLogger::getLogger()->addLogMessage( "Invalid VTK POINTS declaration: " + line, "Marching Cubes", LL_ERROR );
    }
    try
    {
        numPoints = boost::lexical_cast< size_t >( tokens.at( 1 ) );
    }
    catch( const boost::bad_lexical_cast &e )
    {
        WLogger::getLogger()->addLogMessage( "Invalid number of points: " + tokens.at( 1 ), "Marching Cubes", LL_ERROR );
    }

    std::vector< wmath::WPosition > points;
    points.reserve( numPoints );
    float pointData[3];
    for( unsigned int i = 0; i < numPoints; ++i )
    {
        ifs >> pointData[0] >>  pointData[1] >>  pointData[2];
        points.push_back( wmath::WPosition( pointData[0], pointData[1], pointData[2] ) );
    }
    triMesh.setVertices( points );

    // ----- Vertex Ids For Cells---------
    char* cellsMarker = new char[30];
    size_t nbCells;
    size_t nbNumbers;
    ifs >> cellsMarker >> nbCells >> nbNumbers;

    triMesh.resizeTriangles( nbCells );
    unsigned int nbCellVerts;
    for( unsigned int i = 0; i < nbCells; ++i )
    {
        unsigned int tri[3];
        ifs >> nbCellVerts >> tri[0]  >> tri[1]  >> tri[2];
        triMesh.fastAddTriangle( tri[0], tri[1], tri[2] );
        if( nbCellVerts != 3 )
            WLogger::getLogger()->addLogMessage( "Number of cell vertices should be 3 but found " + nbCellVerts, "Marching Cubes", LL_ERROR );
    }

    // ----- Cell Types ---------
    char* cells_typesMarker = new char[30];
    size_t nbCellTypes;
    ifs >> cells_typesMarker >> nbCellTypes;
    unsigned int cellType;
    for( unsigned int i = 0; i < nbCellTypes; ++i )
    {
        ifs >> cellType;
        if( cellType != 5 )
        {
            WLogger::getLogger()->addLogMessage( "Invalid cell type: " + cellType, "Marching Cubes", LL_ERROR );
        }
    }

    ifs.close();

    // TODO(wiebel): probably load point data some time

    return triMesh;
}

void WMMarchingCubes::updateTextures()
{
    if ( m_active->get() )
    {
        m_surfaceGeode->setNodeMask( 0xFFFFFFFF );
    }
    else
    {
        m_surfaceGeode->setNodeMask( 0x0 );
    }

    if ( m_textureChanged || m_opacityProp->changed() || m_useTextureProp->changed()  )
    {
        m_textureChanged = false;

        // grab a list of data textures
        std::vector< boost::shared_ptr< WDataTexture3D > > tex = WDataHandler::getDefaultSubject()->getDataTextures( true );

        if ( tex.size() > 0 )
        {
            osg::StateSet* rootState = m_surfaceGeode->getOrCreateStateSet();

            // reset all uniforms
            for ( int i = 0; i < 10; ++i )
            {
                m_typeUniforms[i]->set( 0 );
            }

            rootState->addUniform( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useTexture", m_useTextureProp->get( true ) ) ) );
            rootState->addUniform( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useLighting", m_shaderUseLighting ) ) );

            if( m_shaderUseTransparency )
            {
                rootState->addUniform( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "opacity", m_opacityProp->get( true ) ) ) );
            }
            else
            {
                rootState->addUniform( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "opacity", 100 ) ) );
            }

            // for each texture -> apply
            int c = 0;
            for ( std::vector< boost::shared_ptr< WDataTexture3D > >::const_iterator iter = tex.begin(); iter != tex.end(); ++iter )
            {
                osg::ref_ptr<osg::Texture3D> texture3D = ( *iter )->getTexture();
                rootState->setTextureAttributeAndModes( c, texture3D, osg::StateAttribute::ON );

                // set threshold/opacity as uniforms
                float t = ( *iter )->getThreshold() / 255.0;
                float a = ( *iter )->getAlpha();

                m_typeUniforms[c]->set( ( *iter )->getDataType() );
                m_thresholdUniforms[c]->set( t );
                m_alphaUniforms[c]->set( a );

                ++c;
            }
        }
    }
}

