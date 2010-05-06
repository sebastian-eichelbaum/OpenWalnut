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

#include <boost/shared_ptr.hpp>

class WItemSelection;

/**
 * This class represents a subset of a WItemSelection. It is a class for managing selections. The class is kept very restrictive. The selection
 * can't be edited after the instantiation of the class to keep the interface clean, easily usable and consistent among multiple threads. So
 * please DO NOT extend it to provide methods for changing it!
 *
 * \NOTE the protected constructor avoids instance creation of classes not the WItemSelection. This is restrictive but needed. Nobody can create
 * instances of it, changing the underlying WItemSelection and using it as selector for another ItemSelection instance.
 */
class WItemSelector
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
    WItemSelector newSelector( IndexList selected );

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
 */
std::ostream& operator<<( std::ostream& out, const WItemSelector& c );

/**
 * Read a selection in string representation from the given input stream.
 */
std::istream& operator>>( std::istream& in, WItemSelector& c );

#endif  // WITEMSELECTOR_H

