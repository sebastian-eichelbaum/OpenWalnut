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

#include <algorithm>
#include <string>
#include <vector>

#include "../common/WException.h"
#include "../common/WLogger.h"
#include "../common/WStructuredTextParser.h"
#include "WModule.h"

#include "WModuleMetaInformation.h"

WModuleMetaInformation::WModuleMetaInformation( std::string name ):
    m_name( name ),
    m_loaded( false )
{
    // initialize members
}

WModuleMetaInformation::WModuleMetaInformation( boost::shared_ptr< WModule > module ):
    m_name( module->getName() ),
    m_description( module->getDescription() ),
    m_loaded( false ),
    m_localPath( module->getLocalPath() )
{
    // check whether file exists
    boost::filesystem::path metafile = module->getLocalPath() / "META";
    if( !boost::filesystem::exists( metafile ) )
    {
        return;
    }

    // try loading it
    try
    {
        m_metaData = WStructuredTextParser::StructuredValueTree( metafile );
        // is there a module definition?
        // If there is no meta info for this module, assume we could not load a meta file
        m_loaded = m_metaData.exists( m_name );
        if( !m_loaded )
        {
            wlog::error( "Module (" + m_name + ")" ) << "Meta file loaded but no entry for module \"" << m_name << "\" found. Ignoring.";
        }
    }
    catch( const WException& e )
    {
        wlog::error( "Module (" + m_name + ")" ) << "Meta file load failed. Message: " << e.what();
    }
}

WModuleMetaInformation::~WModuleMetaInformation()
{
    // cleanup
}

std::string WModuleMetaInformation::getName() const
{
    return m_name;
}

boost::filesystem::path WModuleMetaInformation::getIcon() const
{
    // return a default if not meta data was loaded
    if( !m_loaded )
    {
        return boost::filesystem::path();
    }

    // find key-value pair
    return m_localPath / m_metaData.getValue< boost::filesystem::path >( m_name + "/icon", boost::filesystem::path( "icon.png" ) );
}

std::string WModuleMetaInformation::getWebsite() const
{
    // return a default if not meta data was loaded
    if( !m_loaded )
    {
        return "";
    }

    // find key-value pair
    return m_metaData.getValue< std::string >( m_name + "/website", "" );
}

std::string WModuleMetaInformation::getDescription() const
{
    // return a default if not meta data was loaded
    if( !m_loaded )
    {
        return m_description;
    }

    // find key-value pair
    return m_metaData.getValue< std::string >( m_name + "/description", m_description );
}

boost::filesystem::path WModuleMetaInformation::getHelp() const
{
    // return a default if not meta data was loaded
    if( !m_loaded )
    {
        return boost::filesystem::path();
    }

    // find key-value pair
    return m_localPath / m_metaData.getValue< boost::filesystem::path >( m_name + "/help", boost::filesystem::path( "help.html" ) );
}

std::vector< WModuleMetaInformation::Author > WModuleMetaInformation::getAuthors() const
{
    std::vector< WModuleMetaInformation::Author > r;

    // return a default if not meta data was loaded
    if( !m_loaded )
    {
        return r;
    }

    // how much author information is available?
    std::vector< std::string > authors = m_metaData.getValues< std::string >( m_name + "/author" );
    // prepare some memory
    r.resize( authors.size() );

    // for each author, get some associated data if available
    for( std::vector< std::string >::const_iterator i = authors.begin(); i != authors.end(); ++i )
    {
        r[ i - authors.begin() ].m_email = m_metaData.getValue< std::string >( m_name + "/" + *i + "/email", "" );
        r[ i - authors.begin() ].m_what = m_metaData.getValue< std::string >( m_name + "/" + *i + "/what", "" );
        r[ i - authors.begin() ].m_url = m_metaData.getValue< std::string >( m_name + "/" + *i + "/url", "" );
    }

    // did we find some author info? If not, add a nice default OpenWalnut author
    if( !r.size() )
    {
        WModuleMetaInformation::Author ow = { "OpenWalnut Project", "", "http://www.openwalnut.org", "Design, Development and Bug fixing" };
        r.push_back( ow );
    }

    return r;
}

std::vector< WModuleMetaInformation::Online > WModuleMetaInformation::getOnlineResources() const
{
    std::vector< WModuleMetaInformation::Online > r;
    // return a default if not meta data was loaded
    if( !m_loaded )
    {
        return r;
    }

    // TODO(ebaum): implement me
    return r;
}

