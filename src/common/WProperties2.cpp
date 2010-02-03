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
#include <map>
#include <string>
#include <vector>

#include "WLogger.h"
#include "exceptions/WPropertyUnknown.h"

#include "WProperties2.h"

WProperties2::WProperties2():
    m_iterationLock( boost::shared_lock< boost::shared_mutex >( m_updateLock ) )
{
    m_iterationLock.unlock();

    WPropertyVariable< double > p( "i", "d", 1.0 );
}

WProperties2::~WProperties2()
{
}

void WProperties2::addProperty( boost::shared_ptr< WPropertyBase > prop )
{
    boost::unique_lock< boost::shared_mutex > lock = boost::unique_lock< boost::shared_mutex >( m_updateLock );
    m_properties.insert( prop );
    lock.unlock();
}

bool WProperties2::existsProperty( std::string name )
{
    return ( findProperty( name ) != boost::shared_ptr< WPropertyBase >() );
}

boost::shared_ptr< WPropertyBase > WProperties2::getProperty( std::string name )
{
    boost::shared_ptr< WPropertyBase > p = findProperty( name );
    if ( findProperty( name ) == boost::shared_ptr< WPropertyBase >() )
    {
        throw WPropertyUnknown( "Property \"" + name + "\" can't be found." );
    }

    return p;
}

boost::shared_ptr< WPropertyBase > WProperties2::findProperty( std::string name )
{
    boost::shared_ptr< WPropertyBase > result = boost::shared_ptr< WPropertyBase >();

    // iterate over the items
    for ( PropertyIterator it = beginIteration(); it != getPropertyIteratorEnd(); ++it )
    {
        if ( ( *it )->getName() == name )
        {
            result = ( *it );
            break;
        }
    }
    endIteration();

    return result;
}

const WProperties2::PropertyIterator WProperties2::beginIteration()
{
    m_iterationLock.lock();
    return m_properties.begin();
}

void WProperties2::endIteration()
{
    m_iterationLock.unlock();
}

const WProperties2::PropertyIterator WProperties2::getPropertyIteratorEnd() const
{
    return m_properties.end();
}

