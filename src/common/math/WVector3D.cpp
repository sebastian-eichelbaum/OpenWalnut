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

using wmath::WVector3D;

WVector3D::WVector3D()
    : WValue< double >( 3 )
{
}

WVector3D::WVector3D( double x, double y, double z )
    : WValue< double >( 3 )
{
    (*this)[0] = x;
    (*this)[1] = y;
    (*this)[2] = z;
}

WVector3D::WVector3D( const WVector3D& newVector )
    : WValue< double >( newVector )
{
}

WVector3D::WVector3D( const WValue< double >& newVector )
    : WValue< double >( newVector )
{
    WAssert( this->size() == 3, "Vector size must be 3!" );
}

double WVector3D::distanceSquare( const WVector3D &other ) const
{
    WAssert( this->size() == 3, "Vector size must be 3!" );
    WAssert( other.size() == 3, "Vector size must be 3!" );
    double dist = 0.0;
    double tmp = 0;
    for( size_t i = 0; i < 3; ++i )
    {
        tmp = (*this)[i] - other[i];
        dist += tmp * tmp;
    }
    return dist;
}

