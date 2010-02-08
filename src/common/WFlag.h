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

#ifndef WFLAG_H
#define WFLAG_H

#include "WCondition.h"

/**
 * Class to have a simple notification/condition system for simple flags. This is somewhat similar to the observer design pattern.
 * The type of the flag is specified by the template parameter. Per default, it is of type bool.
 */
template < typename T >
class WFlag
{
public:

    /**
     * Constructor. Uses a given condition to realize the wait/notify functionality. By using this constructor, the specified
     * condition gets deleted whenever this WFlag is deleted.
     *
     * \param condition the condition to use.
     * \note condition can also be a WConditionOneShot.
     * \param initial the initial value of this flag.
     */
    WFlag( WCondition* condition, T initial );

    /**
     * Constructor. Uses a given condition to realize the wait/notify functionality. By using this constructor, the specified
     * condition gets NOT explicitely deleted when this WFlag gets deleted.
     *
     * \param condition the condition to use.
     * \note condition can also be a WConditionOneShot.
     * \param initial the initial value of this flag.
     */
    WFlag( boost::shared_ptr< WCondition > condition, T initial );

    /**
     * Destructor. It deletes the instance of WCondition specified on construction.
     */
    virtual ~WFlag();

    /**
     * Operator returns value of the flag.
     *
     * \return the value.
     */
    virtual const T get() const;

    /**
     * Operator returns value of the flag.
     *
     * \return the value.
     */
    virtual const T operator()() const;

    /**
     * Wait for the flag to change its value. For WConditionOneShot is also recognizes if the flag has changed before.
     */
    virtual void wait() const;

    /**
     * Sets the new value for this flag. Also notifies waiting threads.
     *
     * \param value the new value
     * \param suppressNotification true to avoid a firing condition. This is useful for resetting values.
     *
     * \return true if the value has been set successfully.
     *
     * \note set( get() ) == true
     */
    virtual bool set( T value, bool suppressNotification = false );

    /**
     * Sets the new value for this flag. Also notifies waiting threads.
     *
     * \param value the new value
     */
    virtual void operator()( T value );

    /**
     * Returns the condition that is used by this flag.
     *
     * \return the condition
     */
    boost::shared_ptr< WCondition > getCondition();

    /**
     * Determines whether the specified value is acceptable. In WFlags, this always returns true. To modify the behaviour,
     * implement this function in an appropriate way.
     *
     * \param newValue the new value.
     *
     * \return true if it is a valid/acceptable value.
     */
    virtual bool accept( T newValue );

protected:

    /**
     * The condition to be used for waiting/notifying. Please note, that it gets deleted during destruction.
     */
    boost::shared_ptr< WCondition > m_condition;

    /**
     * The flag value.
     */
    T m_flag;

private:
};

/**
 * Alias for easy usage of WFLag< bool >.
 */
typedef WFlag< bool > WBoolFlag;

template < typename T >
WFlag< T >::WFlag( WCondition* condition, T initial )
{
    m_condition = boost::shared_ptr< WCondition >( condition );
    m_flag = initial;
}

template < typename T >
WFlag< T >::WFlag( boost::shared_ptr< WCondition > condition, T initial )
{
    m_condition = condition;
    m_flag = initial;
}

template < typename T >
WFlag< T >::~WFlag()
{
}

template < typename T >
const T WFlag< T >::operator()() const
{
    return get();
}

template < typename T >
const T WFlag< T >::get() const
{
    return m_flag;
}

template < typename T >
void WFlag< T >::wait() const
{
    m_condition->wait();
}

template < typename T >
void WFlag< T >::operator()( T value )
{
    set( value );
}

template < typename T >
bool WFlag< T >::set( T value, bool suppressNotification )
{
    // if the value is the same as the current one -> do not notify but let the caller know "all ok"
    if ( m_flag == value )
    {
        return true;
    }

    // let the caller know whether the value was acceptable.
    if ( !accept( value ) )
    {
        return false;
    }

    m_flag = value;

    // is the notification suppressed ?
    if ( !suppressNotification )
    {
        m_condition->notify();
    }

    return true;
}

template < typename T >
boost::shared_ptr< WCondition > WFlag< T >::getCondition()
{
    return m_condition;
}

template < typename T >
bool WFlag< T >::accept( T /* newValue */ )
{
    // please implement this method in your class to modify the behaviour.
    return true;
}

#endif  // WFLAG_H

