//---------------------------------------------------------------------------
//
// Project: OpenWalnut
//
// Copyright 2009 SomeCopyrightowner
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
#include "WValue.hpp"

WVector3D::WVector3D()
    : WValue<double>( 3 )
{
}

WVector3D::WVector3D( double x, double y, double z )
    : WValue<double>( 3 )
{
    m_components[0] = x;
    m_components[1] = y;
    m_components[2] = z;
}

WVector3D::WVector3D( const WVector3D & newVector )
    : WValue<double>( newVector )
{
}

double& WVector3D::operator[]( size_t i )
{
    return m_components[i];
}

const double& WVector3D::operator[]( size_t i ) const
{
    return m_components[i];
}










