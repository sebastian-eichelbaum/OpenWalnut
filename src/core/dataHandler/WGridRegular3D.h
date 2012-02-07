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

#include <utility>
#include <vector>

#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>

#include <osg/Matrix>
#include <osg/Vec3>

#include "../common/math/WMatrix.h"
#include "../common/math/linearAlgebra/WLinearAlgebra.h"
#include "../common/WBoundingBox.h"
#include "../common/WCondition.h"
#include "../common/WDefines.h"

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
class WGridRegular3D : public WGrid // NOLINT
{
    /**
     * Only test are allowed as friends.
     */
    friend class WGridRegular3DTest;
public:
    /**
     * Convenience typedef for a boost::shared_ptr< WGridRegular3D >.
     */
    typedef boost::shared_ptr< WGridRegular3D > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WGridRegular3D >.
     */
    typedef boost::shared_ptr< const WGridRegular3D > ConstSPtr;

    /**
     * Defines the number of samples in each coordinate direction as ints,
     * and the transformation of the grid via a grid transform.
     *
     * \param nbPosX number of positions along first axis
     * \param nbPosY number of positions along second axis
     * \param nbPosZ number of positions along third axis
     * \param transform a grid transformation
     */
    WGridRegular3D( unsigned int nbPosX, unsigned int nbPosY, unsigned int nbPosZ,
                    WGridTransformOrtho const transform = WGridTransformOrtho() );

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
    double getOffsetX() const;

    /**
     * Returns the distance between samples in y direction.
     * \return The distance between samples in y direction.
     */
    double getOffsetY() const;

    /**
     * Returns the distance between samples in z direction.
     * \return The distance between samples in z direction.
     */
    double getOffsetZ() const;

    /**
     * Returns the vector determining the direction of samples in x direction.
     * Adding this vector to a grid position in world coordinates yields the position of the next sample
     * along the grids (world coordinate) x-axis.
     * \return The vector determining the direction of samples in x direction.
     */
    WVector3d getDirectionX() const;

    /**
     * Returns the vector determining the direction of samples in y direction.
     * Adding this vector to a grid position in world coordinates yields the position of the next sample
     * along the grids (world coordinate) y-axis.
     * \return The vector determining the direction of samples in y direction.
     */
    WVector3d getDirectionY() const;

    /**
     * Returns the vector determining the direction of samples in z direction.
     * Adding this vector to a grid position in world coordinates yields the position of the next sample
     * along the grids (world coordinate) z-axis.
     * \return The vector determining the direction of samples in z direction.
     */
    WVector3d getDirectionZ() const;

    /**
     * Returns the vector determining the unit (normalized) direction of samples in x direction.
     * \return The vector determining the unit (normalized) direction of samples in x direction.
     */
    WVector3d getUnitDirectionX() const;

    /**
     * Returns the vector determining the unit (normalized) direction of samples in y direction.
     * \return The vector determining the unit (normalized) direction of samples in y direction.
     */
    WVector3d getUnitDirectionY() const;

    /**
     * Returns the vector determining the unit (normalized) direction of samples in z direction.
     * \return The vector determining the unit (normalized) direction of samples in z direction.
     */
    WVector3d getUnitDirectionZ() const;

    /**
     * Returns the position of the origin of the grid.
     * \return The position of the origin of the grid.
     */
    WPosition getOrigin() const;

    /**
     * Returns a 4x4 matrix that represents the grid's transformation.
     * \return The grid's transformation.
     */
    WMatrix< double > getTransformationMatrix() const;

    /**
     * \copybrief WGrid::getBoundingBox()
     * \return \copybrief WGrid::getBoundingBox()
     */
    WBoundingBox getBoundingBox() const;

    /**
     * Returns the i-th position on the grid.
     * \param i id of position to be obtained
     * \return i-th position of the grid.
     */
    WPosition getPosition( unsigned int i ) const;

    /**
     * Returns the position that is the iX-th in x direction, the iY-th in
     * y direction and the iZ-th in z direction.
     * \param iX id along first axis of position to be obtained
     * \param iY id along second axis of position to be obtained
     * \param iZ id along third axis of position to be obtained
     * \return Position (iX,iY,iZ)
     */
    WPosition getPosition( unsigned int iX, unsigned int iY, unsigned int iZ ) const;

    /**
     * Transforms world coordinates to texture coordinates.
     * \param point The point with these coordinates will be transformed.
     * \return point transformed into texture coordinate system
     */
    WVector3d worldCoordToTexCoord( WPosition point );

    /**
     * Transforms texture coordinates to world coordinates.
     * \param coords The point with these coordinates will be transformed.
     * \return coords transformed into world coordinate system.
     */
    WPosition texCoordToWorldCoord( WVector3d coords );

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
    int getVoxelNum( const WPosition& pos ) const;

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
    int getXVoxelCoord( const WPosition& pos ) const;

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
    int getYVoxelCoord( const WPosition& pos ) const;

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
    int getZVoxelCoord( const WPosition& pos ) const;

    /**
     * Computes the X coordinate of that voxel that contains the
     * position pos. Works on rotated grids.
     *
     * \param pos The position which selects the voxel for which the X
     * coordinate is computed.
     *
     * \return The X coordinate or -1 if pos refers to point outside of the
     * grid.
     */
    int getXVoxelCoordRotated( const WPosition& pos ) const;

    /**
     * Computes the Y coordinate of that voxel that contains the
     * position pos. Works on rotated grids.
     *
     * \param pos The position which selects the voxel for which the Y
     * coordinate is computed.
     *
     * \return The Y coordinate or -1 if pos refers to point outside of the
     * grid.
     */
    int getYVoxelCoordRotated( const WPosition& pos ) const;

    /**
     * Computes the Z coordinate of that voxel that contains the
     * position pos. Works on rotated grids.
     *
     * \param pos The position which selects the voxel for which the Z
     * coordinate is computed.
     *
     * \return The Z coordinate or -1 if pos refers to point outside of the
     * grid.
     */
    int getZVoxelCoordRotated( const WPosition& pos ) const;

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
    WValue< int > getVoxelCoord( const WPosition& pos ) const;

    /**
     * Computes the id of the cell containing the position pos. Note that the upper
     * bound of the grid does not belong to any cell
     *
     * \param pos The position selecting the cell.
     * \param success True if the position pos is inside the grid.
     *
     * \return id of the containing the position.
     */
    size_t getCellId( const WPosition& pos, bool* success ) const;

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
    std::vector< size_t > getCellVertexIds( const size_t cellId ) const;

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
    boost::shared_ptr< std::vector< WPosition > > getVoxelVertices( const WPosition& point,
                                                                           const double margin = 0.0 ) const;

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
    bool encloses( const WPosition& pos ) const;

    /**
     * Return whether the transformations of the grid are only translation and/or scaling
     * \return Transformation does not contain rotation?
     */
    bool isNotRotated() const;

    /**
     * Returns the transformation used by this grid.
     * \return The transformation.
     */
    WGridTransformOrtho const getTransform() const;

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
    int getNVoxelCoord( const WPosition& pos, size_t axis ) const;

    /**
     * Adds the specific information of this grid type to the
     * informational properties.
     */
    void initInformationProperties();

    unsigned int m_nbPosX; //!< Number of positions in x direction
    unsigned int m_nbPosY; //!< Number of positions in y direction
    unsigned int m_nbPosZ; //!< Number of positions in z direction

    //! The grid's transformation.
    WGridTransformOrtho const m_transform;
};

/**
 * Convinience function returning all offsets per axis.
 * 0 : xAxis, 1 : yAxis, 2 : zAxis
 * \param grid The grid having the information.
 * \note Implementing this as NonMemberNonFriend was intentional.
 * \return Array of number of samples per axis.
 */
inline boost::array< double, 3 > getOffsets( boost::shared_ptr< const WGridRegular3D > grid )
{
    boost::array< double, 3 > result = { { grid->getOffsetX(), grid->getOffsetY(), grid->getOffsetZ() } }; // NOLINT curly braces
    return result;
}

/**
 * Convinience function returning all number coords per axis.
 * 0 : xAxis, 1 : yAxis, 2 : zAxis
 * \param grid The grid having the information.
 * \note Implementing this as NonMemberNonFriend was intentional.
 * \return Array of number of samples per axis.
 */
inline boost::array< unsigned int, 3 > getNbCoords( boost::shared_ptr< const WGridRegular3D > grid )
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
inline boost::array< WVector3d, 3 > getDirections( boost::shared_ptr< const WGridRegular3D > grid )
{
    boost::array< WVector3d, 3 > result = { { grid->getDirectionX(), grid->getDirectionY(), grid->getDirectionZ() } }; // NOLINT curly braces
    return result;
}

/**
 * Convinience function returning all axis unit directions.
 * 0 : xAxis, 1 : yAxis, 2 : zAxis
 * \param grid The grid having the information.
 * \note Implementing this as NonMemberNonFriend was intentional.
 * \return The direction of each axis as array
 */
inline boost::array< WVector3d, 3 > getUnitDirections( boost::shared_ptr< const WGridRegular3D > grid )
{
    boost::array< WVector3d, 3 > result = { { grid->getUnitDirectionX(), grid->getUnitDirectionY(), grid->getUnitDirectionZ() } }; // NOLINT curly braces
    return result;
}

inline unsigned int WGridRegular3D::getNbCoordsX() const
{
    return m_nbPosX;
}

inline unsigned int WGridRegular3D::getNbCoordsY() const
{
    return m_nbPosY;
}

inline unsigned int WGridRegular3D::getNbCoordsZ() const
{
    return m_nbPosZ;
}

inline double WGridRegular3D::getOffsetX() const
{
    return m_transform.getOffsetX();
}

inline double WGridRegular3D::getOffsetY() const
{
    return m_transform.getOffsetY();
}

inline double WGridRegular3D::getOffsetZ() const
{
    return m_transform.getOffsetZ();
}

inline WVector3d WGridRegular3D::getUnitDirectionX() const
{
    return m_transform.getUnitDirectionX();
}

inline WVector3d WGridRegular3D::getUnitDirectionY() const
{
    return m_transform.getUnitDirectionY();
}

inline WVector3d WGridRegular3D::getUnitDirectionZ() const
{
    return m_transform.getUnitDirectionZ();
}

inline WVector3d WGridRegular3D::getDirectionX() const
{
    return m_transform.getDirectionX();
}

inline WVector3d WGridRegular3D::getDirectionY() const
{
    return m_transform.getDirectionY();
}

inline WVector3d WGridRegular3D::getDirectionZ() const
{
    return m_transform.getDirectionZ();
}

inline WPosition WGridRegular3D::getOrigin() const
{
    return m_transform.getOrigin();
}

inline WGridTransformOrtho const WGridRegular3D::getTransform() const
{
    return m_transform;
}

inline WMatrix< double > WGridRegular3D::getTransformationMatrix() const
{
    return m_transform.getTransformationMatrix();
}
#endif  // WGRIDREGULAR3D_H
