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
#include <string>
#include <list>
#include <vector>
#include <set>

#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>

#include "WLogger.h"

#include "WFlag.h"
#include "WPropertyBase.h"

#include "WCondition.h"
#include "WSharedAssociativeContainer.h"

#include "constraints/WPropertyConstraintTypes.h"
#include "constraints/WPropertyConstraintMin.h"
#include "constraints/WPropertyConstraintMax.h"
#include "constraints/WPropertyConstraintNotEmpty.h"
#include "constraints/WPropertyConstraintPathExists.h"
#include "constraints/WPropertyConstraintIsDirectory.h"
#include "constraints/WPropertyConstraintSelectOnlyOne.h"


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
     * Destructor.
     */
    virtual ~WPropertyVariable();

    /**
     * Determines whether the specified value is acceptable.
     *
     * \param newValue the new value.
     *
     * \return true if it is a valid/acceptable value.
     */
    virtual bool accept( T newValue );

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
    virtual bool ensureValidity( T newValidValue, bool suppressNotification = false );

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
        virtual bool accept( boost::shared_ptr< WPropertyVariable< T > > property, T value ) = 0;

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
    };

    /**
     * For shortening: a type defining a shared set of WModule pointers.
     */
    typedef std::set< boost::shared_ptr< PropertyConstraint > > ConstraintContainerType;

    /**
     * Const iterator for the prototype set.
     */
    typedef typename ConstraintContainerType::const_iterator ConstraintContainerConstIteratorType;

    /**
     * Iterator for the prototype set.
     */
    typedef typename ConstraintContainerType::iterator ConstraintContainerIteratorType;

    /**
     * The alias for a shared container.
     */
    typedef WSharedAssociativeContainer< ConstraintContainerType > ConstraintSharedContainerType;

    /**
     * Alias for the proper access object
     */
    typedef typename ConstraintSharedContainerType::WSharedAccess ConstraintAccess;

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
    ConstraintAccess getConstraints();

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
    static PropertyConstraintMin minConstraint( T min );

    /**
     * Creates a new WPropertyConstraintMax for this WPropertyVariable.
     *
     * \param max the maximum value of the property
     *
     * \return the new constraint.
     */
    static PropertyConstraintMax maxConstraint( T max );

    /**
     * Set a minimum constraint.
     *
     * \param min the minimum value allowed.
     *
     * \return the newly created constraint.
     */
    PropertyConstraintMin setMin( T min );

    /**
     * Set a maximum constraint.
     *
     * \param max the maximum value allowed.
     *
     * \return the newly created constraint.
     */
    PropertyConstraintMax setMax( T max );

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
     * Cleans m_constraints from all existing constrains of the specified type.
     *
     * \param type the type to remove.
     * \param useLock true if a lock should be used and if m_constraintsChanged should be notified. Set this to false to use remove lock from inside
     * another operation already using the lock and notifying.
     *
     */
    void removeConstraints( PROPERTYCONSTRAINT_TYPE type, bool useLock = true );

    /**
     * This method gets called by WFlag whenever the value of the property changes. It re-emits the signal with a this pointer
     */
    void propertyChangeNotifier();

    /**
     * this condition gets notified whenever the list of constraints changes.
     * \note it gets AFTER the write lock has been released. So in callbacks, a read lock can be acquired.
     */
    boost::shared_ptr< WCondition > m_constraintsChanged;

    /**
     * A set of constraints applied on this property.
     */
    ConstraintSharedContainerType m_constraints;

    /**
     * The access object of this constrains list.
     */
    ConstraintAccess m_constraintsAccess;

private:
};

template < typename T >
WPropertyVariable< T >::WPropertyVariable( std::string name, std::string description, const T& initial ):
        WFlag< T >( new WCondition(), initial ),
        WPropertyBase( name, description ),
        m_constraintsChanged( new WCondition() ),
        m_constraints(),
        m_constraintsAccess( m_constraints.getAccessObject() )
{
    updateType();

    // set constraint and change condition to update condition set of WPropertyBase
    m_updateCondition->add( m_constraintsChanged );
    m_updateCondition->add( WFlag< T >::getValueChangeCondition() );
}

template < typename T >
WPropertyVariable< T >::WPropertyVariable( std::string name, std::string description, const T& initial, boost::shared_ptr< WCondition > condition ):
        WFlag< T >( condition, initial ),
        WPropertyBase( name, description ),
        m_constraintsChanged( new WCondition() ),
        m_constraints(),
        m_constraintsAccess( m_constraints.getAccessObject() )
{
    updateType();

    // set constraint and change condition to update condition set of WPropertyBase
    m_updateCondition->add( m_constraintsChanged );
    m_updateCondition->add( WFlag< T >::getValueChangeCondition() );
}

template < typename T >
WPropertyVariable< T >::WPropertyVariable( std::string name, std::string description, const T& initial,
                                           PropertyChangeNotifierType notifier ):
        WFlag< T >( new WCondition(), initial ),
        WPropertyBase( name, description ),
        m_constraintsChanged( new WCondition() ),
        m_constraints(),
        m_constraintsAccess( m_constraints.getAccessObject() )
{
    updateType();

    // set constraint and change condition to update condition set of WPropertyBase
    m_updateCondition->add( m_constraintsChanged );
    m_updateCondition->add( WFlag< T >::getValueChangeCondition() );

    // set custom notifier
    WFlag< T >::getCondition()->subscribeSignal( boost::bind( &WPropertyVariable< T >::propertyChangeNotifier, this ) );
    signal_PropertyChange.connect( notifier );
}

template < typename T >
WPropertyVariable< T >::WPropertyVariable( std::string name, std::string description, const T& initial, boost::shared_ptr< WCondition > condition,
                                           PropertyChangeNotifierType notifier ):
        WFlag< T >( condition, initial ),
        WPropertyBase( name, description ),
        m_constraintsChanged( new WCondition() ),
        m_constraints(),
        m_constraintsAccess( m_constraints.getAccessObject() )
{
    updateType();

    // set constraint and change condition to update condition set of WPropertyBase
    m_updateCondition->add( m_constraintsChanged );
    m_updateCondition->add( WFlag< T >::getValueChangeCondition() );

    // set custom notifier
    WFlag< T >::getCondition()->subscribeSignal( boost::bind( &WPropertyVariable< T >::propertyChangeNotifier, this ) );
    signal_PropertyChange.connect( notifier );
}

template < typename T >
WPropertyVariable< T >::~WPropertyVariable()
{
    // clean up
    m_updateCondition->remove( m_constraintsChanged );
    m_updateCondition->remove( WFlag< T >::getValueChangeCondition() );

    m_notifierConnection.disconnect();
    m_constraintsAccess->beginWrite();
    m_constraintsAccess->get().clear();
    m_constraintsAccess->endWrite();
}

template < typename T >
void WPropertyVariable< T >::propertyChangeNotifier()
{
    // propagate change, include pointer to property
    signal_PropertyChange( shared_from_this() );
}

template < typename T >
bool WPropertyVariable< T >::accept( T newValue )
{
    m_constraintsAccess->beginRead();

    // iterate through the set
    bool acceptable = WFlag< T >::accept( newValue );
    for ( ConstraintContainerConstIteratorType it = m_constraintsAccess->get().begin(); it !=  m_constraintsAccess->get().end(); ++it )
    {
        acceptable &= ( *it )->accept( boost::shared_static_cast< WPropertyVariable< T > >( shared_from_this() ), newValue );
    }
    m_constraintsAccess->endRead();

    return acceptable;
}

template < typename T >
bool WPropertyVariable< T >::setAsString( std::string value )
{
    try
    {
        // use the helper class which can handle different kinds of properties for us
        PROPERTY_TYPE_HELPER::WCreateFromString< T > h = PROPERTY_TYPE_HELPER::WCreateFromString< T >();
        set( h.create( WFlag< T >::get(), value ) );
    }
    catch( const boost::bad_lexical_cast &e )
    {
        return false;
    }

    return true;
}

template < typename T >
std::string WPropertyVariable< T >::getAsString()
{
    std::string val;
    val = boost::lexical_cast< std::string >( WFlag< T >::get() );
    // try catch( const boost::bad_lexical_cast &e ) ? No if this happens something is wrong with the value

    return val;
}

template < typename T >
bool WPropertyVariable< T >::ensureValidity( T newValidValue, bool suppressNotification )
{
    if ( !accept( WFlag< T >::get() ) )
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
    m_constraintsAccess->beginWrite();
    m_constraintsAccess->get().insert( constraint );
    m_constraintsAccess->endWrite();

    m_constraintsChanged->notify();
}

template < typename T >
boost::shared_ptr< WCondition > WPropertyVariable< T >::getContraintsChangedCondition()
{
    return m_constraintsChanged;
}

template < typename T >
void WPropertyVariable< T >::updateType()
{
    PROPERTY_TYPE_HELPER::WTypeIdentifier< T > tid;
    m_type = tid.getType();
}

template < typename T >
boost::shared_ptr< WPropertyConstraintMin< T > > WPropertyVariable< T >::minConstraint( T min )
{
    return boost::shared_ptr< WPropertyConstraintMin< T > >( new WPropertyConstraintMin< T >( min ) );
}

template < typename T >
boost::shared_ptr< WPropertyConstraintMax< T > > WPropertyVariable< T >::maxConstraint( T max )
{
    return boost::shared_ptr< WPropertyConstraintMax< T > >( new WPropertyConstraintMax< T >( max ) );
}

template < typename T >
boost::shared_ptr< WPropertyConstraintMin< T > > WPropertyVariable< T >::setMin( T min )
{
    boost::shared_ptr< WPropertyConstraintMin< T > > c = minConstraint( min );
    replaceConstraint( c, PC_MIN );
    return c;
}

template < typename T >
boost::shared_ptr< WPropertyConstraintMax< T > > WPropertyVariable< T >::setMax( T max )
{
    boost::shared_ptr< WPropertyConstraintMax< T > > c = maxConstraint( max );
    replaceConstraint( c, PC_MAX );
    return c;
}

template < typename T >
void WPropertyVariable< T >::replaceConstraint( boost::shared_ptr< PropertyConstraint > constraint, PROPERTYCONSTRAINT_TYPE type )
{
    m_constraintsAccess->beginWrite();
    removeConstraints( type, false );
    m_constraintsAccess->get().insert( constraint );
    m_constraintsAccess->endWrite();
    m_constraintsChanged->notify();
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
    // lock
    m_constraintsAccess->beginRead();

    // search first appearance of a constraint of the specified type
    for ( ConstraintContainerConstIteratorType it = m_constraintsAccess->get().begin(); it != m_constraintsAccess->get().end(); ++it )
    {
        if ( ( *it )->getType() == type )
        {
            return ( *it );
        }
    }
    m_constraintsAccess->endRead();

    return boost::shared_ptr< PropertyConstraint >();
}

template < typename T >
int WPropertyVariable< T >::countConstraint( PROPERTYCONSTRAINT_TYPE type )
{
    // lock
    m_constraintsAccess->beginRead();

    int i = 0;
    // search first appearance of a constraint of the specified type
    for ( ConstraintContainerConstIteratorType it =  m_constraintsAccess->get().begin(); it !=  m_constraintsAccess->get().end(); ++it )
    {
        if ( ( *it )->getType() == type )
        {
            i++;
        }
    }
    m_constraintsAccess->endRead();

    return i;
}

template < typename T >
boost::shared_ptr< WPropertyConstraintMin< T > > WPropertyVariable< T >::getMin()
{
    // get min
    boost::shared_ptr< PropertyConstraint > c = getFirstConstraint( PC_MIN );
    if ( !c.get() )
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
    if ( !c.get() )
    {
        // return NULL if not found
        return boost::shared_ptr< WPropertyConstraintMax< T > >();
    }

    // cast to proper type
    return boost::shared_static_cast< WPropertyConstraintMax< T > >( c );
}

template< typename T >
typename WPropertyVariable<T>::ConstraintAccess WPropertyVariable<T>::getConstraints()
{
        return m_constraintsAccess;
}

template < typename T >
void WPropertyVariable< T >::removeConstraints( PROPERTYCONSTRAINT_TYPE type, bool useLock )
{
    // lock the constraints set
    if ( useLock )
    {
        m_constraintsAccess->beginWrite();
    }

    for ( ConstraintContainerConstIteratorType it = m_constraintsAccess->get().begin(); it != m_constraintsAccess->get().end(); )
    {
        if ( ( *it )->getType() == type )
        {
            m_constraintsAccess->get().erase( it++ );
        }
        else
        {
            ++it;
        }
    }

    // only notify and unlock if locked earlier.
    if ( useLock )
    {
        m_constraintsAccess->endWrite();
        m_constraintsChanged->notify();
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

