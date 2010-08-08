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
#include <boost/enable_shared_from_this.hpp>

#include "WExportCommon.h"

class WItemSelector;

/**
 * A class containing a list of named items. It is mainly a container for an std::vector but with the difference that there can be so called
 * Selectors which are able to select some subset of the item set. This is especially useful in properties where item selection is needed. The
 * class is kept very restrictive to keep the interface clean and sleek and to keep the item set consistent among several threads. So please do
 * not implement any function that might change the item list. The would cause odd behaviour of all the WItemSelector instances. Items can only
 * be added until the first Selector instance is created.
 */
class OWCOMMON_EXPORT WItemSelection: public boost::enable_shared_from_this< WItemSelection >
{
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
     * Adds an item to the list of selectable items.
     *
     * \param name the name
     * \param description the description
     * \param icon an icon to show together with this item. Useful to illustrate the selection options.
     */
    virtual void addItem( std::string name, std::string description, const char** icon = NULL );

    /**
     * Creates an default selection (all items selected). After the first call of this function, no more items can be added using \ref addItem.
     *
     * \return an selector.
     */
    virtual WItemSelector getSelectorAll();

    /**
     * Creates an default selection (no items selected). After the first call of this function, no more items can be added using \ref addItem.
     *
     * \return an selector.
     */
    virtual WItemSelector getSelectorNone();

    /**
     * Creates an default selection (first item selected). After the first call of this function, no more items can be added using \ref addItem.
     * If no item is in the selection, nothing is selected.
     *
     * \return an selector.
     */
    virtual WItemSelector getSelectorFirst();

    /**
     * Creates an default selection (a specified items selected). After the first call of this function, no more items can be added
     * using \ref addItem.
     *
     * \param item the item to select.
     *
     * \return an selector.
     */
    virtual WItemSelector getSelector( size_t item );

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

protected:

    /**
     * List of items.
     */
    std::vector< Item* > m_items;

    /**
     * True if the selection can be modified.
     */
    bool m_modifyable;

private:
};

#endif  // WITEMSELECTION_H

