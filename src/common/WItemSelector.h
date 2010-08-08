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

#ifndef WITEMSELECTOR_H
#define WITEMSELECTOR_H

#include <istream>
#include <ostream>
#include <vector>
#include <string>

#include <boost/shared_ptr.hpp>

#include "WItemSelection.h"
#include "WExportCommon.h"

/**
 * This class represents a subset of a WItemSelection. It is a class for managing selections. The class is kept very restrictive. The selection
 * can't be edited after the instantiation of the class to keep the interface clean, easily usable and consistent among multiple threads. So
 * please DO NOT extend it to provide methods for changing it!
 *
 * This class can be seen as some kind of special "iterator" providing access to the underlying set without allowing it to be modified. The class
 * provides methods to access the whole set and the subset represented by itself. The restrictive interface ensures thread-safety and enforces
 * that each new selection is done by a new instance of this class, which is needed by the WPropertyVariable to work properly.
 *
 * \note the protected constructor avoids instance creation of classes not the WItemSelection. This is restrictive but needed. Nobody can create
 * instances of it, changing the underlying WItemSelection and using it as selector for another ItemSelection instance.
 */
class OWCOMMON_EXPORT WItemSelector // NOLINT
{
friend class WItemSelection;
public:

    /**
     * The type used for storing index lists. It is a list of integer correlating with the elements in the managed WItemSelection class.
     */
    typedef  std::vector< size_t > IndexList;

    /**
     * Destructor.
     */
    virtual ~WItemSelector();

    /**
     * Creates a new valid instance with the specified items selected. This is especially useful to simply create a new selection if only the old
     * selection is known.
     *
     * \param selected the selected items (their index in WItemSelection).
     *
     * \return the new selector instance
     */
    WItemSelector newSelector( IndexList selected ) const;

    /**
     * Creates a new valid instance with the specified items selected. This is especially useful to simply create a new selection if only the
     * string representing it is known. This somehow correlates to the << operator.
     *
     * \param asString the selected items
     *
     * \return the new selector instance
     */
    WItemSelector newSelector( const std::string asString ) const;

    /**
     * Compares two selector. They are assumed to be equal if the selected items are equal and if the underlying WItemSelection is the same.
     *
     * \param other the selector
     *
     * \return true if equal
     */
    bool operator==( const WItemSelector& other ) const;

    /**
     * Write a selection in string representation to the given output stream.
     *
     * \param out the output stream where to put the information
     *
     * \return the output stream extended by the information of this selector
     */
    std::ostream& operator<<( std::ostream& out ) const;

    /**
     * Gives the count of elements in the set of selectable items. This is \ref size + number of unselected items.
     *
     * \return the number of all items in the item set.
     */
    virtual size_t sizeAll() const;

    /**
     * The number of selected items.
     *
     * \return the number of selected items.
     */
    virtual size_t size() const;

    /**
     * True if the selection is empty.
     *
     * \return true if nothing is selected.
     */
    virtual bool empty() const;

    /**
     * Gets the item with the given index from the WItemSelection. This index does not equal the index of the same item for \ref at. This method
     * is useful to go through the list of ALL items (not only the selected).
     *
     * \param index the index
     *
     * \return the item
     */
    virtual const WItemSelection::Item& atAll( size_t index ) const;

    /**
     * Gets the selected item with the given index. This is not the same index as the element has in the corresponding WItemSelection!
     * This method is especially useful to iterate the through the selected items.
     *
     * \param index the index
     *
     * \return the item
     */
    virtual const WItemSelection::Item& at( size_t index ) const;

    /**
     * Helps to get the index of an selected item in the WItemSelection. This is somehow similar to \ref at, but does not return the item but the
     * index to it.
     *
     * \param index the index in the selection (not the item index in WItemSelection)
     *
     * \return the index in WItemSelection.
     */
    virtual size_t getItemIndexOfSelected( size_t index ) const;

protected:

    /**
     * Constructor creates an selector for the specified selection of items. Noting is selected after construction.
     *
     * \param selection the selection handled by this instance
     * \param selected the set of selected items
     */
    WItemSelector( boost::shared_ptr< WItemSelection > selection, IndexList selected );

    /**
     * The selection handled by this selector.
     */
    boost::shared_ptr< WItemSelection > m_selection;

    /**
     * The list of items currently selected.
     */
    IndexList m_selected;

private:
};

/**
 * Write a selection in string representation to the given output stream.
 *
 * \param out the output stream where to put the information
 * \param other the instance to write out
 *
 * \return the output stream extended by the information of this selector
 */
std::ostream& operator<<( std::ostream& out, const WItemSelector& other );

#endif  // WITEMSELECTOR_H

