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

#ifndef WGERESOURCEMANAGER_H
#define WGERESOURCEMANAGER_H

#include <string>

#include <boost/shared_ptr.hpp>

/**
 * Class used to manage out-of-code resources used in the graphics engine. Those resources are especially fonts, shaders and
 * icons/symbols/textures. This is useful to avoid a lot of hard coded path/filenames in the source tree.
 */
class WGEResourceManager
{
public:

    /**
     * Default constructor.
     */
    WGEResourceManager();

    /**
     * Destructor.
     */
    virtual ~WGEResourceManager();

    /**
     * Returns instance of the resource manager. If it does not exists, it will be created.
     *
     * \return the running resource manager instance.
     */
    static boost::shared_ptr< WGEResourceManager > getResourceManager();

    /**
     * Returns the default font which should be used at the most cases.
     *
     * \return the absolute filename to the font.
     */
    std::string getDefaultFont();

    /**
     * Returns the regular (non-bold) font.
     * \note you should use getDefaultFont() in the most cases.
     *
     * \return the absolute filename to the font.
     */
    std::string getRegularFont();

    /**
     * Returns the italic font.
     * \note you should use getDefaultFont() in the most cases.
     *
     * \return the absolute filename to the font.
     */
    std::string getItalicFont();

    /**
     * Returns the bold font.
     * \note you should use getDefaultFont() in the most cases.
     *
     * \return the absolute filename to the font.
     */
    std::string getBoldFont();

protected:

    /**
     * Filename of standard regular font.
     */
    std::string m_regularFont;

    /**
     * Filename of standard bold font.
     */
    std::string m_boldFont;

    /**
     * Filename of standard italic font.
     */
    std::string m_italicFont;

private:

    /**
     * Singleton instance of WGraphicsEngine.
     */
    static boost::shared_ptr< WGEResourceManager > m_instance;
};

#endif  // WGERESOURCEMANAGER_H

