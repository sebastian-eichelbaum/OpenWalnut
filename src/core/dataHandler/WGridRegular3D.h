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

#ifndef WGRIDREGULAR3D_H
#define WGRIDREGULAR3D_H

#include <cmath>
#include <string>
#include <utility>
#include <vector>

#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>

#include <osg/Matrix>
#include <osg/Vec3>

#include "../common/exceptions/WOutOfBounds.h"
#include "../common/exceptions/WPreconditionNotMet.h"
#include "../common/math/WLinearAlgebraFunctions.h"
#include "../common/math/WMatrix.h"
#include "../common/WBoundingBox.h"
#include "../common/WCondition.h"
#include "../common/WDefines.h"
#include "../common/WProperties.h"

#include "WGrid.h"
#include "WGridTransformOrtho.h"

/**
 * A grid that has parallelepiped cells which all have the same proportion. I.e.
 * the samples along a single axis are equidistant. The distance of samples may
 * vary between axes.
 *
 * \warning Positions on the upper bounddaries in x, y and z are considered outside the grid.
 * \ingroup dataHandler
 */
template< typename T >
class WGridRegular3DTemplate : public WGrid // NOLINT
{
    // this (friend) is necessary to allow casting
    template <class U>
    friend class WGridRegular3DTemplate;
    /**
     * Only test are allowed as friends.
     */
    friend class WGridRegular3DTest;
public:
    /**
     * Convenience typedef for 3d vectors of the appropriate numerical type.
     */
    typedef WMatrixFixed< T, 3, 1 > Vector3Type;

    /**
     * Convenience typedef for a boost::shared_ptr< WGridRegular3DTemplate >.
     */
    typedef boost::shared_ptr< WGridRegular3DTemplate > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WGridRegular3DTemplate >.
     */
    typedef boost::shared_ptr< const WGridRegular3DTemplate > ConstSPtr;

    /**
     * Convenience typedef for a boost::array< size_t, 8 >. Return type of getCellVertexIds.
     */
    typedef boost::array< size_t, 8 > CellVertexArray;

    /**
     * Copy constructor.
     * Copies the data from an WGridRegular3DTemplate object with arbitary numerical type.
     *
     * \param rhs A WGridRegular3DTemplate object, which mustn't have the same numerical type.
     */
    template< typename InputType >
    WGridRegular3DTemplate( WGridRegular3DTemplate< InputType > const& rhs ); // NOLINT -- no explicit, this allows casts

    /**
     * Defines the number of samples in each coordinate direction as ints,
     * and the transformation of the grid via a grid transform.
     *
     * \param nbPosX number of positions along first axis
     * \param nbPosY number of positions along second axis
     * \param nbPosZ number of positions along third axis
     * \param transform a grid transformation
     */
    WGridRegular3DTemplate( unsigned int nbPosX, unsigned int nbPosY, unsigned int nbPosZ,
                            WGridTransformOrthoTemplate< T > const transform = WGridTransformOrthoTemplate< T >() );

    /**
     * Defines the number of samples in each coordinate direction as ints,
     * and the transformation of the grid via a grid transform.
     *
     * \param nbPosX number of positions along first axis
     * \param nbPosY number of positions along second axis
     * \param nbPosZ number of positions along third axis
     * \param scaleX scaling of a voxel in x direction
     * \param scaleY scaling of a voxel in y direction
     * \param scaleZ scaling of a voxel in z direction
     */
    WGridRegular3DTemplate( unsigned int nbPosX, unsigned int nbPosY, unsigned int nbPosZ,
                            double scaleX, double scaleY, double scaleZ );

    /**
     * Returns the number of samples in x direction.
     * \return The number of samples in x direction.
     */
    unsigned int getNbCoordsX() const;

    /**
     * Returns the number of samples in y direction.
     * \return The number of samples in y direction.
     */
    unsigned int getNbCoordsY() const;

    /**
     * Returns the number of samples in z direction.
     * \return The number of samples in z direction.
     */
    unsigned int getNbCoordsZ() const;

    /**
     * Returns the distance between samples in x direction.
     * \return The distance between samples in x direction.
     */
    T getOffsetX() const;

    /**
     * Returns the distance between samples in y direction.
     * \return The distance between samples in y direction.
     */
    T getOffsetY() const;

    /**
     * Returns the distance between samples in z direction.
     * \return The distance between samples in z direction.
     */
    T getOffsetZ() const;

    /**
     * Returns the vector determining the direction of samples in x direction.
     * Adding this vector to a grid position in world coordinates yields the position of the next sample
     * along the grids (world coordinate) x-axis.
     * \return The vector determining the direction of samples in x direction.
     */
    Vector3Type getDirectionX() const;

    /**
     * Returns the vector determining the direction of samples in y direction.
     * Adding this vector to a grid position in world coordinates yields the position of the next sample
     * along the grids (world coordinate) y-axis.
     * \return The vector determining the direction of samples in y direction.
     */
    Vector3Type getDirectionY() const;

    /**
     * Returns the vector determining the direction of samples in z direction.
     * Adding this vector to a grid position in world coordinates yields the position of the next sample
     * along the grids (world coordinate) z-axis.
     * \return The vector determining the direction of samples in z direction.
     */
    Vector3Type getDirectionZ() const;

    /**
     * Returns the vector determining the unit (normalized) direction of samples in x direction.
     * \return The vector determining the unit (normalized) direction of samples in x direction.
     */
    Vector3Type getUnitDirectionX() const;

    /**
     * Returns the vector determining the unit (normalized) direction of samples in y direction.
     * \return The vector determining the unit (normalized) direction of samples in y direction.
     */
    Vector3Type getUnitDirectionY() const;

    /**
     * Returns the vector determining the unit (normalized) direction of samples in z direction.
     * \return The vector determining the unit (normalized) direction of samples in z direction.
     */
    Vector3Type getUnitDirectionZ() const;

    /**
     * Returns the position of the origin of the grid.
     * \return The position of the origin of the grid.
     */
    Vector3Type getOrigin() const;

    /**
     * Returns a 4x4 matrix that represents the grid's transformation.
     * \return The grid's transformation.
     */
    WMatrix< T > getTransformationMatrix() const;

    /**
     * \copybrief WGrid::getBoundingBox()
     * \return \copybrief WGrid::getBoundingBox()
     */
    WBoundingBox getBoundingBox() const;

    /**
     * Calculates the bounding box but includes the border voxel associated cell too.
     *
     * \return the bounding box
     */
    WBoundingBox getBoundingBoxIncludingBorder() const;

    /**
     * Calculate the bounding box in voxel space. In contrast to the cell bounding box, this includes the space of the last voxel in each
     * direction.
     *
     * \return the voxel space bounding box.
     */
    WBoundingBox getVoxelBoundingBox() const;

    /**
     * Returns the i-th position on the grid.
     * \param i id of position to be obtained
     * \return i-th position of the grid.
     */
    Vector3Type getPosition( unsigned int i ) const;

    /**
     * Returns the position that is the iX-th in x direction, the iY-th in
     * y direction and the iZ-th in z direction.
     * \param iX id along first axis of position to be obtained
     * \param iY id along second axis of position to be obtained
     * \param iZ id along third axis of position to be obtained
     * \return Position (iX,iY,iZ)
     */
    Vector3Type getPosition( unsigned int iX, unsigned int iY, unsigned int iZ ) const;

    /**
     * Transforms world coordinates to texture coordinates.
     * \param point The point with these coordinates will be transformed.
     * \return point transformed into texture coordinate system
     */
    Vector3Type worldCoordToTexCoord( Vector3Type point );

    /**
     * Returns the i'th voxel where the given position belongs too.
     *
     * A voxel is a cuboid which surrounds a point on the grid.
     *
     * \verbatim
      Voxel:
                     ______________ ____ (0.5, 0.5, 0.5)
                    /:            /|
                   / :           / |
                  /  :          /  |
                 /   :         /   |
               _/____:_ ___ __/    |
                |    :        |    |
                |    :    *<--|--------- grid point (0, 0, 0)
                |    :........|....|__
         dz == 1|   /         |   /
                |  /          |  / dy == 1
                | /           | /
               _|/____________|/__
                |<- dx == 1 ->|
         -0.5,-0.5,-0.5
       \endverbatim
     *
     * Please note the first voxel has only 1/8 of the size a normal voxel
     * would have since all positions outside the grid do not belong
     * to any voxel. Note: a cell is different to a voxel in terms of position.
     * A voxel has a grid point as center whereas a cell has grid points as
     * corners.
     * \param pos Position for which we want to have the voxel number.
     *
     * \return Voxel number or -1 if the position refers to a point outside of
     * the grid.
     */
    int getVoxelNum( const Vector3Type& pos ) const;

    /**
     * returns the voxel index for a given discrete position in the grid
     *
     * \param x Position for which we want to have the voxel number.
     * \param y Position for which we want to have the voxel number.
     * \param z Position for which we want to have the voxel number.
     *
     * \return Voxel number or -1 if the position refers to a point outside of
     * the grid.
     */
    int getVoxelNum( const size_t x, const size_t y, const size_t z ) const;

    /**
     * Computes the X coordinate of that voxel that contains the
     * position pos.
     *
     * \param pos The position which selects the voxel for which the X
     * coordinate is computed.
     *
     * \return The X coordinate or -1 if pos refers to point outside of the
     * grid.
     */
    int getXVoxelCoord( const Vector3Type& pos ) const;

    /**
     * Computes the Y coordinate of that voxel that contains the
     * position pos.
     *
     * \param pos The position which selects the voxel for which the Y
     * coordinate is computed.
     *
     * \return The Y coordinate or -1 if pos refers to point outside of the
     * grid.
     */
    int getYVoxelCoord( const Vector3Type& pos ) const;

    /**
     * Computes the Z coordinate of that voxel that contains the
     * position pos.
     *
     * \param pos The position which selects the voxel for which the Z
     * coordinate is computed.
     *
     * \return The Z coordinate or -1 if pos refers to point outside of the
     * grid.
     */
    int getZVoxelCoord( const Vector3Type& pos ) const;

    /**
     * Computes the voxel coordinates of that voxel which contains
     * the position pos.
     *
     * \param pos The position selecting the voxel.
     *
     * \return A vector of ints where the first component is the X voxel
     * coordinate, the second the Y component voxel coordinate and the last the
     * Z component of the voxel coordinate. If the selecting position is
     * outside of the grid then -1 -1 -1 is returned.
     */
    WVector3i getVoxelCoord( const Vector3Type& pos ) const;

    /**
     * Computes the id of the cell containing the position pos. Note that the upper
     * bound of the grid does not belong to any cell
     *
     * \param pos The position selecting the cell.
     * \param success True if the position pos is inside the grid.
     *
     * \return id of the containing the position.
     */
    size_t getCellId( const Vector3Type& pos, bool* success ) const;

    /**
     * Computes the ids of the vertices of a cell given by its id.
     *
     * \param cellId The id of the cell we want to know ther vertices of.
     *
     * \return Ids of vertices belonging to cell with given cellId.

     * \verbatim
        z-axis  y-axis
        |      /
        | 6___/_7
        |/:    /|
        4_:___5 |
        | :...|.|
        |.2   | 3
        |_____|/ ____x-axis
       0      1
       \endverbatim
     *
     */
    CellVertexArray getCellVertexIds( size_t cellId ) const;

    /**
     * Computes the vertices for a voxel cuboid around the given point:
     *
     * \verbatim
        z-axis  y-axis
        |      /
        | h___/_g
        |/:    /|
        d_:___c |
        | :...|.|
        |.e   | f
        |_____|/ ____x-axis
       a      b
       \endverbatim
     *
     * As you can see the order of the points is: a, b, c, d, e, f, g, h.
     *
     * \param point Center of the cuboid which must not necesarrily be a point
     * of the grid.
     * \param margin If you need to shrink the Voxel put here the delta > 0.
     *
     * \return Reference to a list of vertices which are the corner points of
     * the cube. Note this must not be a voxel, but has the same size of the an
     * voxel. If you need voxels at grid positions fill this function with
     * voxel center positions aka grid points.
     */
    boost::shared_ptr< std::vector< Vector3Type > > getVoxelVertices( const Vector3Type& point,
                                                                      const T margin = 0.0 ) const;

    /**
     * Return the list of neighbour voxels.
     *
     * \throw WOutOfBounds If the voxel id is outside of the grid.
     *
     * \param id Number of the voxel for which the neighbours should be computed
     *
     * \return Vector of voxel ids which are all neighboured
     */
    std::vector< size_t > getNeighbours( size_t id ) const;

    /**
     * Return the list of all neighbour voxels.
     *
     * \throw WOutOfBounds If the voxel id is outside of the grid.
     *
     * \param id Number of the voxel for which the neighbours should be computed
     *
     * \return Vector of voxel ids which are all neighboured
     */
    std::vector< size_t > getNeighbours27( size_t id ) const;

    /**
     * Return the list of all neighbour voxels.
     *
     * \throw WOutOfBounds If the voxel id is outside of the grid.
     *
     * \param id Number of the voxel for which the neighbours should be computed
     *
     * \param range neighborhood range selected. It specifies the distance to count as neighbour in each direction.
     *
     * \return Vector of voxel ids which are all neighboured
     */
    std::vector< size_t > getNeighboursRange( size_t id, size_t range ) const;

    /**
     * Return the list of all neighbour voxels.
     *
     * \throw WOutOfBounds If the voxel id is outside of the grid.
     *
     * \param id Number of the voxel for which the neighbours should be computed
     *
     * \return Vector of voxel ids which are all neighboured along the XY plane
     */
    std::vector< size_t > getNeighbours9XY( size_t id ) const;

    /**
     * Return the list of all neighbour voxels.
     *
     * \throw WOutOfBounds If the voxel id is outside of the grid.
     *
     * \param id Number of the voxel for which the neighbours should be computed
     *
     * \return Vector of voxel ids which are all neighboured along the YZ plane
     */
    std::vector< size_t > getNeighbours9YZ( size_t id ) const;

    /**
     * Return the list of all neighbour voxels.
     *
     * \throw WOutOfBounds If the voxel id is outside of the grid.
     *
     * \param id Number of the voxel for which the neighbours should be computed
     *
     * \return Vector of voxel ids which are all neighboured along the XZ plane
     */
    std::vector< size_t > getNeighbours9XZ( size_t id ) const;

    /**
     * Decides whether a certain position is inside this grid or not.
     *
     * \param pos Position to test
     *
     * \return True if and only if the given point is inside or on boundary of this grid, otherwise false.
     */
    bool encloses( const Vector3Type& pos ) const;

    /**
     * Return whether the transformations of the grid are only translation and/or scaling
     * \return Transformation does not contain rotation?
     */
    bool isNotRotated() const;

    /**
     * Returns the transformation used by this grid.
     * \return The transformation.
     */
    WGridTransformOrthoTemplate< T > const getTransform() const;

    /**
     * Compares two grids. Matches the transform and x,y,z resolution.
     *
     * \param other the one to compare against
     *
     * \return true if transform and resolution matches
     */
    bool operator==( const WGridRegular3DTemplate< T >& other ) const;

protected:
private:
    /**
     * Computes for the n'th component of the voxel coordinate where the voxel
     * contains the position pos.
     *
     * \param pos The position for which the n'th component of the voxel
     * coordinates should be computed.
     * \param axis The number of the component. (0 == x-axis, 1 == y-axis, ...)
     *
     * \return The n'th component of the voxel coordinate
     */
    int getNVoxelCoord( const Vector3Type& pos, size_t axis ) const;

    /**
     * Adds the specific information of this grid type to the
     * informational properties.
     */
    void initInformationProperties();

    unsigned int m_nbPosX; //!< Number of positions in x direction
    unsigned int m_nbPosY; //!< Number of positions in y direction
    unsigned int m_nbPosZ; //!< Number of positions in z direction

    //! The grid's transformation.
    WGridTransformOrthoTemplate< T > const m_transform;
};

// Convenience typedefs
typedef WGridRegular3DTemplate< double > WGridRegular3D;
typedef WGridRegular3DTemplate< double > WGridRegular3DDouble;
typedef WGridRegular3DTemplate< float > WGridRegular3DFloat;

template< typename T >
template< typename InputType >
WGridRegular3DTemplate< T >::WGridRegular3DTemplate( WGridRegular3DTemplate< InputType > const& rhs ) :
    WGrid( rhs.m_nbPosX * rhs.m_nbPosY * rhs.m_nbPosZ ),
    m_nbPosX( rhs.m_nbPosX ),
    m_nbPosY( rhs.m_nbPosY ),
    m_nbPosZ( rhs.m_nbPosZ ),
    m_transform( rhs.m_transform )
{
    initInformationProperties();
}

template< typename T >
WGridRegular3DTemplate< T >::WGridRegular3DTemplate( unsigned int nbPosX, unsigned int nbPosY, unsigned int nbPosZ,
                                     WGridTransformOrthoTemplate< T > const transform )
    : WGrid( nbPosX * nbPosY * nbPosZ ),
      m_nbPosX( nbPosX ),
      m_nbPosY( nbPosY ),
      m_nbPosZ( nbPosZ ),
      m_transform( transform )
{
    initInformationProperties();
}

template< typename T >
WGridRegular3DTemplate< T >::WGridRegular3DTemplate( unsigned int nbPosX, unsigned int nbPosY, unsigned int nbPosZ,
                                                     double scaleX, double scaleY, double scaleZ ):
    WGrid( nbPosX * nbPosY * nbPosZ ),
    m_nbPosX( nbPosX ),
    m_nbPosY( nbPosY ),
    m_nbPosZ( nbPosZ ),
    m_transform( WGridTransformOrthoTemplate< T >( scaleX, scaleY, scaleZ ) )
{
    initInformationProperties();
}

template< typename T >
inline unsigned int WGridRegular3DTemplate< T >::getNbCoordsX() const
{
    return m_nbPosX;
}

template< typename T >
inline unsigned int WGridRegular3DTemplate< T >::getNbCoordsY() const
{
    return m_nbPosY;
}

template< typename T >
inline unsigned int WGridRegular3DTemplate< T >::getNbCoordsZ() const
{
    return m_nbPosZ;
}

template< typename T >
inline T WGridRegular3DTemplate< T >::getOffsetX() const
{
    return m_transform.getOffsetX();
}

template< typename T >
inline T WGridRegular3DTemplate< T >::getOffsetY() const
{
    return m_transform.getOffsetY();
}

template< typename T >
inline T WGridRegular3DTemplate< T >::getOffsetZ() const
{
    return m_transform.getOffsetZ();
}

template< typename T >
inline typename WGridRegular3DTemplate< T >::Vector3Type WGridRegular3DTemplate< T >::getDirectionX() const
{
    return m_transform.getDirectionX();
}

template< typename T >
inline typename WGridRegular3DTemplate< T >::Vector3Type WGridRegular3DTemplate< T >::getDirectionY() const
{
    return m_transform.getDirectionY();
}

template< typename T >
inline typename WGridRegular3DTemplate< T >::Vector3Type WGridRegular3DTemplate< T >::getDirectionZ() const
{
    return m_transform.getDirectionZ();
}

template< typename T >
inline typename WGridRegular3DTemplate< T >::Vector3Type WGridRegular3DTemplate< T >::getUnitDirectionX() const
{
    return m_transform.getUnitDirectionX();
}

template< typename T >
inline typename WGridRegular3DTemplate< T >::Vector3Type WGridRegular3DTemplate< T >::getUnitDirectionY() const
{
    return m_transform.getUnitDirectionY();
}

template< typename T >
inline typename WGridRegular3DTemplate< T >::Vector3Type WGridRegular3DTemplate< T >::getUnitDirectionZ() const
{
    return m_transform.getUnitDirectionZ();
}

template< typename T >
inline typename WGridRegular3DTemplate< T >::Vector3Type WGridRegular3DTemplate< T >::getOrigin() const
{
    return m_transform.getOrigin();
}

template< typename T >
inline WMatrix< T > WGridRegular3DTemplate< T >::getTransformationMatrix() const
{
    return m_transform.getTransformationMatrix();
}

template< typename T >
inline WBoundingBox WGridRegular3DTemplate< T >::getBoundingBox() const
{
    WBoundingBox result;
    result.expandBy( m_transform.positionToWorldSpace( Vector3Type( 0.0,                0.0,                0.0            ) ) );
    result.expandBy( m_transform.positionToWorldSpace( Vector3Type( getNbCoordsX() - 1, 0.0,                0.0            ) ) );
    result.expandBy( m_transform.positionToWorldSpace( Vector3Type( 0.0,                getNbCoordsY() - 1, 0.0            ) ) );
    result.expandBy( m_transform.positionToWorldSpace( Vector3Type( getNbCoordsX() - 1, getNbCoordsY() - 1, 0.0            ) ) );
    result.expandBy( m_transform.positionToWorldSpace( Vector3Type( 0.0,                0.0,                getNbCoordsZ() - 1 ) ) );
    result.expandBy( m_transform.positionToWorldSpace( Vector3Type( getNbCoordsX() - 1, 0.0,                getNbCoordsZ() - 1 ) ) );
    result.expandBy( m_transform.positionToWorldSpace( Vector3Type( 0.0,                getNbCoordsY() - 1, getNbCoordsZ() - 1 ) ) );
    result.expandBy( m_transform.positionToWorldSpace( Vector3Type( getNbCoordsX() - 1, getNbCoordsY() - 1, getNbCoordsZ() - 1 ) ) );
    return result;
}

template< typename T >
inline WBoundingBox WGridRegular3DTemplate< T >::getBoundingBoxIncludingBorder() const
{
    WBoundingBox result;
    result.expandBy( m_transform.positionToWorldSpace( Vector3Type( 0.0,            0.0,            0.0            ) ) );
    result.expandBy( m_transform.positionToWorldSpace( Vector3Type( getNbCoordsX(), 0.0,            0.0            ) ) );
    result.expandBy( m_transform.positionToWorldSpace( Vector3Type( 0.0,            getNbCoordsY(), 0.0            ) ) );
    result.expandBy( m_transform.positionToWorldSpace( Vector3Type( getNbCoordsX(), getNbCoordsY(), 0.0            ) ) );
    result.expandBy( m_transform.positionToWorldSpace( Vector3Type( 0.0,            0.0,            getNbCoordsZ() ) ) );
    result.expandBy( m_transform.positionToWorldSpace( Vector3Type( getNbCoordsX(), 0.0,            getNbCoordsZ() ) ) );
    result.expandBy( m_transform.positionToWorldSpace( Vector3Type( 0.0,            getNbCoordsY(), getNbCoordsZ() ) ) );
    result.expandBy( m_transform.positionToWorldSpace( Vector3Type( getNbCoordsX(), getNbCoordsY(), getNbCoordsZ() ) ) );
    return result;
}

template< typename T >
inline WBoundingBox WGridRegular3DTemplate< T >::getVoxelBoundingBox() const
{
    WBoundingBox result;
    result.expandBy( m_transform.positionToWorldSpace( Vector3Type( -0.5,                 -0.5,                 -0.5            ) ) );
    result.expandBy( m_transform.positionToWorldSpace( Vector3Type( getNbCoordsX() - 0.5, -0.5,                 -0.5            ) ) );
    result.expandBy( m_transform.positionToWorldSpace( Vector3Type( -0.5,                 getNbCoordsY() - 0.5, -0.5            ) ) );
    result.expandBy( m_transform.positionToWorldSpace( Vector3Type( getNbCoordsX() - 0.5, getNbCoordsY() - 0.5, -0.5            ) ) );
    result.expandBy( m_transform.positionToWorldSpace( Vector3Type( -0.5,                 -0.5,                 getNbCoordsZ() - 0.5 ) ) );
    result.expandBy( m_transform.positionToWorldSpace( Vector3Type( getNbCoordsX() - 0.5, -0.5,                 getNbCoordsZ() - 0.5 ) ) );
    result.expandBy( m_transform.positionToWorldSpace( Vector3Type( -0.5,                 getNbCoordsY() - 0.5, getNbCoordsZ() - 0.5 ) ) );
    result.expandBy( m_transform.positionToWorldSpace( Vector3Type( getNbCoordsX() - 0.5, getNbCoordsY() - 0.5, getNbCoordsZ() - 0.5 ) ) );
    return result;
}

template< typename T >
inline typename WGridRegular3DTemplate< T >::Vector3Type WGridRegular3DTemplate< T >::getPosition( unsigned int i ) const
{
    return getPosition( i % m_nbPosX, ( i / m_nbPosX ) % m_nbPosY, i / ( m_nbPosX * m_nbPosY ) );
}

template< typename T >
inline typename WGridRegular3DTemplate< T >::Vector3Type WGridRegular3DTemplate< T >::getPosition( unsigned int iX,
                                                                                                   unsigned int iY,
                                                                                                   unsigned int iZ ) const
{
    Vector3Type i( iX, iY, iZ );
    return m_transform.positionToWorldSpace( i );
}

template< typename T >
inline typename WGridRegular3DTemplate< T >::Vector3Type WGridRegular3DTemplate< T >::worldCoordToTexCoord( WGridRegular3DTemplate< T >::Vector3Type point ) // NOLINT -- too long line
{
    Vector3Type r( m_transform.positionToGridSpace( point ) );

    // Scale to [0,1]
    r[0] = r[0] / m_nbPosX;
    r[1] = r[1] / m_nbPosY;
    r[2] = r[2] / m_nbPosZ;

    // Correct the coordinates to have the position at the center of the texture voxel.
    r[0] += 0.5 / m_nbPosX;
    r[1] += 0.5 / m_nbPosY;
    r[2] += 0.5 / m_nbPosZ;

    return r;
}

template< typename T >
inline int WGridRegular3DTemplate< T >::getVoxelNum( const Vector3Type& pos ) const
{
    // Note: the reason for the +1 is that the first and last Voxel in a x-axis
    // row are cut.
    //
    //  y-axis
    //  _|_______     ___ this is the 3rd Voxel
    // 1 |   |   |   v
    //   |...............
    //  _|_:_|_:_|_:_|_:____ x-axis
    //   | : | : | : | :
    //   |.:...:...:...:.
    //   0   1   2
    int xVoxelCoord = getXVoxelCoord( pos );
    int yVoxelCoord = getYVoxelCoord( pos );
    int zVoxelCoord = getZVoxelCoord( pos );
    if( xVoxelCoord == -1 || yVoxelCoord == -1 || zVoxelCoord == -1 )
    {
        return -1;
    }
    return xVoxelCoord
         + yVoxelCoord * ( m_nbPosX )
         + zVoxelCoord * ( m_nbPosX ) * ( m_nbPosY );
}

template< typename T >
inline int WGridRegular3DTemplate< T >::getVoxelNum( const size_t x, const size_t y, const size_t z ) const
{
    // since we use size_t here only a check for the upper bounds is needed
    if( x > m_nbPosX || y > m_nbPosY || z > m_nbPosZ )
    {
        return -1;
    }
    return x + y * ( m_nbPosX ) + z * ( m_nbPosX ) * ( m_nbPosY );
}

template< typename T >
inline int WGridRegular3DTemplate< T >::getXVoxelCoord( const WGridRegular3DTemplate< T >::Vector3Type& pos ) const
{
    // the current get*Voxel stuff is too complicated anyway
    Vector3Type v = m_transform.positionToGridSpace( pos );

    // this part could be refactored into an inline function
    T d;
    v[ 2 ] = std::modf( v[ 0 ] + T( 0.5 ), &d );
    int i = static_cast< int >( v[ 0 ] >= T( 0.0 ) && v[ 0 ] < m_nbPosX - T( 1.0 ) );
    return -1 + i * static_cast< int >( T( 1.0 ) + d );
}

template< typename T >
inline int WGridRegular3DTemplate< T >::getYVoxelCoord( const WGridRegular3DTemplate< T >::Vector3Type& pos ) const
{
    Vector3Type v = m_transform.positionToGridSpace( pos );

    T d;
    v[ 0 ] = std::modf( v[ 1 ] + T( 0.5 ), &d );
    int i = static_cast< int >( v[ 1 ] >= T( 0.0 ) && v[ 1 ] < m_nbPosY - T( 1.0 ) );
    return -1 + i * static_cast< int >( T( 1.0 ) + d );
}

template< typename T >
inline int WGridRegular3DTemplate< T >::getZVoxelCoord( const WGridRegular3DTemplate< T >::Vector3Type& pos ) const
{
    Vector3Type v = m_transform.positionToGridSpace( pos );

    T d;
    v[ 0 ] = std::modf( v[ 2 ] + T( 0.5 ), &d );
    int i = static_cast< int >( v[ 2 ] >= T( 0.0 ) && v[ 2 ] < m_nbPosZ - T( 1.0 ) );
    return -1 + i * static_cast< int >( T( 1.0 ) + d );
}

template< typename T >
inline WVector3i WGridRegular3DTemplate< T >::getVoxelCoord( const WGridRegular3DTemplate< T >::Vector3Type& pos ) const
{
    WVector3i result;
    result[0] = getXVoxelCoord( pos );
    result[1] = getYVoxelCoord( pos );
    result[2] = getZVoxelCoord( pos );
    return result;
}

template< typename T >
inline size_t WGridRegular3DTemplate< T >::getCellId( const WGridRegular3DTemplate< T >::Vector3Type& pos, bool* success ) const
{
    Vector3Type v = m_transform.positionToGridSpace( pos );

    T xCellId = floor( v[0] );
    T yCellId = floor( v[1] );
    T zCellId = floor( v[2] );

    *success = xCellId >= 0 && yCellId >=0 && zCellId >= 0 && xCellId < m_nbPosX - 1 && yCellId < m_nbPosY -1 && zCellId < m_nbPosZ -1;

    return xCellId + yCellId * ( m_nbPosX - 1 ) + zCellId * ( m_nbPosX - 1 ) * ( m_nbPosY - 1 );
}

template< typename T >
inline typename WGridRegular3DTemplate< T >::CellVertexArray WGridRegular3DTemplate< T >::getCellVertexIds( size_t cellId ) const
{
    typename WGridRegular3DTemplate< T >::CellVertexArray vertices;
    size_t minVertexIdZ =  cellId / ( ( m_nbPosX - 1 ) * ( m_nbPosY - 1 ) );
    size_t remainderXY = cellId - minVertexIdZ * ( ( m_nbPosX - 1 ) * ( m_nbPosY - 1 ) );
    size_t minVertexIdY = remainderXY  / ( m_nbPosX - 1 );
    size_t minVertexIdX = remainderXY % ( m_nbPosX - 1 );

    size_t minVertexId = minVertexIdX + minVertexIdY * m_nbPosX + minVertexIdZ * m_nbPosX * m_nbPosY;

    vertices[0] = minVertexId;
    vertices[1] = vertices[0] + 1;
    vertices[2] = minVertexId + m_nbPosX;
    vertices[3] = vertices[2] + 1;
    vertices[4] = minVertexId + m_nbPosX * m_nbPosY;
    vertices[5] = vertices[4] + 1;
    vertices[6] = vertices[4] + m_nbPosX;
    vertices[7] = vertices[6] + 1;
    return vertices;
}

template< typename T >
boost::shared_ptr< std::vector< typename WGridRegular3DTemplate< T >::Vector3Type > > WGridRegular3DTemplate< T >::getVoxelVertices( const WGridRegular3DTemplate< T >::Vector3Type& point, const T margin ) const // NOLINT -- too long line
{
    typedef boost::shared_ptr< std::vector< Vector3Type > > ReturnType;
    ReturnType result = ReturnType( new std::vector< Vector3Type > );
    result->reserve( 8 );
    T halfMarginX = getOffsetX() / 2.0 - std::abs( margin );
    T halfMarginY = getOffsetY() / 2.0 - std::abs( margin );
    T halfMarginZ = getOffsetZ() / 2.0 - std::abs( margin );
    result->push_back( Vector3Type( point[0] - halfMarginX, point[1] - halfMarginY, point[2] - halfMarginZ ) ); // a
    result->push_back( Vector3Type( point[0] + halfMarginX, point[1] - halfMarginY, point[2] - halfMarginZ ) ); // b
    result->push_back( Vector3Type( point[0] + halfMarginX, point[1] - halfMarginY, point[2] + halfMarginZ ) ); // c
    result->push_back( Vector3Type( point[0] - halfMarginX, point[1] - halfMarginY, point[2] + halfMarginZ ) ); // d
    result->push_back( Vector3Type( point[0] - halfMarginX, point[1] + halfMarginY, point[2] - halfMarginZ ) ); // e
    result->push_back( Vector3Type( point[0] + halfMarginX, point[1] + halfMarginY, point[2] - halfMarginZ ) ); // f
    result->push_back( Vector3Type( point[0] + halfMarginX, point[1] + halfMarginY, point[2] + halfMarginZ ) ); // g
    result->push_back( Vector3Type( point[0] - halfMarginX, point[1] + halfMarginY, point[2] + halfMarginZ ) ); // h
    return result;
}

template< typename T >
std::vector< size_t > WGridRegular3DTemplate< T >::getNeighbours( size_t id ) const
{
    std::vector< size_t > neighbours;
    size_t x = id % m_nbPosX;
    size_t y = ( id / m_nbPosX ) % m_nbPosY;
    size_t z = id / ( m_nbPosX * m_nbPosY );

    if( x >= m_nbPosX || y >= m_nbPosY || z >= m_nbPosZ )
    {
        std::stringstream ss;
        ss << "This point: " << id << " is not part of this grid: ";
        ss << " nbPosX: " << m_nbPosX;
        ss << " nbPosY: " << m_nbPosY;
        ss << " nbPosZ: " << m_nbPosZ;
        throw WOutOfBounds( ss.str() );
    }
    // for every neighbour we must check if its not on the boundary, it will be skipped otherwise
    if( x > 0 )
    {
        neighbours.push_back( id - 1 );
    }
    if( x < m_nbPosX - 1 )
    {
        neighbours.push_back( id + 1 );
    }
    if( y > 0 )
    {
        neighbours.push_back( id - m_nbPosX );
    }
    if( y < m_nbPosY - 1 )
    {
        neighbours.push_back( id + m_nbPosX );
    }
    if( z > 0 )
    {
        neighbours.push_back( id - ( m_nbPosX * m_nbPosY ) );
    }
    if( z < m_nbPosZ - 1 )
    {
         neighbours.push_back( id + ( m_nbPosX * m_nbPosY ) );
    }
    return neighbours;
}

template< typename T >
std::vector< size_t > WGridRegular3DTemplate< T >::getNeighbours27( size_t id ) const
{
    std::vector< size_t > neighbours;
    size_t x = id % m_nbPosX;
    size_t y = ( id / m_nbPosX ) % m_nbPosY;
    size_t z = id / ( m_nbPosX * m_nbPosY );

    if( x >= m_nbPosX || y >= m_nbPosY || z >= m_nbPosZ )
    {
        std::stringstream ss;
        ss << "This point: " << id << " is not part of this grid: ";
        ss << " nbPosX: " << m_nbPosX;
        ss << " nbPosY: " << m_nbPosY;
        ss << " nbPosZ: " << m_nbPosZ;
        throw WOutOfBounds( ss.str() );
    }
    // for every neighbour we must check if its not on the boundary, it will be skipped otherwise
    std::vector< int >tempResult;

    tempResult.push_back( getVoxelNum( x    , y    , z ) );
    tempResult.push_back( getVoxelNum( x    , y - 1, z ) );
    tempResult.push_back( getVoxelNum( x    , y + 1, z ) );
    tempResult.push_back( getVoxelNum( x - 1, y    , z ) );
    tempResult.push_back( getVoxelNum( x - 1, y - 1, z ) );
    tempResult.push_back( getVoxelNum( x - 1, y + 1, z ) );
    tempResult.push_back( getVoxelNum( x + 1, y    , z ) );
    tempResult.push_back( getVoxelNum( x + 1, y - 1, z ) );
    tempResult.push_back( getVoxelNum( x + 1, y + 1, z ) );

    tempResult.push_back( getVoxelNum( x    , y    , z - 1 ) );
    tempResult.push_back( getVoxelNum( x    , y - 1, z - 1 ) );
    tempResult.push_back( getVoxelNum( x    , y + 1, z - 1 ) );
    tempResult.push_back( getVoxelNum( x - 1, y    , z - 1 ) );
    tempResult.push_back( getVoxelNum( x - 1, y - 1, z - 1 ) );
    tempResult.push_back( getVoxelNum( x - 1, y + 1, z - 1 ) );
    tempResult.push_back( getVoxelNum( x + 1, y    , z - 1 ) );
    tempResult.push_back( getVoxelNum( x + 1, y - 1, z - 1 ) );
    tempResult.push_back( getVoxelNum( x + 1, y + 1, z - 1 ) );

    tempResult.push_back( getVoxelNum( x    , y    , z + 1 ) );
    tempResult.push_back( getVoxelNum( x    , y - 1, z + 1 ) );
    tempResult.push_back( getVoxelNum( x    , y + 1, z + 1 ) );
    tempResult.push_back( getVoxelNum( x - 1, y    , z + 1 ) );
    tempResult.push_back( getVoxelNum( x - 1, y - 1, z + 1 ) );
    tempResult.push_back( getVoxelNum( x - 1, y + 1, z + 1 ) );
    tempResult.push_back( getVoxelNum( x + 1, y    , z + 1 ) );
    tempResult.push_back( getVoxelNum( x + 1, y - 1, z + 1 ) );
    tempResult.push_back( getVoxelNum( x + 1, y + 1, z + 1 ) );

    for( size_t k = 0; k < tempResult.size(); ++k )
    {
        if( tempResult[k] != -1 )
        {
            neighbours.push_back( tempResult[k] );
        }
    }
    return neighbours;
}

template< typename T >
std::vector< size_t > WGridRegular3DTemplate< T >::getNeighboursRange( size_t id, size_t range ) const
{
    std::vector< size_t > neighbours;
    size_t x = id % m_nbPosX;
    size_t y = ( id / m_nbPosX ) % m_nbPosY;
    size_t z = id / ( m_nbPosX * m_nbPosY );

    if( x >= m_nbPosX || y >= m_nbPosY || z >= m_nbPosZ )
    {
        std::stringstream ss;
        ss << "This point: " << id << " is not part of this grid: ";
        ss << " nbPosX: " << m_nbPosX;
        ss << " nbPosY: " << m_nbPosY;
        ss << " nbPosZ: " << m_nbPosZ;
        throw WOutOfBounds( ss.str() );
    }
    // for every neighbour we must check if its not on the boundary, it will be skipped otherwise
    std::vector< int >tempResult;

    for( size_t xx = x - range; xx < x + range + 1; ++xx )
    {
        for( size_t yy = y - range; yy < y + range + 1; ++yy )
        {
            for( size_t zz = z - range; zz < z + range + 1; ++zz )
            {
                tempResult.push_back( getVoxelNum( xx, yy, zz ) );
            }
        }
    }

    for( size_t k = 0; k < tempResult.size(); ++k )
    {
        if( tempResult[k] != -1 )
        {
            neighbours.push_back( tempResult[k] );
        }
    }
    return neighbours;
}

template< typename T >
std::vector< size_t > WGridRegular3DTemplate< T >::getNeighbours9XY( size_t id ) const
{
    std::vector< size_t > neighbours;
    size_t x = id % m_nbPosX;
    size_t y = ( id / m_nbPosX ) % m_nbPosY;
    size_t z = id / ( m_nbPosX * m_nbPosY );

    if( x >= m_nbPosX || y >= m_nbPosY || z >= m_nbPosZ )
    {
        std::stringstream ss;
        ss << "This point: " << id << " is not part of this grid: ";
        ss << " nbPosX: " << m_nbPosX;
        ss << " nbPosY: " << m_nbPosY;
        ss << " nbPosZ: " << m_nbPosZ;
        throw WOutOfBounds( ss.str() );
    }
    // boundary check now happens in the getVoxelNum function
    int vNum;

    vNum = getVoxelNum( x - 1, y, z );
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x - 1, y - 1, z );
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x, y - 1, z );
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x + 1, y - 1, z );
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x + 1, y, z );
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x + 1, y + 1, z );
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x, y + 1, z );
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x - 1, y + 1, z );
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    return neighbours;
}

template< typename T >
std::vector< size_t > WGridRegular3DTemplate< T >::getNeighbours9YZ( size_t id ) const
{
    std::vector< size_t > neighbours;
    size_t x = id % m_nbPosX;
    size_t y = ( id / m_nbPosX ) % m_nbPosY;
    size_t z = id / ( m_nbPosX * m_nbPosY );

    if( x >= m_nbPosX || y >= m_nbPosY || z >= m_nbPosZ )
    {
        std::stringstream ss;
        ss << "This point: " << id << " is not part of this grid: ";
        ss << " nbPosX: " << m_nbPosX;
        ss << " nbPosY: " << m_nbPosY;
        ss << " nbPosZ: " << m_nbPosZ;
        throw WOutOfBounds( ss.str() );
    }
    // boundary check now happens in the getVoxelNum function
    int vNum;

    vNum = getVoxelNum( x, y, z - 1 );
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x, y - 1, z - 1 );
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x, y - 1, z );
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x, y - 1, z + 1 );
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x, y, z + 1 );
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x, y + 1, z + 1 );
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x, y + 1, z );
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x, y + 1, z - 1 );
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }

    return neighbours;
}

template< typename T >
std::vector< size_t > WGridRegular3DTemplate< T >::getNeighbours9XZ( size_t id ) const
{
    std::vector< size_t > neighbours;
    size_t x = id % m_nbPosX;
    size_t y = ( id / m_nbPosX ) % m_nbPosY;
    size_t z = id / ( m_nbPosX * m_nbPosY );

    if( x >= m_nbPosX || y >= m_nbPosY || z >= m_nbPosZ )
    {
        std::stringstream ss;
        ss << "This point: " << id << " is not part of this grid: ";
        ss << " nbPosX: " << m_nbPosX;
        ss << " nbPosY: " << m_nbPosY;
        ss << " nbPosZ: " << m_nbPosZ;
        throw WOutOfBounds( ss.str() );
    }
    // boundary check now happens in the getVoxelNum function
    int vNum;

    vNum = getVoxelNum( x, y, z - 1 );
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x - 1, y, z - 1 );
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x - 1, y, z );
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x - 1, y, z + 1 );
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x, y, z + 1 );
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x + 1, y, z + 1 );
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x + 1, y, z );
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x + 1, y, z - 1 );
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }

    return neighbours;
}

template< typename T >
inline bool WGridRegular3DTemplate< T >::encloses( WGridRegular3DTemplate< T >::Vector3Type const& pos ) const
{
    Vector3Type v = m_transform.positionToGridSpace( pos );

    if( v[ 0 ] < T( 0.0 ) || v[ 0 ] >= static_cast< T >( m_nbPosX - 1 ) )
    {
        return false;
    }
    if( v[ 1 ] < T( 0.0 ) || v[ 1 ] >= static_cast< T >( m_nbPosY - 1 ) )
    {
        return false;
    }
    if( v[ 2 ] < T( 0.0 ) || v[ 2 ] >= static_cast< T >( m_nbPosZ - 1 ) )
    {
        return false;
    }
    return true;
}

template< typename T >
inline bool WGridRegular3DTemplate< T >::isNotRotated() const
{
    return m_transform.isNotRotated();
}

template< typename T >
inline WGridTransformOrthoTemplate< T > const WGridRegular3DTemplate< T >::getTransform() const
{
    return m_transform;
}

template< typename T >
void WGridRegular3DTemplate< T >::initInformationProperties()
{
    WPropInt xDim = m_infoProperties->addProperty( "X dimension: ",
                                                   "The x dimension of the grid.",
                                                   static_cast<int>( getNbCoordsX() ) );
    WPropInt yDim = m_infoProperties->addProperty( "Y dimension: ",
                                                   "The y dimension of the grid.",
                                                   static_cast<int>( getNbCoordsY() ) );
    WPropInt zDim = m_infoProperties->addProperty( "Z dimension: ",
                                                   "The z dimension of the grid.",
                                                   static_cast<int>( getNbCoordsZ() ) );

    WPropDouble xOffset = m_infoProperties->addProperty( "X offset: ",
                                                         "The distance between samples in x direction",
                                                         static_cast< double >( getOffsetX() ) );
    WPropDouble yOffset = m_infoProperties->addProperty( "Y offset: ",
                                                         "The distance between samples in y direction",
                                                         static_cast< double >( getOffsetY() ) );
    WPropDouble zOffset = m_infoProperties->addProperty( "Z offset: ",
                                                         "The distance between samples in z direction",
                                                         static_cast< double >( getOffsetZ() ) );
}

template< typename T >
bool WGridRegular3DTemplate< T >::operator==( const WGridRegular3DTemplate< T >& other ) const
{
    return ( getNbCoordsX() == other.getNbCoordsX() ) &&
           ( getNbCoordsY() == other.getNbCoordsY() ) &&
           ( getNbCoordsZ() == other.getNbCoordsZ() ) &&
           ( m_transform == other.m_transform );
}

// +----------------------+
// | non-member functions |
// +----------------------+

/**
 * Convinience function returning all offsets per axis.
 * 0 : xAxis, 1 : yAxis, 2 : zAxis
 * \param grid The grid having the information.
 * \note Implementing this as NonMemberNonFriend was intentional.
 * \return Array of number of samples per axis.
 */
template< typename T >
inline boost::array< T, 3 > getOffsets( boost::shared_ptr< const WGridRegular3DTemplate< T > > grid )
{
    boost::array< T, 3 > result = { { grid->getOffsetX(), grid->getOffsetY(), grid->getOffsetZ() } }; // NOLINT curly braces
    return result;
}

/**
 * Convinience function returning all number coords per axis.
 * 0 : xAxis, 1 : yAxis, 2 : zAxis
 * \param grid The grid having the information.
 * \note Implementing this as NonMemberNonFriend was intentional.
 * \return Array of number of samples per axis.
 */
template< typename T >
inline boost::array< unsigned int, 3 > getNbCoords( boost::shared_ptr< const WGridRegular3DTemplate< T > > grid )
{
    boost::array< unsigned int, 3 > result = { { grid->getNbCoordsX(), grid->getNbCoordsY(), grid->getNbCoordsZ() } }; // NOLINT curly braces
    return result;
}

/**
 * Convinience function returning all axis directions.
 * 0 : xAxis, 1 : yAxis, 2 : zAxis
 * \param grid The grid having the information.
 * \note Implementing this as NonMemberNonFriend was intentional.
 * \return The direction of each axis as array
 */
template< typename T >
inline boost::array< typename WGridRegular3DTemplate< T >::Vector3Type, 3 > getDirections( boost::shared_ptr< const WGridRegular3DTemplate< T > > grid ) // NOLINT -- too long line
{
    boost::array< typename WGridRegular3DTemplate< T >::Vector3Type, 3 > result = { { grid->getDirectionX(), grid->getDirectionY(), grid->getDirectionZ() } }; // NOLINT curly braces
    return result;
}

/**
 * Convinience function returning all axis unit directions.
 * 0 : xAxis, 1 : yAxis, 2 : zAxis
 * \param grid The grid having the information.
 * \note Implementing this as NonMemberNonFriend was intentional.
 * \return The direction of each axis as array
 */
template< typename T >
inline boost::array< typename WGridRegular3DTemplate< T >::Vector3Type, 3 > getUnitDirections( boost::shared_ptr< const WGridRegular3DTemplate< T > > grid ) // NOLINT -- too long line
{
    boost::array< typename WGridRegular3DTemplate< T >::Vector3Type, 3 > result = { { grid->getUnitDirectionX(), grid->getUnitDirectionY(), grid->getUnitDirectionZ() } }; // NOLINT curly braces
    return result;
}

#endif  // WGRIDREGULAR3D_H
