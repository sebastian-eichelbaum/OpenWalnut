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

#include <string>
#include <vector>
#include <cstdlib>
#include <algorithm>

#include <boost/tokenizer.hpp>

#include "WPathHelper.h"

#ifndef W_LIB_DIR_RELATIVE
    #define W_LIB_DIR_RELATIVE "lib"
#endif

// path helper instance as singleton
boost::shared_ptr< WPathHelper > WPathHelper::m_instance = boost::shared_ptr< WPathHelper >();

WPathHelper::WPathHelper()
{
    // initialize members
}

WPathHelper::~WPathHelper()
{
    // cleanup
}

boost::shared_ptr< WPathHelper > WPathHelper::getPathHelper()
{
    if( !m_instance )
    {
        m_instance = boost::shared_ptr< WPathHelper >( new WPathHelper() );
    }

    return m_instance;
}

void WPathHelper::setBasePaths( boost::filesystem::path appPath, boost::filesystem::path homePath )
{
    m_appPath    = appPath;
    m_homePath   = homePath;
    m_sharePath  = m_appPath / "../share/openwalnut";
    m_docPath    = m_appPath / "../share/doc";
    m_configPath = m_appPath / "../share/openwalnut";
    m_libPath    = m_appPath / ".." / W_LIB_DIR_RELATIVE;   // NOTE: this variable is set by CMake.
    // This is the default search path. Use getAllModulePaths for finding all modules, including those whose
    // directories where defined by the user.
    m_modulePath = m_libPath / "openwalnut"; // This is the default search path.

    // this is the relative path for module resources. It is relative to the path of the lib containing the module.
    // Our build system places modules in lib/openwalnut/packagename/. The relative path needs to go out of the lib directory to a share
    // directory.
    m_moduleResourcePathRelative = boost::filesystem::path( "../../../share/openwalnut/modules" );
}

void WPathHelper::setBasePathsOSXBundle( boost::filesystem::path appPath, boost::filesystem::path homePath )
{
    setBasePaths( appPath, homePath ); // set all to ordinary paths, but the following are different:

    m_sharePath  = m_appPath / "../Resources/openwalnut";
    m_docPath    = m_appPath / "../Resources/doc";
    m_configPath = m_appPath / "../Resources/openwalnut";
    m_modulePath = m_appPath / "../Resources/modules";

    // The MacOSX bundle stores the modules in Resources/modules. We want the additional resources to be stored in the module's directory.
    m_moduleResourcePathRelative = boost::filesystem::path( "." );
}

boost::filesystem::path WPathHelper::getAppPath()
{
    return getPathHelper()->m_appPath;
}

boost::filesystem::path WPathHelper::getFontPath()
{
    return getPathHelper()->m_sharePath / "fonts";
}

boost::filesystem::path WPathHelper::getShaderPath()
{
    return getPathHelper()->m_sharePath / "shaders";
}

WPathHelper::Fonts WPathHelper::getAllFonts()
{
    Fonts fonts;
    fonts.Regular   = getFontPath() / "Regular.ttf";
    fonts.Bold      = getFontPath() / "Bold.ttf";
    fonts.Italic    = getFontPath() / "Italic.ttf";
    fonts.Default   = fonts.Bold;

    return fonts;
}

boost::filesystem::path WPathHelper::getModulePath()
{
    return getPathHelper()->m_modulePath;
}

boost::filesystem::path WPathHelper::getHomePath()
{
    return getPathHelper()->m_homePath;
}

boost::filesystem::path WPathHelper::getLibPath()
{
    return getPathHelper()->m_libPath;
}

boost::filesystem::path WPathHelper::getSharePath()
{
    return getPathHelper()->m_sharePath;
}

boost::filesystem::path WPathHelper::getDocPath()
{
    return getPathHelper()->m_docPath;
}

boost::filesystem::path WPathHelper::getConfigPath()
{
    return getPathHelper()->m_configPath;
}

boost::filesystem::path WPathHelper::getModuleResourcePath( boost::filesystem::path moduleLibPath, std::string packageName )
{
    // relative path to the resources
    boost::filesystem::path resRel = getPathHelper()->m_moduleResourcePathRelative / packageName;

    // This is related to issue #378, the multiarch support. For modules build with multiarch support, the lib dir is one step
    // deeper nested -> hence we need to go up one more step. The resource path is always created, even if empty.
    //
    // If you encounter any problems, please report them to the openwalnut-dev@lists.informatik.uni-leipzig.de.
    if( boost::filesystem::exists( moduleLibPath / resRel ) )
    {
        return moduleLibPath / resRel;
    }
    else if( boost::filesystem::exists( moduleLibPath / ".." / resRel ) )
    {
        return moduleLibPath / ".." / resRel;
    }
    else
    {
        // Lets hope this works ... as both paths are not found, there probably is none.
        return moduleLibPath / resRel;
    }
}

std::vector< boost::filesystem::path > WPathHelper::getAllModulePaths()
{
    // the list of paths
    std::vector< boost::filesystem::path > paths;
    // the first element always is the global search path
    paths.push_back( getModulePath() );
    paths.push_back( getHomePath() / "modules" );

    // the environment variable stores the additional paths
    std::string additionalPaths( getenv( "OW_MODULE_PATH" ) ? getenv( "OW_MODULE_PATH" ) : "" );

    // separate list of additional paths:
    typedef boost::tokenizer< boost::char_separator< char > > tokenizer;
    boost::char_separator< char > sep( ";" );
    tokenizer tok( additionalPaths, sep );
    for( tokenizer::iterator it = tok.begin(); it != tok.end(); ++it )
    {
        paths.push_back( boost::filesystem::path( *it ) );
    }

    // add the additional paths
    for( std::vector< boost::filesystem::path >::const_iterator it = getPathHelper()->m_additionalModulePaths.begin();
                                                                it != getPathHelper()->m_additionalModulePaths.end();
                                                                ++it )
    {
        if( !std::count( paths.begin(), paths.end(), *it ) )
        {
            paths.push_back( *it );
        }
    }

    return paths;
}

void WPathHelper::addAdditionalModulePath( const boost::filesystem::path& path )
{
    if( !std::count( m_additionalModulePaths.begin(), m_additionalModulePaths.end(), path ) )
    {
        m_additionalModulePaths.push_back( path );
    }
}

const std::vector< boost::filesystem::path >& WPathHelper::getAdditionalModulePaths() const
{
    return m_additionalModulePaths;
}
