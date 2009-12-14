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
     * Constructor. Uses a given condition to realize the wait/notify functionality.
     * 
     * \param condition the condition to use. NOTE: can also be a WConditionOneShot.
     * \param initial the initial value of this flag
     */
    WFlag( WCondition* condition, T initial );

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
     * Wait for the flag to change its value.
     */
    virtual void wait() const;

    /** 
     * Sets the new value for this flag. Also notifies waiting threads.
     * 
     * \param value the new value
     */
    virtual void set( T value );

    /** 
     * Sets the new value for this flag. Also notifies waiting threads.
     * 
     * \param value the new value
     */
    virtual void operator()( T value );

protected:

    /** 
     * The condition to be used for waiting/notifying. Please note, that it gets deleted during destruction.
     */
    WCondition* m_condition;

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
    m_condition = condition;
    m_flag = initial;
}

template < typename T >
WFlag< T >::~WFlag()
{
    delete m_condition;
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
void WFlag< T >::set( T value )
{
    if ( m_flag == value )
        return;

    m_flag = value;
    m_condition->notify();
}

#endif  // WFLAG_H

