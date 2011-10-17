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

#ifndef WMARCHINGCUBESALGORITHM_H
#define WMARCHINGCUBESALGORITHM_H

#include <vector>
#include <map>
#include "../../common/math/WMatrix.h"
#include "../../common/WProgressCombiner.h"
#include "../WTriangleMesh.h"
#include "marchingCubesCaseTables.h"
#include "../WExportWGE.h"

/**
 * A point consisting of its coordinates and ID
 */
struct WPointXYZId
{
    unsigned int newID; //!< ID of the point
    double x; //!< x coordinates of the point.
    double y; //!< y coordinates of the point.
    double z; //!< z coordinates of the point.
};

typedef std::map< unsigned int, WPointXYZId > ID2WPointXYZId;

/**
 * Encapsulated ids representing a triangle.
 */
struct WMCTriangle
{
    unsigned int pointID[3]; //!< The IDs of the vertices of the triangle.
};

typedef std::vector<WMCTriangle> WMCTriangleVECTOR;

// -------------------------------------------------------
//
// Numbering of edges (0..B) and vertices (0..7) per cube.
//
//      5--5--6
//     /|    /|
//    4 |   6 |    A=10
//   /  9  /  A
//  4--7--7   |
//  |   | |   |
//  |   1-|1--2
//  8  /  B  /
//  | 0   | 2      B=11
//  |/    |/
//  0--3--3
//
//  |  /
//  z y
//  |/
//  0--x--
//
// -------------------------------------------------------

/**
 * This class does the actual computation of marching cubes.
 */
class WGE_EXPORT WMarchingCubesAlgorithm
{
/**
 * Only UnitTests may be friends.
 */
friend class WMarchingCubesAlgorithmTest;

public:
    /**
     * Constructor needed for matrix initalization.
     */
     WMarchingCubesAlgorithm();

    /**
     * Generate the triangles for the surface on the given dataSet (inGrid, vals). The texture coordinates in the resulting mesh are relative to
     * the grid. This means they are NOT transformed. This ensure faster grid matrix updates in texture space.
     * This might be useful where texture transformation matrices are used.
     *
     * \param nbCoordsX number of vertices in X direction
     * \param nbCoordsY number of vertices in Y direction
     * \param nbCoordsZ number of vertices in Z direction
     * \param mat the matrix transforming the vertices from canonical space
     * \param vals the values at the vertices
     * \param isoValue The surface will run through all positions with this value.
     * \param mainProgress progress combiner used to report our progress to
     *
     * \return the genereated surface
     */
    template< typename T >
    boost::shared_ptr< WTriangleMesh > generateSurface(  size_t nbCoordsX, size_t nbCoordsY, size_t nbCoordsZ,
                                                          const WMatrix< double >& mat,
                                                          const std::vector< T >* vals,
                                                          double isoValue,
                                                          boost::shared_ptr< WProgressCombiner > mainProgress );

protected:
private:
    /**
     * Calculates the intersection point id of the isosurface with an
     * edge.
     *
     * \param vals the values at the vertices
     * \param nX id of cell in x direction
     * \param nY id of cell in y direction
     * \param nZ id of cell in z direction
     * \param nEdgeNo id of the edge the point that will be interpolates lies on
     *
     * \return intersection point id
     */
    template< typename T > WPointXYZId calculateIntersection( const std::vector< T >* vals,
                                                              unsigned int nX, unsigned int nY, unsigned int nZ, unsigned int nEdgeNo );

    /**
     * Interpolates between two grid points to produce the point at which
     * the isosurface intersects an edge.
     *
     * \param fX1 x coordinate of first position
     * \param fY1 y coordinate of first position
     * \param fZ1 z coordinate of first position
     * \param fX2 x coordinate of second position
     * \param fY2 y coordinate of first position
     * \param fZ2 z coordinate of first position
     * \param tVal1 scalar value at first position
     * \param tVal2 scalar value at second position
     *
     * \return interpolated point
     */
    WPointXYZId interpolate( double fX1, double fY1, double fZ1, double fX2, double fY2, double fZ2, double tVal1, double tVal2 );

    /**
     * Returns the edge ID.
     *
     * \param nX ID of desired cell along x axis
     * \param nY ID of desired cell along y axis
     * \param nZ ID of desired cell along z axis
     * \param nEdgeNo id of edge inside cell
     *
     * \return The id of the edge in the large array.
     */
    int getEdgeID( unsigned int nX, unsigned int nY, unsigned int nZ, unsigned int nEdgeNo );

    /**
     * Returns the ID of the vertex given by by the IDs along the axis
     * \param nX ID of desired vertex along x axis
     * \param nY ID of desired vertex along y axis
     * \param nZ ID of desired vertex along z axis
     *
     * \return ID of vertex with the given coordinates
     */
    unsigned int getVertexID( unsigned int nX, unsigned int nY, unsigned int nZ );

    unsigned int m_nCellsX;  //!< No. of cells in x direction.
    unsigned int m_nCellsY;  //!< No. of cells in y direction.
    unsigned int m_nCellsZ;  //!< No. of cells in z direction.

    double m_tIsoLevel;  //!< The isovalue.

    WMatrix< double > m_matrix; //!< The 4x4 transformation matrix for the triangle vertices.

    ID2WPointXYZId m_idToVertices;  //!< List of WPointXYZIds which form the isosurface.
    WMCTriangleVECTOR m_trivecTriangles;  //!< List of WMCTriangleS which form the triangulation of the isosurface.
};


template<typename T> boost::shared_ptr<WTriangleMesh> WMarchingCubesAlgorithm::generateSurface( size_t nbCoordsX, size_t nbCoordsY, size_t nbCoordsZ,
                                                                                                 const WMatrix< double >& mat,
                                                                                                 const std::vector< T >* vals,
                                                                                                 double isoValue,
                                                                                                 boost::shared_ptr< WProgressCombiner > mainProgress )
{
    WAssert( vals, "No value set provided." );

    m_idToVertices.clear();
    m_trivecTriangles.clear();

    m_nCellsX = nbCoordsX - 1;
    m_nCellsY = nbCoordsY - 1;
    m_nCellsZ = nbCoordsZ - 1;

    m_matrix = mat;

    m_tIsoLevel = isoValue;

    unsigned int nX = m_nCellsX + 1;
    unsigned int nY = m_nCellsY + 1;


    unsigned int nPointsInSlice = nX * nY;

    boost::shared_ptr< WProgress > progress = boost::shared_ptr< WProgress >( new WProgress( "Marching Cubes", m_nCellsZ ) );
    mainProgress->addSubProgress( progress );
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
                if( ( *vals )[ z * nPointsInSlice + y * nX + x ] < m_tIsoLevel )
                    tableIndex |= 1;
                if( ( *vals )[ z * nPointsInSlice + ( y + 1 ) * nX + x ] < m_tIsoLevel )
                    tableIndex |= 2;
                if( ( *vals )[ z * nPointsInSlice + ( y + 1 ) * nX + ( x + 1 ) ] < m_tIsoLevel )
                    tableIndex |= 4;
                if( ( *vals )[ z * nPointsInSlice + y * nX + ( x + 1 ) ] < m_tIsoLevel )
                    tableIndex |= 8;
                if( ( *vals )[ ( z + 1 ) * nPointsInSlice + y * nX + x ] < m_tIsoLevel )
                    tableIndex |= 16;
                if( ( *vals )[ ( z + 1 ) * nPointsInSlice + ( y + 1 ) * nX + x ] < m_tIsoLevel )
                    tableIndex |= 32;
                if( ( *vals )[ ( z + 1 ) * nPointsInSlice + ( y + 1 ) * nX + ( x + 1 ) ] < m_tIsoLevel )
                    tableIndex |= 64;
                if( ( *vals )[ ( z + 1 ) * nPointsInSlice + y * nX + ( x + 1 ) ] < m_tIsoLevel )
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

    unsigned int nextID = 0;
    boost::shared_ptr< WTriangleMesh > triMesh( new WTriangleMesh( m_idToVertices.size(), m_trivecTriangles.size() ) );

    // Rename vertices.
    ID2WPointXYZId::iterator mapIterator = m_idToVertices.begin();
    while( mapIterator != m_idToVertices.end() )
    {
        WPosition texCoord = WPosition( mapIterator->second.x / nbCoordsX,
                                                      mapIterator->second.y / nbCoordsY,
                                                      mapIterator->second.z / nbCoordsZ );

        // transform from grid coordinate system to world coordinates
        WPosition pos = WPosition( mapIterator->second.x, mapIterator->second.y, mapIterator->second.z );

        std::vector< double > resultPos4D( 4 );
        resultPos4D[0] = m_matrix( 0, 0 ) * pos[0] + m_matrix( 0, 1 ) * pos[1] + m_matrix( 0, 2 ) * pos[2] + m_matrix( 0, 3 ) * 1;
        resultPos4D[1] = m_matrix( 1, 0 ) * pos[0] + m_matrix( 1, 1 ) * pos[1] + m_matrix( 1, 2 ) * pos[2] + m_matrix( 1, 3 ) * 1;
        resultPos4D[2] = m_matrix( 2, 0 ) * pos[0] + m_matrix( 2, 1 ) * pos[1] + m_matrix( 2, 2 ) * pos[2] + m_matrix( 2, 3 ) * 1;
        resultPos4D[3] = m_matrix( 3, 0 ) * pos[0] + m_matrix( 3, 1 ) * pos[1] + m_matrix( 3, 2 ) * pos[2] + m_matrix( 3, 3 ) * 1;

        ( *mapIterator ).second.newID = nextID;
        triMesh->addVertex( resultPos4D[0] / resultPos4D[3], resultPos4D[1] / resultPos4D[3], resultPos4D[2] / resultPos4D[3] );
        triMesh->addTextureCoordinate( texCoord );
        nextID++;
        mapIterator++;
    }

    // Now rename triangles.
    WMCTriangleVECTOR::iterator vecIterator = m_trivecTriangles.begin();
    while( vecIterator != m_trivecTriangles.end() )
    {
        for( unsigned int i = 0; i < 3; i++ )
        {
            unsigned int newID = m_idToVertices[( *vecIterator ).pointID[i]].newID;
            ( *vecIterator ).pointID[i] = newID;
        }
        triMesh->addTriangle( ( *vecIterator ).pointID[0], ( *vecIterator ).pointID[1], ( *vecIterator ).pointID[2] );
        vecIterator++;
    }

    progress->finish();
    return triMesh;
}

template< typename T > WPointXYZId WMarchingCubesAlgorithm::calculateIntersection( const std::vector< T >* vals,
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

    x1 = v1x;
    y1 = v1y;
    z1 = v1z;
    x2 = v2x;
    y2 = v2y;
    z2 = v2z;

    unsigned int nPointsInSlice = ( m_nCellsX + 1 ) * ( m_nCellsY + 1 );
    double val1 = ( *vals )[ v1z * nPointsInSlice + v1y * ( m_nCellsX + 1 ) + v1x ];
    double val2 = ( *vals )[ v2z * nPointsInSlice + v2y * ( m_nCellsX + 1 ) + v2x ];

    WPointXYZId intersection = interpolate( x1, y1, z1, x2, y2, z2, val1, val2 );
    intersection.newID = 0;
    return intersection;
}

#endif  // WMARCHINGCUBESALGORITHM_H
