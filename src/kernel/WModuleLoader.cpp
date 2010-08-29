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
#include <vector>

#include "../common/WIOTools.h"
#include "../common/WPathHelper.h"
#include "../common/WPreferences.h"
#include "../common/WSharedLib.h"

#include "WKernel.h"

#include "WModuleLoader.h"

WModuleLoader::WModuleLoader( )
{
    // initialize members
}

WModuleLoader::~WModuleLoader()
{
    // cleanup all the handles
    m_libs.clear();
}

void WModuleLoader::load( WSharedAssociativeContainer< std::set< boost::shared_ptr< WModule > > >::WriteTicket ticket,
                          boost::filesystem::path dir, unsigned int level )
{
    for( boost::filesystem::directory_iterator i = boost::filesystem::directory_iterator( dir );
         i != boost::filesystem::directory_iterator(); ++i )
    {
        // all modules need to begin with this
        std::string suffix = wiotools::getSuffix( i->leaf() );
        std::string stem = i->path().stem();

        if( !boost::filesystem::is_directory( *i ) && ( suffix == WSharedLib::getSystemSuffix() ) &&
            ( stem.compare( 0, getModulePrefix().length(), getModulePrefix() ) == 0 ) )
        {
            try
            {
                WSharedLib l( i->path() );

                // get instantiation function
                typedef void ( *createInstanceFunc )( boost::shared_ptr< WModule > & );
                createInstanceFunc f;
                l.fetchFunction< createInstanceFunc >( W_LOADABLE_MODULE_SYMBOL, f );

                // get the first prototype
                boost::shared_ptr< WModule > m;
                f( m );

                // could the prototype be created?
                if( !m )
                {
                    WLogger::getLogger()->addLogMessage( "Load failed for module \"" + i->path().file_string() + "\". Could not create " +
                                                         "prototype instance.", "Module Loader", LL_ERROR );
                    continue;
                }
                else
                {
                    // yes, add it to the list of prototypes
                    WLogger::getLogger()->addLogMessage( "Loaded " + i->path().file_string(), "Module Loader", LL_INFO );
                    m->setLocalPath( i->path().parent_path() );
                    ticket->get().insert( m );
                    m_libs.push_back( l );
                }

                // lib gets closed if l looses focus
            }
            catch( const WException& e )
            {
                WLogger::getLogger()->addLogMessage( "Load failed for module \"" + i->path().file_string() + "\". " + e.what() + ". Ignoring.",
                                                     "Module Loader", LL_ERROR );
            }
        }
        else if ( ( level == 0 ) && boost::filesystem::is_directory( *i ) )     // this only traverses down one level
        {
            // if it a dir -> traverse down
            load( ticket, *i, level + 1 );
        }
        // this little construct will enable vc to find the dll's in the correct build mode we are in
#ifdef _MSC_VER
        else if ( ( level == 1 ) &&
#ifdef _DEBUG
            boost::filesystem::path( *i ).filename() == "Debug"
#else // _DEBUG
            boost::filesystem::path( *i ).filename() == "Release"
#endif // _DEBUG
            )
        {
            load( ticket, *i, level + 1 );
        }
#endif // _MSC_VER
    }
}

void WModuleLoader::load( WSharedAssociativeContainer< std::set< boost::shared_ptr< WModule > > >::WriteTicket ticket )
{
    std::vector< boost::filesystem::path > allPaths = WPathHelper::getAllModulePaths();

    // go through each of the paths
    for ( std::vector< boost::filesystem::path >::const_iterator path = allPaths.begin(); path != allPaths.end(); ++path )
    {
        WLogger::getLogger()->addLogMessage( "Searching modules in \"" + ( *path ).file_string() + "\".", "Module Loader", LL_INFO );

        // does the directory exist?
        if ( !boost::filesystem::is_directory( *path ) || !boost::filesystem::exists( *path ) )
        {
            WLogger::getLogger()->addLogMessage( "Searching modules in \"" + ( *path ).file_string() +
                                                 "\" failed. It is not a directory or does not exist." +
                                                 " Ignoring.", "Module Loader", LL_WARNING );

            continue;
        }

        // directly search the path for libOWmodule_ files
        load( ticket, *path );
    }
}

std::string WModuleLoader::getModulePrefix()
{
    // all module file names need to have this prefix:
    return WSharedLib::getSystemPrefix() + "OWmodule";
}

