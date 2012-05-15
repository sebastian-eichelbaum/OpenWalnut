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

#ifndef WITEMSELECTIONITEMTYPED_H
#define WITEMSELECTIONITEMTYPED_H

#include <cstddef>
#include <string>

#include "WItemSelectionItem.h"

/**
 * A derivation of WItemSelection which can store a value of any type.
 */
template< typename T > class WItemSelectionItemTyped: public WItemSelectionItem // NOLINT
{
public:
    /**
     * Constructs a new item with the specified values.
     *
     * \param value Value which is stored by the item.
     * \param name Name of item.
     * \param description Description, can be empty.
     * \param icon Icon, can be NULL.
     */
    WItemSelectionItemTyped( T value, std::string name, std::string description = "", const char** icon = NULL ) :
                    WItemSelectionItem( name, description, icon ), m_value( value )
    {
    }

    /**
     * Destruction. Does NOT delete the icon!
     */
    virtual ~WItemSelectionItemTyped()
    {
    }

    /**
     * Returns the value.
     *
     * \return Value which is stored.
     */
    T getValue() const
    {
        return m_value;
    }

    /**
     * Sets a new value, which is associated with this item.
     *
     * \param value new value which should be stored by this item.
     */
    void setValue( T value )
    {
        m_value = value;
    }

private:
    /**
     * Value which is stored by this item.
     */
    T m_value;
};

#endif  // WITEMSELECTIONITEMTYPED_H
