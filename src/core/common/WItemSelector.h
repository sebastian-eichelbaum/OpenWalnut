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
#include <boost/signals2/signal.hpp>

#include "WItemSelection.h"
#include "WItemSelectionItem.h"
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
     * Copy constructor. Creates a new copy of the selector and ensure proper signal subscriptions to the underlying selection.
     *
     * \param other the selector to copy
     */
    WItemSelector( const WItemSelector& other );

    /**
     * Copy assignment. Creates a new copy of the selector and ensure proper signal subscriptions to the underlying selection.
     *
     * \param other the selector to copy
     *
     * \return this.
     */
    WItemSelector& operator=( const WItemSelector & other );

    /**
     * Destructor.
     */
    virtual ~WItemSelector();

    /**
     * Creates a new valid instance with the specified items selected. This is especially useful to simply create a new selection if only the old
     * selection is known.
     *
     * \note Please be aware that, in the moment this method returns, another thread can make all selectors invalid again causing the returned
     * one to be invalid too. To avoid this, use the newSelector method only if the old has locked the selection using \ref lock and \ref unlock.
     *
     * \param selected the selected items (their index in WItemSelection).
     *
     * \return the new selector instance
     */
    WItemSelector newSelector( IndexList selected ) const;

    /**
     * Creates a new valid instance with the specified items selected. This can be useful to add a certain index. The new selector has the
     * selection from this AND the specified one. If you want to create a selector containing only one selected item, use the method that uses
     * the IndexList.
     *
     * \note Please be aware that, in the moment this method returns, another thread can make all selectors invalid again causing the returned
     * one to be invalid too. To avoid this, use the newSelector method only if the old has locked the selection using \ref lock and \ref unlock.
     *
     * \param selected the selected item (the index in WItemSelection).
     *
     * \return the new selector instance
     */
    WItemSelector newSelector( size_t selected ) const;

    /**
     * Creates a new valid instance with the specified items selected. This is especially useful to simply create a new selection if only the
     * string representing it is known. This somehow correlates to the << operator.
     *
     * \note Please be aware that, in the moment this method returns, another thread can make all selectors invalid again causing the returned
     * one to be invalid too. To avoid this, use the newSelector method only if the old has locked the selection using \ref lock and \ref unlock.
     *
     * \param asString the selected items
     *
     * \return the new selector instance
     */
    WItemSelector newSelector( const std::string asString ) const;

    /**
     * Creates a new selector, but basing on this instance as old one. The new selector tries to keep the old selection but makes the internal
     * selection list valid with the current underlying selection.
     *
     * \note Please be aware that, in the moment this method returns, another thread can make all selectors invalid again causing the returned
     * one to be invalid too. To avoid this, use the newSelector method only if the old has locked the selection using \ref lock and \ref unlock.
     *
     * \return the new (valid) selector.
     */
    WItemSelector newSelector() const;

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
    virtual const boost::shared_ptr< WItemSelectionItem > atAll( size_t index ) const;

    /**
     * Gets the selected item with the given index. This is not the same index as the element has in the corresponding WItemSelection!
     * This method is especially useful to iterate the through the selected items.
     *
     * \param index the index
     *
     * \return the item
     */
    virtual const boost::shared_ptr< WItemSelectionItem > at( size_t index ) const;

    /**
     * Helps to get the index of an selected item in the WItemSelection. This is somehow similar to \ref at, but does not return the item but the
     * index to it.
     *
     * \param index the index in the selection (not the item index in WItemSelection)
     *
     * \return the index in WItemSelection.
     */
    virtual size_t getItemIndexOfSelected( size_t index ) const;

    /**
     * Checks whether the selection is valid anymore. If a selector is not valid anymore, you should ask the one providing the selectors (most
     * probably a WPropSelection) for a new one.
     *
     * \return true if valid.
     */
    virtual bool isValid() const;

    /**
     * Read locks the underlying selection. This ensure, that the selection stays fixed as long as this selector is locked. This also ensures
     * that no invalidation can be issued as long as this selector has the lock. BUT it is possible that an invalidation occurs while this
     * selector waits. So please always check for validity of the selector ater locking.
     */
    void lock();

    /**
     * Unlocks the selection again. Always call this after a lock.
     */
    void unlock();

    /**
     * Allow cast from selector to unsigned int.
     *
     * \return the index of the first selected item in the selection.
     */
    operator unsigned int() const;

    /**
     * Casts the selector to a list of indices currently selected. It contains the list of index in the corresponding WItemSelection. This is
     * especially useful if the whole index list is needed without nasty iterations.
     *
     * \return the list of index.
     */
    IndexList getIndexList() const;

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

    /**
     * Stores the connection made using WItemSelection::subscribeInvalidateSignal.
     */
    boost::signals2::connection m_invalidateSignalConnection;

private:
    /**
     * Creates a new selector instance using the specified index list. Handles all needed signal subscription stuff.
     *
     * \param selected the index list of selected items
     *
     * \return new selector
     */
    WItemSelector createSelector( const IndexList& selected ) const;

    /**
     * Handles the case of invalidation.
     */
    void invalidate();

    /**
     * If true the selector is valid.
     */
    bool m_valid;

    /**
     * This locks prevents the selection to be modified during selector iteration.
     */
    WItemSelection::ReadTicket m_lock;
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

