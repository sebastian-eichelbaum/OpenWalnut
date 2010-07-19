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

#include "../../common/math/WVector3D.h"

namespace wmath
{
/**
 * This class stores coordinates on the unit sphere.
 */
class WUnitSphereCoordinates
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
     * Constructor for euclidean coordinates.
     * \param vector euclidean coordinates
     */
    explicit WUnitSphereCoordinates( wmath::WVector3D vector );

    /**
     * Destructor.
     */
    virtual ~WUnitSphereCoordinates();

    /**
     * Return the theta angle.
     */
    double getTheta() const;

    /**
     * Return the phi angle.
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
     * Returns the stored sphere coordinates as euclidean coordinates.
     */
    wmath::WVector3D getEuclidean() const;

protected:

private:
    /** coordinate */
    double m_theta;
    /** coordinate */
    double m_phi;
};
}

#endif  // WUNITSPHERECOORDINATES_H
