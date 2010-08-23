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

#include <vector>
#include <utility>

#include <boost/shared_ptr.hpp>

#include <osg/Vec3>

#include "../common/math/WMatrix.h"
#include "../common/math/WPosition.h"
#include "../common/math/WVector3D.h"
#include "WGrid.h"
#include "WExportDataHandler.h"

/**
 * A grid that has parallelepiped cells which all have the same proportion. I.e.
 * the samples along a single axis are equidistant. The distance of samples may
 * vary between axes.
 *
 * \warning Positions on the upper bounddaries in x, y and z are considered outside the grid.
 * \ingroup dataHandler
 */
class OWDATAHANDLER_EXPORT WGridRegular3D : public WGrid // NOLINT
{
    /**
     * Only test are allowed as friends.
     */
    friend class WGridRegular3DTest;
public:
    /**
     * Defines the position of the origin of the grid, the number of
     * samples in each coordinate direction and the offset between the
     * samples in the different coordinate directions as vector.
     * \param nbPosX number of positions along first axis
     * \param nbPosY number of positions along second axis
     * \param nbPosZ number of positions along third axis
     * \param originX x-coordinate of grid origin
     * \param originY y-coordinate of grid origin
     * \param originZ z-coordinate of grid origin
     * \param directionX direction of the first axis
     * \param directionY direction of the second axis
     * \param directionZ direction of the third axis
     * \param offsetX distance of samples along first axis
     * \param offsetY distance of samples along second axis
     * \param offsetZ distance of samples along third axis
     */
    WGridRegular3D(
                   unsigned int nbPosX, unsigned int nbPosY, unsigned int nbPosZ,
                   double originX, double originY, double originZ,
                   const wmath::WVector3D& directionX,
                   const wmath::WVector3D& directionY,
                   const wmath::WVector3D& directionZ,
                   double offsetX, double offsetY, double offsetZ );

    /**
     * Defines the number of samples in each coordinate direction as ints,
     * and the position of the origin of the grid and the offset between the
     * samples in the different coordinate directions as one 4x4 transformation
     * matrix using homogeneous coordinates (but only affine transformations are
     * allowed).
     * \param nbPosX number of positions along first axis
     * \param nbPosY number of positions along second axis
     * \param nbPosZ number of positions along third axis
     * \param mat 4x4 transformation matrix using homogeneous coordinates
     * \param offsetX distance of samples along first axis
     * \param offsetY distance of samples along second axis
     * \param offsetZ distance of samples along third axis
     */
    WGridRegular3D(
                   unsigned int nbPosX, unsigned int nbPosY, unsigned int nbPosZ,
                   const wmath::WMatrix< double >& mat,
                   double offsetX, double offsetY, double offsetZ );

    /**
     * Defines the number of samples in each coordinate direction as ints,
     * and the position of the origin of the grid and the offset between the
     * samples in the different coordinate directions as one 4x4 transformation
     * matrix using homogeneous coordinates (but only affine transformations are
     * allowed).
     * \param nbPosX number of positions along first axis
     * \param nbPosY number of positions along second axis
     * \param nbPosZ number of positions along third axis
     * \param qFormMat 4x4 transformation matrix using homogeneous coordinates
     * \param sFormMat 4x4 transformation matrix using homogeneous coordinates
     * \param offsetX distance of samples along first axis
     * \param offsetY distance of samples along second axis
     * \param offsetZ distance of samples along third axis
     */
    WGridRegular3D(
                   unsigned int nbPosX, unsigned int nbPosY, unsigned int nbPosZ,
                   const wmath::WMatrix< double >& qFormMat,
                   const wmath::WMatrix< double >& sFormMat,
                   double offsetX, double offsetY, double offsetZ );



    /**
     * Defines the position of the origin of the grid, the number of
     * samples in each coordinate direction and the offset between the
     * samples in the different coordinate directions as scalar.
     * \param nbPosX number of positions along first axis
     * \param nbPosY number of positions along second axis
     * \param nbPosZ number of positions along third axis
     * \param originX x-coordinate of grid origin
     * \param originY y-coordinate of grid origin
     * \param originZ z-coordinate of grid origin
     * \param offsetX distance of samples along first axis
     * \param offsetY distance of samples along second axis
     * \param offsetZ distance of samples along third axis
     */
    WGridRegular3D(
                   unsigned int nbPosX, unsigned int nbPosY, unsigned int nbPosZ,
                   double originX, double originY, double originZ,
                   double offsetX, double offsetY, double offsetZ );

    /**
     * Defines the position of the origin of the grid, the number of
     * samples in each coordinate direction and the offset between the
     * samples in the different coordinate directions as scalar.
     * \param nbPosX number of positions along first axis
     * \param nbPosY number of positions along second axis
     * \param nbPosZ number of positions along third axis
     * \param origin the point of origin of this grid
     * \param offsetX distance of samples along first axis
     * \param offsetY distance of samples along second axis
     * \param offsetZ distance of samples along third axis
     */
    WGridRegular3D(
                   unsigned int nbPosX, unsigned int nbPosY, unsigned int nbPosZ,
                   wmath::WPosition origin,
                   double offsetX, double offsetY, double offsetZ );

    /**
     * Convenience constructor that does the same as the one above but
     * uses the origin (0,0,0) as default.
     * \param nbPosX number of positions along first axis
     * \param nbPosY number of positions along second axis
     * \param nbPosZ number of positions along third axis
     * \param offsetX distance of samples along first axis
     * \param offsetY distance of samples along second axis
     * \param offsetZ distance of samples along third axis
     */
    WGridRegular3D(
                   unsigned int nbPosX, unsigned int nbPosY, unsigned int nbPosZ,
                   double offsetX, double offsetY, double offsetZ );

    /**
     * Returns the number of samples in x direction.
     */
    unsigned int getNbCoordsX() const;

    /**
     * Returns the number of samples in y direction.
     */
    unsigned int getNbCoordsY() const;

    /**
     * Returns the number of samples in z direction.
     */
    unsigned int getNbCoordsZ() const;

    /**
     * Returns the distance between samples in x direction.
     */
    double getOffsetX() const;

    /**
     * Returns the distance between samples in y direction.
     */
    double getOffsetY() const;

    /**
     * Returns the distance between samples in z direction.
     */
    double getOffsetZ() const;

    /**
     * Returns the vector determining the direction of samples in x direction.
     */
    const wmath::WVector3D& getDirectionX() const;

    /**
     * Returns the vector determining the direction of samples in y direction.
     */
    const wmath::WVector3D& getDirectionY() const;

    /**
     * Returns the vector determining the direction of samples in z direction.
     */
    const wmath::WVector3D& getDirectionZ() const;

    /**
     * Returns the position of the origin of the grid.
     */
    wmath::WPosition getOrigin() const;

    /**
     * Returns the two positions representing the bounding box of the grid.
     */
    std::pair< wmath::WPosition, wmath::WPosition > getBoundingBox() const;

    /**
     * Returns the i-th position on the grid.
     * \param i id of position to be obtained
     */
    wmath::WPosition getPosition( unsigned int i ) const;

    /**
     * Returns the position that is the iX-th in x direction, the iY-th in
     * y direction and the iZ-th in z direction.
     * \param iX id along first axis of position to be obtained
     * \param iY id along second axis of position to be obtained
     * \param iZ id along third axis of position to be obtained
     */
    wmath::WPosition getPosition( unsigned int iX, unsigned int iY, unsigned int iZ ) const;

    /**
     * Return the matrix storing the transformation of the grid. This information is redundant.
     * Please use m_origin and m_direction? for all normal computations.
     * Use matrix only where you really need a matrix for multiplication.
     */
    wmath::WMatrix<double> getTransformationMatrix() const;

    /**
     * Transforms world coordinates to texture coordinates.
     * \param point The point with these coordinated will be transformed.
     */
    wmath::WVector3D worldCoordToTexCoord( wmath::WPosition point );

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
    int getVoxelNum( const wmath::WPosition& pos ) const;

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
    int getXVoxelCoord( const wmath::WPosition& pos ) const;

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
    int getYVoxelCoord( const wmath::WPosition& pos ) const;

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
    int getZVoxelCoord( const wmath::WPosition& pos ) const;

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
    wmath::WValue< int > getVoxelCoord( const wmath::WPosition& pos ) const;

    /**
     * Computes the id of the cell containing the position pos. Note that the upper
     * bound of the grid does not belong to any cell
     *
     * \param pos The position selecting the cell.
     * \param success True if the position pos is inside the grid.
     */
    size_t getCellId( const wmath::WPosition& pos, bool* success ) const;

    /**
     * Computes the ids of the vertices of a cell given by its id.
     *
     * \param cellId The id of the cell we want to know ther vertices of.

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
    boost::shared_ptr< std::vector< wmath::WPosition > > getVoxelVertices( const wmath::WPosition& point,
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
    bool encloses( const wmath::WPosition& pos ) const;

    /**
     * Return whether the transformations of the grid are only translation and/or scaling
     */
    bool isNotRotatedOrSheared() const;

    /**
     * translates the texture along a given vector
     *
     * \param translation the translation vector
     */
    void translate( wmath::WPosition translation );

    /**
     * stretches the texture
     *
     * \param str the stretch factors in x,y,z direction
     */
    void stretch( wmath::WPosition str );

    /**
     * rotates the texture around the x,y,z axis
     *
     * \param rot the angles for each axis
     */
    void rotate( wmath::WPosition rot );

    /**
     * sets the active matrix
     *
     * \param matrix which matrix to use
     */
    void setActiveMatrix( int matrix );

    /**
     * gets the active matrix
     *
     * \return matrix in use
     */
    int getActiveMatrix();

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
    int getNVoxelCoord( const wmath::WPosition& pos, size_t axis ) const;

    /**
     * execute the texture transformation on the original transformation matrix with the stored
     * translate, stretch and rotate vectors
     */
    void doCustomTransformations();

    wmath::WPosition m_origin; //!< Origin of the grid.

    unsigned int m_nbPosX; //!< Number of positions in x direction
    unsigned int m_nbPosY; //!< Number of positions in y direction
    unsigned int m_nbPosZ; //!< Number of positions in z direction

    wmath::WVector3D m_directionX; //!< Direction of the x axis
    wmath::WVector3D m_directionY; //!< Direction of the y axis
    wmath::WVector3D m_directionZ; //!< Direction of the z axis

    double m_offsetX; //!< Offset between samples along x axis
    double m_offsetY; //!< Offset between samples along y axis
    double m_offsetZ; //!< Offset between samples along z axis

    double m_offsetXorig; //!< Offset between samples along x axis, stores the original value for manipulation
    double m_offsetYorig; //!< Offset between samples along y axis, stores the original value for manipulation
    double m_offsetZorig; //!< Offset between samples along z axis, stores the original value for manipulation

    /**
     * Matrix storing the transformation of the grid. This is redundant.
     * Please use m_origin and m_direction? for all normal computations.
     * Use matrix only where you really need a matrix for multiplication.
     *
     * This is the matrix we are working with
     */
    wmath::WMatrix<double> m_matrix;

    wmath::WMatrix<double> m_matrixInverse; //!< Inverse of m_matrix

    /**
     * Matrix storing the original stretch and translation
     */
    wmath::WMatrix<double> m_matrixNoMatrix;

    /**
     * Matrix storing the original qform matrix from the niftii file header
     */
    wmath::WMatrix<double> m_matrixQForm;

    /**
     * Matrix storing the original sform matrix from the niftii file header
     */
    wmath::WMatrix<double> m_matrixSForm;

    /**
     * indicates which transformation matrix is used
     *
     * 0 = no matrix, just stretch and translation
     * 1 = matrix 0, usually the qform matrix
     * 2 = matrix 1, usually the sform matrix
     */
    int m_matrixActive;

    wmath::WPosition m_translate; //!< stores the translation vector

    wmath::WPosition m_stretch; //!< stores the stretch vector

    wmath::WPosition m_rotation; //!< stores the rotation vector
};

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
    return m_directionX.norm();
}

inline double WGridRegular3D::getOffsetY() const
{
    return m_directionY.norm();
}

inline double WGridRegular3D::getOffsetZ() const
{
    return m_directionZ.norm();
}

inline const wmath::WVector3D& WGridRegular3D::getDirectionX() const
{
    return m_directionX;
}

inline const wmath::WVector3D& WGridRegular3D::getDirectionY() const
{
    return m_directionY;
}

inline const wmath::WVector3D& WGridRegular3D::getDirectionZ() const
{
    return m_directionZ;
}

inline wmath::WPosition WGridRegular3D::getOrigin() const
{
    return m_origin;
}

#endif  // WGRIDREGULAR3D_H
