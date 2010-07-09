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

#include <set>
#include <string>

#include "../common/WIOTools.h"
#include "../common/WPreferences.h"
#include "../common/WSharedLib.h"

#include "WModuleLoader.h"

WModuleLoader::WModuleLoader( ):
    m_path( "." )
{
    // initialize members
    std::string libPath = "../lib/modules";
    WPreferences::getPreference( "modules.path", &libPath );
    m_path = boost::filesystem::path( libPath );
}

WModuleLoader::~WModuleLoader()
{
    // cleanup all the handles
    m_libs.clear();
}

void WModuleLoader::load( WSharedAssociativeContainer< std::set< boost::shared_ptr< WModule > > >::WriteTicket ticket )
{
    // iterate module directory, look for .so,.dll,.dylib files
    WAssert( boost::filesystem::exists( m_path ), "" );

    for( boost::filesystem::directory_iterator i = boost::filesystem::directory_iterator( m_path );
         i != boost::filesystem::directory_iterator(); ++i )
    {
        // all modules need to begin with this
        std::string suffix = wiotools::getSuffix( i->leaf() );
        if( !boost::filesystem::is_directory( *i ) && ( suffix == WSharedLib::getSystemSuffix() ) )
        {
            try
            {
                WSharedLib l( i->path() );

                // get instantiation function
                typedef boost::shared_ptr< WModule > ( *createInstanceFunc )( void );
                createInstanceFunc f;
                l.fetchFunction< createInstanceFunc >( W_LOADABLE_MODULE_SYMBOL, f );

                // get the first prototype
                boost::shared_ptr< WModule > m = f();

                // could the prototype be created?
                if( !m )
                {
                    WLogger::getLogger()->addLogMessage( "Load failed for module \"" + i->leaf() + "\". Could not create " +
                                                         "prototype instance.", "Module Loader", LL_ERROR );
                    continue;
                }
                else
                {
                    // yes, add it to the list of prototypes
                    WLogger::getLogger()->addLogMessage( "Loaded " + i->leaf(), "Module Loader", LL_INFO );
                    ticket->get().insert( m );
                    m_libs.push_back( l );
                }

                // lib gets closed if l looses focus
            }
            catch( const WException& e )
            {
                WLogger::getLogger()->addLogMessage( "Load failed for module \"" + i->leaf() + "\". " + e.what() + ". Ignoring.",
                                                     "Module Loader", LL_ERROR );
            }
        }
    }
}
