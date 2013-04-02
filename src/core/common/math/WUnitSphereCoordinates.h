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

#include <cmath>
#include <vector>

#include "../../common/math/linearAlgebra/WMatrixFixed.h"

/**
 * This class stores coordinates on the unit sphere.
 */

template< typename T > class WUnitSphereCoordinates // NOLINT
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
    WUnitSphereCoordinates( T theta, T phi );

    /**
     * Constructor for Euclidean coordinates.
     * \param vector Euclidean coordinates
     */
    explicit WUnitSphereCoordinates( const WMatrixFixed< T, 3, 1 >& vector );

    /**
     * Destructor.
     */
    virtual ~WUnitSphereCoordinates();

    /**
     * Return the theta angle.
     *
     * \return theta angle
     */
    T getTheta() const;

    /**
     * Return the phi angle.
     *
     * \return phi angle
     */
    T getPhi() const;

    /**
     * Set theta angle.
     * \param theta Value for theta.
     */
    void setTheta( T theta );

    /**
     * Set phi angle.
     * \param phi Value for phi.
     */
    void setPhi( T phi );

    /**
     * Returns the stored sphere coordinates as Euclidean coordinates.
     *
     * \return sphere coordinates in euclidean space
     */
    WMatrixFixed< T, 3, 1 > getEuclidean() const;

    /**
     * Returns the stored sphere coordinates as Euclidean coordinates.
     *
     * \param vector coordinates in euclidean space
     */
    void setEuclidean( WMatrixFixed< T, 3, 1 > vector );

protected:
private:
    /** coordinate */
    T m_theta;
    /** coordinate */
    T m_phi;
};

template< typename T >
WUnitSphereCoordinates< T >::WUnitSphereCoordinates()
    : m_theta( 0.0 ),
      m_phi( 0.0 )
{
}

template< typename T >
WUnitSphereCoordinates< T >::WUnitSphereCoordinates( T theta, T phi )
    : m_theta( theta ),
      m_phi( phi )
{
}

template< typename T >
WUnitSphereCoordinates< T >::WUnitSphereCoordinates( const WMatrixFixed< T, 3, 1 >& vector )
{
    setEuclidean( vector );
}

template< typename T >
WUnitSphereCoordinates< T >::~WUnitSphereCoordinates()
{
}

template< typename T >
T WUnitSphereCoordinates< T >::getTheta() const
{
    return m_theta;
}

template< typename T >
T WUnitSphereCoordinates< T >::getPhi() const
{
    return m_phi;
}

template< typename T >
void WUnitSphereCoordinates< T >::setTheta( T theta )
{
    m_theta = theta;
}

template< typename T >
void WUnitSphereCoordinates< T >::setPhi( T phi )
{
    m_phi = phi;
}

template< typename T >
WMatrixFixed< T, 3, 1 > WUnitSphereCoordinates< T >::getEuclidean() const
{
    return WMatrixFixed< T, 3, 1 >( std::sin( m_theta )*std::cos( m_phi ), std::sin( m_theta )*std::sin( m_phi ), std::cos( m_theta ) );
}

template< typename T >
void WUnitSphereCoordinates< T >::setEuclidean( WMatrixFixed< T, 3, 1 > vector )
{
    vector = normalize( vector );
    // calculate angles
    m_theta = std::acos( vector[2] );
    m_phi = std::atan2( vector[1], vector[0] );
}

#endif  // WUNITSPHERECOORDINATES_H
