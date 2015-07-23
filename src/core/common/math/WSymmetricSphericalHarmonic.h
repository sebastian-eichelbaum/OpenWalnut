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

#ifndef WSYMMETRICSPHERICALHARMONIC_H
#define WSYMMETRICSPHERICALHARMONIC_H

#include <stdint.h>

#include <cmath>
#include <vector>

#include <boost/math/special_functions/spherical_harmonic.hpp>

#include "core/common/WLogger.h"
#include "core/common/math/WGeometryFunctions.h"
#include "../exceptions/WPreconditionNotMet.h"
#include "linearAlgebra/WVectorFixed.h"
#include "WLinearAlgebraFunctions.h"
#include "WMath.h"
#include "WMatrix.h"
#include "WTensorSym.h"
#include "WUnitSphereCoordinates.h"
#include "WValue.h"


/**
 * Class for symmetric spherical harmonics
 * The index scheme of the coefficients/basis values is like in the Descoteaux paper "Regularized, Fast, and Robust Analytical Q-Ball Imaging".
 */
template< typename T > class WSymmetricSphericalHarmonic  // NOLINT
{
// friend class WSymmetricSphericalHarmonicTest;
public:
    /**
     * Default constructor.
     */
    WSymmetricSphericalHarmonic();

    /**
     * Constructor.
     * \param SHCoefficients the initial coefficients (stored like in the mentioned Descoteaux paper).
     */
    explicit WSymmetricSphericalHarmonic( const WValue< T >& SHCoefficients );

    /**
     * Destructor.
     */
    virtual ~WSymmetricSphericalHarmonic();

    /**
     * Return the value on the sphere.
     * \param theta angle for the position on the unit sphere
     * \param phi angle for the position on the unit sphere
     *
     * \return value on sphere
     */
    T getValue( T theta, T phi ) const;

    /**
     * Return the value on the sphere.
     * \param coordinates for the position on the unit sphere
     *
     * \return value on sphere
     */
    T getValue( const WUnitSphereCoordinates< T >& coordinates ) const;

    /**
     * Returns the used coefficients (stored like in the mentioned 2007 Descoteaux paper).
     *
     * \return coefficient list
     */
    const WValue< T >& getCoefficients() const;

    /**
     * Returns the coefficients for Schultz' SH base.
     *
     * \return coefficient list
     */
    WValue< T > getCoefficientsSchultz() const;

    /**
     * Returns the coefficients for the complex base.
     *
     * \return coefficiend list
     */
    WValue< std::complex< T > > getCoefficientsComplex() const;

    /**
     * Set the coeffs from complex base coeffs.
     *
     * \param coeffs The complex coefficients, sorted by order, then phase, ascending.
     * \param order The order of the spherical harmonic.
     */
    void setFromComplex( WValue< std::complex< T > > const& coeffs, int order );

    /**
     * Applies the Funk-Radon-Transformation. This is faster than matrix multiplication.
     * ( O(n) instead of O(n²) )
     *
     * \param frtMat the frt matrix as calculated by calcFRTMatrix()
     */
    void applyFunkRadonTransformation( WMatrix< T > const& frtMat );

    /**
     * Return the order of the spherical harmonic.
     *
     * \return order of SH
     */
    size_t getOrder() const;

    /**
     * Calculate the generalized fractional anisotropy for this ODF.
     *
     * See: David S. Tuch, "Q-Ball Imaging", Magn. Reson. Med. 52, 2004, 1358-1372
     *
     * \note this only makes sense if this is an ODF, meaning funk-radon-transform was applied etc.
     *
     * \param orientations A vector of unit sphere coordinates.
     *
     * \return The generalized fractional anisotropy.
     */
    T calcGFA( std::vector< WUnitSphereCoordinates< T > > const& orientations ) const;

    /**
     * Calculate the generalized fractional anisotropy for this ODF. This version of
     * the function uses precomputed base functions (because calculating the base function values
     * is rather expensive). Use this version if you want to compute the GFA for multiple ODFs
     * with the same base functions. The base function Matrix can be computed using \see calcBMatrix().
     *
     * See: David S. Tuch, "Q-Ball Imaging", Magn. Reson. Med. 52, 2004, 1358-1372
     *
     * \note this only makes sense if this is an ODF, meaning funk-radon-transform was applied etc.
     *
     * \param B The matrix of SH base functions.
     *
     * \return The generalized fractional anisotropy.
     */
    T calcGFA( WMatrix< T > const& B ) const;

    /**
    * This calculates the transformation/fitting matrix T like in the 2007 Descoteaux paper. The orientations are given as WMatrixFixed< T, 3, 1 >.
    * \param orientations The vector with the used orientation on the unit sphere (usually the gradients of the HARDI)
    * \param order The order of the spherical harmonics intended to create
    * \param lambda Regularization parameter for smoothing matrix
    * \param withFRT include the Funk-Radon-Transformation?
    * \return Transformation matrix
    */
    static WMatrix< T > getSHFittingMatrix( const std::vector< WMatrixFixed< T, 3, 1 > >& orientations,
                                               int order,
                                               T lambda,
                                               bool withFRT );

    /**
    * This calculates the transformation/fitting matrix T like in the 2007 Descoteaux paper. The orientations are given as WUnitSphereCoordinates< T >.
    * \param orientations The vector with the used orientation on the unit sphere (usually the gradients of the HARDI)
    * \param order The order of the spherical harmonics intended to create
    * \param lambda Regularization parameter for smoothing matrix
    * \param withFRT include the Funk-Radon-Transformation?
    * \return Transformation matrix
    */
    static WMatrix< T > getSHFittingMatrix( const std::vector< WUnitSphereCoordinates< T > >& orientations,
                                               int order,
                                               T lambda,
                                               bool withFRT );

    /**
    * This calculates the transformation/fitting matrix T like in the 2010 Aganj paper. The orientations are given as WUnitSphereCoordinates< T >.
    * \param orientations The vector with the used orientation on the unit sphere (usually the gradients of the HARDI)
    * \param order The order of the spherical harmonics intended to create
    * \param lambda Regularization parameter for smoothing matrix
    * \return Transformation matrix
    */
    static WMatrix< T > getSHFittingMatrixForConstantSolidAngle( const std::vector< WMatrixFixed< T, 3, 1 > >& orientations,
                                                                    int order,
                                                                    T lambda );

    /**
    * This calculates the transformation/fitting matrix T like in the 2010 Aganj paper. The orientations are given as WUnitSphereCoordinates< T >.
    * \param orientations The vector with the used orientation on the unit sphere (usually the gradients of the HARDI)
    * \param order The order of the spherical harmonics intended to create
    * \param lambda Regularization parameter for smoothing matrix
    * \return Transformation matrix
    */
    static WMatrix< T > getSHFittingMatrixForConstantSolidAngle( const std::vector< WUnitSphereCoordinates< T > >& orientations,
                                                                 int order,
                                                                 T lambda );

    /**
    * Calculates the base matrix B like in the dissertation of Descoteaux.
    * \param orientations The vector with the used orientation on the unit sphere (usually the gradients of the HARDI)
    * \param order The order of the spherical harmonics intended to create
    * \return The base Matrix B
    */
    static WMatrix< T > calcBaseMatrix( const std::vector< WUnitSphereCoordinates< T > >& orientations, int order );

    /**
    * Calculates the base matrix B for the complex spherical harmonics.
    * \param orientations The vector with the used orientation on the unit sphere (usually the gradients of the HARDI)
    * \param order The order of the spherical harmonics intended to create
    * \return The base Matrix B
    */
    static WMatrix< std::complex< T > > calcComplexBaseMatrix( std::vector< WUnitSphereCoordinates< T > > const& orientations,
                                                                           int order );
    /**
    * Calc eigenvalues for SH elements.
    * \param order The order of the spherical harmonic
    * \return The eigenvalues of the coefficients
    */
    static WValue< T > calcEigenvalues( size_t order );

    /**
    * Calc matrix with the eigenvalues of the SH elements on its diagonal.
    * \param order The order of the spherical harmonic
    * \return The matrix with the eigenvalues of the coefficients
    */
    static WMatrix< T > calcMatrixWithEigenvalues( size_t order );

    /**
    * This calcs the smoothing matrix L from the 2007 Descoteaux Paper "Regularized, Fast, and Robust Analytical Q-Ball Imaging"
    * \param order The order of the spherical harmonic
    * \return The smoothing matrix L
    */
    static WMatrix< T > calcSmoothingMatrix( size_t order );

    /**
    * Calculates the Funk-Radon-Transformation-Matrix P from the 2007 Descoteaux Paper "Regularized, Fast, and Robust Analytical Q-Ball Imaging"
    * \param order The order of the spherical harmonic
    * \return The Funk-Radon-Matrix P
    */
    static WMatrix< T > calcFRTMatrix( size_t order );

   /**
     * Calculates a matrix that converts spherical harmonics to symmetric tensors of equal or lower order.
     *
     * \param order The order of the symmetric tensor.
     *
     * \return the conversion matrix
     */
    static WMatrix< T > calcSHToTensorSymMatrix( std::size_t order );

   /**
     * Calculates a matrix that converts spherical harmonics to symmetric tensors of equal or lower order.
     *
     * \param order The order of the symmetric tensor.
     * \param orientations A vector of at least (orderTensor+1) * (orderTensor+2) / 2 orientations.
     *
     * \return the conversion matrix
     */
    static WMatrix< T > calcSHToTensorSymMatrix( std::size_t order, const std::vector< WUnitSphereCoordinates< T > >& orientations );

    /**
     * Normalize this SH in place.
     */
    void normalize();

protected:
private:
    /** order of the spherical harmonic */
    size_t m_order;

    /** coefficients of the spherical harmonic */
    WValue< T > m_SHCoefficients;
};

template< typename T >
WSymmetricSphericalHarmonic< T >::WSymmetricSphericalHarmonic() :
    m_order( 0 ),
    m_SHCoefficients( 0 )
{
}

template< typename T >
WSymmetricSphericalHarmonic< T >::WSymmetricSphericalHarmonic( const WValue< T >& SHCoefficients ) :
  m_SHCoefficients( SHCoefficients )
{
  // calculate order from SHCoefficients.size:
  // - this is done by reversing the R=(l+1)*(l+2)/2 formula from the Descoteaux paper
  T q = std::sqrt( 0.25 + 2.0 * static_cast< T >( m_SHCoefficients.size() ) ) - 1.5;
  m_order = static_cast<size_t>( q );
}

template< typename T >
WSymmetricSphericalHarmonic< T >::~WSymmetricSphericalHarmonic()
{
}

template< typename T >
T WSymmetricSphericalHarmonic< T >::getValue( T theta, T phi ) const
{
  T result = 0.0;
  int j = 0;
  const T rootOf2 = std::sqrt( 2.0 );
  for( int k = 0; k <= static_cast<int>( m_order ); k+=2 )
  {
    // m = 1 .. k
    for( int m = 1; m <= k; m++ )
    {
      j = ( k*k+k+2 ) / 2 + m;
      result += m_SHCoefficients[ j-1 ] * rootOf2 *
                  static_cast< T >( std::pow( -1.0, m+1 ) ) * boost::math::spherical_harmonic_i( k, m, theta, phi );
    }
    // m = 0
    result += m_SHCoefficients[ ( k*k+k+2 ) / 2 - 1 ] * boost::math::spherical_harmonic_r( k, 0, theta, phi );
    // m = -k .. -1
    for( int m = -k; m < 0; m++ )
    {
      j = ( k*k+k+2 ) / 2 + m;
      result += m_SHCoefficients[ j-1 ] * rootOf2 * boost::math::spherical_harmonic_r( k, -m, theta, phi );
    }
  }
  return result;
}

template< typename T >
T WSymmetricSphericalHarmonic< T >::getValue( const WUnitSphereCoordinates< T >& coordinates ) const
{
  return getValue( coordinates.getTheta(), coordinates.getPhi() );
}

template< typename T >
const WValue< T >& WSymmetricSphericalHarmonic< T >::getCoefficients() const
{
  return m_SHCoefficients;
}

template< typename T >
WValue< T > WSymmetricSphericalHarmonic< T >::getCoefficientsSchultz() const
{
    WValue< T > res( m_SHCoefficients.size() );
    size_t k = 0;
    for( int l = 0; l <= static_cast< int >( m_order ); l += 2 )
    {
        for( int m = -l; m <= l; ++m )
        {
            res[ k ] = m_SHCoefficients[ k ];
            if( m < 0 && ( ( -m ) % 2 == 1 ) )
            {
                res[ k ] *= -1.0;
            }
            else if( m > 0 && ( m % 2 == 0 ) )
            {
                res[ k ] *= -1.0;
            }
            ++k;
        }
    }
    return res;
}

template< typename T >
WValue< std::complex< T > > WSymmetricSphericalHarmonic< T >::getCoefficientsComplex() const
{
    WValue< std::complex< T > > res( m_SHCoefficients.size() );
    size_t k = 0;
    T r = 1.0 / sqrt( 2.0 );
    std::complex< T > i( 0.0, -1.0 );
    for( int l = 0; l <= static_cast< int >( m_order ); l += 2 )
    {
        for( int m = -l; m <= l; ++m )
        {
            if( m == 0 )
            {
                res[ k ] = m_SHCoefficients[ k ];
            }
            else if( m < 0 )
            {
                res[ k ] += i * r * m_SHCoefficients[ k - 2 * m ];
                res[ k ] += ( -m % 2 == 1 ? -r : r ) * m_SHCoefficients[ k ];
            }
            else if( m > 0 )
            {
                res[ k ] += i * ( -m % 2 == 0 ? -r : r ) * m_SHCoefficients[ k ];
                res[ k ] += r * m_SHCoefficients[ k - 2 * m ];
            }
            ++k;
        }
    }
    return res;
}

template< typename T >
void WSymmetricSphericalHarmonic< T >::setFromComplex( WValue< std::complex< T > > const& coeffs, int order )
{
    if( order < 0 || order % 2 == 1 )
    {
        return;
    }

    m_order = order;
    int elements = ( m_order + 1 ) * ( m_order + 2 ) / 2;
    m_SHCoefficients.resize( elements );
    for( int k = 0; k < elements; ++k )
    {
        m_SHCoefficients[ k ] = 0.0;
    }

    size_t k = 0;
    T r = 1.0 / sqrt( 2.0 );
    std::complex< T > i( 0.0, -1.0 );
    for( int l = 0; l <= static_cast< int >( m_order ); l += 2 )
    {
        if( ( l + 1 ) * ( l + 2 ) > 2 * static_cast< int >( coeffs.size() ) )
        {
            break;
        }

        for( int m = -l; m <= l; ++m )
        {
            if( m == 0 )
            {
                m_SHCoefficients[ k ] = coeffs[ k ].real();
            }
            else if( m < 0 )
            {
                m_SHCoefficients[ k ] = std::complex< T >( r * coeffs[ k - 2 * m ] ).real();
                m_SHCoefficients[ k ] += std::complex< T >( ( -m % 2 == 0 ? r : -r ) * coeffs[ k ] ).real();
            }
            else if( m > 0 )
            {
                m_SHCoefficients[ k ] = std::complex< T >( i * ( m % 2 == 1 ? -r : r ) * coeffs[ k ] ).real();
                m_SHCoefficients[ k ] += std::complex< T >( -r * i * coeffs[ k - 2 * m ] ).real();
            }
            ++k;
        }
    }
}

template< typename T >
T WSymmetricSphericalHarmonic< T >::calcGFA( std::vector< WUnitSphereCoordinates< T > > const& orientations ) const
{
    T n = static_cast< T >( orientations.size() );
    T d = 0.0;
    T gfa = 0.0;
    T mean = 0.0;
    std::vector< T > v( orientations.size() );

    for( std::size_t i = 0; i < orientations.size(); ++i )
    {
        v[ i ] = getValue( orientations[ i ] );
        mean += v[ i ];
    }
    mean /= n;

    for( std::size_t i = 0; i < orientations.size(); ++i )
    {
        T f = v[ i ];
        // we use gfa as a temporary here
        gfa += f * f;
        f -= mean;
        d += f * f;
    }

    if( gfa == 0.0 || n <= 1.0 )
    {
        return 0.0;
    }
    // this is the real gfa
    gfa = sqrt( ( n * d ) / ( ( n - 1.0 ) * gfa ) );

    WAssert( gfa >= -0.01 && gfa <= 1.01, "" );
    if( gfa < 0.0 )
    {
        return 0.0;
    }
    else if( gfa > 1.0 )
    {
        return 1.0;
    }
    return gfa;
}

template< typename T >
T WSymmetricSphericalHarmonic< T >::calcGFA( WMatrix< T > const& B ) const
{
    // sh coeffs
    WMatrix< T > s( B.getNbCols(), 1 );
    WAssert( B.getNbCols() == m_SHCoefficients.size(), "" );
    for( std::size_t k = 0; k < B.getNbCols(); ++k )
    {
        s( k, 0 ) = m_SHCoefficients[ k ];
    }
    s = B * s;
    WAssert( s.getNbRows() == B.getNbRows(), "" );
    WAssert( s.getNbCols() == 1u, "" );

    T n = static_cast< T >( s.getNbRows() );
    T d = 0.0;
    T gfa = 0.0;
    T mean = 0.0;
    for( std::size_t i = 0; i < s.getNbRows(); ++i )
    {
        mean += s( i, 0 );
    }
    mean /= n;

    for( std::size_t i = 0; i < s.getNbRows(); ++i )
    {
        T f = s( i, 0 );
        // we use gfa as a temporary here
        gfa += f * f;
        f -= mean;
        d += f * f;
    }

    if( gfa == 0.0 || n <= 1.0 )
    {
        return 0.0;
    }
    // this is the real gfa
    gfa = sqrt( ( n * d ) / ( ( n - 1.0 ) * gfa ) );

    WAssert( gfa >= -0.01 && gfa <= 1.01, "" );
    if( gfa < 0.0 )
    {
        return 0.0;
    }
    else if( gfa > 1.0 )
    {
        return 1.0;
    }
    return gfa;
}

template< typename T >
void WSymmetricSphericalHarmonic< T >::applyFunkRadonTransformation( WMatrix< T > const& frtMat )
{
    WAssert( frtMat.getNbCols() == m_SHCoefficients.size(), "" );
    WAssert( frtMat.getNbRows() == m_SHCoefficients.size(), "" );
    // Funk-Radon-Transformation as in Descoteaux's thesis
    for( size_t j = 0; j < m_SHCoefficients.size(); j++ )
    {
        m_SHCoefficients[ j ] *= frtMat( j, j );
    }
}

template< typename T >
size_t WSymmetricSphericalHarmonic< T >::getOrder() const
{
  return m_order;
}

template< typename T >
WMatrix< T > WSymmetricSphericalHarmonic< T >::getSHFittingMatrix( const std::vector< WMatrixFixed< T, 3, 1 > >& orientations,
                                                                        int order,
                                                                        T lambda,
                                                                        bool withFRT )
{
    // convert euclid 3d orientations/gradients to sphere coordinates
    std::vector< WUnitSphereCoordinates< T > > sphereCoordinates;
    for( typename std::vector< WMatrixFixed< T, 3, 1 > >::const_iterator it = orientations.begin(); it != orientations.end(); it++ )
    {
        sphereCoordinates.push_back( WUnitSphereCoordinates< T >( *it ) );
    }
    return WSymmetricSphericalHarmonic< T >::getSHFittingMatrix( sphereCoordinates, order, lambda, withFRT );
}

template< typename T >
WMatrix< T > WSymmetricSphericalHarmonic< T >::getSHFittingMatrix( const std::vector< WUnitSphereCoordinates< T > >& orientations,
                                                                 int order,
                                                                 T lambda,
                                                                 bool withFRT )
{
  WMatrix< T > B( WSymmetricSphericalHarmonic< T >::calcBaseMatrix( orientations, order ) );
  WMatrix< T > Bt( B.transposed() );
  WMatrix< T > result( Bt*B );
  if( lambda != 0.0 )
  {
    WMatrix< T > L( WSymmetricSphericalHarmonic< T >::calcSmoothingMatrix( order ) );
    result += lambda*L;
  }

  result = pseudoInverse( result )*Bt;
  if( withFRT )
  {
    WMatrix< T > P( WSymmetricSphericalHarmonic< T >::calcFRTMatrix( order ) );
    return ( P * result );
  }
  return result;
}

template< typename T >
WMatrix< T > WSymmetricSphericalHarmonic< T >::getSHFittingMatrixForConstantSolidAngle( const std::vector< WMatrixFixed< T, 3, 1 > >& orientations,
                                                                                      int order,
                                                                                      T lambda )
{
    // convert euclid 3d orientations/gradients to sphere coordinates
    std::vector< WUnitSphereCoordinates< T > > sphereCoordinates;
    for( typename std::vector< WMatrixFixed< T, 3, 1 > >::const_iterator it = orientations.begin(); it != orientations.end(); it++ )
    {
        sphereCoordinates.push_back( WUnitSphereCoordinates< T >( *it ) );
    }
    return WSymmetricSphericalHarmonic< T >::getSHFittingMatrixForConstantSolidAngle( sphereCoordinates, order, lambda );
}

template< typename T >
WMatrix< T > WSymmetricSphericalHarmonic< T >::getSHFittingMatrixForConstantSolidAngle(
    const std::vector< WUnitSphereCoordinates< T > >& orientations,
    int order,
    T lambda )
{
  WMatrix< T > B( WSymmetricSphericalHarmonic< T >::calcBaseMatrix( orientations, order ) );
  WMatrix< T > Bt( B.transposed() );
  WMatrix< T > result( Bt*B );

  // regularisation
  if( lambda != 0.0 )
  {
    WMatrix< T > L( WSymmetricSphericalHarmonic< T >::calcSmoothingMatrix( order ) );
    result += lambda*L;
  }

  result = pseudoInverse( result )*Bt;

  // multiply with eigenvalues of coefficients / Laplace-Beltrami operator
  WMatrix< T > LB( WSymmetricSphericalHarmonic< T >::calcMatrixWithEigenvalues( order ) );
  wlog::debug( "" ) << "LB: " << LB;
  result = LB*result;
  wlog::debug( "" ) << "LB*result: " << result;

  // apply FRT
  WMatrix< T > P( WSymmetricSphericalHarmonic< T >::calcFRTMatrix( order ) );
  result = P * result;
  wlog::debug( "" ) << "P: " << P;
  wlog::debug( "" ) << "P*result: " << result;

  // correction factor
  T correctionFactor = 1.0 / ( 16.0 * std::pow( static_cast< T >( pi() ), 2 ) );
  result *= correctionFactor;

  return result;
}


template< typename T >
WMatrix< T > WSymmetricSphericalHarmonic< T >::calcBaseMatrix( const std::vector< WUnitSphereCoordinates< T > >& orientations,
                                                                    int order )
{
  WMatrix< T > B( orientations.size(), ( ( order + 1 ) * ( order + 2 ) ) / 2 );

  // calc B Matrix like in the 2007 Descoteaux paper ("Regularized, Fast, and Robust Analytical Q-Ball Imaging")
  int j = 0;
  const T rootOf2 = std::sqrt( 2.0 );
  for(uint32_t row = 0; row < orientations.size(); row++ )
  {
    const T theta = orientations[row].getTheta();
    const T phi = orientations[row].getPhi();
    for( int k = 0; k <= order; k+=2 )
    {
      // m = 1 .. k
      for( int m = 1; m <= k; m++ )
      {
        j = ( k * k + k + 2 ) / 2 + m;
        B( row, j-1 ) = rootOf2 * static_cast< T >( std::pow( static_cast< T >( -1 ), m + 1 ) )
                        * boost::math::spherical_harmonic_i( k, m, theta, phi );
      }
      // m = 0
      B( row, ( k * k + k + 2 ) / 2 - 1 ) = boost::math::spherical_harmonic_r( k, 0, theta, phi );
      // m = -k .. -1
      for( int m = -k; m < 0; m++ )
      {
        j = ( k * k + k + 2 ) / 2 + m;
        B( row, j-1 ) = rootOf2*boost::math::spherical_harmonic_r( k, -m, theta, phi );
      }
    }
  }
  return B;
}

template< typename T >
WMatrix< std::complex< T > >
WSymmetricSphericalHarmonic< T >::calcComplexBaseMatrix( std::vector< WUnitSphereCoordinates< T > > const& orientations, int order )
{
    WMatrix< std::complex< T > > B( orientations.size(), ( ( order + 1 ) * ( order + 2 ) ) / 2 );

    for( std::size_t row = 0; row < orientations.size(); row++ )
    {
        const T theta = orientations[ row ].getTheta();
        const T phi = orientations[ row ].getPhi();

        int j = 0;
        for( int k = 0; k <= order; k += 2 )
        {
            for( int m = -k; m < 0; m++ )
            {
                B( row, j ) = boost::math::spherical_harmonic( k, m, theta, phi );
                ++j;
            }
            B( row, j ) = boost::math::spherical_harmonic( k, 0, theta, phi );
            ++j;
            for( int m = 1; m <= k; m++ )
            {
                B( row, j ) = boost::math::spherical_harmonic( k, m, theta, phi );
                ++j;
            }
        }
    }
    return B;
}

template< typename T >
WValue< T > WSymmetricSphericalHarmonic< T >::calcEigenvalues( size_t order )
{
    size_t R = ( ( order + 1 ) * ( order + 2 ) ) / 2;
    std::size_t i = 0;
    WValue< T > result( R );
    for( size_t k = 0; k <= order; k += 2 )
    {
        for( int m = -static_cast< int >( k ); m <= static_cast< int >( k ); ++m )
        {
            result[ i ] = -static_cast< T > ( k * ( k + 1 ) );
            ++i;
        }
    }
    return result;
}

template< typename T >
WMatrix< T > WSymmetricSphericalHarmonic< T >::calcMatrixWithEigenvalues( size_t order )
{
    WValue< T > eigenvalues( WSymmetricSphericalHarmonic< T >::calcEigenvalues( order ) );
    WMatrix< T > L( eigenvalues.size(), eigenvalues.size() );
    for( std::size_t i = 0; i < eigenvalues.size(); ++i )
    {
        L( i, i ) = eigenvalues[ i ];
    }
    return L;
}

template< typename T >
WMatrix< T > WSymmetricSphericalHarmonic< T >::calcSmoothingMatrix( size_t order )
{
    WValue< T > eigenvalues( WSymmetricSphericalHarmonic< T >::calcEigenvalues( order ) );
    WMatrix< T > L( eigenvalues.size(), eigenvalues.size() );
    for( std::size_t i = 0; i < eigenvalues.size(); ++i )
    {
        L( i, i ) = std::pow( eigenvalues[ i ], 2 );
    }
    return L;
}

template< typename T >
WMatrix< T > WSymmetricSphericalHarmonic< T >::calcFRTMatrix( size_t order )
{
    size_t R = ( ( order + 1 ) * ( order + 2 ) ) / 2;
    std::size_t i = 0;
    WMatrix< T > result( R, R );
    for( size_t k = 0; k <= order; k += 2 )
    {
        T h = 2.0 * static_cast< T >( pi() ) * static_cast< T >( std::pow( static_cast< T >( -1 ), static_cast< T >( k / 2 ) ) ) *
                    static_cast< T >( oddFactorial( ( k <= 1 ) ? 1 : k - 1 ) ) / static_cast< T >( evenFactorial( k ) );
        for( int m = -static_cast< int >( k ); m <= static_cast< int >( k ); ++m )
        {
            result( i, i ) = h;
            ++i;
        }
    }
    return result;
}

template< typename T >
WMatrix< T > WSymmetricSphericalHarmonic< T >::calcSHToTensorSymMatrix( std::size_t order )
{
    std::vector< WVector3d > vertices;
    std::vector< unsigned int > triangles;
    // calc necessary tesselation level
    int level = static_cast< int >( log( static_cast< float >( ( order + 1 ) * ( order + 2 ) ) ) / 2.0f + 1.0f );
    tesselateIcosahedron( &vertices, &triangles, level );
    std::vector< WUnitSphereCoordinates< T > > orientations;
    for( typename std::vector< WVector3d >::const_iterator cit = vertices.begin(); cit != vertices.end(); cit++ )
    {
        // avoid linear dependent orientations
        if( ( *cit )[ 0 ] >= 0.0001 )
        {
            orientations.push_back( WUnitSphereCoordinates< T >( WMatrixFixed< T, 3, 1 >( *cit ) ) );
        }
    }
    return WSymmetricSphericalHarmonic< T >::calcSHToTensorSymMatrix( order, orientations );
}

template< typename T >
WMatrix< T > WSymmetricSphericalHarmonic< T >::calcSHToTensorSymMatrix( std::size_t order,
                                                                        const std::vector< WUnitSphereCoordinates< T > >& orientations )
{
    std::size_t numElements = ( order + 1 ) * ( order + 2 ) / 2;
    WPrecondEquals( order % 2, 0u );
    WPrecondLess( numElements, orientations.size() + 1 );

    // store first numElements orientations as 3d-vectors
    std::vector< WMatrixFixed< T, 3, 1 > > directions( numElements );
    for( std::size_t i = 0; i < numElements; ++i )
    {
        directions[ i ] = orientations[ i ].getEuclidean();
    }

    // initialize an index array
    std::vector< std::size_t > indices( order, 0 );

    // calc tensor evaluation matrix
    WMatrix< T > TEMat( numElements, numElements );
    for( std::size_t j = 0; j < numElements; ++j ) // j is the 'permutation index'
    {
        // stores how often each value is represented in the index array
        std::size_t amount[ 3 ] = { 0, 0, 0 };
        for( std::size_t k = 0; k < order; ++k )
        {
            ++amount[ indices[ k ] ];
        }

        // from WTensorSym.h
        std::size_t multiplicity = calcSupersymmetricTensorMultiplicity( order, amount[ 0 ], amount[ 1 ], amount[ 2 ] );
        for( std::size_t i = 0; i < numElements; ++i ) // i is the 'direction index'
        {
            TEMat( i, j ) = multiplicity;
            for( std::size_t k = 0; k < order; ++k )
            {
                TEMat( i, j ) *= directions[ i ][ indices[ k ] ];
            }
        }

        // from TensorBase.h
        positionIterateSortedOneStep( order, 3, indices );
    }
    directions.clear();

    // we do not want more orientations than nessessary
    std::vector< WUnitSphereCoordinates< T > > ori2( orientations.begin(), orientations.begin() + numElements );

    WMatrix< T > p = pseudoInverse( TEMat );

    WAssert( ( TEMat*p ).isIdentity( 1e-3 ), "Test of inverse matrix failed. Are the given orientations linear independent?" );

    return p * calcBaseMatrix( ori2, order );
}

template< typename T >
void WSymmetricSphericalHarmonic< T >::normalize()
{
  T scale = 0.0;
  if( m_SHCoefficients.size() > 0 )
  {
      scale = std::sqrt( 4.0 * static_cast< T >( pi() ) ) * m_SHCoefficients[0];
  }
  for( size_t i = 0; i < m_SHCoefficients.size(); i++ )
  {
      m_SHCoefficients[ i ] /= scale;
  }
}

#endif  // WSYMMETRICSPHERICALHARMONIC_H
