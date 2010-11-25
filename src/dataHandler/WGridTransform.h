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

#ifndef WGRIDTRANSFORM_H
#define WGRIDTRANSFORM_H

#include "../common/math/WVector3D.h"
#include "../common/math/WMatrix.h"

/**
 * A class that represents a transformation from grid space to world space.
 * Offers several functions for transforming points and vectors between grid and
 * world spaces. 
 */
class WGridTransform
{
public:
    /**
     * Destructor.
     */
    virtual ~WGridTransform();

    /**
     * Transforms a position from grid space to world space.
     * \param position The position in grid space.
     * \return The same position in world space.
     */
    virtual wmath::WVector3D positionToWorldSpace( wmath::WVector3D const& position ) const = 0;

    /**
     * Transforms a position from world space to grid space.
     * \param position The position in world space.
     * \return The same position in grid space.
     */
    virtual wmath::WVector3D positionToGridSpace( wmath::WVector3D const& position ) const = 0;

    /**
     * Transforms a direction from grid space to world space.
     * \param position The direction in grid space.
     * \return The same direction in world space.
     */
    virtual wmath::WVector3D directionToWorldSpace( wmath::WVector3D const& direction ) const = 0;

    /**
     * Transforms a position from world space to grid space.
     * \param position The position in world space.
     * \return The same position in grid space.
     */
    virtual wmath::WVector3D directionToGridSpace( wmath::WVector3D const& direction ) const = 0;

    /**
     * Returns the distance between samples in x direction.
     */
    virtual double getOffsetX() const = 0;

    /**
     * Returns the distance between samples in y direction.
     */
    virtual double getOffsetY() const = 0;

    /**
     * Returns the distance between samples in z direction.
     */
    virtual double getOffsetZ() const = 0;

    /**
     * Returns the vector determining the direction of samples in x direction.
     * Adding this vector to a grid position in world coordinates yields the position of the next sample
     * along the grids (world coordinate) x-axis.
     */
    virtual wmath::WVector3D getDirectionX() const = 0;

    /**
     * Returns the vector determining the direction of samples in y direction.
     * Adding this vector to a grid position in world coordinates yields the position of the next sample
     * along the grids (world coordinate) y-axis.
     */
    virtual wmath::WVector3D getDirectionY() const = 0;

    /**
     * Returns the vector determining the direction of samples in z direction.
     * Adding this vector to a grid position in world coordinates yields the position of the next sample
     * along the grids (world coordinate) z-axis.
     */
    virtual wmath::WVector3D getDirectionZ() const = 0;

    /**
     * Returns the vector determining the unit (normalized) direction of samples in x direction.
     */
    virtual wmath::WVector3D getUnitDirectionX() const = 0;

    /**
     * Returns the vector determining the unit (normalized) direction of samples in y direction.
     */
    virtual wmath::WVector3D getUnitDirectionY() const = 0;

    /**
     * Returns the vector determining the unit (normalized) direction of samples in z direction.
     */
    virtual wmath::WVector3D getUnitDirectionZ() const = 0;

    /**
     * Returns the position of the origin of the grid.
     */
    virtual wmath::WPosition getOrigin() const = 0;

    /**
     * Returns a 4x4 matrix that represents the grid's transformaion.
     */
    // NOTE: this is temporary and should be removed as soon as all modules are
    // adapted to the grid transform object
    virtual wmath::WMatrix< double > getTransformationMatrix() const = 0;
};

#endif  // WGRIDTRANSFORM_H
