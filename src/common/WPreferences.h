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

#ifndef WPREFERENCES_H
#define WPREFERENCES_H

#include <string>
#include <fstream>

// Use filesystem version 2 for compatibility with newer boost versions.
#ifndef BOOST_FILESYSTEM_VERSION
    #define BOOST_FILESYSTEM_VERSION 2
#endif
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include "WExportCommon.h"
#include "WIOTools.h"
#include "WLogger.h"
#include "WProperties.h"

/**
 * Fetches and caches preferences set in file.
 */
class OWCOMMON_EXPORT WPreferences
{
public:
    /**
     * Get the value of a preference with a given name.
     * \param prefName string that identifies the preference inf the config file
     * \param retVal pointer to the value belonging to the name. This parameter is left unchanged if there is no preference with prefName.
     * \return True if value could be found, false otherwise.
     */
    template< typename T> static bool getPreference( std::string prefName, T* retVal );

    /**
     * Sets the configuration file globally.
     *
     * \param cfg the config file to use.
     */
    static void setPreferenceFile( boost::filesystem::path cfg )
    {
        m_preferenceFile = cfg;
    }

protected:
private:
    static WProperties m_preferences; //!< Structure for caching the preferences.

    /**
     * The file containing the config options.
     */
    static boost::filesystem::path m_preferenceFile;
};

template< typename T > bool WPreferences::getPreference( std::string prefName, T* retVal )
{
    if( prefName == "" )
    {
        return false;
    }

    boost::shared_ptr< WPropertyBase > pref = m_preferences.findProperty( prefName );
    if( pref )
    {
        *retVal =  pref->toPropertyVariable< T >()->get();
        return true;
    }

    namespace po = boost::program_options; // since the namespace is far to big we use a shortcut here

    //=====================
    // CONFIGURATION FILE
    po::options_description configurationDescription( "OpenWalnut Preferences" );

    configurationDescription.add_options()
        ( prefName.c_str(), po::value< T >() );

    boost::program_options::variables_map configuration;
    if( boost::filesystem::exists( m_preferenceFile ) )
    {
        try
        {
            // since overloaded function "const char*" dont work in boost 1.42.0
            std::ifstream ifs( m_preferenceFile.file_string().c_str(), std::ifstream::in );
            po::store( po::parse_config_file( ifs, configurationDescription, true ), configuration );
        }
        catch( const po::error &e )
        {
            wlog::error( "WPreferences" ) << "Invalid configuration file \"" << m_preferenceFile.file_string() << "\": " << e.what();
            return false;
        }
    }
    else
    {
        wlog::info( "WPreferences" ) << "No Config file: " << m_preferenceFile.file_string() << " found";
    }

    po::notify( configuration );
    if( configuration.count( prefName ) )
    {
        *retVal = configuration[ prefName ].as< T >();
        m_preferences.addProperty( prefName, "Preference cache.", *retVal );
        return true;
    }
    else
    {
        return false;
    }
}

#endif  // WPREFERENCES_H
