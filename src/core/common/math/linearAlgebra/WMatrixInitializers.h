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

#ifndef WMATRIXINITIALIZERS_H
#define WMATRIXINITIALIZERS_H

#include "WMatrixFixed.h"
#include "WVectorFixed.h"

// This file contains a lot of useful matrix initializers, especially useful for 4x4 matrix.

/**
 * Create a scaling matrix, scaling with a given factor along each axis.
 *
 * \tparam ValueT type of the scaling parameters
 * \param sx scaling in X direction
 * \param sy scaling in Y direction
 * \param sz scaling in Z direction
 *
 * \return created matrix.
 */
template< typename ValueT >
WMatrix4d makeScale( ValueT sx, ValueT sy, ValueT sz )
{
    WMatrix4d m;
    m( 0, 0 ) = sx;
    m( 1, 1 ) = sy;
    m( 2, 2 ) = sz;
    m( 3, 3 ) = 1.0;
    return m;
}

/**
 * Create a scaling matrix, scaling with a given factor along each axis.
 *
 * \tparam VectorT the vector types used
 * \param vec vector whose elements describe the scaling.
 *
 * \return the created matrix.
 */
template< typename VectorT >
WMatrix4d makeScale( const VectorT& vec )
{
    return makeScale( vec[0], vec[1], vec[2] );
}

/**
 * Create a translation matrix, translating with a given factor along each axis.
 *
 * \tparam ValueT type of the translation parameters
 * \param tx translation in X direction
 * \param ty translation in Y direction
 * \param tz translation in Z direction
 *
 * \return created matrix.
 */
template< typename ValueT >
WMatrix4d makeTranslate( ValueT tx, ValueT ty, ValueT tz )
{
    WMatrix4d m;
    m( 0, 3 ) = tx;
    m( 1, 3 ) = ty;
    m( 2, 3 ) = tz;
    m( 3, 3 ) = 1.0;
    return m;
}

/**
 * Create a translation matrix, translating with a given factor along each axis.
 *
 * \tparam VectorT the vector types used
 * \param vec vector whose elements describe the scaling.
 *
 * \return the created matrix.
 */
template< typename VectorT >
WMatrix4d makeTranslate( const VectorT& vec )
{
    return makeTranslate( vec[0], vec[1], vec[2] );
}

/**
 * Creates a rotation matrix.
 *
 * \tparam ValueT type of the parameters
 * \param angle the angle to rotate in degree
 * \param x rotation in x direction
 * \param y rotation in y direction
 * \param z rotation in z direction
 *
 * \return created matrix.
 */
template< typename ValueT >
WMatrix4d makeRotate( ValueT angle, ValueT x, ValueT y, ValueT z )
{
    // This can be read in the OpenGL Red Book -- Appendix Homogeneous Coordinates and Transformation Matrices.

    // First: create some vectors and matrices we need.
    // Normalize axis
    WVector3d u( normalize( WVector3d( x, y, z ) ) );
    // The row vector of the axis
    WVectorRow3d uT( transpose( u ) );
    WMatrix3d uuT = u * uT;
    WMatrix3d s;
    s( 0, 0 ) = 0.0;   s( 0, 1 ) = -u[2]; s( 0, 2 ) = u[1];     // NOLINT - multiple commands on one line
    s( 1, 0 ) = u[2];  s( 1, 1 ) = 0.0;   s( 1, 2 ) = -u[0];    // NOLINT - multiple commands on one line
    s( 2, 0 ) = -u[1]; s( 2, 1 ) = u[0];  s( 2, 2 ) = 0.0;      // NOLINT - multiple commands on one line

    // Now we can formulate the rotation matrix:
    return WMatrix4d::fromMatrices(
        WMatrix4d::identity(),
        uuT + cos( angle ) * ( WMatrix3d::identity() - uuT ) + sin( angle ) * s
    );
}

/**
 * Creates a rotation matrix using the specified vector, which describes the axe
 *
 * \tparam ValueT type of the parameters
 * \param angle the angle to rotate in degree
 * \param vec rotation axe
 *
 * \return created matrix.
 */
template< typename VectorT >
WMatrix4d makeRotate( const VectorT& vec )
{
    return makeRotate( vec[0], vec[1], vec[2] );
}

#endif  // WMATRIXINITIALIZERS_H

