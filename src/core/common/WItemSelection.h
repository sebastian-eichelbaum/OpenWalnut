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
#include "WItemSelectionItem.h"

class WItemSelector;

/**
 * A class containing a list of named items. It is mainly a container for an std::vector but with the difference that there can be so called
 * Selectors which are able to select some subset of the item set. This is especially useful in properties where item selection is needed. The
 * class is kept very restrictive to keep the interface clean and sleek and to keep the item set consistent among several threads. So please do
 * not implement any function that might change the item list, use the provided ones. If the item list changes, existing selectors get invalid
 * automatically using the change condition of the inherited WSharedSequenceContainer.
 */
class WItemSelection: public boost::enable_shared_from_this< WItemSelection >,
                      public WSharedSequenceContainer< std::vector< boost::shared_ptr< WItemSelectionItem > > >
{
    friend class WItemSelector; //!< for proper locking and unlocking
public:
    /**
     * Convenience typedef for a boost::shared_ptr< WItemSelection >
     */
    typedef boost::shared_ptr< WItemSelection > SPtr;

    /**
     * Convenience typedef for a  boost::shared_ptr< const WItemSelection >
     */
    typedef boost::shared_ptr< const WItemSelection > ConstSPtr;

    /**
     * Default constructor.
     */
    WItemSelection();

    /**
     * Destructor.
     */
    virtual ~WItemSelection();

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
     * Creates an default selection (last item selected). The selector gets invalid if another item is added.
     *
     * \return an selector.
     */
    virtual WItemSelector getSelectorLast();

    /**
     * Creates an default selection (a specified items selected). The selector gets invalid if another item is added.
     *
     * \param item the item to select.
     *
     * \return an selector.
     */
    virtual WItemSelector getSelector( size_t item );

    /**
     * Convenience method to create a new item.
     *
     * \param name name of the item
     * \param description the description, can be empty
     * \param icon the icon, can be NULL
     *
     * \return the Item.
     */
    static boost::shared_ptr< WItemSelectionItem > Item( std::string name, std::string description = "", const char** icon = NULL )
    {
        return boost::shared_ptr< WItemSelectionItem >( new WItemSelectionItem( name, description, icon ) );
    }

    /**
     * Convenience method to add a new item.
     *
     * \param name name of the item
     * \param description the description, can be empty
     * \param icon the icon, can be NULL
     *
     */
    void addItem( std::string name, std::string description = "", const char** icon = NULL );

    /**
     * Method to add a new item, which can be derived from WItemSelectionItem.
     *
     * \param item WItemSelectionItem or derivation which should be add.
     */
    void addItem( boost::shared_ptr< WItemSelectionItem > item );

private:
};

#endif  // WITEMSELECTION_H

