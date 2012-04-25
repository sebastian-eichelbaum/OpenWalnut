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

#ifndef WSTRATEGYHELPER_H
#define WSTRATEGYHELPER_H

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include "WLogger.h"
#include "WItemSelection.h"
#include "WItemSelector.h"
#include "WSharedSequenceContainer.h"
#include "WProperties.h"
#include "WPropertyHelper.h"

/**
 * This class allows for an easy strategy pattern-based switching between properties and strategy instances using a WPropSelection. The idea of
 * this class is that you specify the type of some class you want to serve as the base class of several strategies. Each of these strategies has
 * a name, description and several properties. An instance of this class automatically provides a WPropSelection containing an item for each strategy
 * instance you add. A switch in this property causes to automatically hide all properties not belonging to this strategy. This class will ease
 * the writing of modules that provide multiple "ways of doing it". If you utilize this class in your module, you should add ALL your strategies
 * before you add this WStrategyHelper's properties to your module's properties.
 *
 * \tparam StrategyType the base class type of your strategies.
 *
 * The type in StrategyType needs to comply to the following rules:
 * <ul>
 *  <li> provide a typedef SPtr, representing the pointer type of this class. Usually, this is a boost::shared_ptr
 *  <li> provide a typedef ConstSPtr, representing the pointer type of this class. Usually, this is a boost::shared_ptr< const >
 *  <li> provide a method std::string getName() const
 *  <li> provide a method std::string getDescription() const
 *  <li> provide a method const char** getXPMIcon() const
 *  <li> provide a method WProperties::SPtr getProperties() const;
 * </ul>
 * The cool thing is, this class complies to its own requirements on strategy base classes. This allows you to nest strategy selections.
 *
 * \note the class is thread-safe, although it might not be a nice idea to modify the strategy list while the user tries to select some. He will
 * probably be very annoyed.
 *
 * \note you should use \ref WObjectNDIP which complies to this rules. This furthermore eases the task of writing strategies.
 */
template< class StrategyType >
class WStrategyHelper
{
public:
    /**
     * Convenience typedef for a boost::shared_ptr< WStrategyHelper >.
     */
    typedef boost::shared_ptr< WStrategyHelper > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WStrategyHelper >.
     */
    typedef boost::shared_ptr< const WStrategyHelper > ConstSPtr;

    /**
     * Constructs an empty strategy selector. Use one of the addStrategy methods to register strategies.
     *
     * \param name name of this strategy selector
     * \param description a description for this selection
     * \param icon an icon for this selection. Can be NULL.
     * \param selectorName the name of the selection property used to switch. If empty, the name of the WStrategyHelper will be used.
     * \param selectorDescription the description of the selection property used to switch. If empty, description of the WStrategyHelper is used.
     */
    WStrategyHelper( std::string name, std::string description, const char** icon = NULL, std::string selectorName = std::string(),
                                                                                          std::string selectorDescription = std::string() );

    /**
     * Destructor.
     */
    ~WStrategyHelper();

    /**
     * Gets the name of this strategy selector.
     *
     * \return the name.
     */
    std::string getName() const;

    /**
     * Gets the description for this strategy selector.
     *
     * \return the description
     */
    std::string getDescription() const;

    /**
     * Get the icon for this strategy selectior in XPM format.
     *
     * \return The icon.
     */
    const char** getXPMIcon() const;

    /**
     * Get this strategy selectors properties. This group contains the WPropSelection property to switch the strategy as well as groups for all
     * registered strategies.
     *
     * \return properties
     */
    WProperties::SPtr getProperties() const;

    /**
     * Adds the given strategy to the list of all strategies.
     *
     * \param strategy the strategy to add.
     */
    void addStrategy( typename StrategyType::SPtr strategy );

    /**
     * Return the currently active strategy.
     *
     * \return the active strategy
     */
    typename StrategyType::ConstSPtr operator()() const;

    /**
     * Return the currently active strategy.
     *
     * \return the active strategy
     */
    typename StrategyType::SPtr operator()();

protected:
private:
    const char** m_icon; //!< the icon pointer
    WProperties::SPtr m_properties; //!< stores the selection property and the strategy property groups

    /**
     * A list of items that can be selected. Will be extended for each added strategy.
     */
    WItemSelection::SPtr m_possibleSelections;

    /**
     * The property allowing the user to switch the strategy. Will be extended for each added strategy.
     */
    WPropSelection m_possibleSelectionProp;

    /**
     * The type used to securely manage the strategies
     */
    typedef WSharedSequenceContainer< std::vector< typename StrategyType::SPtr > > ContainerType;

    /**
     * This is the list of all strategies
     */
    ContainerType m_strategies;

    /**
     * This lock is needed to protect the addStrategy function. Although the m_strategies member is protected due to the use of a WSharedObject,
     * an update in the selection  (m_possibleSelectionProp) causes an update of the hide status of all property groups in m_strategies. This
     * would cause a deadlock if m_strategies is still locked. This lock is only locked if addStrategy is  called.
     */
    boost::mutex m_addLock;

    /**
     * Connection between \ref update and the update condition of  \ref m_possibleSelectionProp.
     */
    boost::signals2::connection m_updateConnection;

    /**
     * Update strategy's property hide status on updates in \ref m_possibleSelectionProp.
     */
    void update();
};

template< typename StrategyType >
WStrategyHelper< StrategyType >::WStrategyHelper( std::string name, std::string description, const char** icon,
                                                  std::string selectorName, std::string selectorDescription ):
    m_icon( icon ),
    m_properties( new WProperties( name, description ) ),
    m_possibleSelections( new WItemSelection() )
{
    // Create the main selector property:
    selectorName = selectorName.empty() ? name : selectorName;
    selectorDescription = selectorDescription.empty() ? name : selectorDescription;
    m_possibleSelectionProp = m_properties->addProperty( selectorName, selectorDescription, m_possibleSelections->getSelectorNone() );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_possibleSelectionProp );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_possibleSelectionProp );

    // if the selection changes, we want to hide all not selected strategy groups. So we register a change callback
    m_updateConnection = m_possibleSelectionProp->getUpdateCondition()->subscribeSignal(
        boost::bind( &WStrategyHelper< StrategyType >::update, this )
    );
}

template< typename StrategyType >
WStrategyHelper< StrategyType >::~WStrategyHelper()
{
    // cleanup
}

template< typename StrategyType >
void WStrategyHelper< StrategyType >::update()
{
    // get lock
    typename ContainerType::WriteTicket w = m_strategies.getWriteTicket();

    // update each hide state
    size_t currentID = 0;
    size_t selectedID = m_possibleSelectionProp->get();

    for( typename ContainerType::Iterator i = w->get().begin(); i != w->get().end(); ++i )
    {
        ( *i )->getProperties()->setHidden( currentID != selectedID );
        currentID++;
    }
    // w unlocks automatically
}

template< typename StrategyType >
std::string WStrategyHelper< StrategyType >::getName() const
{
    return m_properties->getName();
}

template< typename StrategyType >
std::string WStrategyHelper< StrategyType >::getDescription() const
{
    return m_properties->getDescription();
}

template< typename StrategyType >
const char** WStrategyHelper< StrategyType >::getXPMIcon() const
{
    return m_icon;
}

template< typename StrategyType >
WProperties::SPtr WStrategyHelper< StrategyType >::getProperties() const
{
    return m_properties;
}

template< typename StrategyType >
void WStrategyHelper< StrategyType >::addStrategy( typename StrategyType::SPtr strategy )
{
    // lock this method.
    boost::lock_guard< boost::mutex > lock( m_addLock );

    // add strategy to list of strategies
    typename ContainerType::WriteTicket w = m_strategies.getWriteTicket();
    w->get().push_back( strategy );
    size_t size = w->get().size();

    // add strategy to selector:
    m_possibleSelections->addItem( strategy->getName(), strategy->getDescription(), strategy->getIcon() );
    m_properties->addProperty( strategy->getProperties() );

    // we can safely unlock m_strategies now. This is needed since an update in m_possibleSelectionProp requests a read lock and will deadlock if
    // w was not unlocked.
    w.reset();

    // the first strategy. Select it. If this somehow changes the selection, the update mechanism ensures proper hide/unhide on all property
    // groups.
    if( size == 1 )
    {
        m_possibleSelectionProp->set( m_possibleSelections->getSelectorFirst() );
    }
    else
    {
        m_possibleSelectionProp->set( m_possibleSelectionProp->get().newSelector() );
    }

    // lock unlocked automatically
}

template< typename StrategyType >
typename StrategyType::ConstSPtr WStrategyHelper< StrategyType >::operator()() const
{
    // get lock
    typename ContainerType::ReadTicket r = m_strategies.getReadTicket();
    return r->get()[ m_possibleSelectionProp->get() ];
    // r unlocks automatically
}

template< typename StrategyType >
typename StrategyType::SPtr WStrategyHelper< StrategyType >::operator()()
{
    // get lock
    typename ContainerType::WriteTicket w = m_strategies.getWriteTicket();
    return w->get()[ m_possibleSelectionProp->get() ];
    // w unlocks automatically
}

#endif  // WSTRATEGYHELPER_H

