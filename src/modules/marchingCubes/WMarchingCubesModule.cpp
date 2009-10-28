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

#include "WMarchingCubesModule.h"
#include "marchingCubesCaseTables.h"
#include "WTriangleMesh.h"

#include <osg/Geode>
#include <osg/Geometry>

#include "../../math/WVector3D.h"
#include "../../dataHandler/WSubject.h"
#include "../../dataHandler/WGridRegular3D.h"
#include "../../kernel/WKernel.h"


WMarchingCubesModule::WMarchingCubesModule():
    WModule()
{
    // WARNING: initializing connectors inside the constructor will lead to an exception.
    // Implement WModule::initializeConnectors instead.
}

WMarchingCubesModule::~WMarchingCubesModule()
{
    // cleanup
    removeConnectors();
}

const std::string WMarchingCubesModule::getName() const
{
    return "Marching Cubes";
}

const std::string WMarchingCubesModule::getDescription() const
{
    return "This description has to be improved when the module is completed."
" By now lets say the following: This module implement the marching cubes"
" algorithm with a consisten triangulation. It allows to compute isosurfaces"
" for a given isovalue on data given on grid only consisting of cubes. It yields"
" the surface as triangle soup.";
}


void WMarchingCubesModule::threadMain()
{
    // TODO(wiebel): MC fix this hack when possible by using an input connector.
    while ( !WKernel::getRunningKernel() )
    {
        sleep( 1 );
    }
    while ( !WKernel::getRunningKernel()->getDataHandler() )
    {
        sleep( 1 );
    }
    while ( !WKernel::getRunningKernel()->getDataHandler()->getNumberOfSubjects() )
    {
        sleep( 1 );
    }
    boost::shared_ptr< WDataHandler > dh = WKernel::getRunningKernel()->getDataHandler();
    boost::shared_ptr< WSubject > subject = (*dh)[0];
    m_dataSet = boost::shared_dynamic_cast< const WDataSetSingle >( (*subject)[0] );

    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( m_dataSet->getGrid() );
    assert( grid );
    m_vals =  boost::shared_dynamic_cast< WValueSet< unsigned char > >( m_dataSet->getValueSet() );
    assert( m_vals );

    m_fCellLengthX = grid->getOffsetX();
    m_fCellLengthY = grid->getOffsetY();
    m_fCellLengthZ = grid->getOffsetZ();

    m_nCellsX = grid->getNbCoordsX() - 1;
    m_nCellsY = grid->getNbCoordsY() - 1;
    m_nCellsZ = grid->getNbCoordsZ() - 1;

    WLogger::getLogger()->addLogMessage( "Computing surface ...", "Marching Cubes", LL_DEBUG );

    // TODO(wiebel): MC set correct isoValue here
    generateSurface( 100 );

    WLogger::getLogger()->addLogMessage( "Rendering surface ...", "Marching Cubes", LL_DEBUG );

    renderSurface();

    WLogger::getLogger()->addLogMessage( "Done!", "Marching Cubes", LL_DEBUG );
}

void WMarchingCubesModule::connectors()
{
    // initialize connectors

    m_input = boost::shared_ptr<WModuleInputData< boost::shared_ptr< WDataSet > > >(
        new WModuleInputData< boost::shared_ptr< WDataSet > >( shared_from_this(),
                                                               "in", "Dataset to compute isosurface for." )
        );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_input );

    // call WModules initialization
    WModule::connectors();
}

void WMarchingCubesModule::properties()
{
    ( m_properties->addDouble( "isoValue", 80 ) )->connect( boost::bind( &WMarchingCubesModule::slotPropertyChanged, this, _1 ) );
}

void WMarchingCubesModule::slotPropertyChanged( std::string propertyName )
{
    if( propertyName == "isoValue" )
    {
        // updateTextures();
    }
    else
    {
        assert( 0 && "This property name is not soppurted by this function yet." );
    }
}
void WMarchingCubesModule::generateSurface( double isoValue )
{
    m_tIsoLevel = isoValue;

    unsigned int nX = m_nCellsX + 1;
    unsigned int nY = m_nCellsY + 1;


    unsigned int nPointsInSlice = nX * nY;

    // Generate isosurface.
    for( unsigned int z = 0; z < m_nCellsZ; z++ )
    {
        for( unsigned int y = 0; y < m_nCellsY; y++ )
        {
            for( unsigned int x = 0; x < m_nCellsX; x++ )
            {
                // Calculate table lookup index from those
                // vertices which are below the isolevel.
                unsigned int tableIndex = 0;
                if( m_vals->getScalar( z * nPointsInSlice + y * nX + x ) < m_tIsoLevel )
                    tableIndex |= 1;
                if( m_vals->getScalar( z * nPointsInSlice + ( y + 1 ) * nX + x ) < m_tIsoLevel )
                    tableIndex |= 2;
                if( m_vals->getScalar( z * nPointsInSlice + ( y + 1 ) * nX + ( x + 1 ) ) < m_tIsoLevel )
                    tableIndex |= 4;
                if( m_vals->getScalar( z * nPointsInSlice + y * nX + ( x + 1 ) ) < m_tIsoLevel )
                    tableIndex |= 8;
                if( m_vals->getScalar( ( z + 1 ) * nPointsInSlice + y * nX + x ) < m_tIsoLevel )
                    tableIndex |= 16;
                if( m_vals->getScalar( ( z + 1 ) * nPointsInSlice + ( y + 1 ) * nX + x ) < m_tIsoLevel )
                    tableIndex |= 32;
                if( m_vals->getScalar( ( z + 1 ) * nPointsInSlice + ( y + 1 ) * nX + ( x + 1 ) ) < m_tIsoLevel )
                    tableIndex |= 64;
                if( m_vals->getScalar( ( z + 1 ) * nPointsInSlice + y * nX + ( x + 1 ) ) < m_tIsoLevel )
                    tableIndex |= 128;

                // Now create a triangulation of the isosurface in this
                // cell.
                if( m_edgeTable[tableIndex] != 0 )
                {
                    if( m_edgeTable[tableIndex] & 8 )
                    {
                        WPointXYZId pt = calculateIntersection( x, y, z, 3 );
                        unsigned int id = getEdgeID( x, y, z, 3 );
                        m_idToVertices.insert( ID2WPointXYZId::value_type( id, pt ) );
                    }
                    if( m_edgeTable[tableIndex] & 1 )
                    {
                        WPointXYZId pt = calculateIntersection( x, y, z, 0 );
                        unsigned int id = getEdgeID( x, y, z, 0 );
                        m_idToVertices.insert( ID2WPointXYZId::value_type( id, pt ) );
                    }
                    if( m_edgeTable[tableIndex] & 256 )
                    {
                        WPointXYZId pt = calculateIntersection( x, y, z, 8 );
                        unsigned int id = getEdgeID( x, y, z, 8 );
                        m_idToVertices.insert( ID2WPointXYZId::value_type( id, pt ) );
                    }

                    if( x == m_nCellsX - 1 )
                    {
                        if( m_edgeTable[tableIndex] & 4 )
                        {
                            WPointXYZId pt = calculateIntersection( x, y, z, 2 );
                            unsigned int id = getEdgeID( x, y, z, 2 );
                            m_idToVertices.insert( ID2WPointXYZId::value_type( id, pt ) );
                        }
                        if( m_edgeTable[tableIndex] & 2048 )
                        {
                            WPointXYZId pt = calculateIntersection( x, y, z, 11 );
                            unsigned int id = getEdgeID( x, y, z, 11 );
                            m_idToVertices.insert( ID2WPointXYZId::value_type( id, pt ) );
                        }
                    }
                    if( y == m_nCellsY - 1 )
                    {
                        if( m_edgeTable[tableIndex] & 2 )
                        {
                            WPointXYZId pt = calculateIntersection( x, y, z, 1 );
                            unsigned int id = getEdgeID( x, y, z, 1 );
                            m_idToVertices.insert( ID2WPointXYZId::value_type( id, pt ) );
                        }
                        if( m_edgeTable[tableIndex] & 512 )
                        {
                            WPointXYZId pt = calculateIntersection( x, y, z, 9 );
                            unsigned int id = getEdgeID( x, y, z, 9 );
                            m_idToVertices.insert( ID2WPointXYZId::value_type( id, pt ) );
                        }
                    }
                    if( z == m_nCellsZ - 1 )
                    {
                        if( m_edgeTable[tableIndex] & 16 )
                        {
                            WPointXYZId pt = calculateIntersection( x, y, z, 4 );
                            unsigned int id = getEdgeID( x, y, z, 4 );
                            m_idToVertices.insert( ID2WPointXYZId::value_type( id, pt ) );
                        }
                        if( m_edgeTable[tableIndex] & 128 )
                        {
                            WPointXYZId pt = calculateIntersection( x, y, z, 7 );
                            unsigned int id = getEdgeID( x, y, z, 7 );
                            m_idToVertices.insert( ID2WPointXYZId::value_type( id, pt ) );
                        }
                    }
                    if( ( x == m_nCellsX - 1 ) && ( y == m_nCellsY - 1 ) )
                        if( m_edgeTable[tableIndex] & 1024 )
                        {
                            WPointXYZId pt = calculateIntersection( x, y, z, 10 );
                            unsigned int id = getEdgeID( x, y, z, 10 );
                            m_idToVertices.insert( ID2WPointXYZId::value_type( id, pt ) );
                        }
                    if( ( x == m_nCellsX - 1 ) && ( z == m_nCellsZ - 1 ) )
                        if( m_edgeTable[tableIndex] & 64 )
                        {
                            WPointXYZId pt = calculateIntersection( x, y, z, 6 );
                            unsigned int id = getEdgeID( x, y, z, 6 );
                            m_idToVertices.insert( ID2WPointXYZId::value_type( id, pt ) );
                        }
                    if( ( y == m_nCellsY - 1 ) && ( z == m_nCellsZ - 1 ) )
                        if( m_edgeTable[tableIndex] & 32 )
                        {
                            WPointXYZId pt = calculateIntersection( x, y, z, 5 );
                            unsigned int id = getEdgeID( x, y, z, 5 );
                            m_idToVertices.insert( ID2WPointXYZId::value_type( id, pt ) );
                        }

                    for( int i = 0; m_triTable[tableIndex][i] != -1; i += 3 )
                    {
                        WMCTriangle triangle;
                        unsigned int pointID0, pointID1, pointID2;
                        pointID0 = getEdgeID( x, y, z, m_triTable[tableIndex][i] );
                        pointID1 = getEdgeID( x, y, z, m_triTable[tableIndex][i + 1] );
                        pointID2 = getEdgeID( x, y, z, m_triTable[tableIndex][i + 2] );
                        triangle.pointID[0] = pointID0;
                        triangle.pointID[1] = pointID1;
                        triangle.pointID[2] = pointID2;
                        m_trivecTriangles.push_back( triangle );
                    }
                }
            }
        }
    }
}


WPointXYZId WMarchingCubesModule::calculateIntersection( unsigned int nX, unsigned int nY, unsigned int nZ,
                                                       unsigned int nEdgeNo )
{
    double x1, y1, z1, x2, y2, z2;
    unsigned int v1x = nX, v1y = nY, v1z = nZ;
    unsigned int v2x = nX, v2y = nY, v2z = nZ;

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
    double val1 = m_vals->getScalar( v1z * nPointsInSlice + v1y * ( m_nCellsX + 1 ) + v1x );
    double val2 = m_vals->getScalar( v2z * nPointsInSlice + v2y * ( m_nCellsX + 1 ) + v2x );

    WPointXYZId intersection = interpolate( x1, y1, z1, x2, y2, z2, val1, val2 );
    intersection.newID = 0;
    return intersection;
}

WPointXYZId WMarchingCubesModule::interpolate( double fX1, double fY1, double fZ1, double fX2, double fY2, double fZ2,
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

int WMarchingCubesModule::getEdgeID( unsigned int nX, unsigned int nY, unsigned int nZ, unsigned int nEdgeNo )
{
    switch ( nEdgeNo )
    {
        case 0:
            return getVertexID( nX, nY, nZ ) + 1;
        case 1:
            return getVertexID( nX, nY + 1, nZ );
        case 2:
            return getVertexID( nX + 1, nY, nZ ) + 1;
        case 3:
            return getVertexID( nX, nY, nZ );
        case 4:
            return getVertexID( nX, nY, nZ + 1 ) + 1;
        case 5:
            return getVertexID( nX, nY + 1, nZ + 1 );
        case 6:
            return getVertexID( nX + 1, nY, nZ + 1 ) + 1;
        case 7:
            return getVertexID( nX, nY, nZ + 1 );
        case 8:
            return getVertexID( nX, nY, nZ ) + 2;
        case 9:
            return getVertexID( nX, nY + 1, nZ ) + 2;
        case 10:
            return getVertexID( nX + 1, nY + 1, nZ ) + 2;
        case 11:
            return getVertexID( nX + 1, nY, nZ ) + 2;
        default:
            // Invalid edge no.
            return -1;
    }
}

unsigned int WMarchingCubesModule::getVertexID( unsigned int nX, unsigned int nY, unsigned int nZ )
{
    return 3* (nZ *(m_nCellsY + 1)*(m_nCellsX + 1) + nY*(m_nCellsX + 1) + nX);
}

void WMarchingCubesModule::renderSurface()
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

    // ========================
    // drawing

    osg::Geometry* surfaceGeometry = new osg::Geometry();

    osg::Geode *geode = new osg::Geode;
    osg::Vec3Array* vertices = new osg::Vec3Array;
    for( size_t i = 0; i < triMesh.getNumVertices(); ++i )
    {
        wmath::WPosition vertPos;
        vertPos = triMesh.getVertex( i );
        vertices->push_back( osg::Vec3( vertPos[0], vertPos[1], vertPos[2] ) );
    }
    surfaceGeometry->setVertexArray( vertices );

    osg::DrawElementsUInt* surfaceElement;

    surfaceElement = new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES, 0 );
    for( unsigned int triId = 0; triId < triMesh.getNumTriangles(); ++triId )
    {
        for( unsigned int vertId = 0; vertId < 3; ++vertId )
        {
            surfaceElement->push_back( triMesh.getTriangleVertexId( triId, vertId ) );
        }
    }
    surfaceGeometry->addPrimitiveSet( surfaceElement );

    // normals
    osg::ref_ptr< osg::Vec3Array> normals( new osg::Vec3Array() );
    for( unsigned int triId = 0; triId < triMesh.getNumTriangles(); ++triId )
    {
        wmath::WVector3D tmpNormal = triMesh.getTriangleNormal( triId );
        normals->push_back( osg::Vec3( tmpNormal[0], tmpNormal[1], tmpNormal[1] ) );
    }

    surfaceGeometry->setNormalArray( normals.get() );

    // TODO(wiebel): MC change to BIND_PER_VERTEX to make it more beautiful.
    surfaceGeometry->setNormalBinding( osg::Geometry::BIND_PER_PRIMITIVE );

    osg::Vec4Array* colors = new osg::Vec4Array;

    // TODO(wiebel): MC transparency is set to 0.5 here
    colors->push_back( osg::Vec4( 1.0f, 1.0f, 0.0f, .5f ) );
    surfaceGeometry->setColorArray( colors );
    surfaceGeometry->setColorBinding( osg::Geometry::BIND_OVERALL );

    geode->addDrawable( surfaceGeometry );
    geode->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::ON );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->addChild( geode );


    // TODO(wiebel): MC make the filename set automatically
    save( "/tmp/isosurfaceTestMesh.vtk", triMesh );
}


// TODO(wiebel): MC move this to a separate module in the future
bool WMarchingCubesModule::save( std::string filename, const WTriangleMesh& triMesh ) const
{
    const char* c_file = filename.c_str();
    std::ofstream dataFile( c_file );

    if ( dataFile )
    {
        std::cout << "opening file" << std::endl;
    }
    else
    {
        std::cout << "open file failed: " << filename.c_str() << std::endl;
        return false;
    }

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
    std::cout << " saving  done" << std::endl;
    return true;
}
