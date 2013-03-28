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

#ifndef WLINEARALGEBRAFUNCTIONS_H
#define WLINEARALGEBRAFUNCTIONS_H

#include <Eigen/SVD>

#include "WMatrix.h"
#include "linearAlgebra/WLinearAlgebra.h"

template< typename > class WMatrix;

/**
 * helper routine to multiply a 3x3 matrix with a vector
 *
 * \param mat 3x3 matrix
 * \param vec vector
 */
WVector3d multMatrixWithVector3D( WMatrix<double> mat, WVector3d vec );

/**
 * Applies a coordinate transformation in homogenous coordinates to a vector.
 * This differs from transformPosition3DWithMatrix4D in that it DOES NOT incorporate the translation
 *
 * \param mat 4x4 matrix
 * \param vec vector
 */
WVector3d transformVector3DWithMatrix4D( WMatrix<double> mat, WVector3d vec );

/**
 * Applies a coordinate transformation in homogenous coordinates to a position.
 * This differs from transformVector3DWithMatrix4D in that it incorporates the translation.
 *
 * \param mat 4x4 matrix
 * \param vec vector
 */
WPosition transformPosition3DWithMatrix4D( WMatrix<double> mat, WPosition vec );

/**
 * helper routine to invert a 3x3 matrix
 *
 * \param mat 3x3 matrix
 *
 * \return inverted 3x3 matrix
 */
WMatrix<double> invertMatrix3x3( WMatrix<double> mat );

/**
 * helper routine to invert a 4x4 matrix
 *
 * \param mat 4x4 matrix
 *
 * \return inverted 4x4 matrix
 */
WMatrix<double> invertMatrix4x4( WMatrix<double> mat );

/**
 * Checks if the given two vectors are linearly independent.
 *
 * \param u First vector
 * \param v Second vector
 *
 * \return True if they are linear independent.
 *
 * \note This check is performed with the cross product != (0,0,0) but in numerical stability with FLT_EPS.
 */
bool linearIndependent( const WVector3d& u, const WVector3d& v );

/**
 * Computes the SVD for the Matrix \param A
 *
 * A=U*S*V^T
 *
 * \tparam T The data type.
 * \param A Input Matrix
 * \param U Output Matrix
 * \param S Output of the entries in the diagonal matrix
 * \param V Output Matrix
 *
 */
template< typename T >
void computeSVD( const WMatrix< T >& A, WMatrix< T >& U, WMatrix< T >& V, WValue< T >& S );

/**
 * Calculates for a matrix the pseudo inverse.
 *
 * \tparam T The data type.
 * \param input Matrix to invert
 *
 * \return Inverted Matrix
 *
 */
template< typename T >
WMatrix< T > pseudoInverse( const WMatrix< T >& input );


template< typename T >
void computeSVD( const WMatrix< T >& A,
                 WMatrix< T >& U,
                 WMatrix< T >& V,
                 WValue< T >& S )
{
    Eigen::Matrix< T, -1, -1 > eigenA( A );
    Eigen::JacobiSVD< Eigen::Matrix< T, -1, -1 > > svd( eigenA, Eigen::ComputeFullU | Eigen::ComputeFullV );
    U = WMatrix< T >( svd.matrixU() );
    V = WMatrix< T >( svd.matrixV() );
    S = WValue< T >( svd.singularValues() );
}

template< typename T >
WMatrix< T > pseudoInverse( const WMatrix< T >& input )
{
    // calc pseudo inverse
    WMatrix< T > U( input.getNbRows(), input.getNbCols() );
    WMatrix< T > V( input.getNbCols(), input.getNbCols() );
    WValue< T > Svec( input.size() );
    computeSVD( input, U, V, Svec );

    // create diagonal matrix S
    WMatrix< T > S( input.getNbCols(), input.getNbCols() );
    S.setZero();
    for( size_t i = 0; i < Svec.size() && i < S.getNbRows() && i < S.getNbCols(); i++ )
    {
        S( i, i ) = ( Svec[ i ] == 0.0 ) ? 0.0 : 1.0 / Svec[ i ];
    }

    return WMatrix< T >( V*S*U.transposed() );
}

#endif  // WLINEARALGEBRAFUNCTIONS_H
