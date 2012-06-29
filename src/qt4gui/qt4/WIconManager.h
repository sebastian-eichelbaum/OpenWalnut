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

#include <boost/filesystem/path.hpp>

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
     * Adds an Icon to the icon store. Use this to load an icon from a file.
     *
     * \throw WFileNotFound if file could not be loaded or found.
     *
     * \param name The icon's name
     * \param filename The path to the file.
     */
    void addIcon( std::string name, boost::filesystem::path filename );

    /**
     * Searches icons in the internal map and all modules for the given icon name.
     * \param name Name of the requested icon
     * \return A QIcon copy if the icon name was found otherwise a WAssert will fail.
     */
    QIcon getIcon( const std::string name );

    /**
     * Searches icons in the internal map and all modules for the given icon name. In contrast to the single argument getIcon, this does not throw
     * an exception if the icon is not found. It returns a default.
     * \param name Name of the requested icon
     * \param defaultIcon the default icon to return if no other icon was found.
     * \return A QIcon copy if the icon name was found otherwise, the default is returned
     */
    QIcon getIcon( const std::string name, const QIcon& defaultIcon );
protected:
private:
    std::map< std::string, QIcon* > m_iconList; //!< A map storing icons and the names used to identify them
};

#endif  // WICONMANAGER_H
