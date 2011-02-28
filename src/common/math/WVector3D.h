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

#include <iomanip>
#include <string>
#include <vector>

#include <osg/Vec3d>

#include "../WExportCommon.h"
#include "../WLimits.h"
#include "../WStringUtils.h"

/**
 * Efficient three-dimensional vector that allows many vector algebra operations.
 * It is based on osg::vec3d
 *
 * \warning Do not introduce any virtual functions to this class! It might reduce performance significantly.
 */
class OWCOMMON_EXPORT WVector3D : public osg::Vec3d
{
public:
    /**
     * Copy constructor from osg::Vec3d
     *
     * \param vec This vector is used to initialize the constructed one.
     */
    inline WVector3D( osg::Vec3d vec );

    /**
     * Constructor that initializes all member with zero.
     */
    inline WVector3D();

    /**
     * Constructs a new vector from the three given value.
     *
     * \param x x-component of vector
     * \param y y-component of vector
     * \param z z-component of vector
     */
    inline WVector3D( osg::Vec3d::value_type x, osg::Vec3d::value_type y, osg::Vec3d::value_type z );

    /**
     * Calculate Euclidean square distance between this Position and another one.
     *
     * \param other The other position.
     * \return Square distance.
     */
    inline osg::Vec3d::value_type distanceSquare( const WVector3D &other ) const;

    /**
     * Returns the norm of the vector
     */
    inline osg::Vec3d::value_type norm() const;

    /**
     * Returns a normalized version of the vector
     */
    inline WVector3D normalized() const;

    /**
     * Compute the cross product of the current WVector3D with the parameter.
     * \param factor2 This vector will be multiplied with the current vector. (right hand side of the product)
     *
     * \return the crossproduct
     */
    const WVector3D crossProduct( const WVector3D& factor2 ) const;

    /**
     * Compute the dot product of the current WVector3D with the parameter.
     * \param factor2 This vector will be multiplied with the current vector. (right hand side of the product)
     */
    inline osg::Vec3d::value_type dotProduct( const WVector3D& factor2 ) const;

    /**
     * Sum of squares of elements.
     */
    inline osg::Vec3d::value_type normSquare() const;

    /**
     * Return number of elements.
     */
    inline size_t size() const;

    /**
     * Component-wise addition.
     * \param addend The right hand side of the addition
     */
    inline const WVector3D operator+( const WVector3D& addend ) const;

    /**
     * Component-wise subtraction.
     * \param subtrahend The right hand side of the subtraction
     */
    inline const WVector3D operator-( const WVector3D& subtrahend ) const;

private:
};

/**
 * Define WPosition as an alias for WVector3D
 */
typedef WVector3D WPosition;

/**
 * Writes a meaningful representation of that object to the given stream.
 *
 * \param os The operator will write to this stream.
 * \param rhs This will be written to the stream.
 *
 * \return the output stream
 */

inline std::ostream& operator<<( std::ostream& os, const WVector3D &rhs )
{
    os << "[" << std::scientific << std::setprecision( 16 );
    os << rhs.x() << ", " << rhs.y() << ", " << rhs.z() << "]";
    return os;
}

/**
 * Write an input stream into a WVector3D.
 *
 * \param in the input stream
 * \param rhs the value to where to write the stream
 *
 * \return the input stream
 */
inline std::istream& operator>>( std::istream& in, WVector3D &rhs )
{
    std::string str;
    in >> str;
    string_utils::trim( str, "[]" ); // remove preceeding and trailing brackets '[', ']' if any
    std::vector< std::string > tokens = string_utils::tokenize( str, ", " );
    for( size_t i = 0; i < tokens.size(); ++i )
    {
        rhs[i] = boost::lexical_cast< osg::Vec3d::value_type >( tokens[i] );
    }
    return in;
}

/**
 * Multiplies a WVector3D with a scalar
 * This functions only exists to make scalar multiplication commutative
 * \param lhs left hand side of product
 * \param rhs right hand side of product
 */
inline WVector3D operator*( osg::Vec3d::value_type lhs, const WVector3D& rhs )
{
    WVector3D result( rhs );
    result *= lhs;
    return result;
}

inline WVector3D::WVector3D( osg::Vec3d vec ) :
    osg::Vec3d( vec )
{
}

inline WVector3D::WVector3D() :
    osg::Vec3d()
{
}

inline WVector3D::WVector3D( osg::Vec3d::value_type x, osg::Vec3d::value_type y, osg::Vec3d::value_type z ) :
    osg::Vec3d( x, y, z )
{
}

inline const WVector3D WVector3D::operator+( const WVector3D& addend ) const
{
    WVector3D result( *this );
    result += addend;
    return result;
}

inline const WVector3D WVector3D::operator-( const WVector3D& subtrahend ) const
{
    WVector3D result( *this );
    result -= subtrahend;
    return result;
}

inline size_t WVector3D::size() const
{
    return num_components;
}

inline osg::Vec3d::value_type WVector3D::normSquare() const
{
    return this->length2();
}

inline osg::Vec3d::value_type WVector3D::dotProduct( const WVector3D& factor2 ) const
{
    return *this * factor2;
}

inline const WVector3D WVector3D::crossProduct( const WVector3D& factor2 ) const
{
    WVector3D result;
    result = ( *this ^ factor2 );
    return result;
}

inline WVector3D WVector3D::normalized() const
{
    WVector3D result = *this;
    result.normalize();
    return result;
}

inline osg::Vec3d::value_type WVector3D::norm() const
{
    return this->length();
}

inline osg::Vec3d::value_type WVector3D::distanceSquare( const WVector3D &other ) const
{
    osg::Vec3d::value_type dist = 0.0;
    osg::Vec3d::value_type tmp = 0;
    for( size_t i = 0; i < num_components; ++i )
    {
            tmp = (*this)[i] - other[i];
            dist += tmp * tmp;
    }
    return dist;
}

#endif  // WVECTOR3D_H
