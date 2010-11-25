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

#include "WGridTransform.h"

/**
 * Implements an orthogonal grid transformation.
 *
 * \class WGridTransformOrtho
 */
class WGridTransformOrtho : public WGridTransform
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
    WGridTransformOrtho( wmath::WMatrix< double > const& mat );  // NOLINT

    /**
     * Destructor.
     */
    virtual ~WGridTransformOrtho();

    /**
     * Transforms a position from grid space to world space.
     * \param position The position in grid space.
     * \return The same position in world space.
     */
    virtual wmath::WVector3D positionToWorldSpace( wmath::WVector3D const& position ) const;

    /**
     * Transforms a position from world space to grid space.
     * \param position The position in world space.
     * \return The same position in grid space.
     */
    virtual wmath::WVector3D positionToGridSpace( wmath::WVector3D const& position ) const;

    /**
     * Transforms a direction from grid space to world space.
     * \param direction The direction in grid space.
     * \return The same direction in world space.
     */
    virtual wmath::WVector3D directionToWorldSpace( wmath::WVector3D const& direction ) const;

    /**
     * Transforms a direction from world space to grid space.
     * \param direction The position in world space.
     * \return The same position in grid space.
     */
    virtual wmath::WVector3D directionToGridSpace( wmath::WVector3D const& direction ) const;

    /**
     * Returns the distance between samples in x direction.
     */
    virtual double getOffsetX() const;

    /**
     * Returns the distance between samples in y direction.
     */
    virtual double getOffsetY() const;

    /**
     * Returns the distance between samples in z direction.
     */
    virtual double getOffsetZ() const;

    /**
     * Returns the vector determining the direction of samples in x direction.
     * Adding this vector to a grid position in world coordinates yields the position of the next sample
     * along the grids (world coordinate) x-axis.
     */
    virtual wmath::WVector3D getDirectionX() const;

    /**
     * Returns the vector determining the direction of samples in y direction.
     * Adding this vector to a grid position in world coordinates yields the position of the next sample
     * along the grids (world coordinate) y-axis.
     */
    virtual wmath::WVector3D getDirectionY() const;

    /**
     * Returns the vector determining the direction of samples in z direction.
     * Adding this vector to a grid position in world coordinates yields the position of the next sample
     * along the grids (world coordinate) z-axis.
     */
    virtual wmath::WVector3D getDirectionZ() const;

    /**
     * Returns the vector determining the unit (normalized) direction of samples in x direction.
     */
    virtual wmath::WVector3D getUnitDirectionX() const;

    /**
     * Returns the vector determining the unit (normalized) direction of samples in y direction.
     */
    virtual wmath::WVector3D getUnitDirectionY() const;

    /**
     * Returns the vector determining the unit (normalized) direction of samples in z direction.
     */
    virtual wmath::WVector3D getUnitDirectionZ() const;

    /**
     * Returns the position of the origin of the grid.
     */
    virtual wmath::WPosition getOrigin() const;

    /**
     * Returns a 4x4 matrix that represents the grid's transformaion.
     */
    // NOTE: this is temporary and should be removed as soon as all modules are
    // adapted to the grid transform object
    virtual wmath::WMatrix< double > getTransformationMatrix() const;

private:
    //! normalized direction of the grid's x-axis in world coordinates
    wmath::WVector3D m_directionX;

    //! normalized direction of the grid's y-axis in world coordinates
    wmath::WVector3D m_directionY;

    //! normalized direction of the grid's z-axis in world coordinates
    wmath::WVector3D m_directionZ;

    //! the scaling factors for the 3 axes, i.e. the distance between samples
    wmath::WVector3D m_scaling;

    //! the origin of the grid in world coordinates
    wmath::WVector3D m_origin;
};

#endif  // WGRIDTRANSFORMORTHO_H
