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

#ifndef WMODULEMETAINFORMATION_H
#define WMODULEMETAINFORMATION_H

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/filesystem/path.hpp>

#include "../common/WStructuredTextParser.h"

class WModule;

/**
 * A class abstracting module meta information. It encapsulates module name, description, icon, author lists, help resources, online resources
 * and much more. It is guaranteed to, at least, provide a module name. Everything else is optional. It also encapsulates the
 * WStructuredTextParser class for loading the data.
 */
class WModuleMetaInformation
{
public:
    /**
     * Convenience typedef for a boost::shared_ptr< WModuleMetaInformation >.
     */
    typedef boost::shared_ptr< WModuleMetaInformation > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WModuleMetaInformation >.
     */
    typedef boost::shared_ptr< const WModuleMetaInformation > ConstSPtr;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // these are some structs to simply handling of the meta file structures

    /**
     * Structure to contain all supported author information
     */
    struct Author
    {
        /**
         * Author name. Will never be empty.
         */
        std::string m_name;

        /**
         * URL to a website of the author. Can be empty.
         */
        std::string m_url;

        /**
         * E-Mail contact to the author. Can be empty.
         */
        std::string m_email;

        /**
         * What has this author done on the module? Can be empty.
         */
        std::string m_what;
    };

    /**
     * Structure to encapsulate the META info online resources.
     */
    struct Online
    {
        /**
         * Online resource's name.
         */
        std::string m_name;

        /**
         * Online resource's description.
         */
        std::string m_description;

        /**
         * The url to the resource.
         */
        std::string m_url;
    };

    /**
     * Structure to encapsulate a screenshot info
     */
    struct Screenshot
    {
        /**
         * The screenshot filename
         */
        boost::filesystem::path m_filename;

        /**
         * The description text shown for the screenshot.
         */
        std::string m_description;
    };

    /**
     * Constructor. The help object will be empty, meaning there is no further meta info available. The name is the only required value. Of
     * course, this is of limited use in most cases.
     *
     * \param name the name of the module
     */
    explicit WModuleMetaInformation( std::string name );

    /**
     * Construct a meta info object that loads all information from the specified file. If the file exist and could not be parsed, only
     * an error log is printed. No exception is thrown.
     *
     * \param module The module to load the meta file for.
     */
    explicit WModuleMetaInformation( boost::shared_ptr< WModule > module );

    /**
     * Destructor. Cleans internal list.
     */
    virtual ~WModuleMetaInformation();

    /**
     * The name of the module. Will always return the name of the module given on construction.
     *
     * \return the name
     */
    std::string getName() const;

    /**
     * Get the icon path. Can be invalid. Check for existence before opening.
     *
     * \return the path to the icon file
     */
    boost::filesystem::path getIcon() const;

    /**
     * Check whether the meta info contained an icon.
     *
     * \return true if icon is available. Does not check existence or validity of image file.
     */
    bool isIconAvailable() const;

    /**
     * The URL to a module website. Can be empty.
     *
     * \return URL to website
     */
    std::string getWebsite() const;

    /**
     * A module description. Can be empty but is initialized with the description of the module given on construction.
     *
     * \return the description.
     */
    std::string getDescription() const;

    /**
     * Path to a text or HTML file containing some module help. Can be invalid. Check for existence before opening.
     *
     * \return the path to help
     */
    boost::filesystem::path getHelp() const;

    /**
     * A list of authors. If the META file did not contain any author information, this returns the OpenWalnut Team as author.
     *
     * \return Author list.
     */
    std::vector< Author > getAuthors() const;

    /**
     * A list of online resources. Can be empty.
     *
     * \return list of online material
     */
    std::vector< Online > getOnlineResources() const;

    /**
     * A list of tags provided for the module.
     *
     * \return the tag list.
     */
    std::vector< std::string > getTags() const;

    /**
     * Returns the list of screenshots.
     *
     * \return the screenshot list.
     */
    std::vector< Screenshot > getScreenshots() const;

    /**
     * Query a value from the META file.
     *
     * \tparam ResultType the type of the result of the query. The function tries to cast the found value to this type. If this is not possible,
     * the default value will be returned.
     * \param path the absolute path in the META file. Please be aware that, if you specify a value inside your modules meta block, you need to
     * add the module name too. The path is absolute!
     * \param defaultValue the default value to return in case of an non-existing element or cast problems.
     *
     * \throw WTypeMismatch if the value cannot be cast to the specified target type
     *
     * \return the value, or defaultType.
     */
    template< typename ResultType >
    ResultType query( std::string path, ResultType defaultValue = ResultType() ) const
    {
        // find key-value pair
        return m_metaData.getValue< ResultType >( path, defaultValue );
    }

    /**
     * Query multiple values from the META file.
     *
     * \tparam ResultType the type of the result of the query. The function tries to cast the found value to this type. If this is not possible,
     * the default value will be returned.
     * \param path the absolute path in the META file. Please be aware that, if you specify a value inside your modules meta block, you need to
     * add the module name too. The path is absolute!
     * \param defaultValues the default value to return in case of an non-existing element or cast problems.
     *
     * \throw WTypeMismatch if the value cannot be cast to the specified target type
     *
     * \return the value vector, or defaultType.
     */
    template< typename ResultType >
    std::vector< ResultType > query( std::string path, const std::vector< ResultType >& defaultValues ) const
    {
        // find key-value pair
        return m_metaData.getValues< ResultType >( path, defaultValues );
    }

    /**
     * Check whether the value specified by "path" exists.
     *
     * \param path the absolute path in the META file. Please be aware that, if you specify a value inside your modules meta block, you need to
     * add the module name too. The path is absolute!
     *
     * \return true if it exists.
     */
    bool valueExists( std::string path ) const;

protected:
private:
    /**
     * The name of the module providing this meta information.
     */
    std::string m_name;

    /**
     * The default description if none was specified in the META file. Initialized with the description of the module specified during
     * construction.
     */
    std::string m_description;

    /**
     * The tree representing the data
     */
    WStructuredTextParser::StructuredValueTree m_metaData;

    /**
     * If true, m_metaData should be queried in all getters. If false, you can query m_meta but it will only tell you that the desired value
     * could not be found.
     */
    bool m_loaded;

    /**
     * The module local path. Used for several meta infos returning a path.
     */
    boost::filesystem::path m_localPath;
};

#endif  // WMODULEMETAINFORMATION_H

