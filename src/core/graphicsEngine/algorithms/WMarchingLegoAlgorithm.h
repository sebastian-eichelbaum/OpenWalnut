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

#ifndef WMARCHINGLEGOALGORITHM_H
#define WMARCHINGLEGOALGORITHM_H

#include <vector>
#include <map>

#include "../../common/math/WMatrix.h"
#include "../../common/WProgressCombiner.h"
#include "../WTriangleMesh.h"
#include "../WExportWGE.h"

/**
 * A point consisting of its coordinates and ID
 */
struct WMLPointXYZId
{
    unsigned int newID; //!< ID of the point
    double x; //!< x coordinates of the point.
    double y; //!< y coordinates of the point.
    double z; //!< z coordinates of the point.
};

typedef std::map< unsigned int, WMLPointXYZId > ID2WMLPointXYZId;

/**
 * Encapsulated ids representing a triangle.
 */
struct WMLTriangle
{
    unsigned int pointID[3]; //!< The IDs of the vertices of the triangle.
};

typedef std::vector<WMLTriangle> WMLTriangleVECTOR;


/**
 * Creates a non interpolated triangulation of an isosurface
 */
class WGE_EXPORT WMarchingLegoAlgorithm
{
public:
    /**
     * standard constructor
     */
    WMarchingLegoAlgorithm();

    /**
     * destructor
     */
    ~WMarchingLegoAlgorithm();

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
     * \param mainProgress Pointer to the parent's progress reporter. Leave empty if no progress should be shown
     *
     * \return the created triangle mesh
     */
    template< typename T >
    boost::shared_ptr< WTriangleMesh > generateSurface( size_t nbCoordsX, size_t nbCoordsY, size_t nbCoordsZ,
                                                        const WMatrix< double >& mat,
                                                        const std::vector< T >* vals,
                                                        double isoValue,
                                                        boost::shared_ptr<WProgressCombiner> mainProgress
                                                            = boost::shared_ptr < WProgressCombiner >() );

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
     *
     * \return the created triangle mesh
     */
    boost::shared_ptr< WTriangleMesh > genSurfaceOneValue( size_t nbCoordsX, size_t nbCoordsY, size_t nbCoordsZ,
                                                           const WMatrix< double >& mat,
                                                           const std::vector< size_t >* vals,
                                                           size_t isoValue,
                                                           boost::shared_ptr<WProgressCombiner> progress
                                                                = boost::shared_ptr < WProgressCombiner >() );

protected:
private:
    /**
     * adds 2 triangles for a given face of the voxel
     * \param x position of the voxel
     * \param y position of the voxel
     * \param z position of the voxel
     * \param surface which side of the voxel to paint
     */
    void addSurface( size_t x, size_t y, size_t z, size_t surface );

    /**
     * returns a vertex id for a given grid point
     * \param nX x position in space
     * \param nY y position in space
     * \param nZ z position in space
     * \return the id
     */
    size_t getVertexID( size_t nX, size_t nY, size_t nZ );

    unsigned int m_nCellsX;  //!< No. of cells in x direction.
    unsigned int m_nCellsY;  //!< No. of cells in y direction.
    unsigned int m_nCellsZ;  //!< No. of cells in z direction.

    double m_tIsoLevel;  //!< The isovalue.

    WMatrix< double > m_matrix; //!< The 4x4 transformation matrix for the triangle vertices.

    ID2WMLPointXYZId m_idToVertices;  //!< List of WPointXYZIds which form the isosurface.
    WMLTriangleVECTOR m_trivecTriangles;  //!< List of WMCTriangleS which form the triangulation of the isosurface.
};

template<typename T> boost::shared_ptr<WTriangleMesh>
WMarchingLegoAlgorithm::generateSurface( size_t nbCoordsX, size_t nbCoordsY, size_t nbCoordsZ,
                                         const WMatrix< double >& mat,
                                         const std::vector< T >* vals,
                                         double isoValue,
                                         boost::shared_ptr<WProgressCombiner> mainProgress )
{
    WAssert( vals, "No value set provided." );

    m_idToVertices.clear();
    m_trivecTriangles.clear();

    m_nCellsX = nbCoordsX - 1;
    m_nCellsY = nbCoordsY - 1;
    m_nCellsZ = nbCoordsZ - 1;

    m_matrix = mat;

    m_tIsoLevel = isoValue;

    size_t nX = nbCoordsX;
    size_t nY = nbCoordsY;

    size_t nPointsInSlice = nX * nY;

    boost::shared_ptr< WProgress > progress;
    if ( mainProgress )
    {
        progress = boost::shared_ptr< WProgress >( new WProgress( "Marching Cubes", m_nCellsZ ) );
        mainProgress->addSubProgress( progress );
    }

    // Generate isosurface.
    for( size_t z = 0; z < m_nCellsZ; z++ )
    {
        if ( progress )
        {
            ++*progress;
        }
        for( size_t y = 0; y < m_nCellsY; y++ )
        {
            for( size_t x = 0; x < m_nCellsX; x++ )
            {
                if( ( *vals )[ z * nPointsInSlice + y * nX + x ] < m_tIsoLevel )
                {
                    continue;
                }

                if( x > 0 && ( ( *vals )[ z * nPointsInSlice + y * nX + x - 1 ] < m_tIsoLevel ) )
                {
                    addSurface( x, y, z, 1 );
                }
                if( x < m_nCellsX - 1 && ( ( *vals )[ z * nPointsInSlice + y * nX + x + 1 ] < m_tIsoLevel ) )
                {
                    addSurface( x, y, z, 2 );
                }

                if( y > 0 && ( ( *vals )[ z * nPointsInSlice + ( y - 1 ) * nX + x ] < m_tIsoLevel ) )
                {
                    addSurface( x, y, z, 3 );
                }

                if( y < m_nCellsY - 1 && ( ( *vals )[ z * nPointsInSlice + ( y + 1 ) * nX + x ] < m_tIsoLevel ) )
                {
                    addSurface( x, y, z, 4 );
                }

                if( z > 0 && ( ( *vals )[ ( z - 1 ) * nPointsInSlice + y * nX + x ] < m_tIsoLevel ) )
                {
                    addSurface( x, y, z, 5 );
                }

                if( z < m_nCellsZ - 1 && ( ( *vals )[ ( z + 1 ) * nPointsInSlice + y * nX + x ] < m_tIsoLevel ) )
                {
                    addSurface( x, y, z, 6 );
                }

                if( x == 0 )
                {
                    addSurface( x, y, z, 1 );
                }
                if( x == m_nCellsX - 1 )
                {
                    addSurface( x, y, z, 2 );
                }

                if( y == 0 )
                {
                    addSurface( x, y, z, 3 );
                }

                if( y == m_nCellsY - 1 )
                {
                    addSurface( x, y, z, 4 );
                }

                if( z == 0 )
                {
                    addSurface( x, y, z, 5 );
                }

                if( z == m_nCellsZ - 1 )
                {
                    addSurface( x, y, z, 6 );
                }
            }
        }
    }
    unsigned int nextID = 0;
    boost::shared_ptr< WTriangleMesh > triMesh( new WTriangleMesh( m_idToVertices.size(), m_trivecTriangles.size() ) );

    // Rename vertices.
    ID2WMLPointXYZId::iterator mapIterator = m_idToVertices.begin();
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
        triMesh->addVertex( resultPos4D[0] / resultPos4D[3],
                            resultPos4D[1] / resultPos4D[3],
                            resultPos4D[2] / resultPos4D[3] );
        triMesh->addTextureCoordinate( texCoord );
        nextID++;
        mapIterator++;
    }

    // Now rename triangles.
    WMLTriangleVECTOR::iterator vecIterator = m_trivecTriangles.begin();
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
    if ( progress )
    {
        progress->finish();
    }
    return triMesh;
}
#endif  // WMARCHINGLEGOALGORITHM_H
