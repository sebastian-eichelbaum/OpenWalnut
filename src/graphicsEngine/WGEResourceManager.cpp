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

#include <stdlib.h>

#include <string>

#include <boost/filesystem.hpp>

#include "WGEResourceManager.h"
#include "WGraphicsEngine.h"

// resource manager instance as singleton
boost::shared_ptr< WGEResourceManager > WGEResourceManager::m_instance = boost::shared_ptr< WGEResourceManager >();

WGEResourceManager::WGEResourceManager()
{
    // initialize
}

WGEResourceManager::~WGEResourceManager()
{
    // cleanup
}

boost::shared_ptr< WGEResourceManager > WGEResourceManager::getResourceManager()
{
    if ( !m_instance )
    {
        m_instance = boost::shared_ptr< WGEResourceManager >( new WGEResourceManager() );
    }

    return m_instance;
}

std::string WGEResourceManager::getDefaultFont()
{
    // NOTE: as osgVoewer::StatsHandler uses hard coded font names, we need to ensure that our default font also is available as file "arial.ttf"
    // in the fonts subdirectory. So, if you change the default font here please also ensure arial.ttf points to/is the same as the new default
    // font.
    return m_boldFont;
}

std::string WGEResourceManager::getRegularFont()
{
    return m_regularFont;
}

std::string WGEResourceManager::getItalicFont()
{
    return m_italicFont;
}

std::string WGEResourceManager::getBoldFont()
{
    return m_boldFont;
}

void WGEResourceManager::setFontPath( std::string path )
{
    // NOTE: the osgViewer::StatsHandler uses a hard coded font filename. :-(. Fortunately OSG allows us to modify the search path using
    // environment variables:
    setenv( "OSGFILEPATH", path.c_str(), 1 );

    namespace fs = boost::filesystem;
    fs::path fontPath = path;
    m_regularFont = fs::path( fontPath / "LiberationMono-Regular.ttf" ).file_string();
    m_boldFont = fs::path( fontPath / "LiberationMono-Bold.ttf" ).file_string();
    m_italicFont = fs::path( fontPath / "LiberationMono-Italic.ttf" ).file_string();
}

