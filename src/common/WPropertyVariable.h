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
#include <vector>
#include <set>

#include <boost/filesystem.hpp>
#include <boost/thread.hpp>

#include "WFlag.h"
#include "WPropertyBase.h"
#include "WPropertyConstraintMin.h"
#include "WPropertyConstraintMax.h"
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

    /**
     * Class building the base for user defined constraints on a property instance.
     */
    class PropertyConstraint
    {
    public:
        typedef enum
        {
            PC_UNKNOWN,         // type ID for arbitrary WPropertyConstraint
            PC_MIN,             // type ID for WPropertyConstraintMin
            PC_MAX              // type ID for WPropertyConstraintMax
        }
        PROPERTYCONSTRAINT_TYPE;

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
         * Allows simple identification
         *
         * \return
         */
        virtual PROPERTYCONSTRAINT_TYPE getType();
    };

    /**
     * Add a new constraint. This is useful to disallow several (custom) values for this property.
     *
     * \param constraint the new constraint.
     *
     */
    void addConstraint( boost::shared_ptr< PropertyConstraint > constraint );

    /**
     * Creates a new WPropertyConstraintMin for this WPropertyVariable.
     *
     * \param min the minimum value.
     *
     * \return the new constraint.
     */
    static boost::shared_ptr< WPropertyConstraintMin< T > > minConstraint( T min );

    /**
     * Creates a new WPropertyConstraintMax for this WPropertyVariable.
     *
     * \param max the maximum value of the property
     *
     * \return the new constraint.
     */
    static boost::shared_ptr< WPropertyConstraintMax< T > > maxConstraint( T max );

    /**
     * Set a minimum constraint.
     *
     * \param min the minimum value allowed.
     *
     * \return the newly created constraint.
     */
    boost::shared_ptr< WPropertyConstraintMin< T > > setMin( T min );

    /**
     * Set a maximum constraint.
     *
     * \param max the maximum value allowed.
     *
     * \return the newly created constraint.
     */
    boost::shared_ptr< WPropertyConstraintMax< T > > setMax( T max );

    /**
     * Gets the current minimum constraint value.
     *
     * \return the minimum constraint.
     */
    boost::shared_ptr< WPropertyConstraintMin< T > > getMin();

    /**
     * Gets the current maximum constraint value.
     *
     * \return the maximum constraint.
     */
    boost::shared_ptr< WPropertyConstraintMax< T > > getMax();

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
     * A set of constraints applied on this property.
     */
    std::set< boost::shared_ptr< PropertyConstraint > > m_constraints;

    /**
     * The iterator used togo through the set
     */
    typedef typename std::set< boost::shared_ptr< PropertyConstraint > >::const_iterator constraintIterator;

    /**
     * boost mutex object for thread safety of updating of m_propertyConstraints.
     */
    boost::shared_mutex m_constraintsLock;

    /**
     * The currently set min constraint.
     *
     * \todo this is ugly. Use m_constraints to keep the min constraint
     */
    boost::shared_ptr< WPropertyConstraintMin< T > > m_minConstraint;

    /**
     * The currently set max constraint.
     *
     * \todo this is ugly. Use m_constraints to keep the max constraint
     */
    boost::shared_ptr< WPropertyConstraintMax< T > > m_maxConstraint;

private:
};

template < typename T >
WPropertyVariable< T >::WPropertyVariable( std::string name, std::string description, const T& initial ):
        WFlag< T >( new WCondition(), initial ),
        WPropertyBase( name, description )
{
    updateType();
}

template < typename T >
WPropertyVariable< T >::WPropertyVariable( std::string name, std::string description, const T& initial, boost::shared_ptr< WCondition > condition ):
        WFlag< T >( condition, initial ),
        WPropertyBase( name, description )
{
    updateType();
}

template < typename T >
WPropertyVariable< T >::WPropertyVariable( std::string name, std::string description, const T& initial,
                                           WCondition::t_ConditionNotifierType notifier ):
        WFlag< T >( new WCondition(), initial ),
        WPropertyBase( name, description )
{
    updateType();

    // set custom notifier
    m_notifierConnection = WFlag< T >::getCondition()->subscribeSignal( notifier );
}

template < typename T >
WPropertyVariable< T >::WPropertyVariable( std::string name, std::string description, const T& initial, boost::shared_ptr< WCondition > condition,
                                           WCondition::t_ConditionNotifierType notifier ):
        WFlag< T >( condition, initial ),
        WPropertyBase( name, description )
{
    updateType();
    // set custom notifier
    m_notifierConnection = WFlag< T >::getCondition()->subscribeSignal( notifier );
}

template < typename T >
WPropertyVariable< T >::~WPropertyVariable()
{
    // clean up
    m_notifierConnection.disconnect();
    m_constraints.clear();
}

template < typename T >
bool WPropertyVariable< T >::accept( T newValue )
{
    boost::shared_lock< boost::shared_mutex > lock = boost::shared_lock< boost::shared_mutex >( m_constraintsLock );

    // iterate through the set
    bool acceptable = true;
    for ( constraintIterator it = m_constraints.begin(); it != m_constraints.end(); ++it )
    {
        acceptable &= ( *it )->accept( boost::shared_static_cast< WPropertyVariable< T > >( shared_from_this() ), newValue );
    }
    lock.unlock();

    return acceptable;
}

template < typename T >
void WPropertyVariable< T >::addConstraint( boost::shared_ptr< PropertyConstraint > constraint )
{
    boost::unique_lock< boost::shared_mutex > lock = boost::unique_lock< boost::shared_mutex >( m_constraintsLock );
    m_constraints.insert( constraint );
    lock.unlock();
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
    m_minConstraint = minConstraint( min );
    return m_minConstraint;
}

template < typename T >
boost::shared_ptr< WPropertyConstraintMax< T > > WPropertyVariable< T >::setMax( T max )
{
    m_maxConstraint = maxConstraint( max );
    return m_maxConstraint;
}

template < typename T >
boost::shared_ptr< WPropertyConstraintMin< T > > WPropertyVariable< T >::getMin()
{
    return m_minConstraint;
}

template < typename T >
boost::shared_ptr< WPropertyConstraintMax< T > > WPropertyVariable< T >::getMax()
{
    return m_maxConstraint;
}

template < typename T >
WPropertyVariable< T >::PropertyConstraint::PropertyConstraint()
{
}

template < typename T >
WPropertyVariable< T >::PropertyConstraint::~PropertyConstraint()
{
}

#endif  // WPROPERTYVARIABLE_H

