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

#ifndef WITEMSELECTION_H
#define WITEMSELECTION_H

#include <vector>
#include <string>
#include <utility>

#include <boost/tuple/tuple.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "WSharedSequenceContainer.h"
#include "WExportCommon.h"

class WItemSelector;

/**
 * A class containing a list of named items. It is mainly a container for an std::vector but with the difference that there can be so called
 * Selectors which are able to select some subset of the item set. This is especially useful in properties where item selection is needed. The
 * class is kept very restrictive to keep the interface clean and sleek and to keep the item set consistent among several threads. So please do
 * not implement any function that might change the item list, use the provided ones. If the item list changes, existing selectors get invalid
 * automatically.
 */
class OWCOMMON_EXPORT WItemSelection: public boost::enable_shared_from_this< WItemSelection >
{
    friend class WItemSelector; // for proper locking and unlocking
public:
    /**
     * For shortening, it is the type of an item.
     */
    typedef struct
    {
        /**
         * Name
         */
        std::string name;

        /**
         * Description, can be empty.
         */
        std::string description;

        /**
         * Icon shown in the item selection box. Can be NULL.
         */
        const char** icon;
    }
    Item;

    /**
     * Default constructor.
     */
    WItemSelection();

    /**
     * Destructor.
     */
    virtual ~WItemSelection();

    /**
     * Adds an item to the list of selectable items. If there are any selectors created (using ::getSelectorAll, ::getSelectorFirst,
     * ::getSelectorNone or ::getSelector) previously, there are marked as invalid.
     *
     * \param name the name
     * \param description the description
     * \param icon an icon to show together with this item. Useful to illustrate the selection options.
     */
    virtual void addItem( std::string name, std::string description, const char** icon = NULL );

    /**
     * Creates an default selection (all items selected). The selector gets invalid if another item is added.
     *
     * \return an selector.
     */
    virtual WItemSelector getSelectorAll();

    /**
     * Creates an default selection (no items selected). The selector gets invalid if another item is added.
     *
     * \return an selector.
     */
    virtual WItemSelector getSelectorNone();

    /**
     * Creates an default selection (first item selected). The selector gets invalid if another item is added.
     *
     * \return an selector.
     */
    virtual WItemSelector getSelectorFirst();

    /**
     * Creates an default selection (a specified items selected). The selector gets invalid if another item is added.
     *
     * \param item the item to select.
     *
     * \return an selector.
     */
    virtual WItemSelector getSelector( size_t item );

    /**
     * Invalidates all selectors currently existing somewhere. This is especially useful with properties when the underlying selection changes
     * for a property to ensure nobody can set the property to an old selection.
     */
    void invalidateSelectors();

    /**
     * The number of selectable items.
     *
     * \return the number of items.
     */
    virtual size_t size() const;

    /**
     * Gets the item with the given index. This is not the same index as the element has in the corresponding WItemSelector!
     * This method is especially useful to iterate the through all items.
     *
     * \param index the index
     *
     * \return the item
     */
    virtual Item& at( size_t index ) const;

    /**
     * Subscribes the specified callback to the invalidation signal getting emitted whenever this selection got invalidated. All selectors using
     * this selection subscribe to this signal.
     *
     * \param invalidationCallback the callback
     *
     * \return connection. The subscriber needs to disconnect it.
     */
    boost::signals2::connection subscribeInvalidationSignal( boost::function< void( void ) > invalidationCallback );

protected:
    /**
     * Shortcut for the list type
     */
    typedef WSharedSequenceContainer< std::vector< Item* > > ItemListType;

    /**
     * List of items.
     */
    ItemListType m_items;

    /**
     * This signal is emitted whenever the selection gets invalidated. All created selectors subscribed to it.
     */
    boost::signals2::signal< void( void ) > signal_invalidate;

private:
};

#endif  // WITEMSELECTION_H

