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

#ifndef WITEMSELECTIONITEM_H
#define WITEMSELECTIONITEM_H

#include <string>

/**
 * Class for keeping a single named item in a WItemSelection.
 */
class WItemSelectionItem // NOLINT
{
public:
    /**
     * Constructs a new item with the specified values.
     *
     * \param name Name of item.
     * \param description Description, can be empty.
     * \param icon Icon, can be NULL.
     */
    WItemSelectionItem( std::string name, std::string description = "", const char** icon = NULL );

    /**
     * Destruction. Does NOT delete the icon!
     */
    virtual ~WItemSelectionItem();

    /**
     * Returns the name of the item.
     *
     * \return the name
     */
    std::string getName() const;

    /**
     * The description of the item.
     *
     * \return the description
     */
    std::string getDescription() const;

    /**
     * The icon associated with this item. Can be NULL.
     *
     * \return the icon, might be NULL.
     */
    const char** getIcon() const;

    /**
     * Dynamic cast of the object, if a derivative of WItemSelectionItem was add to WItemSelection.
     *
     * \return Returns the converted item of new type T or 0 if a conversion is not possible.
     */
    template< typename T > T* getAs()
    {
        return dynamic_cast< T* >( this );
    }

    /**
     * Compares this and another item using their names only.
     *
     * \param other the second to compare the this one with
     *
     * \return true if the names are equal.
     */
    bool operator==( const WItemSelectionItem& other ) const;

protected:
    /**
     * Item name.
     */
    std::string m_name;

    /**
     * Item description.
     */
    std::string m_description;

    /**
     * Item icon.
     */
    const char** m_icon;

private:
};

#endif  // WITEMSELECTIONITEM_H
