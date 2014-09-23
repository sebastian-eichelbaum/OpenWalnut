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

#include "core/graphicsEngine/WGEImage.h"

/**
 * Manages icon access. Icons stored either here inside or inside of modules.
 */
class WIconManager
{
public:
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

    /**
     * Add a mapping for icons. This is useful if you want to use "telling" icons names in your code but want to map these names to some standard
     * icon. The mapping of the new name needs to be unique. Adding multiple mappings for the newName will be ignored. You can overwrite a
     * mapping.
     *
     * \param newName the name getting mapped
     * \param mapToThis the icon to use when calling getIcon( newName ). Never add a file anding as png or jpg!
     */
    void addMapping( const std::string& newName, const std::string& mapToThis );

    /**
     * Convert a WGEImage to an QIcon. Image must be 2D. Everything else causes an empty icon to be returned.
     *
     * \param image the image.
     *
     * \return icon
     */
    static QIcon convertToIcon( WGEImage::SPtr image );

    /**
     * Return an icon representing a default "No Icon" icon.
     *
     * \return the icon
     */
    static QIcon getNoIconDefault();
protected:
private:
    std::map< std::string, std::string > m_iconMappingList; //!< A map storing icons and the names used to identify them
};

#endif  // WICONMANAGER_H
