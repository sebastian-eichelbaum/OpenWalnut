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

#ifndef WPATHHELPER_H
#define WPATHHELPER_H

#include <vector>

#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>

#include "WExportCommon.h"
/**
 * Singleton class helping to find files and paths. It is a useful to to search for resources and the central place to "hardcode" relative paths.
 * It contains global paths only. Modules have their OWN local paths.
 */
class OWCOMMON_EXPORT WPathHelper // NOLINT
{
public:

    /**
     * Destructor.
     */
    virtual ~WPathHelper();

    /**
     * Returns instance of the path helper. If it does not exists, it will be created.
     *
     * \return the running path helper instance.
     */
    static boost::shared_ptr< WPathHelper > getPathHelper();

    /**
     * Set the current application path. This should be called only once.
     *
     * \param appPath the application path
     */
    void setAppPath( boost::filesystem::path appPath );

    /**
     * The path where the binary file resides in. This is for example /usr/bin.
     *
     * \return the application path.
     */
    static boost::filesystem::path getAppPath();

    /**
     * The path of the configuration file walnut.cfg. Always use this file to parse configurations.
     *
     * \return the config file.
     */
    static boost::filesystem::path getConfigFile();

    /**
     * The path where font files reside in.
     *
     * \return the font path.
     */
    static boost::filesystem::path getFontPath();

    /**
     * Paths to all known fonts.
     */
    typedef struct
    {
        /**
         * The default font to use in most cases.
         */
        boost::filesystem::path Default;

        /**
         * The Regular font (not bold, not italic)
         */
        boost::filesystem::path Regular;

        /**
         * Italic font.
         */
        boost::filesystem::path Italic;

        /**
         * Bold font.
         */
        boost::filesystem::path Bold;
    }
    Fonts;

    /**
     * The paths to all fonts supported.
     *
     * \return the file paths to all fonts
     */
    static Fonts getAllFonts();

    /**
     * The path to the global shaders. Modules usually have their own local shader directory.
     *
     * \return global shader path.
     */
    static boost::filesystem::path getShaderPath();

    /**
     * The path to the globally installed modules. This does not respect any environment variables or config options! Use this only to search
     * global modules. To get a list of all module search paths, including user defined ones, use getAllModulePaths().
     *
     * \return path to globally installed modules.
     */
    static boost::filesystem::path getModulePath();

    /**
     * This returns a list of search paths for modules. This list is defined by the "modules.path" - option in walnut.cfg. All of these
     * directories CAN contain modules. On startup, they get searched in the specified order.
     *
     * \return list of search paths for modules
     */
    static std::vector< boost::filesystem::path > getAllModulePaths();

    /**
     * The path to the OW libs. You normally should not need this.
     *
     * \return the path to the libs.
     */
    static boost::filesystem::path getLibPath();

    /**
     * The path where shared files reside in.
     *
     * \return the shared files path.
     */
    static boost::filesystem::path getSharePath();

protected:

    /**
     * Constructors are protected because this is a Singleton.
     */
    WPathHelper();

private:

    /**
     * Application path. NOT the path of the binary. The binary path is m_appPath+"/walnut".
     */
    boost::filesystem::path m_appPath;

    /**
     * The path where all the shared files reside in.
     */
    boost::filesystem::path m_sharePath;

    /**
     * The path to the globally installed modules.
     */
    boost::filesystem::path m_modulePath;

    /**
     * The path to the OW libs.
     */
    boost::filesystem::path m_libPath;

    /**
     * Singleton instance of WPathHelper.
     */
    static boost::shared_ptr< WPathHelper > m_instance;
};

#endif  // WPATHHELPER_H

