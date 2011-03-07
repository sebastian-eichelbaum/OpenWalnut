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

#ifndef WGRIDTRANSFORMORTHO_H
#define WGRIDTRANSFORMORTHO_H

#include "../common/math/WVector3D.h"
#include "../common/math/WMatrix.h"
#include "../common/math/WMatrix4x4.h"

#include "WExportDataHandler.h"

/**
 * Implements an orthogonal grid transformation.
 *
 * \class WGridTransformOrtho
 */
class OWDATAHANDLER_EXPORT WGridTransformOrtho
{
public:
    /**
     * Constructs an identity transform.
     */
    WGridTransformOrtho();

    /**
     * Construct a transformation that scales the grid space.
     * \param scaleX The scale in the x-direction.
     * \param scaleY The scale in the y-direction.
     * \param scaleZ The scale in the z-direction.
     */
    WGridTransformOrtho( double scaleX, double scaleY, double scaleZ );

    /**
     * Construct a transformation from a transformation matrix. The provided matrix
     * represents the transformation from grid to world space.
     * \param mat The matrix.
     */
    WGridTransformOrtho( WMatrix< double > const& mat );  // NOLINT

    /**
     * Destructor.
     */
    ~WGridTransformOrtho();

    /**
     * Transforms a position from grid space to world space.
     * \param position The position in grid space.
     * \return The same position in world space.
     */
    WVector3D positionToWorldSpace( WVector3D const& position ) const;

    /**
     * Transforms a position from world space to grid space.
     * \param position The position in world space.
     * \return The same position in grid space.
     */
    WVector3D positionToGridSpace( WVector3D const& position ) const;

    /**
     * Transforms a direction from grid space to world space.
     * \param direction The direction in grid space.
     * \return The same direction in world space.
     */
    WVector3D directionToWorldSpace( WVector3D const& direction ) const;

    /**
     * Transforms a direction from world space to grid space.
     * \param direction The position in world space.
     * \return The same position in grid space.
     */
    WVector3D directionToGridSpace( WVector3D const& direction ) const;

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
     * Adding this vector to a grid position in world coordinates yields the position of the next sample
     * along the grids (world coordinate) x-axis.
     */
    WVector3D getDirectionX() const;

    /**
     * Returns the vector determining the direction of samples in y direction.
     * Adding this vector to a grid position in world coordinates yields the position of the next sample
     * along the grids (world coordinate) y-axis.
     */
    WVector3D getDirectionY() const;

    /**
     * Returns the vector determining the direction of samples in z direction.
     * Adding this vector to a grid position in world coordinates yields the position of the next sample
     * along the grids (world coordinate) z-axis.
     */
    WVector3D getDirectionZ() const;

    /**
     * Returns the vector determining the unit (normalized) direction of samples in x direction.
     */
    WVector3D getUnitDirectionX() const;

    /**
     * Returns the vector determining the unit (normalized) direction of samples in y direction.
     */
    WVector3D getUnitDirectionY() const;

    /**
     * Returns the vector determining the unit (normalized) direction of samples in z direction.
     */
    WVector3D getUnitDirectionZ() const;

    /**
     * Returns the position of the origin of the grid.
     */
    WPosition getOrigin() const;

    /**
     * Returns a 4x4 matrix that represents the grid's transformaion.
     */
    // NOTE: this is temporary and should be removed as soon as all modules are
    // adapted to the grid transform object
    WMatrix< double > getTransformationMatrix() const;

    /**
     * Cast the transformation to the corresponding 4x4 matrix.
     *
     * \return the matrix representing the transform
     */
    operator WMatrix4x4() const;

    /**
     * Check if this transform does not include a rotation.
     *
     * \return True, if this transform only scales and translates.
     */
    bool isNotRotated() const;

    /**
     * Translate by a vector.
     *
     * \param vec The vector.
     */
    template< typename VecType >
    void translate( VecType const& vec );

    /**
     * Scale the transform.
     *
     * \param scale A vector of scaling coeffs for the 3 directions.
     */
    template< typename VecType >
    void scale( VecType const& scale );

private:
    //! normalized direction of the grid's x-axis in world coordinates
    WVector3D m_directionX;

    //! normalized direction of the grid's y-axis in world coordinates
    WVector3D m_directionY;

    //! normalized direction of the grid's z-axis in world coordinates
    WVector3D m_directionZ;

    //! the scaling factors for the 3 axes, i.e. the distance between samples
    WVector3D m_scaling;

    //! the origin of the grid in world coordinates
    WVector3D m_origin;
};

template< typename VecType >
void WGridTransformOrtho::translate( VecType const& vec )
{
    m_origin[ 0 ] += vec[ 0 ];
    m_origin[ 1 ] += vec[ 1 ];
    m_origin[ 2 ] += vec[ 2 ];
}

template< typename VecType >
void WGridTransformOrtho::scale( VecType const& scale )
{
    m_scaling[ 0 ] *= scale[ 0 ];
    m_scaling[ 1 ] *= scale[ 1 ];
    m_scaling[ 2 ] *= scale[ 2 ];
}

#endif  // WGRIDTRANSFORMORTHO_H
