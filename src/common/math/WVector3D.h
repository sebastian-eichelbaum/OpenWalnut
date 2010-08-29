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

#ifndef WVECTOR3D_H
#define WVECTOR3D_H

#include <string>
#include <vector>

#include <boost/lexical_cast.hpp>

#include "../WLimits.h"
#include "WValue.h"

#include "../WExportCommon.h"

namespace wmath
{
/**
 * Efficient three-dimensional vector that allows many vector algebra operations
 */
class OWCOMMON_EXPORT WVector3D : public WValue< double >
{
    /**
     * Only UnitTests are allowed to be friends of this class
     */
    friend class WVector3DTest;
public:
    /**
     * Produces a zero vector.
     */
    WVector3D();

    /**
     * Produces a vector consisting of the given components.
     * \param x first component of the vector
     * \param y second component of the vector
     * \param z third component of the vector
     */
    WVector3D( double x, double y, double z );

    /**
     * Copies the values of the given WVector3D.
     * \param newVector The vector to be copied
     */
    WVector3D( const WVector3D& newVector );

    /**
     * TODO(wiebel): Ticket #141 (How to use WValue-operators in child classes)
     * Copies the values of the given WValue< double >. Used for casting.
     * \param newVector The WValue to be cast to WVector3D
     */
    WVector3D( const WValue< double >& newVector );  // NOLINT because this constructor is intended for implicit casting

    /**
     * Compute the cross product of the current WValue with the parameter.
     * \param factor2 This vector will be multiplied with the current vector. (right hand side of the product)
     */
    WVector3D crossProduct( const WVector3D& factor2 ) const;

    /**
     * Compute the dot product of the current WValue with the parameter.
     * \param factor2 This vector will be multiplied with the current vector. (right hand side of the product)
     */
    double dotProduct( const WVector3D& factor2 ) const;

    /**
     * Calculate euclidean square distance between this Position and another one.
     *
     * \param other The other position.
     * \return Square distance.
     */
    double distanceSquare( const WVector3D &other ) const;

    /**
     * Determines if a position is "smaller" than another one.
     * \warning This ordering makes no sense at all, but is needed for a map or set!
     *
     * \param rhs Right hand side vector to compare with
     *
     * \return True if and only if the first, second or third components of the vectors are smaller
     */
    bool operator<( const wmath::WVector3D& rhs ) const;

protected:
private:
};

/**
 * Define WPosition as an alias for WVector3D
 */
typedef WVector3D WPosition;

inline WVector3D WVector3D::crossProduct( const WVector3D& factor2 ) const
{
    WVector3D result;
    result[0] = (*this)[1] * factor2[2] - (*this)[2] * factor2[1];
    result[1] = (*this)[2] * factor2[0] - (*this)[0] * factor2[2];
    result[2] = (*this)[0] * factor2[1] - (*this)[1] * factor2[0];
    return result;
}

inline double WVector3D::dotProduct( const WVector3D& factor2 ) const
{
    double result = 0.0;
    for( unsigned int i = 0; i < 3; ++i )
    {
        result += (*this)[i] * factor2[i];
    }
    return result;
}

inline bool WVector3D::operator<( const wmath::WVector3D& rhs ) const
{
    if( (*this)[0] < rhs[0] )
    {
        return true;
    }
    if( (*this)[0] > rhs[0] )
    {
        return false;
    }
    else
    {
        if( (*this)[1] < rhs[1] )
        {
            return true;
        }
        if( (*this)[1] > rhs[1] )
        {
            return false;
        }
        else
        {
            if( (*this)[2] < rhs[2] )
            {
                return true;
            }
            return false;
        }
    }
}
}  // End of namespace
#endif  // WVECTOR3D_H
