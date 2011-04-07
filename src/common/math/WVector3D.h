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

#include "../WDefines.h"
#include "../../ext/Eigen/Core"

/**
 * The new vector type. It is a stack-allocated double vector with three dimension.
 *
 * \see http://eigen.tuxfamily.org/dox/classEigen_1_1Matrix.html
 * \see http://eigen.tuxfamily.org/dox/classEigen_1_1MatrixBase.html
 */
typedef Eigen::Matrix< double, 3, 1 > WVector3D_2;

/**
 * The new vector type. It is a stack-allocated double vector with three dimension.
 *
 * \see http://eigen.tuxfamily.org/dox/classEigen_1_1Matrix.html
 * \see http://eigen.tuxfamily.org/dox/classEigen_1_1MatrixBase.html
 */
typedef Eigen::Matrix< double, 2, 1 > WVector2D_2;

/**
 * Define WPosition as an alias for WVector3D_2
 *
 * \see http://eigen.tuxfamily.org/dox/classEigen_1_1Matrix.html
 * \see http://eigen.tuxfamily.org/dox/classEigen_1_1MatrixBase.html
 */
typedef WVector3D_2 WPosition_2;

/**
 * The new dynamic vector type. It is a heap-allocated double vector with dynamic size.
 *
 * \see http://eigen.tuxfamily.org/dox/classEigen_1_1Matrix.html
 * \see http://eigen.tuxfamily.org/dox/classEigen_1_1MatrixBase.html
 */
typedef Eigen::Matrix< double, Eigen::Dynamic, 1 > WVector_2;

/**
 * A complex double vector of dynamic size. Heap-allocated.
 *
 * \see http://eigen.tuxfamily.org/dox/classEigen_1_1Matrix.html
 * \see http://eigen.tuxfamily.org/dox/classEigen_1_1MatrixBase.html
 */
typedef Eigen::VectorXcd WVectorComplex_2;

/**
 * Forward declare the OLD vector class. Needed for the conversion functions.
 */
class WVector3D;

/**
 * Converts a WVector3D_2 to a osg::Vec3f which is commonly used in many modules.
 *
 * \param v the vector to convert
 *
 * \return the osg::Vec3f instance.
 */
osg::Vec3f toOsgVec3f( const WVector3D_2& v );

/**
 * Convert a WVector3D_2 to WVector3D.
 *
 * \param v the vector to convert
 *
 * \return the new vector
 */
WVector3D toWVector3D( const WVector3D_2& v );

/**
 * Convert a WVector3D to an WVector3D_2
 *
 * \param v the vector to convert
 *
 * \return the new vector
 */
WVector3D_2 toWVector3D_2( const WVector3D& v );

/**
 * Convert an osg::Vec3f to an WVector3D_2
 *
 * \param v the vector to convert
 *
 * \return the new vector
 */
WVector3D_2 toWVector3D_2( const osg::Vec3f& v );

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
    WVector3D( osg::Vec3d vec );

    /**
     * Constructor that initializes all member with zero.
     */
    WVector3D();

    /**
     * Constructs a new vector from the three given value.
     *
     * \param x x-component of vector
     * \param y y-component of vector
     * \param z z-component of vector
     */
    WVector3D( osg::Vec3d::value_type x, osg::Vec3d::value_type y, osg::Vec3d::value_type z );

    // NOTE: osg::Vec3d members made deprecated.
    OW_API_DEPRECATED double length() const { return osg::Vec3d::length(); } // NOLINT
    OW_API_DEPRECATED double length2() const { return osg::Vec3d::length2(); } // NOLINT
    OW_API_DEPRECATED double normalize() { return osg::Vec3d::normalize(); } // NOLINT
    OW_API_DEPRECATED void 	set (double x, double y, double z) { osg::Vec3d::set( x, y, z ); } // NOLINT
    OW_API_DEPRECATED void 	set (const Vec3d &rhs) { osg::Vec3d::set( rhs ); } // NOLINT
    OW_API_DEPRECATED double & 	operator[] (int i) { return osg::Vec3d::operator[]( i ); } // NOLINT
    OW_API_DEPRECATED double 	operator[] (int i) const  { return osg::Vec3d::operator[]( i ); } // NOLINT
    OW_API_DEPRECATED double & 	x ()  { return osg::Vec3d::x(); } // NOLINT
    OW_API_DEPRECATED double & 	y ()  { return osg::Vec3d::y(); } // NOLINT
    OW_API_DEPRECATED double & 	z ()  { return osg::Vec3d::z(); } // NOLINT
    OW_API_DEPRECATED double 	x () const { return osg::Vec3d::x(); } // NOLINT
    OW_API_DEPRECATED double 	y () const { return osg::Vec3d::y(); } // NOLINT
    OW_API_DEPRECATED double 	z () const { return osg::Vec3d::z(); } // NOLINT
    OW_API_DEPRECATED bool valid() const { return osg::Vec3d::valid(); } // NOLINT
    OW_API_DEPRECATED bool isNaN() const { return osg::Vec3d::isNaN(); } // NOLINT

    /**
     * Calculate Euclidean square distance between this Position and another one.
     *
     * \param other The other position.
     * \return Square distance.
     */
    OW_API_DEPRECATED osg::Vec3d::value_type distanceSquare( const WVector3D &other ) const;

    /**
     * Returns the norm of the vector
     */
    OW_API_DEPRECATED osg::Vec3d::value_type norm() const;

    /**
     * Returns a normalized version of the vector
     */
    OW_API_DEPRECATED WVector3D normalized() const;

    /**
     * Compute the cross product of the current WVector3D with the parameter.
     * \param factor2 This vector will be multiplied with the current vector. (right hand side of the product)
     *
     * \return the crossproduct
     */
    OW_API_DEPRECATED const WVector3D crossProduct( const WVector3D& factor2 ) const;

    /**
     * Compute the dot product of the current WVector3D with the parameter.
     * \param factor2 This vector will be multiplied with the current vector. (right hand side of the product)
     */
    OW_API_DEPRECATED osg::Vec3d::value_type dotProduct( const WVector3D& factor2 ) const;

    /**
     * Sum of squares of elements.
     */
    OW_API_DEPRECATED osg::Vec3d::value_type normSquare() const;

    /**
     * Return number of elements.
     */
    OW_API_DEPRECATED size_t size() const;

    /**
     * Component-wise addition.
     * \param addend The right hand side of the addition
     */
    OW_API_DEPRECATED const WVector3D operator+( const WVector3D& addend ) const;

    /**
     * Component-wise subtraction.
     * \param subtrahend The right hand side of the subtraction
     */
    OW_API_DEPRECATED const WVector3D operator-( const WVector3D& subtrahend ) const;

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
OW_API_DEPRECATED std::ostream& operator<<( std::ostream& os, const WVector3D &rhs );


/**
 * Write an input stream into a WVector3D.
 *
 * \param in the input stream
 * \param rhs the value to where to write the stream
 *
 * \return the input stream
 */
OW_API_DEPRECATED std::istream& operator>>( std::istream& in, WVector3D &rhs );

/**
 * Multiplies a WVector3D with a scalar
 * This functions only exists to make scalar multiplication commutative
 * \param lhs left hand side of product
 * \param rhs right hand side of product
 */
OW_API_DEPRECATED WVector3D operator*( osg::Vec3d::value_type lhs, const WVector3D& rhs );

#endif  // WVECTOR3D_H
