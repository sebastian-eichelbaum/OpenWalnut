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
 * class to store and handle access to all available icons
 */
class WIconManager
{
public:
    /**
     * function to add an Icon to the icon store
     * \param name the icon's name
     * \param xpm the icon itself in XPM format
     */
    void addIcon( std::string name, const char* const xpm[] );

    /**
     * returns a previously stored icon
     * \param name name of the requested icon
     */
    QIcon getIcon( const std::string name );

protected:
private:
    std::map< std::string, QIcon* >m_iconList; //!< A map storing icons and the names used to identify them
};

#endif  // WICONMANAGER_H
