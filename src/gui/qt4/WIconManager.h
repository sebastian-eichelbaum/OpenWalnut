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

#ifndef WICONMANAGER_H
#define WICONMANAGER_H

#include <map>
#include <string>

#include <QtGui/QIcon>

/**
 * Manages icon access. Icons stored either here inside or inside of modules.
 */
class WIconManager
{
public:
    /**
     * Adds an Icon to the icon store
     * \param name The icon's name
     * \param xpm The icon itself in XPM format
     */
    void addIcon( std::string name, const char* const xpm[] );

    /**
     * Searches icons in the internal map and all modules for the given icon name.
     * \param name Name of the requested icon
     * \return A QIcon copy if the icon name was found otherwise a WAssert will fail.
     */
    QIcon getIcon( const std::string name );

protected:
private:
    std::map< std::string, QIcon* > m_iconList; //!< A map storing icons and the names used to identify them
};

#endif  // WICONMANAGER_H
