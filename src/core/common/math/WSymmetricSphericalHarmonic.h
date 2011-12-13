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

#include <vector>

#include "../WExportCommon.h"
#include "linearAlgebra/WLinearAlgebra.h"
#include "WMath.h"
#include "WUnitSphereCoordinates.h"
#include "WMatrix.h"
#include "WValue.h"

/**
 * Class for symmetric spherical harmonics
 * The index scheme of the coefficients/basis values is like in the Descoteaux paper "Regularized, Fast, and Robust Analytical Q-Ball Imaging".
 */
class OWCOMMON_EXPORT WSymmetricSphericalHarmonic  // NOLINT
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
    explicit WSymmetricSphericalHarmonic( const WValue<double>& SHCoefficients );

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
    double getValue( double theta, double phi ) const;

    /**
     * Return the value on the sphere.
     * \param coordinates for the position on the unit sphere
     *
     * \return value on sphere
     */
    double getValue( const WUnitSphereCoordinates& coordinates ) const;

    /**
     * Returns the used coefficients (stored like in the mentioned 2007 Descoteaux paper).
     *
     * \return coefficient list
     */
    const WValue<double>& getCoefficients() const;

    /**
     * Returns the coefficients for Schultz' SH base.
     *
     * \return coefficient list
     */
    WValue< double > getCoefficientsSchultz() const;

    /**
     * Returns the coefficients for the complex base.
     *
     * \return coefficiend list
     */
    WValue< std::complex< double > > getCoefficientsComplex() const;

    /**
     * Applies the Funk-Radon-Transformation. This is faster than matrix multiplication.
     * ( O(n) instead of O(n²) )
     *
     * \param frtMat the frt matrix as calculated by calcFRTMatrix()
     */
    void applyFunkRadonTransformation( WMatrix< double > const& frtMat );

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
    double calcGFA( std::vector< WUnitSphereCoordinates > const& orientations ) const;

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
    double calcGFA( WMatrix< double > const& B ) const;

    /**
    * This calculates the transformation/fitting matrix T like in the 2007 Descoteaux paper. The orientations are given as WVector3d.
    * \param orientations The vector with the used orientation on the unit sphere (usually the gradients of the HARDI)
    * \param order The order of the spherical harmonics intended to create
    * \param lambda Regularization parameter for smoothing matrix
    * \param withFRT include the Funk-Radon-Transformation?
    * \return Transformation matrix
    */
    static WMatrix<double> getSHFittingMatrix( const std::vector< WVector3d >& orientations,
                                                      int order,
                                                      double lambda,
                                                      bool withFRT );

    /**
    * This calculates the transformation/fitting matrix T like in the 2007 Descoteaux paper. The orientations are given as WUnitSphereCoordinates .
    * \param orientations The vector with the used orientation on the unit sphere (usually the gradients of the HARDI)
    * \param order The order of the spherical harmonics intended to create
    * \param lambda Regularization parameter for smoothing matrix
    * \param withFRT include the Funk-Radon-Transformation?
    * \return Transformation matrix
    */
    static WMatrix<double> getSHFittingMatrix( const std::vector< WUnitSphereCoordinates >& orientations,
                                                      int order,
                                                      double lambda,
                                                      bool withFRT );

    /**
    * Calculates the base matrix B like in the dissertation of Descoteaux.
    * \param orientations The vector with the used orientation on the unit sphere (usually the gradients of the HARDI)
    * \param order The order of the spherical harmonics intended to create
    * \return The base Matrix B
    */
    static WMatrix<double> calcBaseMatrix( const std::vector< WUnitSphereCoordinates >& orientations, int order );

    /**
    * Calculates the base matrix B for the complex spherical harmonics.
    * \param orientations The vector with the used orientation on the unit sphere (usually the gradients of the HARDI)
    * \param order The order of the spherical harmonics intended to create
    * \return The base Matrix B
    */
    static WMatrix< std::complex< double > > calcComplexBaseMatrix( std::vector< WUnitSphereCoordinates > const& orientations,
                                                                           int order );

    /**
    * This calcs the smoothing matrix L from the 2007 Descoteaux Paper "Regularized, Fast, and Robust Analytical Q-Ball Imaging"
    * \param order The order of the spherical harmonic
    * \return The smoothing matrix L
    */
    static WMatrix<double> calcSmoothingMatrix( size_t order );

    /**
    * Calculates the Funk-Radon-Transformation-Matrix P from the 2007 Descoteaux Paper "Regularized, Fast, and Robust Analytical Q-Ball Imaging"
    * \param order The order of the spherical harmonic
    * \return The Funk-Radon-Matrix P
    */
    static WMatrix<double> calcFRTMatrix( size_t order );

   /**
     * Calculates a matrix that converts spherical harmonics to symmetric tensors of equal or lower order.
     *
     * \param order The order of the symmetric tensor.
     * \param orientations A vector of at least (orderTensor+1) * (orderTensor+2) / 2 orientations.
     *
     * \return the conversion matrix
     */
    static WMatrix< double > calcSHToTensorSymMatrix( std::size_t order, const std::vector< WUnitSphereCoordinates >& orientations );

    /**
     * Normalize this SH in place.
     */
    void normalize();

protected:
private:
    /** order of the spherical harmonic */
    size_t m_order;

    /** coefficients of the spherical harmonic */
    WValue<double> m_SHCoefficients;
};

#endif  // WSYMMETRICSPHERICALHARMONIC_H
