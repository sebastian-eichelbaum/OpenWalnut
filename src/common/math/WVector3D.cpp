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

#include "WVector3D.h"

osg::Vec3f toOsgVec3f( const WVector3D_2& v )
{
    return osg::Vec3f( v( 0 ), v( 1 ), v( 2 ) );
}

WVector3D toWVector3D( const WVector3D_2& v )
{
    return WVector3D( v( 0 ), v( 1 ), v( 2 ) );
}

WVector3D_2 toWVector3D_2( const WVector3D& v )
{
    return WVector3D_2( v[ 0 ], v[ 1 ], v[ 2 ] );
}

WVector3D_2 toWVector3D_2( const osg::Vec3f& v )
{
    return WVector3D_2( v[ 0 ], v[ 1 ], v[ 2 ] );
}

WVector3D::WVector3D( osg::Vec3d vec ) :
    osg::Vec3d( vec )
{
}

WVector3D::WVector3D() :
    osg::Vec3d()
{
}

WVector3D::WVector3D( osg::Vec3d::value_type x, osg::Vec3d::value_type y, osg::Vec3d::value_type z ) :
    osg::Vec3d( x, y, z )
{
}

const WVector3D WVector3D::operator+( const WVector3D& addend ) const
{
    WVector3D result( *this );
    result += addend;
    return result;
}

const WVector3D WVector3D::operator-( const WVector3D& subtrahend ) const
{
    WVector3D result( *this );
    result -= subtrahend;
    return result;
}

size_t WVector3D::size() const
{
    return num_components;
}

osg::Vec3d::value_type WVector3D::normSquare() const
{
    return this->length2();
}

osg::Vec3d::value_type WVector3D::dotProduct( const WVector3D& factor2 ) const
{
    return *this * factor2;
}

const WVector3D WVector3D::crossProduct( const WVector3D& factor2 ) const
{
    WVector3D result;
    result = ( *this ^ factor2 );
    return result;
}

WVector3D WVector3D::normalized() const
{
    WVector3D result = *this;
    result.normalize();
    return result;
}

osg::Vec3d::value_type WVector3D::norm() const
{
    return this->length();
}

osg::Vec3d::value_type WVector3D::distanceSquare( const WVector3D &other ) const
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

std::ostream& operator<<( std::ostream& os, const WVector3D &rhs )
{
    os << "[" << std::scientific << std::setprecision( 16 );
    os << rhs.x() << ", " << rhs.y() << ", " << rhs.z() << "]";
    return os;
}

std::istream& operator>>( std::istream& in, WVector3D &rhs )
{
    char vec[256];
    in.getline( vec, 256 );
    std::string str = std::string( vec );
    str = string_utils::trim( str, "[]" ); // remove preceeding and trailing brackets '[', ']' if any
    std::vector< std::string > tokens = string_utils::tokenize( str, ", " );
    for( size_t i = 0; i < tokens.size(); ++i )
    {
        rhs[i] = boost::lexical_cast< osg::Vec3d::value_type >( tokens[i] );
    }
    return in;
}

WVector3D operator*( osg::Vec3d::value_type lhs, const WVector3D& rhs )
{
    WVector3D result( rhs );
    result *= lhs;
    return result;
}

