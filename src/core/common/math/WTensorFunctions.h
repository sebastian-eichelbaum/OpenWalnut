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

#ifndef WTENSORFUNCTIONS_H
#define WTENSORFUNCTIONS_H

#include <algorithm>
#include <cmath>
#include <complex>
#include <iostream>
#include <utility>
#include <vector>

#include <boost/array.hpp>

#include <Eigen/Dense>

#include "../WAssert.h"
#include "../WLimits.h"
#include "WCompileTimeFunctions.h"
#include "WTensor.h"
#include "WTensorSym.h"
#include "WMatrix.h"

/**
 * An eigensystem has all eigenvalues as well as its corresponding eigenvectors. A RealEigenSystem is an EigenSystem where all
 * eigenvalues are real and not complex.
 */
typedef boost::array< std::pair< double, WVector3d >, 3 > RealEigenSystem;

/**
 * An eigensystem has all eigenvalues as well its corresponding eigenvectors.
 */
typedef boost::array< std::pair< std::complex< double >, WVector3d >, 3 > EigenSystem;

std::ostream& operator<<( std::ostream& os, const RealEigenSystem& sys );

/**
 * Helper function to sort eigen values on their size. As each eigenvalues has an eigenvector they must be twiddled the same.
 *
 * \param es Eigensystem consisting of eigenvalues and eigenvectors.
 */
inline void sortRealEigenSystem( RealEigenSystem* es )
{
    if( ( *es )[0].first > ( *es )[2].first )
    {
        std::swap( ( *es )[0], ( *es )[2] );
    }
    if( ( *es )[0].first > ( *es )[1].first )
    {
        std::swap( ( *es )[0], ( *es )[1] );
    }
    if( ( *es )[1].first > ( *es )[2].first )
    {
        std::swap( ( *es )[1], ( *es )[2] );
    }
}

/**
 * Compute all eigenvalues as well as the corresponding eigenvectors of a
 * symmetric real Matrix.
 *
 * \note Data_T must be castable to double.
 *
 * \param[in] mat A real symmetric matrix.
 * \param[out] RealEigenSystem A pointer to an RealEigenSystem.
 */
template< typename Data_T >
void jacobiEigenvector3D( WTensorSym< 2, 3, Data_T > const& mat, RealEigenSystem* es )
{
    RealEigenSystem& result = *es; // alias for the result
    WTensorSym< 2, 3, Data_T > in( mat );
    WTensor< 2, 3, Data_T > ev;
    ev( 0, 0 ) = ev( 1, 1 ) = ev( 2, 2 ) = 1.0;

    int iter = 50;
    Data_T evp[ 3 ];
    Data_T evq[ 3 ];

    while( iter >= 0 )
    {
        int p = 1;
        int q = 0;

        for( int i = 0; i < 2; ++i )
        {
            if( fabs( in( 2, i ) ) > fabs( in( p, q ) ) )
            {
                p = 2;
                q = i;
            }
        }

        // Note: If all non diagonal elements sum up to nearly zero, we may quit already!
        // Thereby the chosen threshold 1.0e-50 was taken arbitrarily and is just a guess.
        if( std::abs( in( 0, 1 ) ) + std::abs( in( 0, 2 ) ) + std::abs( in( 1, 2 ) ) < 1.0e-50 )
        {
            for( int i = 0; i < 3; ++i )
            {
                result[i].first = in( i, i );
                for( int j = 0; j < 3; ++j )
                {
                    result[i].second[j] = static_cast< double >( ev( j, i ) );
                }
            }
            sortRealEigenSystem( es );
            return;
        }

        Data_T r = in( q, q ) - in( p, p );
        Data_T o = r / ( 2.0 * in( p, q ) );

        Data_T t;
        Data_T signofo = ( o < 0.0 ? -1.0 : 1.0 );
        if( o * o > wlimits::MAX_DOUBLE )
        {
            t = signofo / ( 2.0 * fabs( o ) );
        }
        else
        {
            t = signofo / ( fabs( o ) + sqrt( o * o + 1.0 ) );
        }

        Data_T c;

        if( t * t < wlimits::DBL_EPS )
        {
            c = 1.0;
        }
        else
        {
            c = 1.0 / sqrt( t * t + 1.0 );
        }

        Data_T s = c * t;

        int k = 0;
        while( k == q || k == p )
        {
            ++k;
        }

        Data_T u = ( 1.0 - c ) / s;

        Data_T x = in( k, p );
        Data_T y = in( k, q );
        in( p, k ) = in( k, p ) = x - s * ( y + u * x );
        in( q, k ) = in( k, q ) = y + s * ( x - u * y );
        x = in( p, p );
        y = in( q, q );
        in( p, p ) = x - t * in( p, q );
        in( q, q ) = y + t * in( p, q );
        in( q, p ) = in( p, q ) = 0.0;

        for( int l = 0; l < 3; ++l )
        {
            evp[ l ] = ev( l, p );
            evq[ l ] = ev( l, q );
        }
        for( int l = 0; l < 3; ++l )
        {
            ev( l, p ) = c * evp[ l ] - s * evq[ l ];
            ev( l, q ) = s * evp[ l ] + c * evq[ l ];
        }

        --iter;
    }
    WAssert( iter >= 0, "Jacobi eigenvector iteration did not converge." );
}

/**
 * Compute all eigenvalues as well as the corresponding eigenvectors of a
 * symmetric real Matrix.
 *
 * \note Data_T must be castable to double.
 *
 * \param[in] mat A real symmetric matrix.
 * \param[out] RealEigenSystem A pointer to an RealEigenSystem.
 */
template< typename Data_T >
void eigenEigenvector3D( WTensorSym< 2, 3, Data_T > const& mat, RealEigenSystem* es )
{
    RealEigenSystem& result = *es; // alias for the result
    typedef Eigen::Matrix< Data_T, 3, 3 > MatrixType;

    MatrixType m;
    m << mat( 0, 0 ), mat( 0, 1 ), mat( 0, 2 ),
         mat( 1, 0 ), mat( 1, 1 ), mat( 1, 2 ),
         mat( 2, 0 ), mat( 2, 1 ), mat( 2, 2 );

    Eigen::SelfAdjointEigenSolver< MatrixType > solv( m );

    for( std::size_t k = 0; k < 3; ++k )
    {
        result[ k ].first = solv.eigenvalues()[ k ];
        for( std::size_t j = 0; j < 3; ++j )
        {
            result[ k ].second[ j ] = solv.eigenvectors()( j, k );
        }
    }
}

/**
 * Calculate eigenvectors via the characteristic polynomial. This is essentially the same
 * function as in the GPU glyph shaders. This is for 3 dimensions only.
 *
 * \param m The symmetric matrix to calculate the eigenvalues from.
 * \return A std::vector of 3 eigenvalues in descending order (of their magnitude).
 */
std::vector< double > getEigenvaluesCardano( WTensorSym< 2, 3 > const& m );

/**
 * Multiply tensors of order 2. This is essentially a matrix-matrix multiplication.
 *
 * Tensors must have the same data type and dimension, however both symmetric and asymmetric
 * tensors (in any combination) are allowed as operands. The returned tensor is always an asymmetric tensor.
 *
 * \param one The first operand, a WTensor or WTensorSym of order 2.
 * \param other The second operand, a WTensor or WTensorSym of order 2.
 *
 * \return A WTensor that is the product of the operands.
 */
template< template< std::size_t, std::size_t, typename > class TensorType1, // NOLINT brainlint can't find TensorType1
          template< std::size_t, std::size_t, typename > class TensorType2, // NOLINT
          std::size_t dim,
          typename Data_T >
WTensor< 2, dim, Data_T > operator * ( TensorType1< 2, dim, Data_T > const& one,
                                       TensorType2< 2, dim, Data_T > const& other )
{
    WTensor< 2, dim, Data_T > res;

    // calc res_ij = one_ik * other_kj
    for( std::size_t i = 0; i < dim; ++i )
    {
        for( std::size_t j = 0; j < dim; ++j )
        {
            // components are initialized to zero, so there is no need to zero them here
            for( std::size_t k = 0; k < dim; ++k )
            {
                res( i, j ) += one( i, k ) * other( k, j );
            }
        }
    }

    return res;
}

/**
 * Evaluate a spherical function represented by a symmetric 4th-order tensor for a given gradient.
 *
 * \tparam Data_T The integral type used to store the tensor elements.
 *
 * \param tens The tensor representing the spherical function.
 * \param gradient The normalized vector that represents the gradient direction.
 *
 * \note If the gradient is not normalized, the result is undefined.
 */
template< typename Data_T >
double evaluateSphericalFunction( WTensorSym< 4, 3, Data_T > const& tens, WVector3d const& gradient )
{
    // use symmetry to reduce computation overhead
    // temporaries for some of the gradient element multiplications could further reduce
    // computation time
    return gradient[ 0 ] * gradient[ 0 ] * gradient[ 0 ] * gradient[ 0 ] * tens( 0, 0, 0, 0 )
         + gradient[ 1 ] * gradient[ 1 ] * gradient[ 1 ] * gradient[ 1 ] * tens( 1, 1, 1, 1 )
         + gradient[ 2 ] * gradient[ 2 ] * gradient[ 2 ] * gradient[ 2 ] * tens( 2, 2, 2, 2 )
         + static_cast< Data_T >( 4 ) *
         ( gradient[ 0 ] * gradient[ 0 ] * gradient[ 0 ] * gradient[ 1 ] * tens( 0, 0, 0, 1 )
         + gradient[ 0 ] * gradient[ 0 ] * gradient[ 0 ] * gradient[ 2 ] * tens( 0, 0, 0, 2 )
         + gradient[ 1 ] * gradient[ 1 ] * gradient[ 1 ] * gradient[ 0 ] * tens( 1, 1, 1, 0 )
         + gradient[ 2 ] * gradient[ 2 ] * gradient[ 2 ] * gradient[ 0 ] * tens( 2, 2, 2, 0 )
         + gradient[ 1 ] * gradient[ 1 ] * gradient[ 1 ] * gradient[ 2 ] * tens( 1, 1, 1, 2 )
         + gradient[ 2 ] * gradient[ 2 ] * gradient[ 2 ] * gradient[ 1 ] * tens( 2, 2, 2, 1 ) )
         + static_cast< Data_T >( 12 ) *
         ( gradient[ 2 ] * gradient[ 1 ] * gradient[ 0 ] * gradient[ 0 ] * tens( 2, 1, 0, 0 )
         + gradient[ 0 ] * gradient[ 2 ] * gradient[ 1 ] * gradient[ 1 ] * tens( 0, 2, 1, 1 )
         + gradient[ 0 ] * gradient[ 1 ] * gradient[ 2 ] * gradient[ 2 ] * tens( 0, 1, 2, 2 ) )
         + static_cast< Data_T >( 6 ) *
         ( gradient[ 0 ] * gradient[ 0 ] * gradient[ 1 ] * gradient[ 1 ] * tens( 0, 0, 1, 1 )
         + gradient[ 0 ] * gradient[ 0 ] * gradient[ 2 ] * gradient[ 2 ] * tens( 0, 0, 2, 2 )
         + gradient[ 1 ] * gradient[ 1 ] * gradient[ 2 ] * gradient[ 2 ] * tens( 1, 1, 2, 2 ) );
}

/**
 * Evaluate a spherical function represented by a symmetric 2nd-order tensor for a given gradient.
 *
 * \tparam Data_T The integral type used to store the tensor elements.
 *
 * \param tens The tensor representing the spherical function.
 * \param gradient The normalized vector that represents the gradient direction.
 *
 * \note If the gradient is not normalized, the result is undefined.
 */
template< typename Data_T >
double evaluateSphericalFunction( WTensorSym< 2, 3, Data_T > const& tens, WVector3d const& gradient )
{
    return gradient[ 0 ] * gradient[ 0 ] * tens( 0, 0 )
         + gradient[ 1 ] * gradient[ 1 ] * tens( 1, 1 )
         + gradient[ 2 ] * gradient[ 2 ] * tens( 2, 2 )
         + static_cast< Data_T >( 2 ) *
         ( gradient[ 0 ] * gradient[ 1 ] * tens( 0, 1 )
         + gradient[ 0 ] * gradient[ 2 ] * tens( 0, 2 )
         + gradient[ 1 ] * gradient[ 2 ] * tens( 1, 2 ) );
}

/**
 * Calculate the logarithm of the given real symmetric tensor.
 *
 * \param tens The tensor.
 * \return The logarithm of the tensor.
 */
template< typename T >
WTensorSym< 2, 3, T > tensorLog( WTensorSym< 2, 3, T > const& tens )
{
    WTensorSym< 2, 3, T > res;

    // calculate eigenvalues and eigenvectors
    RealEigenSystem sys;
    eigenEigenvector3D( tens, &sys );

    // first, we check for negative or zero eigenvalues
    if( sys[ 0 ].first <= 0.0 || sys[ 1 ].first <= 0.0 || sys[ 2 ].first <= 0.0 )
    {
        res( 0, 0 ) = res( 1, 1 ) = res( 2, 2 ) = 1.0;
        return res;
    }

    // this implements the matrix product U * log( E ) * U.transposed()
    // note that u( i, j ) = jth value of the ith eigenvector
    res( 0, 0 ) = sys[ 0 ].second[ 0 ] * sys[ 0 ].second[ 0 ] * log( sys[ 0 ].first )
                + sys[ 1 ].second[ 0 ] * sys[ 1 ].second[ 0 ] * log( sys[ 1 ].first )
                + sys[ 2 ].second[ 0 ] * sys[ 2 ].second[ 0 ] * log( sys[ 2 ].first );
    res( 0, 1 ) = sys[ 0 ].second[ 0 ] * sys[ 0 ].second[ 1 ] * log( sys[ 0 ].first )
                + sys[ 1 ].second[ 0 ] * sys[ 1 ].second[ 1 ] * log( sys[ 1 ].first )
                + sys[ 2 ].second[ 0 ] * sys[ 2 ].second[ 1 ] * log( sys[ 2 ].first );
    res( 0, 2 ) = sys[ 0 ].second[ 0 ] * sys[ 0 ].second[ 2 ] * log( sys[ 0 ].first )
                + sys[ 1 ].second[ 0 ] * sys[ 1 ].second[ 2 ] * log( sys[ 1 ].first )
                + sys[ 2 ].second[ 0 ] * sys[ 2 ].second[ 2 ] * log( sys[ 2 ].first );
    res( 1, 1 ) = sys[ 0 ].second[ 1 ] * sys[ 0 ].second[ 1 ] * log( sys[ 0 ].first )
                + sys[ 1 ].second[ 1 ] * sys[ 1 ].second[ 1 ] * log( sys[ 1 ].first )
                + sys[ 2 ].second[ 1 ] * sys[ 2 ].second[ 1 ] * log( sys[ 2 ].first );
    res( 1, 2 ) = sys[ 0 ].second[ 1 ] * sys[ 0 ].second[ 2 ] * log( sys[ 0 ].first )
                + sys[ 1 ].second[ 1 ] * sys[ 1 ].second[ 2 ] * log( sys[ 1 ].first )
                + sys[ 2 ].second[ 1 ] * sys[ 2 ].second[ 2 ] * log( sys[ 2 ].first );
    res( 2, 2 ) = sys[ 0 ].second[ 2 ] * sys[ 0 ].second[ 2 ] * log( sys[ 0 ].first )
                + sys[ 1 ].second[ 2 ] * sys[ 1 ].second[ 2 ] * log( sys[ 1 ].first )
                + sys[ 2 ].second[ 2 ] * sys[ 2 ].second[ 2 ] * log( sys[ 2 ].first );
    return res;
}

/**
 * Calculate the exponential of the given real symmetric tensor.
 *
 * \param tens The tensor.
 * \return The exponential of the tensor.
 */
template< typename T >
WTensorSym< 2, 3, T > tensorExp( WTensorSym< 2, 3, T > const& tens )
{
    WTensorSym< 2, 3, T > res;

    // calculate eigenvalues and eigenvectors
    RealEigenSystem sys;
    eigenEigenvector3D( tens, &sys );

    // this implements the matrix product U * exp( E ) * U.transposed()
    // note that u( i, j ) = jth value of the ith eigenvector
    res( 0, 0 ) = sys[ 0 ].second[ 0 ] * sys[ 0 ].second[ 0 ] * exp( sys[ 0 ].first )
                + sys[ 1 ].second[ 0 ] * sys[ 1 ].second[ 0 ] * exp( sys[ 1 ].first )
                + sys[ 2 ].second[ 0 ] * sys[ 2 ].second[ 0 ] * exp( sys[ 2 ].first );
    res( 0, 1 ) = sys[ 0 ].second[ 0 ] * sys[ 0 ].second[ 1 ] * exp( sys[ 0 ].first )
                + sys[ 1 ].second[ 0 ] * sys[ 1 ].second[ 1 ] * exp( sys[ 1 ].first )
                + sys[ 2 ].second[ 0 ] * sys[ 2 ].second[ 1 ] * exp( sys[ 2 ].first );
    res( 0, 2 ) = sys[ 0 ].second[ 0 ] * sys[ 0 ].second[ 2 ] * exp( sys[ 0 ].first )
                + sys[ 1 ].second[ 0 ] * sys[ 1 ].second[ 2 ] * exp( sys[ 1 ].first )
                + sys[ 2 ].second[ 0 ] * sys[ 2 ].second[ 2 ] * exp( sys[ 2 ].first );
    res( 1, 1 ) = sys[ 0 ].second[ 1 ] * sys[ 0 ].second[ 1 ] * exp( sys[ 0 ].first )
                + sys[ 1 ].second[ 1 ] * sys[ 1 ].second[ 1 ] * exp( sys[ 1 ].first )
                + sys[ 2 ].second[ 1 ] * sys[ 2 ].second[ 1 ] * exp( sys[ 2 ].first );
    res( 1, 2 ) = sys[ 0 ].second[ 1 ] * sys[ 0 ].second[ 2 ] * exp( sys[ 0 ].first )
                + sys[ 1 ].second[ 1 ] * sys[ 1 ].second[ 2 ] * exp( sys[ 1 ].first )
                + sys[ 2 ].second[ 1 ] * sys[ 2 ].second[ 2 ] * exp( sys[ 2 ].first );
    res( 2, 2 ) = sys[ 0 ].second[ 2 ] * sys[ 0 ].second[ 2 ] * exp( sys[ 0 ].first )
                + sys[ 1 ].second[ 2 ] * sys[ 1 ].second[ 2 ] * exp( sys[ 1 ].first )
                + sys[ 2 ].second[ 2 ] * sys[ 2 ].second[ 2 ] * exp( sys[ 2 ].first );
    return res;
}

/**
 * Find the maxima of a spherical function represented by a symmetric tensor.
 *
 *
 *
 */
template< std::size_t order, typename Data_T >
void findSymmetricSphericalFunctionMaxima( WTensorSym< order, 3, Data_T > const& tensor,
                                           double threshold, double minAngularSeparationCosine, double stepSize,
                                           std::vector< WVector3d > const& startingPoints,
                                           std::vector< WVector3d >& maxima ) //  NOLINT
{
    std::vector< double > values;

    std::vector< WVector3d >::const_iterator it;
    for( it = startingPoints.begin(); it != startingPoints.end(); ++it )
    {
        WVector3d position = *it;
        WVector3d gradient = approximateGradient( tensor, position );
        int iter;

        for( iter = 0; iter < 100; ++iter )
        {
            WVector3d newPosition = position + stepSize * gradient;
            normalize( newPosition );
            WVector3d newGradient = approximateGradient( tensor, newPosition );

            double cos = dot( newGradient, gradient );
            if( cos > 0.985 ) // less then 10 degree
            {
                stepSize *= 2.0;
            }
            else if( cos < 0.866 ) // more than 30 degree
            {
                stepSize *= 0.5;
                if( stepSize < 0.000001 )
                {
                    break;
                }
            }
            if( cos > 0.886 ) // less than 30 degree
            {
                gradient = newGradient;
                position = newPosition;
            }
        }
        if( iter != 100 )
        {
            double newFuncValue = tensor.evaluateSphericalFunction( position );

            bool add = true;

            std::vector< int > m;
            m.reserve( maxima.size() );

            for( std::size_t k = 0; k != maxima.size(); ++k )
            {
                // find all maxima that are to close to this one
                if( dot( position, maxima[ k ] ) > minAngularSeparationCosine
                 || dot( maxima[ k ], -1.0 * position ) > minAngularSeparationCosine )
                {
                    m.push_back( k );
                    if( values[ k ] >= newFuncValue )
                    {
                        add = false;
                    }
                }
            }
            if( add )
            {
                maxima.push_back( position );
                values.push_back( newFuncValue );

                for( int k = static_cast< int >( m.size() - 1 ); k > 0; --k )
                {
                    maxima.erase( maxima.begin() + m[ k ] );
                    values.erase( values.begin() + m[ k ] );
                }
            }
        }
    }

    // remove maxima that are too small
    double max = 0;
    for( std::size_t k = 0; k != maxima.size(); ++k )
    {
        if( values[ k ] > max )
        {
            max = values[ k ];
        }
    }

    std::size_t k = 0;
    while( k < maxima.size() )
    {
        if( values[ k ] < threshold * max )
        {
            maxima.erase( maxima.begin() + k );
            values.erase( values.begin() + k );
        }
        else
        {
            ++k;
        }
    }
}

template< std::size_t order, typename Data_T >
WVector3d approximateGradient( WTensorSym< order, 3, Data_T > const& tensor, WVector3d const& pos )
{
    WVector3d eXY;
    WVector3d eZ;

    if( fabs( fabs( pos[ 2 ] ) - 1.0 ) < 0.001 )
    {
        eXY = WVector3d( 1.0, 0.0, 0.0 );
        eZ = WVector3d( 0.0, 1.0, 0.0 );
    }
    else
    {
        // this is vectorProduct( z, pos )
        eXY = WVector3d( -pos[ 1 ], pos[ 0 ], 0.0 );
        normalize( eXY );
        // this is vectorProduct( eXY, pos )
        eZ = WVector3d( eXY[ 1 ] * pos[ 2 ], -eXY[ 0 ] * pos[ 2 ], eXY[ 0 ] * pos[ 1 ] - eXY[ 1 ] * pos[ 0 ] );
        normalize( eZ );
    }

    double dXY = ( tensor.evaluateSphericalFunction( normalize( pos + eXY * 0.0001 ) )
                 - tensor.evaluateSphericalFunction( normalize( pos - eXY * 0.0001 ) ) )
                 / 0.0002;
    double dZ = ( tensor.evaluateSphericalFunction( normalize( pos + eZ * 0.0001 ) )
                - tensor.evaluateSphericalFunction( normalize( pos - eZ * 0.0001 ) ) )
                / 0.0002;

    // std::sqrt( 1.0 - z² ) = sin( acos( z ) ) = sin( theta ) in spherical coordinates
    double d = 1.0 - pos[ 2 ] * pos[ 2 ];
    if( d < 0.0 ) // avoid possible numerical problems
    {
        d = 0.0;
    }
    WVector3d res = eZ * dZ + eXY * ( dXY / std::sqrt( d ) );
    return normalize( res );
}

#endif  // WTENSORFUNCTIONS_H
