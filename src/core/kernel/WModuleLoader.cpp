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

#include <boost/regex.hpp>

#include "../common/WIOTools.h"
#include "../common/WPathHelper.h"
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
        std::string suffix = getSuffix( i->leaf() );
        std::string stem = i->path().stem();

#ifdef _MSC_VER
        std::string supposedFilename = getModulePrefix() + '_' + i->path().parent_path().filename()
#ifdef _DEBUG
            + 'd'
#endif
            + WSharedLib::getSystemSuffix();
        std::string isFileName = i->path().filename();
#endif // _MSC_VER

        // we want to strip the search directory from the path
        std::string relPath = i->path().file_string();
        relPath.erase( 0, dir.file_string().length() + 1 ); // NOTE: +1 because we want to remove the "/" too

        // is it a lib? Use a regular expression to check this
        // NOTE:: the double \\ is needed to escape the escape char
        #ifdef __WIN32__
            static const boost::regex CheckLibMMP( "^.*\\" + WSharedLib::getSystemSuffix() +"$" );
        #elif __APPLE__
            static const boost::regex CheckLibMMP( "^.*\\.[0-9]+\\.[0-9]+\\.[0-9]+\\" + WSharedLib::getSystemSuffix() + "$" );
        #else
            static const boost::regex CheckLibMMP( "^.*\\" + WSharedLib::getSystemSuffix() + "\\.[0-9]+\\.[0-9]+\\.[0-9]+$" );
        #endif
        boost::smatch matches;

        if( !boost::filesystem::is_directory( *i ) &&
            ( boost::regex_match( i->path().string(), matches, CheckLibMMP ) ) &&
            ( stem.compare( 0, getModulePrefix().length(), getModulePrefix() ) == 0 )
#ifdef _MSC_VER
            && supposedFilename == isFileName
#endif
            )
        {
            try
            {
                // load lib
                boost::shared_ptr< WSharedLib > l = boost::shared_ptr< WSharedLib >( new WSharedLib( i->path() ) );

                // get instantiation function
                W_LOADABLE_MODULE_SIGNATURE f;
                l->fetchFunction< W_LOADABLE_MODULE_SIGNATURE >( W_LOADABLE_MODULE_SYMBOL, f );

                // get the first prototype
                WModuleList m;
                f( m );

                // could the prototype be created?
                if( m.empty() )
                {
                    WLogger::getLogger()->addLogMessage( "Load failed for module \"" + relPath + "\". Could not create any " +
                                                         "prototype instance.", "Module Loader", LL_ERROR );
                    continue;
                }
                else
                {
                    // yes, add it to the list of prototypes
                    for( WModuleList::const_iterator iter = m.begin(); iter != m.end(); ++iter )
                    {
                        ( *iter )->setLocalPath( i->path().parent_path() );
                        ticket->get().insert( *iter );
                        m_libs.push_back( l );
                    }

                    wlog::debug( "Module Loader" ) << "Loaded " << m.size() << " modules from " << relPath;
                }

                // lib gets closed if l looses focus
            }
            catch( const WException& e )
            {
                WLogger::getLogger()->addLogMessage( "Load failed for module \"" + relPath + "\". " + e.what() + ". Ignoring.",
                                                     "Module Loader", LL_ERROR );
            }
        }
        else if( ( level == 0 ) && boost::filesystem::is_directory( *i ) )     // this only traverses down one level
        {
            // if it a dir -> traverse down
            load( ticket, *i, level + 1 );
        }
    }
}

void WModuleLoader::load( WSharedAssociativeContainer< std::set< boost::shared_ptr< WModule > > >::WriteTicket ticket )
{
    std::vector< boost::filesystem::path > allPaths = WPathHelper::getAllModulePaths();

    // go through each of the paths
    for( std::vector< boost::filesystem::path >::const_iterator path = allPaths.begin(); path != allPaths.end(); ++path )
    {
        WLogger::getLogger()->addLogMessage( "Searching modules in \"" + ( *path ).file_string() + "\".", "Module Loader", LL_INFO );

        // does the directory exist?
        if( !boost::filesystem::is_directory( *path ) || !boost::filesystem::exists( *path ) )
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
    return WSharedLib::getSystemPrefix();
}

