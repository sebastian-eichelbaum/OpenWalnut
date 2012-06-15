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

std::string WPropertyWrapper::getFilename( bool notify ) const
{
    return m_prop->toPropFilename()->get( notify ).string();
}

int WPropertyWrapper::getSelection( bool notify ) const
{
    return static_cast< int >( m_prop->toPropSelection()->get( notify ).getItemIndexOfSelected( 0 ) );
}

void WPropertyWrapper::setBool( bool b )
{
    m_prop->toPropBool()->set( b, false );
}

void WPropertyWrapper::setInt( int i )
{
    m_prop->toPropInt()->set( i, false );
}

void WPropertyWrapper::setString( std::string const& s )
{
    m_prop->toPropString()->set( s, false );
}

void WPropertyWrapper::setDouble( double d )
{
    m_prop->toPropDouble()->set( d, false );
}

void WPropertyWrapper::setFilename( std::string const& fn )
{
    m_prop->toPropFilename()->set( boost::filesystem::path( fn ), false );
}

void WPropertyWrapper::setSelection( int s )
{
    WItemSelector::IndexList it;
    it.push_back( static_cast< std::size_t >( s ) );
    WItemSelector sel = m_prop->toPropSelection()->get().newSelector( it );
    m_prop->toPropSelection()->set( sel, false );
}

void WPropertyWrapper::click()
{
    m_prop->toPropTrigger()->set( WPVBaseTypes::PV_TRIGGER_TRIGGERED, false );
}

void WPropertyWrapper::waitForUpdate()
{
    m_prop->getUpdateCondition()->wait();
}
