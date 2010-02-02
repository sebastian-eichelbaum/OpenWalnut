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

#ifndef WPROPERTYVARIABLE_H
#define WPROPERTYVARIABLE_H

#include <stdint.h>

#include <string>
#include <list>

#include <boost/filesystem.hpp>

#include "WFlag.h"
#include "WPropertyBase.h"
#include "WCondition.h"

/**
 * A named property class with a concrete type.
 */
template< typename T >
class WPropertyVariable: public WFlag< T >,
                         public WPropertyBase
{
public:

    /**
     * Create an empty instance just containing a name.
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     */
    WPropertyVariable( std::string name, std::string description, const T& initial );

    /**
     * Create an empty instance just containing a name. This constructor allows an external condition to be used for notifiaction.
     * This is practical if one would like to share a condition among several properties.
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param condition use this external condition for notification.
     */
    WPropertyVariable( std::string name, std::string description, const T& initial, boost::shared_ptr< WCondition > condition );

    /**
     * Create an empty instance just containing a name. This constructor allows an external callback to be used for notification.
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     *
     * \note: instead of setting another notifier, you should consider using the callbacks the condition offers.
     * \note: the notifiers gets connected to the notification callback of the internal condition. So be careful when using the
     *        condition ( getCondition() ) for other properties, since they would also share the callbacks
     *
     */
    WPropertyVariable( std::string name, std::string description, const T& initial, WCondition::t_ConditionNotifierType notifier );

    /**
     * Create an empty instance just containing a name. This constructor allows an external callback and condition to be used for notification.
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param condition use this external condition for notification
     *
     * \note: instead of setting another notifier, you should consider using the callbacks the condition offers.
     * \note: the notifiers gets connected to the notification callback of the internal condition. So be careful when using the
     *        condition ( getCondition() ) for other properties, since they would also share the callbacks
     *
     */
    WPropertyVariable( std::string name, std::string description, const T& initial, boost::shared_ptr< WCondition > condition,
                       WCondition::t_ConditionNotifierType notifier );

    /**
     * Destructor.
     */
    virtual ~WPropertyVariable();

    /**
     * Determines whether the specified value is acceptable.
     *
     * \param newValue the new value.
     *
     * \return true if it is a valid/acceptable value.
     *
     * \note this currently is a dummy.
     */
    bool accept( T newValue );

protected:

    /**
     * The connection used for notification.
     */
    boost::signals2::connection m_notifierConnection;

    /**
     * Uses typeid() to set the proper type constant.
     */
    virtual void updateType();

private:
};

template < typename T >
WPropertyVariable< T >::WPropertyVariable( std::string name, std::string description, const T& initial ):
        WFlag< T >( new WCondition(), initial ),
        WPropertyBase( name, description )
{
}

template < typename T >
WPropertyVariable< T >::WPropertyVariable( std::string name, std::string description, const T& initial, boost::shared_ptr< WCondition > condition ):
        WFlag< T >( condition, initial ),
        WPropertyBase( name, description )
{
}

template < typename T >
WPropertyVariable< T >::WPropertyVariable( std::string name, std::string description, const T& initial,
                                           WCondition::t_ConditionNotifierType notifier ):
        WFlag< T >( new WCondition(), initial ),
        WPropertyBase( name, description )
{
    // set custom notifier
    m_notifierConnection = WFlag< T >::getCondition()->subscribeSignal( notifier );
}

template < typename T >
WPropertyVariable< T >::WPropertyVariable( std::string name, std::string description, const T& initial, boost::shared_ptr< WCondition > condition,
                                           WCondition::t_ConditionNotifierType notifier ):
        WFlag< T >( condition, initial ),
        WPropertyBase( name, description )
{
    // set custom notifier
    m_notifierConnection = WFlag< T >::getCondition()->subscribeSignal( notifier );
}

template < typename T >
WPropertyVariable< T >::~WPropertyVariable()
{
    // clean up
    m_notifierConnection.disconnect();
}

template < typename T >
bool WPropertyVariable< T >::accept( T newValue )
{
    // this currently is a dummy. Later this can be implemented to determine whether a value is valid.
    return true;
}

template < typename T >
void WPropertyVariable< T >::updateType()
{
    m_type = UNKNOWN;

    if ( typeid( int32_t ) == typeid( T ) )
    {
        m_type = INT;
    }
    else if ( typeid( double ) == typeid( T ) )
    {
        m_type = DOUBLE;
    }
    else if ( typeid( bool ) == typeid( T ) )
    {
        m_type = BOOL;
    }
    else if ( typeid( std::string ) == typeid( T ) )
    {
        m_type = STRING;
    }
    else if ( typeid( boost::filesystem::path ) == typeid( T ) )
    {
        m_type = PATH;
    }
    else if ( typeid( boost::filesystem::path ) == typeid( T ) )
    {
        m_type = LIST;
    }
}

/**
 * Some convenience type alias for a even more easy usage of WPropertyVariable
 */
/**
 * Alias for int32_t property variables.
 */
typedef WPropertyVariable< int32_t > WPropInt;

/**
 * Alias for int32_t property variables.
 */
typedef WPropertyVariable< double > WPropDouble;

/**
 * Alias for bool property variables.
 */
typedef WPropertyVariable< bool > WPropBool;

/**
 * Alias for string property variables.
 */
typedef WPropertyVariable< std::string > WPropString;

/**
 * Alias for filename property variables.
 */
typedef WPropertyVariable< boost::filesystem::path > WPropFilename;

/**
 * Alias for string list property variables.
 */
typedef WPropertyVariable< std::list< std::string > > WPropList;

#endif  // WPROPERTYVARIABLE_H

