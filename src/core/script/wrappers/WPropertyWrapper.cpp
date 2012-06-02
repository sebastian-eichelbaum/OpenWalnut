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

#include <string>
#include <boost/shared_ptr.hpp>

#include "../../common/WProperties.h"

#include "WPropertyWrapper.h"

WPropertyWrapper::WPropertyWrapper( boost::shared_ptr< WPropertyBase > prop )
    : m_prop( prop )
{
}

std::string WPropertyWrapper::getName() const
{
    return m_prop->getName();
}

std::string WPropertyWrapper::getDescription() const
{
    return m_prop->getDescription();
}

bool WPropertyWrapper::getBool( bool notify ) const
{
    return m_prop->toPropBool()->get( notify );
}

int WPropertyWrapper::getInt( bool notify ) const
{
    return m_prop->toPropInt()->get( notify );
}

std::string WPropertyWrapper::getString( bool notify ) const
{
    return m_prop->toPropString()->get( notify );
}

double WPropertyWrapper::getDouble( bool notify ) const
{
    return m_prop->toPropDouble()->get( notify );
}

void WPropertyWrapper::setBool( bool b )
{
    m_prop->toPropBool()->set( b, true );
}

void WPropertyWrapper::setInt( int i )
{
    m_prop->toPropInt()->set( i, true );
}

void WPropertyWrapper::setString( std::string const& s )
{
    m_prop->toPropString()->set( s, true );
}

void WPropertyWrapper::setDouble( double d )
{
    m_prop->toPropDouble()->set( d, true );
}
