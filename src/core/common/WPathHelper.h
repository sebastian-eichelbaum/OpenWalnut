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

// Use filesystem version 2 for compatibility with newer boost versions.
#ifndef BOOST_FILESYSTEM_VERSION
    #define BOOST_FILESYSTEM_VERSION 2
#endif
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
     * Set the current application path. This should be called only once. The home path hereby is NOT the users home. It is an directory, where
     * OW can write user specific data. A good default here is to specify USERHOME/.OpenWalnut for example.
     *
     * \param appPath the application path
     * \param homePath the OW home path
     */
    void setBasePaths( boost::filesystem::path appPath, boost::filesystem::path homePath );

    /**
     * The path where the binary file resides in. This is for example /usr/bin.
     *
     * \return the application path.
     */
    static boost::filesystem::path getAppPath();

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
     * The path to the OW dir in the user's home. This will not be the home dir directly. It is something like $HOME/.OpenWalnut.
     *
     * \return OW home path
     */
    static boost::filesystem::path getHomePath();

    /**
     * This returns a list of search paths for modules. This list is defined by the environment variable "OW_MODULE_PATH" and the list of additional
     * module paths. All of these
     * directories CAN contain modules. On startup, they get searched in the specified order.
     *
     * \return list of search paths for modules
     */
    static std::vector< boost::filesystem::path > getAllModulePaths();

    /**
     * This method adds the given path to the list of module paths. This way, arbitrary paths can be specified to search for modules. Each path
     * is searched recursively.
     *
     * \param path the path to add.
     */
    void addAdditionalModulePath( const boost::filesystem::path& path );

    /**
     * Returns the list of paths added using addAdditionalModulePath. This does NOT contain the paths in OW_MODULE_PATH. Use getAllModulePaths
     * for this.
     *
     * \return the list of additional paths
     */
    const std::vector< boost::filesystem::path >& getAdditionalModulePaths() const;

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

    /**
     * The path where the doc files reside in.
     *
     * \return the doc file path.
     */
    static boost::filesystem::path getDocPath();

    /**
     * The path where the config files reside in.
     *
     * \return the config file path.
     */
    static boost::filesystem::path getConfigPath();

protected:

    /**
     * Constructors are protected because this is a Singleton.
     */
    WPathHelper();

private:

    /**
     * Application path. NOT the path of the binary. The application path is the directory in which the binary is placed.
     * The binary path is m_appPath+"/openwalnut".
     */
    boost::filesystem::path m_appPath;

    /**
     * The path where all the shared files reside in.
     */
    boost::filesystem::path m_sharePath;

    /**
     * The path where all the documentation files reside in.
     */
    boost::filesystem::path m_docPath;

    /**
     * The path where all the config files reside in.
     */
    boost::filesystem::path m_configPath;

    /**
     * The path to the globally installed modules.
     */
    boost::filesystem::path m_modulePath;

    /**
     * The path to the OW libs.
     */
    boost::filesystem::path m_libPath;

    /**
     * The path of a user specific OW directory.
     */
    boost::filesystem::path m_homePath;

    /**
     * A list of additional paths to search for modules. This does not contain the paths in the environment variable OW_MODULE_PATH. This method
     * is not thread-safe. You should only use it before the module factory loads the modules.
     */
    std::vector< boost::filesystem::path > m_additionalModulePaths;

    /**
     * Singleton instance of WPathHelper.
     */
    static boost::shared_ptr< WPathHelper > m_instance;
};

#endif  // WPATHHELPER_H

