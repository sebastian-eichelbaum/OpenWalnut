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

#include <cmath>

#include "WUnitSphereCoordinates.h"

WUnitSphereCoordinates::WUnitSphereCoordinates()
    : m_theta( 0.0 ),
      m_phi( 0.0 )
{
}

WUnitSphereCoordinates::WUnitSphereCoordinates( double theta, double phi )
    : m_theta( theta ),
      m_phi( phi )
{
}

WUnitSphereCoordinates::WUnitSphereCoordinates( WVector3d vector )
{
    vector = normalize( vector );
    // calculate angles
    m_theta = std::acos( vector[2] );
    m_phi = std::atan2( vector[1], vector[0] );
}

WUnitSphereCoordinates::~WUnitSphereCoordinates()
{
}

double WUnitSphereCoordinates::getTheta() const
{
    return m_theta;
}

double WUnitSphereCoordinates::getPhi() const
{
    return m_phi;
}

void WUnitSphereCoordinates::setTheta( double theta )
{
    m_theta = theta;
}

void WUnitSphereCoordinates::setPhi( double phi )
{
    m_phi = phi;
}

WVector3d WUnitSphereCoordinates::getEuclidean() const
{
    return WVector3d( std::sin( m_theta )*std::cos( m_phi ), std::sin( m_theta )*std::sin( m_phi ), std::cos( m_theta ) );
}
