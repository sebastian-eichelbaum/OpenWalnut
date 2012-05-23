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

#include <iostream>
#include <list>
#include <set>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/signals2.hpp>

#include "constraints/WPropertyConstraintIsDirectory.h"
#include "constraints/WPropertyConstraintMax.h"
#include "constraints/WPropertyConstraintMin.h"
#include "constraints/WPropertyConstraintNotEmpty.h"
#include "constraints/WPropertyConstraintPathExists.h"
#include "constraints/WPropertyConstraintSelectOnlyOne.h"
#include "constraints/WPropertyConstraintTypes.h"
#include "WCondition.h"
#include "WFlag.h"
#include "WPropertyBase.h"
#include "WSharedAssociativeContainer.h"
#include "WSharedObjectTicketRead.h"
#include "WSharedObjectTicketWrite.h"

/**
 * A named property class with a concrete type.
 */
template< typename T >
class WPropertyVariable: public WFlag< T >,
                         public WPropertyBase
{
friend class WPropertyVariableTest;
public:
    /**
     * Convenience typedef for a shared_ptr of WPropertyVariable.
     */
    typedef boost::shared_ptr< WPropertyVariable< T > > SPtr;

    /**
     * Convenience typedef for a shared_ptr of const WPropertyVariable.
     */
    typedef boost::shared_ptr< const WPropertyVariable< T > > ConstSPtr;

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
    WPropertyVariable( std::string name, std::string description, const T& initial, PropertyChangeNotifierType notifier );

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
                       PropertyChangeNotifierType notifier );

    /**
     * Copy constructor. Creates a deep copy of this property. As boost::signals2 and condition variables are non-copyable, new instances get
     * created. The subscriptions to a signal are LOST as well as all listeners to a condition.
     * The conditions you can grab using getValueChangeConditon and getCondition are not the same as in the original! This is because
     * the class corresponds to the observer/observable pattern. You won't expect a clone to fire a condition if a original variable is changed
     * (which after cloning is completely decoupled from the clone).
     *
     * \param from the instance to copy.
     */
    explicit WPropertyVariable( const WPropertyVariable< T >& from );

    /**
     * Destructor.
     */
    virtual ~WPropertyVariable();

    /**
     * This method clones a property and returns the clone. It does a deep copy and, in contrast to a copy constructor, creates property with the
     * correct type without explicitly requiring the user to specify it. It creates a NEW change condition and change signal. This means, alls
     * subscribed signal handlers are NOT copied.
     *
     * \note this simply ensures the copy constructor of the runtime type is issued.
     *
     * \return the deep clone of this property.
     */
    virtual boost::shared_ptr< WPropertyBase > clone();

    /**
     * Determines whether the specified value is acceptable.
     *
     * \param newValue the new value.
     *
     * \return true if it is a valid/acceptable value.
     */
    virtual bool accept( const T& newValue );

    /**
     * This method is useful to ensure, that there is a valid value in the property. Assume the following situation. The property p got a min
     * value of 10. p->setMin( 10 ). Now, p gets set by the GUI to 11. Now your module sets another min value: p->setMin( 15 ). As the property
     * already has been set, the property can't decide what to do; it simply stays invalid. To ensure a valid value, you can use this method. It
     * only sets the new value if the old value is invalid.
     *
     * \param newValidValue the new value to set.
     * \param suppressNotification true to avoid a firing condition.
     *
     * \return true if the new value has been accepted ( if it was valid ) - for short true if the property NOW is valid
     */
    virtual bool ensureValidity( const T& newValidValue, bool suppressNotification = false );

    /**
     * Class building the base for user defined constraints on a property instance.
     */
    class PropertyConstraint
    {
    public:
        /**
         * Default constructor.
         */
        PropertyConstraint();

        /**
         * Destructor.
         */
        virtual ~PropertyConstraint();

        /**
         * This method decides whether the specified value is valid for a specific property.
         *
         * \param value the new value.
         * \param property the property to which the value should be set.
         *
         * \return true whenever the new value is acceptable for the property.
         */
        virtual bool accept( boost::shared_ptr< WPropertyVariable< T > > property, const T& value ) = 0;

        /**
         * Allows simple identification of the real constraint type.
         *
         * \return the type
         */
        virtual PROPERTYCONSTRAINT_TYPE getType();

        /**
         * This method creates a constraint using the specified type. This is a useful convenience class for easily adding
         * constraints.
         *
         * \param type the type of the constraint to create
         *
         * \return NULL if the type is unknown or an constraint instance
         */
        static boost::shared_ptr< PropertyConstraint > create( PROPERTYCONSTRAINT_TYPE type );

        /**
         * Method to clone the constraint and create a new one with the correct dynamic type.
         *
         * \return the constraint.
         */
        virtual boost::shared_ptr< PropertyConstraint > clone() = 0;
    };

    /**
     * The alias for a shared container.
     */
    typedef WSharedAssociativeContainer< std::set< boost::shared_ptr< PropertyConstraint > > > ConstraintContainerType;

    /**
     * Alias for min constraints. It is an alias for convenience.
     */
    typedef boost::shared_ptr< WPropertyConstraintMin< T > > PropertyConstraintMin;

    /**
     * Alias for max constraints. It is an alias for convenience.
     */
    typedef boost::shared_ptr< WPropertyConstraintMax< T > > PropertyConstraintMax;

    /**
     * Add a new constraint. This is useful to disallow several (custom) values for this property.
     *
     * \param constraint the new constraint.
     *
     */
    void addConstraint( boost::shared_ptr< PropertyConstraint > constraint );

    /**
     * Returns all the current constraints of a WPropertyVariable. They can be iterated using the provided access object.
     *
     * \return the constraint access object
     */
    ConstraintContainerType getConstraints();

    /**
     * Gets the condition, which gets notified whenever the list of constraints changes. It is notified AFTER the write lock has been released so
     * a read lock can be acquired in the callback.
     *
     * \return the condition.
     */
    boost::shared_ptr< WCondition > getContraintsChangedCondition();

    /**
     * Creates a new WPropertyConstraintMin for this WPropertyVariable.
     *
     * \param min the minimum value.
     *
     * \return the new constraint.
     */
    static PropertyConstraintMin minConstraint( const T& min );

    /**
     * Creates a new WPropertyConstraintMax for this WPropertyVariable.
     *
     * \param max the maximum value of the property
     *
     * \return the new constraint.
     */
    static PropertyConstraintMax maxConstraint( const T& max );

    /**
     * Set a minimum constraint.
     *
     * \param min the minimum value allowed.
     *
     * \return the newly created constraint.
     */
    PropertyConstraintMin setMin( const T& min );

    /**
     * Set a maximum constraint.
     *
     * \param max the maximum value allowed.
     *
     * \return the newly created constraint.
     */
    PropertyConstraintMax setMax( const T& max );

    /**
     * Gets the current minimum constraint value.
     *
     * \return the minimum constraint, or NULL if none.
     */
    PropertyConstraintMin getMin();

    /**
     * Gets the current maximum constraint value.
     *
     * \return the maximum constraint, or NULL if none.
     */
    PropertyConstraintMax getMax();

    /**
     * This replaces all existing constraints of a certain type by a new specified constraint.
     *
     * \param constraint the new constraint
     * \param type the type of constraints to replace
     */
    void replaceConstraint( boost::shared_ptr< PropertyConstraint > constraint, PROPERTYCONSTRAINT_TYPE type );

    /**
     * This replaces all existing constraints of a certain type by a new specified constraint.
     *
     * \param constraint the new constraint
     * \param type the type of constraints to replace
     * \return the constraint created
     */
    boost::shared_ptr< PropertyConstraint > replaceConstraint( PROPERTYCONSTRAINT_TYPE constraint, PROPERTYCONSTRAINT_TYPE type );

    /**
     * Cleans list of constraints from all existing constrains of the specified type.
     *
     * \param type the type to remove.
     */
    void removeConstraint( PROPERTYCONSTRAINT_TYPE type );

    /**
     * Removes the specified constraint if existent.
     *
     * \param constraint the constraint to remove.
     */
    void removeConstraint( boost::shared_ptr< PropertyConstraint > constraint );

    /**
     * Method searching the first appearance of a constrained with the specified type.
     *
     * \param type the type of the searched constraint
     *
     * \return the constraint, or NULL if none.
     */
    boost::shared_ptr< PropertyConstraint > getFirstConstraint( PROPERTYCONSTRAINT_TYPE type );

    /**
     * Method searching the first appearance of a constrained with the specified type.
     *
     * \param type the type of the searched constraint
     *
     * \return the constraint, or NULL if none.
     */
    int countConstraint( PROPERTYCONSTRAINT_TYPE type );

    /**
     * This methods allows properties to be set by a string value. This is especially useful when a property is only available as string and the
     * real type of the property is unknown. This is a shortcut for casting the property and then setting the lexically casted value.
     *
     * \param value the new value to set.
     *
     * \return true if value could be set.
     */
    virtual bool setAsString( std::string value );

    /**
     * Returns the current value as a string. This is useful for debugging or project files. It is not implemented as << operator, since the <<
     * should also print min/max constraints and so on. This simply is the value.
     *
     * \return the value as a string.
     */
    virtual std::string getAsString();

    /**
     * Sets the value from the specified property to this one. This is especially useful to copy a value without explicitly casting/knowing the
     * dynamic type of the property.
     *
     * \param value the new value.
     * \param recommendedOnly if true, property types which support recommended values apply the given value as recommendation.
     *
     * \return true if the value has been accepted.
     */
    virtual bool set( boost::shared_ptr< WPropertyBase > value, bool recommendedOnly = false );

    /**
     * Sets the new value for this flag. Also notifies waiting threads. After setting a value, changed() will be true.
     *
     * \param value the new value
     * \param suppressNotification true to avoid a firing condition. This is useful for resetting values.
     *
     * \return true if the value has been set successfully.
     *
     * \note set( get() ) == true
     * \note this is defined here to help the compiler to disambiguate between WFlag::set and the WPropertyBase::set.
     */
    virtual bool set( const T& value, bool suppressNotification = false );

    /**
     * Sets the specified value as recommended value. The difference to \ref set is simple. If some value was set using the method \ref set
     * earlier, the \ref setRecommendedValue call is ignored. This is very useful in modules, where incoming data yields some useful default values
     * but you do not want to overwrite a user-value which might have been set.
     *
     * \param value the new value to set if the user did not yet set the value
     *
     * \return true if value has been set successfully.
     */
    virtual bool setRecommendedValue( const T& value );

protected:
    /**
     * The connection used for notification.
     */
    boost::signals2::connection m_notifierConnection;

    /**
     * Uses typeid() to set the proper type constant.
     */
    virtual void updateType();

    /**
     * Cleans list of constraints from all existing constrains of the specified type.
     *
     * \param type the type to remove.
     * \param ticket the write ticket if already existent.
     */
    void removeConstraints( PROPERTYCONSTRAINT_TYPE type,
                            typename WPropertyVariable< T >::ConstraintContainerType::WriteTicket ticket
                            = ConstraintContainerType::WriteTicket() );

    /**
     * This method gets called by WFlag whenever the value of the property changes. It re-emits the signal with a this pointer
     */
    void propertyChangeNotifier();

    /**
     * A set of constraints applied on this property.
     */
    boost::shared_ptr< ConstraintContainerType > m_constraints;

private:
    /**
     * This is true, if the user did not set a value until now using \ref set.
     */
    bool m_notYetSet;
};

template < typename T >
WPropertyVariable< T >::WPropertyVariable( std::string name, std::string description, const T& initial ):
        WFlag< T >( new WCondition(), initial ),
        WPropertyBase( name, description ),
        m_constraints( new ConstraintContainerType() ),
        m_notYetSet( true )
{
    updateType();

    // set constraint and change condition to update condition set of WPropertyBase
    m_updateCondition->add( m_constraints->getChangeCondition() );
    m_updateCondition->add( WFlag< T >::getValueChangeCondition() );
}

template < typename T >
WPropertyVariable< T >::WPropertyVariable( std::string name, std::string description, const T& initial, boost::shared_ptr< WCondition > condition ):
        WFlag< T >( condition, initial ),
        WPropertyBase( name, description ),
        m_constraints( new ConstraintContainerType() ),
        m_notYetSet( true )
{
    updateType();

    // set constraint and change condition to update condition set of WPropertyBase
    m_updateCondition->add( m_constraints->getChangeCondition() );
    m_updateCondition->add( WFlag< T >::getValueChangeCondition() );
}

template < typename T >
WPropertyVariable< T >::WPropertyVariable( std::string name, std::string description, const T& initial,
                                           PropertyChangeNotifierType notifier ):
        WFlag< T >( new WCondition(), initial ),
        WPropertyBase( name, description ),
        m_constraints( new ConstraintContainerType() ),
        m_notYetSet( true )
{
    updateType();

    // set constraint and change condition to update condition set of WPropertyBase
    m_updateCondition->add( m_constraints->getChangeCondition() );
    m_updateCondition->add( WFlag< T >::getValueChangeCondition() );

    // set custom notifier
    m_notifierConnection = WFlag< T >::getValueChangeCondition()->subscribeSignal(
            boost::bind( &WPropertyVariable< T >::propertyChangeNotifier, this )
    );
    signal_PropertyChange.connect( notifier );
}

template < typename T >
WPropertyVariable< T >::WPropertyVariable( std::string name, std::string description, const T& initial, boost::shared_ptr< WCondition > condition,
                                           PropertyChangeNotifierType notifier ):
        WFlag< T >( condition, initial ),
        WPropertyBase( name, description ),
        m_constraints( new ConstraintContainerType() ),
        m_notYetSet( true )
{
    updateType();

    // set constraint and change condition to update condition set of WPropertyBase
    m_updateCondition->add( m_constraints->getChangeCondition() );
    m_updateCondition->add( WFlag< T >::getValueChangeCondition() );

    // set custom notifier
    m_notifierConnection = WFlag< T >::getValueChangeCondition()->subscribeSignal(
            boost::bind( &WPropertyVariable< T >::propertyChangeNotifier, this )
    );
    signal_PropertyChange.connect( notifier );
}

template < typename T >
WPropertyVariable< T >::WPropertyVariable( const WPropertyVariable< T >& from ):
    WFlag< T >( from ),
    WPropertyBase( from ),
    m_constraints( new ConstraintContainerType() ),
    m_notYetSet( from.m_notYetSet )
{
    // copy the constraints

    // lock, unlocked if l looses focus
    typename WPropertyVariable< T >::ConstraintContainerType::ReadTicket l =
        const_cast< WPropertyVariable< T >& >( from ).m_constraints->getReadTicket();

    // get write ticket too
    typename WPropertyVariable< T >::ConstraintContainerType::WriteTicket w = m_constraints->getWriteTicket();

    // we need to make a deep copy here.
    for( typename ConstraintContainerType::ConstIterator iter = l->get().begin(); iter != l->get().end(); ++iter )
    {
        // clone them to keep dynamic type
        w->get().insert( ( *iter )->clone() );
    }

    // set constraint and change condition to update condition set of WPropertyBase
    m_updateCondition->add( m_constraints->getChangeCondition() );
    m_updateCondition->add( WFlag< T >::getValueChangeCondition() );
}

template < typename T >
WPropertyVariable< T >::~WPropertyVariable()
{
    // clean up
    m_updateCondition->remove( m_constraints->getChangeCondition() );
    m_updateCondition->remove( WFlag< T >::getValueChangeCondition() );

    m_notifierConnection.disconnect();

    // lock, unlocked if l looses focus
    typename WPropertyVariable< T >::ConstraintContainerType::WriteTicket l = m_constraints->getWriteTicket();
    l->get().clear();
}

template < typename T >
boost::shared_ptr< WPropertyBase > WPropertyVariable< T >::clone()
{
    return boost::shared_ptr< WPropertyBase >( new WPropertyVariable< T >( *this ) );
}

template < typename T >
void WPropertyVariable< T >::propertyChangeNotifier()
{
    // propagate change, include pointer to property
    signal_PropertyChange( shared_from_this() );
}

template < typename T >
bool WPropertyVariable< T >::accept( const T& newValue )
{
    // lock, lock vanishes if l looses focus
    typename WPropertyVariable< T >::ConstraintContainerType::ReadTicket l = m_constraints->getReadTicket();

    // iterate through the set
    bool acceptable = WFlag< T >::accept( newValue );
    for( typename ConstraintContainerType::ConstIterator it = l->get().begin(); it !=  l->get().end(); ++it )
    {
        acceptable &= ( *it )->accept( boost::shared_static_cast< WPropertyVariable< T > >( shared_from_this() ), newValue );
    }

    return acceptable;
}

template < typename T >
bool WPropertyVariable< T >::setAsString( std::string value )
{
    try
    {
        // use the helper class which can handle different kinds of properties for us
        PROPERTY_TYPE_HELPER::WStringConversion< T > h = PROPERTY_TYPE_HELPER::WStringConversion< T >();
        return set( h.create( WFlag< T >::get(), value ) );
    }
    catch( const std::exception &e )
    {
        return false;
    }
}

template < typename T >
std::string WPropertyVariable< T >::getAsString()
{
    std::string val;
    // use the helper class which can handle different kinds of properties for us
    PROPERTY_TYPE_HELPER::WStringConversion< T > h = PROPERTY_TYPE_HELPER::WStringConversion< T >();
    return h.asString( WFlag< T >::get() );

    return val;
}

template < typename T >
bool WPropertyVariable< T >::set( boost::shared_ptr< WPropertyBase > value, bool recommendedOnly )
{
    // try to cast the given property to a WPropertyVariable of right type:
    boost::shared_ptr< WPropertyVariable< T > > v = boost::shared_dynamic_cast< WPropertyVariable< T > >( value );
    if( v )
    {
        if( recommendedOnly )
        {
            return setRecommendedValue( v->get() );
        }
        else
        {
            return set( v->get() );
        }
    }
    else
    {
        return false;
    }
}

template < typename T >
bool WPropertyVariable< T >::set( const T& value, bool suppressNotification )
{
    m_notYetSet = false;
    return WFlag< T >::set( value, suppressNotification );
}

template < typename T >
bool WPropertyVariable< T >::setRecommendedValue( const T& value )
{
    // NOTE: well this is quite problematic when used multi-threaded ...
    if( m_notYetSet )
    {
        bool ret = set( value );
        m_notYetSet = true;
        return ret;
    }
    else
    {
        return false;
    }
}

template < typename T >
bool WPropertyVariable< T >::ensureValidity( const T& newValidValue, bool suppressNotification )
{
    if( !accept( WFlag< T >::get() ) )
    {
        // the currently set constraints forbid the current value.
        // reset it to the new value
        return WFlag< T >::set( newValidValue, suppressNotification );
    }

    return true;
}

template < typename T >
void WPropertyVariable< T >::addConstraint( boost::shared_ptr< PropertyConstraint > constraint )
{
    // lock, unlocked if l looses focus
    typename WPropertyVariable< T >::ConstraintContainerType::WriteTicket l = m_constraints->getWriteTicket();
    l->get().insert( constraint );

    // unlock by hand
    l.reset();
}

template < typename T >
boost::shared_ptr< WCondition > WPropertyVariable< T >::getContraintsChangedCondition()
{
    return m_constraints->getChangeCondition();
}

template < typename T >
void WPropertyVariable< T >::updateType()
{
    PROPERTY_TYPE_HELPER::WTypeIdentifier< T > tid;
    m_type = tid.getType();
}

template < typename T >
boost::shared_ptr< WPropertyConstraintMin< T > > WPropertyVariable< T >::minConstraint( const T& min )
{
    return boost::shared_ptr< WPropertyConstraintMin< T > >( new WPropertyConstraintMin< T >( min ) );
}

template < typename T >
boost::shared_ptr< WPropertyConstraintMax< T > > WPropertyVariable< T >::maxConstraint( const T& max )
{
    return boost::shared_ptr< WPropertyConstraintMax< T > >( new WPropertyConstraintMax< T >( max ) );
}

template < typename T >
boost::shared_ptr< WPropertyConstraintMin< T > > WPropertyVariable< T >::setMin( const T& min )
{
    boost::shared_ptr< WPropertyConstraintMin< T > > c = minConstraint( min );
    replaceConstraint( c, PC_MIN );
    return c;
}

template < typename T >
boost::shared_ptr< WPropertyConstraintMax< T > > WPropertyVariable< T >::setMax( const T& max )
{
    boost::shared_ptr< WPropertyConstraintMax< T > > c = maxConstraint( max );
    replaceConstraint( c, PC_MAX );
    return c;
}

template < typename T >
void WPropertyVariable< T >::replaceConstraint( boost::shared_ptr< PropertyConstraint > constraint, PROPERTYCONSTRAINT_TYPE type )
{
    // lock, unlocked if l looses focus
    typename WPropertyVariable< T >::ConstraintContainerType::WriteTicket l = m_constraints->getWriteTicket();

    removeConstraints( type, l );
    l->get().insert( constraint );
}

template < typename T >
boost::shared_ptr< typename WPropertyVariable< T >::PropertyConstraint >
WPropertyVariable< T >::replaceConstraint( PROPERTYCONSTRAINT_TYPE constraint, PROPERTYCONSTRAINT_TYPE type )
{
    boost::shared_ptr< typename WPropertyVariable< T >::PropertyConstraint > c = PropertyConstraint::create( constraint );
    replaceConstraint( c, type );
    return c;
}

template < typename T >
boost::shared_ptr< typename WPropertyVariable< T >::PropertyConstraint >
WPropertyVariable< T >::getFirstConstraint( PROPERTYCONSTRAINT_TYPE type )
{
    // lock, unlocked if l looses focus
    typename WPropertyVariable< T >::ConstraintContainerType::ReadTicket l = m_constraints->getReadTicket();

    // search first appearance of a constraint of the specified type
    for( typename ConstraintContainerType::ConstIterator it = l->get().begin(); it != l->get().end(); ++it )
    {
        if( ( *it )->getType() == type )
        {
            return ( *it );
        }
    }

    return boost::shared_ptr< PropertyConstraint >();
}

template < typename T >
int WPropertyVariable< T >::countConstraint( PROPERTYCONSTRAINT_TYPE type )
{
    // lock, unlocked if l looses focus
    typename WPropertyVariable< T >::ConstraintContainerType::ReadTicket l = m_constraints->getReadTicket();

    int i = 0;
    // search first appearance of a constraint of the specified type
    for( typename ConstraintContainerType::ConstIterator it =  l->get().begin(); it != l->get().end(); ++it )
    {
        if( ( *it )->getType() == type )
        {
            i++;
        }
    }

    return i;
}

template < typename T >
boost::shared_ptr< WPropertyConstraintMin< T > > WPropertyVariable< T >::getMin()
{
    // get min
    boost::shared_ptr< PropertyConstraint > c = getFirstConstraint( PC_MIN );
    if( !c.get() )
    {
        // return NULL if not found
        return boost::shared_ptr< WPropertyConstraintMin< T > >();
    }

    // cast to proper type
    return boost::shared_static_cast< WPropertyConstraintMin< T > >( c );
}

template < typename T >
boost::shared_ptr< WPropertyConstraintMax< T > > WPropertyVariable< T >::getMax()
{
    // get min
    boost::shared_ptr< PropertyConstraint > c = getFirstConstraint( PC_MAX );
    if( !c.get() )
    {
        // return NULL if not found
        return boost::shared_ptr< WPropertyConstraintMax< T > >();
    }

    // cast to proper type
    return boost::shared_static_cast< WPropertyConstraintMax< T > >( c );
}

template< typename T >
typename WPropertyVariable<T>::ConstraintContainerType WPropertyVariable<T>::getConstraints()
{
    return m_constraints;
}

template < typename T >
void WPropertyVariable< T >::removeConstraints( PROPERTYCONSTRAINT_TYPE type,
                                                typename WPropertyVariable< T >::ConstraintContainerType::WriteTicket ticket )
{
    typename WPropertyVariable< T >::ConstraintContainerType::WriteTicket l = ticket;

    bool useLock = !ticket;

    // lock the constraints set
    if( useLock )
    {
        // lock, unlocked if l looses focus
        l = m_constraints->getWriteTicket();
    }

    size_t nbErased = 0;    // count how much items have been removed
    for( typename ConstraintContainerType::ConstIterator it = l->get().begin(); it != l->get().end(); )
    {
        if( ( *it )->getType() == type )
        {
            l->get().erase( it++ );
            ++nbErased;
        }
        else
        {
            ++it;
        }
    }

    // only notify and unlock if locked earlier.
    if( useLock )
    {
        // no operations done? No condition fired
        if( nbErased == 0 )
        {
            l->suppressUnlockCondition();
        }

        // unlock by hand
        l.reset();
    }
}

template < typename T >
void WPropertyVariable< T >::removeConstraint( PROPERTYCONSTRAINT_TYPE type )
{
    // simply forward the call
    removeConstraints( type, typename WPropertyVariable< T >::ConstraintContainerType::WriteTicket() );
}

template < typename T >
void WPropertyVariable< T >::removeConstraint( boost::shared_ptr< PropertyConstraint > constraint )
{
    // lock released automatically
    typename WPropertyVariable< T >::ConstraintContainerType::WriteTicket l = m_constraints->getWriteTicket();

    if( l->get().erase( constraint ) == 0 )
    {
        // nothing changed. Suppress update condition to fire
        l->suppressUnlockCondition();
    }
}

template < typename T >
WPropertyVariable< T >::PropertyConstraint::PropertyConstraint()
{
}

template < typename T >
WPropertyVariable< T >::PropertyConstraint::~PropertyConstraint()
{
}

template < typename T >
PROPERTYCONSTRAINT_TYPE WPropertyVariable< T >::PropertyConstraint::getType()
{
    return PC_UNKNOWN;
}

#endif  // WPROPERTYVARIABLE_H

