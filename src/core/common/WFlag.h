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

#include <boost/shared_ptr.hpp>

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
     * The type for later access.
     */
    typedef T ValueType;

    /**
     * Convenience typedef for a boost::shared_ptr.
     */
    typedef boost::shared_ptr< WFlag< T > > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr. Const.
     */
    typedef boost::shared_ptr< const WFlag< T > > ConstSPtr;

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
     * Copy constructor. Creates a deep copy of this property. As boost::signals2 and condition variables are non-copyable, new instances get
     * created. The subscriptions to a signal are LOST as well as all listeners to a condition.
     * The conditions you can grab using getValueChangeConditon and getCondition are not the same as in the original! This is because
     * the class corresponds to the observer/observable pattern. You won't expect a clone to fire a condition if a original flag is changed
     * (which after cloning is completely decoupled from the clone).
     *
     * \param from the instance to copy.
     */
    explicit WFlag( const WFlag& from );

    /**
     * Destructor. It deletes the instance of WCondition specified on construction.
     */
    virtual ~WFlag();

    /**
     * Operator returns value of the flag.
     *
     * \param resetChangeState when true, the changed() flag gets reset to false.
     *
     * \return the value.
     */
    virtual const T get( bool resetChangeState = false );

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
     * Operator returns value of the flag. It does not reset the change flag.
     *
     * \return the value.
     */
    virtual operator T() const;

    /**
     * Wait for the flag to change its value. For WConditionOneShot is also recognizes if the flag has changed before.
     */
    virtual void wait() const;

    /**
     * Sets the new value for this flag. Also notifies waiting threads. After setting a value, changed() will be true.
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
     * Returns the condition denoting a value change. In contrast to getCondition, this condition fires regardless of notification is suppressed
     * during set() or not.
     *
     * \return the condition denoting a value change.
     */
    boost::shared_ptr< WCondition > getValueChangeCondition();

    /**
     * Determines whether the specified value is acceptable. In WFlags, this always returns true. To modify the behaviour,
     * implement this function in an appropriate way.
     *
     * \param newValue the new value.
     *
     * \return true if it is a valid/acceptable value.
     */
    virtual bool accept( T newValue );

    /**
     * Tests whether a flag is currently valid. It is equal to accept( get() );
     *
     * \return true if current value is valid.
     */
    virtual bool isValid();

    /**
     * True whenever the value inside this flag has changed since the last reset. It stays true until get( true ) is called or the reset value is
     * true.
     *
     * \param reset if true, the change flag gets reset.
     *
     * \return true when the value has changed and not yet been reseted.
     */
    virtual bool changed( bool reset = false );

protected:

    /**
     * The condition to be used for waiting/notifying. Please note, that it gets deleted during destruction.
     */
    boost::shared_ptr< WCondition > m_condition;

    /**
     * This condition is fired whenever the value changes. In contrast to m_condition, this also fires if set() is called with
     * suppressNotification=true.
     */
    boost::shared_ptr< WCondition > m_valueChangeCondition;

    /**
     * The flag value.
     */
    T m_flag;

    /**
     * Denotes whether the value has changed since the last reset.
     */
    bool m_changed;

private:
};

/**
 * Alias for easy usage of WFLag< bool >.
 */
typedef WFlag< bool > WBoolFlag;

template < typename T >
WFlag< T >::WFlag( WCondition* condition, T initial ):
    m_condition( boost::shared_ptr< WCondition >( condition ) ),
    m_valueChangeCondition( boost::shared_ptr< WCondition >( new WCondition() ) ),
    m_flag( initial ),
    m_changed( true )
{
}

template < typename T >
WFlag< T >::WFlag( boost::shared_ptr< WCondition > condition, T initial ):
    m_condition( condition ),
    m_valueChangeCondition( boost::shared_ptr< WCondition >( new WCondition() ) ),
    m_flag( initial ),
    m_changed( true )
{
}

template < typename T >
WFlag< T >::WFlag( const WFlag& from ):
    m_condition( boost::shared_ptr< WCondition >( new WCondition() ) ),
    m_valueChangeCondition( boost::shared_ptr< WCondition >( new WCondition() ) ),
    m_flag( from.m_flag ),
    m_changed( from.m_changed )
{
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
const T WFlag< T >::get( bool resetChangeState )
{
    if( resetChangeState )
    {
        m_changed = false;
    }
    return m_flag;
}

template < typename T >
const T WFlag< T >::get() const
{
    return m_flag;
}

template < typename T >
WFlag< T >::operator T() const
{
    return get();
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
    if( m_flag == value )
    {
        return true;
    }

    // let the caller know whether the value was acceptable.
    if( !accept( value ) )
    {
        return false;
    }

    m_flag = value;
    m_changed = true;

    // is the notification suppressed ?
    if( !suppressNotification )
    {
        m_condition->notify();
    }
    m_valueChangeCondition->notify();

    return true;
}

template < typename T >
boost::shared_ptr< WCondition > WFlag< T >::getCondition()
{
    return m_condition;
}

template < typename T >
boost::shared_ptr< WCondition > WFlag< T >::getValueChangeCondition()
{
    return m_valueChangeCondition;
}

template < typename T >
bool WFlag< T >::accept( T /* newValue */ )
{
    // please implement this method in your class to modify the behaviour.
    return true;
}

template < typename T >
bool WFlag< T >::isValid()
{
    return accept( get() );
}

template < typename T >
bool WFlag< T >::changed( bool reset )
{
    bool tmp = m_changed;
    if( reset )
    {
        m_changed = false;
    }
    return tmp;
}

#endif  // WFLAG_H

