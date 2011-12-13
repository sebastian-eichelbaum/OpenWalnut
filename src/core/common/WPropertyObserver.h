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

#ifndef WPROPERTYOBSERVER_H
#define WPROPERTYOBSERVER_H

#include <map>
#include <string>
#include <set>

#include <boost/signals2/signal.hpp>
#include <boost/thread.hpp>

#include "WCondition.h"
#include "WProperties.h"
#include "WExportCommon.h"

/**
 * This class can observe properties inside a property group. The property group to observer can simply be set and replaced comfortably. Whenever
 * one of the child properties updates, the observer fires too. If the observed property group itself
 * changes (added properties, removed properties and so on), the observer gets updated automatically.
 */
class OWCOMMON_EXPORT WPropertyObserver: public WCondition
{
public:
    /**
     * Convenience type for a set of property instances.
     */
    typedef std::map< std::string, boost::shared_ptr< WPropertyBase > > PropertyNameMap;

    /**
     * Default constructor.
     */
    WPropertyObserver();

    /**
     * Destructor.
     */
    virtual ~WPropertyObserver();

    /**
     * Defines the property group whose children should be watched. You can define a list of names manually if you are not interested in updates
     * of ALL properties.
     * \note this also resets the updated flag and the list of the last fired properties.
     *
     * \param properties the group whose children should be watched.
     * \param names list of names. If specified, only these properties are observed.
     */
    void observe( boost::shared_ptr< WProperties > properties, std::set< std::string > names = std::set< std::string >() );

    /**
     * Is true if one observed property fired. This is reset by the \ref handled method.
     *
     * \return true if one property fired.
     */
    bool updated() const;

    /**
     * Resets the update flag and the list of fired properties.
     *
     * \return the set of properties fired until the last call of \ref handled.
     */
    PropertyNameMap handled();

    /**
     * Creates a new instance of WPropertyObserver. Useful to save some typing as it creates an shared pointer for you.
     *
     * \return the new instance.
     */
    static boost::shared_ptr< WPropertyObserver > create();

protected:
private:
    /**
     * Disallow copy construction.
     *
     * \param rhs the other threaded runner.
     */
    WPropertyObserver( const WPropertyObserver& rhs );

    /**
     * Disallow copy assignment.
     *
     * \param rhs the other threaded runner.
     * \return this.
     */
    WPropertyObserver& operator=( const WPropertyObserver& rhs );

    /**
     * Cancels all current subscriptions and cleans m_subscriptions.
     */
    void cancelSubscriptions();

    /**
     * Subscribes each property update condition which matches an entry in m_propNames.
     */
    void updateSubscriptions();

    /**
     * Gets called by the update callback of the property. The property given as parameter was the property that fired.
     *
     * \param property the property that fired.
     */
    void propertyUpdated( boost::shared_ptr< WPropertyBase > property );

    /**
     * Type for shared container with signal connections.
     */
    typedef WSharedAssociativeContainer< std::map< boost::shared_ptr< WPropertyBase >, boost::signals2::connection > > Subscriptions;

    /**
     * The subscription to each property which was subscribed.
     */
    Subscriptions m_subscriptions;

    /**
     * True if a property fired.
     */
    bool m_updated;

    /**
     * The properties handled by this observer.
     */
    boost::shared_ptr< WProperties > m_properties;

    /**
     * The names of the properties which shall be observed if they are or become available.
     */
    std::set< std::string > m_propNames;

    /**
     * The connection used to get notified about m_properties updates.
     */
    boost::signals2::scoped_connection m_updateConditionConnection;

    /**
     * Type of shared container for the list of last updated properties.
     */
    typedef WSharedAssociativeContainer< PropertyNameMap > LastUpdated;

    /**
     * The queue of properties that fired before handled() is called.
     */
    LastUpdated m_lastUpdated;
};

#endif  // WPROPERTYOBSERVER_H

