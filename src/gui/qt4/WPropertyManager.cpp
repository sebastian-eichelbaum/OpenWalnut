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

#include <iostream>
#include <string>

#include <boost/thread/locks.hpp>

#include "WPropertyManager.h"

WPropertyManager::WPropertyManager()
{
}

WPropertyManager::~WPropertyManager()
{
}

void WPropertyManager::connectProperties( boost::shared_ptr< WProperties2 > properties )
{
    // make sure only one property gets connected at a time
    boost::mutex::scoped_lock lock( m_PropertiesLock );

    m_connectedProperties.push_back( properties );
}

void WPropertyManager::slotBoolChanged( QString name, bool value )
{
    boost::mutex::scoped_lock lock( m_PropertiesLock );

    for ( size_t i = 0; i < m_connectedProperties.size(); ++i )
    {
        if ( m_connectedProperties[i]->existsProperty( name.toStdString() ) )
        {
            m_connectedProperties[i]->getProperty( name.toStdString() )->toPropBool()->set( value );
        }
    }
}

void WPropertyManager::slotIntChanged( QString name, int value )
{
    boost::mutex::scoped_lock lock( m_PropertiesLock );
    for ( size_t i = 0; i < m_connectedProperties.size(); ++i )
    {
        if ( m_connectedProperties[i]->existsProperty( name.toStdString() ) )
        {
            m_connectedProperties[i]->getProperty( name.toStdString() )->toPropInt()->set( value );
        }
    }
}

void WPropertyManager::slotFloatChanged( QString name, float value )
{
    boost::mutex::scoped_lock lock( m_PropertiesLock );

    for ( size_t i = 0; i < m_connectedProperties.size(); ++i )
    {
        if ( m_connectedProperties[i]->existsProperty( name.toStdString() ) )
        {
            m_connectedProperties[i]->getProperty( name.toStdString() )->toPropDouble()->set( value );
        }
    }
}

void WPropertyManager::slotStringChanged( QString name, QString value )
{
    boost::mutex::scoped_lock lock( m_PropertiesLock );

    for ( size_t i = 0; i < m_connectedProperties.size(); ++i )
    {
        if ( m_connectedProperties[i]->existsProperty( name.toStdString() ) )
        {
            m_connectedProperties[i]->getProperty( name.toStdString() )->toPropString()->set( value.toStdString() );
        }
    }
}
