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

#include "WMarchingCubesModule.h"
#include "marchingCubesCaseTables.h"

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
    return "This description has to be improved when the module is completed. By now lets say the following: This module implement the marching cubes algorithm with a consisten triangulation. It allows to compute isosurfaces for a given isovalue on data given on grid only consisting of cubes. It yields the surface as triangle soup.";
}

void WMarchingCubesModule::threadMain()
{
    // TODO(wiebel): fix this hack when possible by using an input connector.
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

    std::cout << " -- THREADMAIN for Marching Cubes 1 -- " << m_dataSet->getFileName() << std::endl;

    generateSurface( 100 );

    std::cout << " -- THREADMAIN for Marching Cubes 2 -- " << m_dataSet->getFileName() << std::endl;
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
    ( m_properties.addDouble( "isoValue", 80 ) )->connect( boost::bind( &WMarchingCubesModule::slotPropertyChanged, this, _1 ) );
}

void WMarchingCubesModule::slotPropertyChanged( std::string propertyName )
{
    if ( propertyName == "isoValue" )
    {
        //updateTextures();
    }
    else
    {
        assert( 0 && "This property name is not soppurted by this function yet." );
    }
}


void WMarchingCubesModule::generateSurface( double isoValue )
{
    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( m_dataSet->getGrid() );
    m_vals =  boost::shared_dynamic_cast< WValueSet< unsigned char > >( m_dataSet->getValueSet() );
    assert( grid );
    assert( m_vals );

    m_tIsoLevel = isoValue;

    unsigned int nX = grid->getNbCoordsX();
    unsigned int nY = grid->getNbCoordsY();
    unsigned int nZ = grid->getNbCoordsZ();

    unsigned int nPointsInSlice = nX * nY;

    // Generate isosurface.
    for ( unsigned int z = 0; z < ( nZ - 1 ); z++ )
    {
        for ( unsigned int y = 0; y < ( nY - 1 ); y++ )
        {
            for ( unsigned int x = 0; x < ( nX - 1 ); x++ )
            {
                // Calculate table lookup index from those
                // vertices which are below the isolevel.
                unsigned int tableIndex = 0;
                if ( m_vals->getScalar( z * nPointsInSlice + y * nX + x ) < m_tIsoLevel )
                    tableIndex |= 1;
                if ( m_vals->getScalar( z * nPointsInSlice + ( y + 1 ) * nX + x ) < m_tIsoLevel )
                    tableIndex |= 2;
                if ( m_vals->getScalar( z * nPointsInSlice + ( y + 1 ) * nX + ( x + 1 ) ) < m_tIsoLevel )
                    tableIndex |= 4;
                if ( m_vals->getScalar( z * nPointsInSlice + y * nX + ( x + 1 ) ) < m_tIsoLevel )
                    tableIndex |= 8;
                if ( m_vals->getScalar( ( z + 1 ) * nPointsInSlice + y * nX + x ) < m_tIsoLevel )
                    tableIndex |= 16;
                if ( m_vals->getScalar( ( z + 1 ) * nPointsInSlice + ( y + 1 ) * nX + x ) < m_tIsoLevel )
                    tableIndex |= 32;
                if ( m_vals->getScalar( ( z + 1 ) * nPointsInSlice + ( y + 1 ) * nX + ( x + 1 ) ) < m_tIsoLevel )
                    tableIndex |= 64;
                if ( m_vals->getScalar( ( z + 1 ) * nPointsInSlice + y * nX + ( x + 1 ) ) < m_tIsoLevel )
                    tableIndex |= 128;

                // Now create a triangulation of the isosurface in this
                // cell.
                if ( m_edgeTable[tableIndex] != 0 )
                {
                    if ( m_edgeTable[tableIndex] & 8 )
                    {
                        WPointXYZId pt = calculateIntersection( x, y, z, 3 );
                        unsigned int id = getEdgeID( x, y, z, 3 );
                        m_idToVertices.insert( ID2WPointXYZId::value_type( id, pt ) );
                    }
                    if ( m_edgeTable[tableIndex] & 1 )
                    {
                        WPointXYZId pt = calculateIntersection( x, y, z, 0 );
                        unsigned int id = getEdgeID( x, y, z, 0 );
                        m_idToVertices.insert( ID2WPointXYZId::value_type( id, pt ) );
                    }
                    if ( m_edgeTable[tableIndex] & 256 )
                    {
                        WPointXYZId pt = calculateIntersection( x, y, z, 8 );
                        unsigned int id = getEdgeID( x, y, z, 8 );
                        m_idToVertices.insert( ID2WPointXYZId::value_type( id, pt ) );
                    }

                    if ( x == m_nCellsX - 1 )
                    {
                        if ( m_edgeTable[tableIndex] & 4 )
                        {
                            WPointXYZId pt = calculateIntersection( x, y, z, 2 );
                            unsigned int id = getEdgeID( x, y, z, 2 );
                            m_idToVertices.insert( ID2WPointXYZId::value_type( id, pt ) );
                        }
                        if ( m_edgeTable[tableIndex] & 2048 )
                        {
                            WPointXYZId pt = calculateIntersection( x, y, z, 11 );
                            unsigned int id = getEdgeID( x, y, z, 11 );
                            m_idToVertices.insert( ID2WPointXYZId::value_type( id, pt ) );
                        }
                    }
                    if ( y == m_nCellsY - 1 )
                    {
                        if ( m_edgeTable[tableIndex] & 2 )
                        {
                            WPointXYZId pt = calculateIntersection( x, y, z, 1 );
                            unsigned int id = getEdgeID( x, y, z, 1 );
                            m_idToVertices.insert( ID2WPointXYZId::value_type( id, pt ) );
                        }
                        if ( m_edgeTable[tableIndex] & 512 )
                        {
                            WPointXYZId pt = calculateIntersection( x, y, z, 9 );
                            unsigned int id = getEdgeID( x, y, z, 9 );
                            m_idToVertices.insert( ID2WPointXYZId::value_type( id, pt ) );
                        }
                    }
                    if ( z == m_nCellsZ - 1 )
                    {
                        if ( m_edgeTable[tableIndex] & 16 )
                        {
                            WPointXYZId pt = calculateIntersection( x, y, z, 4 );
                            unsigned int id = getEdgeID( x, y, z, 4 );
                            m_idToVertices.insert( ID2WPointXYZId::value_type( id, pt ) );
                        }
                        if ( m_edgeTable[tableIndex] & 128 )
                        {
                            WPointXYZId pt = calculateIntersection( x, y, z, 7 );
                            unsigned int id = getEdgeID( x, y, z, 7 );
                            m_idToVertices.insert( ID2WPointXYZId::value_type( id, pt ) );
                        }
                    }
                    if ( ( x == m_nCellsX - 1 ) && ( y == m_nCellsY - 1 ) )
                        if ( m_edgeTable[tableIndex] & 1024 )
                        {
                            WPointXYZId pt = calculateIntersection( x, y, z, 10 );
                            unsigned int id = getEdgeID( x, y, z, 10 );
                            m_idToVertices.insert( ID2WPointXYZId::value_type( id, pt ) );
                        }
                    if ( ( x == m_nCellsX - 1 ) && ( z == m_nCellsZ - 1 ) )
                        if ( m_edgeTable[tableIndex] & 64 )
                        {
                            WPointXYZId pt = calculateIntersection( x, y, z, 6 );
                            unsigned int id = getEdgeID( x, y, z, 6 );
                            m_idToVertices.insert( ID2WPointXYZId::value_type( id, pt ) );
                        }
                    if ( ( y == m_nCellsY - 1 ) && ( z == m_nCellsZ - 1 ) )
                        if ( m_edgeTable[tableIndex] & 32 )
                        {
                            WPointXYZId pt = calculateIntersection( x, y, z, 5 );
                            unsigned int id = getEdgeID( x, y, z, 5 );
                            m_idToVertices.insert( ID2WPointXYZId::value_type( id, pt ) );
                        }

                    for ( int i = 0; m_triTable[tableIndex][i] != -1; i += 3 )
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

    unsigned int nPointsInSlice = nX * nY;
    double val1 = m_vals->getScalar( v1z * nPointsInSlice + v1y * nX + v1x );
    double val2 = m_vals->getScalar( v2z * nPointsInSlice + v2y * nX + v2x );
    WPointXYZId intersection = interpolate( x1, y1, z1, x2, y2, z2, val1, val2 );
    intersection.newID = 0;
    return intersection;
}

WPointXYZId WMarchingCubesModule::interpolate( double fX1, double fY1, double fZ1, double fX2, double fY2, double fZ2,
                                             double tVal1, double tVal2 )
{
    WPointXYZId interpolation;
    double mu;

    mu = double( ( m_tIsoLevel - tVal1 ) ) / ( tVal2 - tVal1 );
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
