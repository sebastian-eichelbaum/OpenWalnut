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

#ifndef WUNITSPHERECOORDINATES_H
#define WUNITSPHERECOORDINATES_H

#include <vector>

#include "../../common/math/linearAlgebra/WLinearAlgebra.h"
#include "../../common/WExportCommon.h"


/**
 * This class stores coordinates on the unit sphere.
 */
class OWCOMMON_EXPORT WUnitSphereCoordinates // NOLINT
{
// TODO(all): implement test
// friend class WUnitSphereCoordinatesTest;
public:
    /**
     * Default constructor.
     */
    WUnitSphereCoordinates();

    /**
     * Constructor for unit sphere angles.
     * \param theta coordinate
     * \param phi coordinate
     */
    WUnitSphereCoordinates( double theta, double phi );

    /**
     * Constructor for Euclidean coordinates.
     * \param vector Euclidean coordinates
     */
    explicit WUnitSphereCoordinates( WVector3d vector );

    /**
     * Destructor.
     */
    virtual ~WUnitSphereCoordinates();

    /**
     * Return the theta angle.
     *
     * \return theta angle
     */
    double getTheta() const;

    /**
     * Return the phi angle.
     *
     * \return phi angle
     */
    double getPhi() const;

    /**
     * Set theta angle.
     * \param theta Value for theta.
     */
    void setTheta( double theta );

    /**
     * Set phi angle.
     * \param phi Value for phi.
     */
    void setPhi( double phi );

    /**
     * Returns the stored sphere coordinates as Euclidean coordinates.
     *
     * \return sphere coordinates in euclidean space
     */
    WVector3d getEuclidean() const;

protected:

private:
    /** coordinate */
    double m_theta;
    /** coordinate */
    double m_phi;
};

#endif  // WUNITSPHERECOORDINATES_H
