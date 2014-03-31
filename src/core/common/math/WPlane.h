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

#ifndef WPLANE_H
#define WPLANE_H

#include <set>

#include <boost/shared_ptr.hpp>

#include "../../dataHandler/WGridRegular3D.h"

#include "linearAlgebra/WVectorFixed.h"

/**
 * Represents a plane with a normal vector and a position in space.
 */
class WPlane // NOLINT
{
public:
    /**
     * Constructs a plane with its normal and containing the point.
     *
     * \param normal Direction of the plane
     * \param pos Position of the plane
     *
     * \return
     */
    WPlane( const WVector3d& normal, const WPosition& pos );

    /**
     * Constructs a plane with its normal and its base point/origin as well as explicitly specifying its vectors in the plane.
     *
     * \param normal Normal vector for the direction
     * \param pos Base point of the plane, aka origin.
     * \param first First vector perpendicular to the normal
     * \param second Second vector perpendicular to the normal and linearly independent from first.
     *
     * \note Due to numerical stability a comparison to 0.0 is not performed. Instead the absolute value of the dot product is checked to
     * be smaller than the FLT_EPS. FLT_EPS is used instead of DBL_EPS just numerical errors may sum up above DBL_EPS.
     */
    WPlane( const WVector3d& normal, const WPosition& pos, const WVector3d& first, const WVector3d& second );

    /**
     * Destructor.
     */
    virtual ~WPlane();

    /**
     * Determines whether a given point is in this plane or not.
     *
     * \param point Position to query
     *
     * \return True if and only if the point is in this plane.
     */
    bool isInPlane( WPosition point ) const;

    /**
     * Reset the position of the plane, normal remains the same.
     *
     * \param newPos New Position (point in plane).
     */
    void resetPosition( WPosition newPos );

    /**
     * Computes sample points on that plane.
     *
     * \param grid
     * \param stepWidth
     *
     * \return Set of positions on the plane
     */
    boost::shared_ptr< std::set< WPosition > > samplePoints( const WGridRegular3D& grid, double stepWidth );


    /**
     * Computes with relative coordinates a point in this plane. (0,0) means its position is returned.
     *
     * \param x how far along the direction of the first vector which spans the plane
     * \param y how far along the direction of the second vector which spans the plane too
     *
     * \return the new calculated position
     */
    WPosition getPointInPlane( double x, double y ) const;

    /**
     * Returns a point in that plane.
     *
     * \return The point in that plane describing its position
     */
    const WPosition& getPosition() const;

    /**
     * Returns the normal of the plane.
     *
     * \return Normalized normal vector.
     */
    const WVector3d& getNormal() const;

    /**
     * Resets the vector spanning the plane. Both must be linear independent and perpendicular to the already
     * existing normal vector. After setting the vectors they are normalized.
     *
     * \param first First vector spanning the plane
     * \param second Second vector spanning the plane
     */
    void setPlaneVectors( const WVector3d& first, const WVector3d& second );

    /**
     * Resets the normal of this plane.
     *
     * \param normal New normal for this plane.
     */
    void setNormal( const WVector3d& normal )
    {
        m_normal = normalize( normal );
        WVector3d gen( 1, 0, 0 );
        if( cross( normal, gen ) ==  WVector3d( 0, 0, 0 ) )
        {
            gen = WVector3d( 0, 1, 0 );
        }
        m_first = cross( normal, gen );
        m_first = normalize( m_first );
        m_second = cross( normal, m_first );
        m_second = normalize( m_second );
    }

    /**
     * Computes a fixed number of sample points on that plane.
     *
     * \param stepWidth
     * \param numX
     * \param numY
     *
     * \return Set of positions on the plane
     */
    boost::shared_ptr< std::set< WPosition > > samplePoints( double stepWidth, size_t numX, size_t numY ) const;

protected:
    WVector3d m_normal; //!< Direction of the plane
    WPosition m_pos; //!< Position of the plane specifying the center
    WVector3d m_first; //!< First vector in the plane
    WVector3d m_second; //!< Second vector in the plane

private:
};

inline const WPosition& WPlane::getPosition() const
{
    return m_pos;
}

inline const WVector3d& WPlane::getNormal() const
{
    return m_normal;
}

#endif  // WPLANE_H
