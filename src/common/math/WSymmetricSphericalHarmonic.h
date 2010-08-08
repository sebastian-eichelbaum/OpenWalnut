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

#include "WMath.h"
#include "WUnitSphereCoordinates.h"
#include "WValue.h"
#include "../WExportCommon.h"

namespace wmath
{
/**
 * Class for symmetric spherical harmonics
 * The index scheme of the coefficients/basis values is like in the Descoteaux paper "Regularized, Fast, and Robust Analytical Q-Ball Imaging".
 */
class OWCOMMON_EXPORT WSymmetricSphericalHarmonic  // NOLINT
{
// TODO(all): implement test
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
     */
    double getValue( double theta, double phi ) const;

    /**
     * Return the value on the sphere.
     * \param coordinates for the position on the unit sphere
     */
    double getValue( const WUnitSphereCoordinates& coordinates ) const;

    /**
     * Returns the used coefficients (stored like in the mentioned Descoteaux paper).
     */
    const wmath::WValue<double>& getCoefficients() const;

    /**
     * Applies the Funk-Radon-Transformation.
     */
    void applyFunkRadonTransformation();

    /**
     * Return the order of the spherical harmonic.
     */
    size_t getOrder() const;

protected:

private:
    /** order of the spherical harmonic */
    size_t m_order;
    /** coefficients of the spherical harmonic */
    WValue<double> m_SHCoefficients;
};
}

#endif  // WSYMMETRICSPHERICALHARMONIC_H
