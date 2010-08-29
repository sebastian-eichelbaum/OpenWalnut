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

#include <boost/tokenizer.hpp>

#include "WPreferences.h"

#include "WPathHelper.h"

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
    if ( !m_instance )
    {
        m_instance = boost::shared_ptr< WPathHelper >( new WPathHelper() );
    }

    return m_instance;
}

void WPathHelper::setAppPath( boost::filesystem::path appPath )
{
    m_appPath    = appPath;
    m_sharePath  = m_appPath / "../share/OpenWalnut";
    m_libPath    = m_appPath / "../lib";
    m_modulePath = m_libPath / "modules";
}

boost::filesystem::path WPathHelper::getAppPath()
{
    return getPathHelper()->m_appPath;
}

boost::filesystem::path WPathHelper::getConfigFile()
{
    // TODO(ebaum): we should prefer a user config file in ~.walnut.cfg in future. This needs to be platform independent of course.
    return getPathHelper()->m_sharePath / "walnut.cfg";
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
    fonts.Regular   = getFontPath() / "LiberationMono-Regular.ttf";
    fonts.Bold      = getFontPath() / "LiberationMono-Bold.ttf";
    fonts.Italic    = getFontPath() / "LiberationMono-Italic.ttf";
    fonts.Default   = fonts.Bold;

    return fonts;
}

boost::filesystem::path WPathHelper::getModulePath()
{
    return getPathHelper()->m_modulePath;
}

boost::filesystem::path WPathHelper::getLibPath()
{
    return getPathHelper()->m_libPath;
}

boost::filesystem::path WPathHelper::getSharePath()
{
    return getPathHelper()->m_sharePath;
}

std::vector< boost::filesystem::path > WPathHelper::getAllModulePaths()
{
    // the list of paths
    std::vector< boost::filesystem::path > paths;
    // the first element always is the global search path
    paths.push_back( getModulePath() );

    std::string additionalPaths = "";
    if ( !WPreferences::getPreference< std::string >( "modules.path", &additionalPaths ) )
    {
        // no config option found.
        return paths;
    }

    // separate list of additional paths:
    typedef boost::tokenizer< boost::char_separator< char > > tokenizer;
    boost::char_separator< char > sep( ";" );
    tokenizer tok( additionalPaths, sep );
    for ( tokenizer::iterator it = tok.begin(); it != tok.end(); ++it )
    {
        paths.push_back( boost::filesystem::path( *it ) );
    }

    return paths;
}

