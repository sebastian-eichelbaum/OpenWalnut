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
}

WProperties2::~WProperties2()
{
}

void WProperties2::addProperty( boost::shared_ptr< WPropertyBase > prop )
{
    boost::unique_lock< boost::shared_mutex > lock = boost::unique_lock< boost::shared_mutex >( m_updateLock );
    m_properties.push_back( prop );
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


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// convenience methods for
// template< typename T>
// boost::shared_ptr< WPropertyVariable< T > > addProperty( std::string name, std::string description, const T& initial, bool hide = false );
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WPropBool WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_BOOL&   initial, bool hide )
{
    return addProperty< WPVBaseTypes::PV_BOOL >( name, description, initial, hide );
}

WPropInt WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_INT&    initial, bool hide )
{
    return addProperty< WPVBaseTypes::PV_INT >( name, description, initial, hide );
}

WPropDouble WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_DOUBLE& initial, bool hide )
{
    return addProperty< WPVBaseTypes::PV_DOUBLE >( name, description, initial, hide );
}

WPropString WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_STRING& initial, bool hide )
{
    return addProperty< WPVBaseTypes::PV_STRING >( name, description, initial, hide );
}

WPropFilename WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_PATH&   initial, bool hide )
{
    return addProperty< WPVBaseTypes::PV_PATH >( name, description, initial, hide );
}

WPropList WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_LIST&   initial, bool hide )
{
    return addProperty< WPVBaseTypes::PV_LIST >( name, description, initial, hide );
}

WPropPosition WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_POSITION&   initial, bool hide )
{
    return addProperty< WPVBaseTypes::PV_POSITION >( name, description, initial, hide );
}

WPropColor WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_COLOR&   initial, bool hide )
{
    return addProperty< WPVBaseTypes::PV_COLOR >( name, description, initial, hide );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// convenience methods for
// template< typename T>
// boost::shared_ptr< WPropertyVariable< T > > addProperty( std::string name, std::string description, const T& initial,
//                                                          boost::shared_ptr< WCondition > condition, bool hide = false );
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WPropBool WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_BOOL&   initial,
                                     boost::shared_ptr< WCondition > condition, bool hide )
{
    return addProperty< WPVBaseTypes::PV_BOOL >( name, description, initial, condition, hide );
}

WPropInt WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_INT&    initial,
                                    boost::shared_ptr< WCondition > condition, bool hide )
{
    return addProperty< WPVBaseTypes::PV_INT >( name, description, initial, condition, hide );
}

WPropDouble WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_DOUBLE& initial,
                                       boost::shared_ptr< WCondition > condition, bool hide )
{
    return addProperty< WPVBaseTypes::PV_DOUBLE >( name, description, initial, condition, hide );
}

WPropString WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_STRING& initial,
                                       boost::shared_ptr< WCondition > condition, bool hide )
{
    return addProperty< WPVBaseTypes::PV_STRING >( name, description, initial, condition, hide );
}

WPropFilename WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_PATH&   initial,
                                         boost::shared_ptr< WCondition > condition, bool hide )
{
    return addProperty< WPVBaseTypes::PV_PATH >( name, description, initial, condition, hide );
}

WPropList WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_LIST&   initial,
                                     boost::shared_ptr< WCondition > condition, bool hide )
{
    return addProperty< WPVBaseTypes::PV_LIST >( name, description, initial, condition, hide );
}

WPropPosition WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_POSITION&   initial,
                                         boost::shared_ptr< WCondition > condition, bool hide )
{
    return addProperty< WPVBaseTypes::PV_POSITION >( name, description, initial, condition, hide );
}

WPropColor WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_COLOR&   initial,
                                     boost::shared_ptr< WCondition > condition, bool hide )
{
    return addProperty< WPVBaseTypes::PV_COLOR >( name, description, initial, condition, hide );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// convenience methods for
// template< typename T>
// boost::shared_ptr< WPropertyVariable< T > > addProperty( std::string name, std::string description, const T& initial,
//                                                          WCondition::t_ConditionNotifierType notifier, bool hide = false );
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WPropBool WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_BOOL&   initial,
                                     WCondition::t_ConditionNotifierType notifier, bool hide )
{
    return addProperty< WPVBaseTypes::PV_BOOL >( name, description, initial, notifier, hide );
}

WPropInt WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_INT&    initial,
                                    WCondition::t_ConditionNotifierType notifier, bool hide )
{
    return addProperty< WPVBaseTypes::PV_INT >( name, description, initial, notifier, hide );
}

WPropDouble WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_DOUBLE& initial,
                                       WCondition::t_ConditionNotifierType notifier, bool hide )
{
    return addProperty< WPVBaseTypes::PV_DOUBLE >( name, description, initial, notifier, hide );
}

WPropString WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_STRING& initial,
                                       WCondition::t_ConditionNotifierType notifier, bool hide )
{
    return addProperty< WPVBaseTypes::PV_STRING >( name, description, initial, notifier, hide );
}

WPropFilename WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_PATH&   initial,
                                         WCondition::t_ConditionNotifierType notifier, bool hide )
{
    return addProperty< WPVBaseTypes::PV_PATH >( name, description, initial, notifier, hide );
}

WPropList WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_LIST&   initial,
                                     WCondition::t_ConditionNotifierType notifier, bool hide )
{
    return addProperty< WPVBaseTypes::PV_LIST >( name, description, initial, notifier, hide );
}

WPropPosition WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_POSITION&   initial,
                                         WCondition::t_ConditionNotifierType notifier, bool hide )
{
    return addProperty< WPVBaseTypes::PV_POSITION >( name, description, initial, notifier, hide );
}

WPropColor WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_COLOR&   initial,
                                     WCondition::t_ConditionNotifierType notifier, bool hide )
{
    return addProperty< WPVBaseTypes::PV_COLOR >( name, description, initial, notifier, hide );
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// convenience methods for
// template< typename T>
// boost::shared_ptr< WPropertyVariable< T > > addProperty( std::string name, std::string description, const T& initial,
//                                                          boost::shared_ptr< WCondition > condition,
//                                                          WCondition::t_ConditionNotifierType notifier, bool hide = false );
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WPropBool WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_BOOL&   initial,
                                     boost::shared_ptr< WCondition > condition,
                                     WCondition::t_ConditionNotifierType notifier, bool hide )
{
    return addProperty< WPVBaseTypes::PV_BOOL >( name, description, initial, condition, notifier, hide );
}

WPropInt WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_INT&    initial,
                                     boost::shared_ptr< WCondition > condition,
                                     WCondition::t_ConditionNotifierType notifier, bool hide )
{
    return addProperty< WPVBaseTypes::PV_INT >( name, description, initial, condition, notifier, hide );
}

WPropDouble WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_DOUBLE& initial,
                                     boost::shared_ptr< WCondition > condition,
                                     WCondition::t_ConditionNotifierType notifier, bool hide )
{
    return addProperty< WPVBaseTypes::PV_DOUBLE >( name, description, initial, condition, notifier, hide );
}

WPropString WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_STRING& initial,
                                     boost::shared_ptr< WCondition > condition,
                                     WCondition::t_ConditionNotifierType notifier, bool hide )
{
    return addProperty< WPVBaseTypes::PV_STRING >( name, description, initial, condition, notifier, hide );
}

WPropFilename WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_PATH&   initial,
                                     boost::shared_ptr< WCondition > condition,
                                     WCondition::t_ConditionNotifierType notifier, bool hide )
{
    return addProperty< WPVBaseTypes::PV_PATH >( name, description, initial, condition, notifier, hide );
}

WPropList WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_LIST&   initial,
                                     boost::shared_ptr< WCondition > condition,
                                     WCondition::t_ConditionNotifierType notifier, bool hide )
{
    return addProperty< WPVBaseTypes::PV_LIST >( name, description, initial, condition, notifier, hide );
}

WPropPosition WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_POSITION&   initial,
                                         boost::shared_ptr< WCondition > condition,
                                         WCondition::t_ConditionNotifierType notifier, bool hide )
{
    return addProperty< WPVBaseTypes::PV_POSITION >( name, description, initial, condition, notifier, hide );
}

WPropColor WProperties2::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_COLOR&   initial,
                                     boost::shared_ptr< WCondition > condition,
                                     WCondition::t_ConditionNotifierType notifier, bool hide )
{
    return addProperty< WPVBaseTypes::PV_COLOR >( name, description, initial, condition, notifier, hide );
}

