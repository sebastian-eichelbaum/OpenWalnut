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
#include <iostream>
#include <fstream>

#include <boost/program_options.hpp>

#include "../utils/WIOTools.h"
#include "WProperties.h"
#include "WLogger.h"

/**
 * Fetches and caches preferences set in file.
 */
class WPreferences
{
public:
    /**
     * Get the value of a preference with a given name.
     * \param prefName string that identifies the preference inf the config file
     * \param retVal pointer to the value belonging to the name
     * \return True if value could be found, false otherwise.
     */
    template< typename T> static bool getPreference( std::string prefName, T* retVal );
protected:
private:
    static WProperties m_preferences; //!< Structure for caching the preferences.
};

template< typename T > bool WPreferences::getPreference( std::string prefName, T* retVal )
{
    if( prefName == "" )
    {
        return false;
    }
    if( m_preferences.existsProp( prefName ) )
    {
        *retVal =  m_preferences.getValue< T >( prefName );
        return true;
    }

    namespace po = boost::program_options; // since the namespace is far to big we use a shortcut here

    //=====================
    // CONFIGURATION FILE
    po::options_description configurationDescription( "OpenWalnut Preferences" );

#ifndef _WIN32
// TODO(wiebel): this does not link on windows at the moment. But it should!
    configurationDescription.add_options()
        ( prefName.c_str(), po::value< T >() );
#endif

    std::string cfgFileName( "walnut.cfg" );

    boost::program_options::variables_map configuration;
    if( wiotools::fileExists( cfgFileName ) )
    {
        wlog::info( "GUI" ) << "Reading config file: " << cfgFileName;
        std::ifstream ifs( cfgFileName.c_str(), std::ifstream::in );

        try
        {
            po::store( po::parse_config_file( ifs, configurationDescription, true ), configuration );
        }
        catch( const po::error &e )
        {
            std::cerr << "Error in configuration file \"" << cfgFileName << "\": " << e.what() << std::endl;
            return false;
        }
    }
    else
    {
        wlog::info( "Preferences" ) << "No Config file: " << cfgFileName << " found";
    }

    po::notify( configuration );
    if( configuration.count( prefName ) )
    {
        *retVal = configuration[ prefName ].as< T >();
        m_preferences.addProperty( prefName, *retVal, true );
        return true;
    }
    else
    {
        return false;
    }
}

#endif  // WPREFERENCES_H
