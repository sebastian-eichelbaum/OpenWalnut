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

#include "../common/WIOTools.h"

#include "WModuleLoader.h"

WModuleLoader::WModuleLoader( const boost::filesystem::path& relPath )
	: m_path( relPath )
{
}

WModuleLoader::~WModuleLoader()
{
	for( std::vector< void* >::iterator it = m_handles.begin(); it != m_handles.end(); ++it )
	{
		if( dlclose( *it ) != 0 )
		{
			WLogger::getLogger()->addLogMessage( "Couldn't close shared object handle!", "Module Loader", LL_ERROR );
		}
	}
	m_handles.clear();
}

void WModuleLoader::load( WSharedAssociativeContainer< std::set< boost::shared_ptr< WModule > > >::WriteTicket ticket )
{
	// iterate module directory, look for .so files
	WAssert( boost::filesystem::exists( m_path ), "" );
	
	for( boost::filesystem::directory_iterator i = boost::filesystem::directory_iterator( m_path ); i != boost::filesystem::directory_iterator(); ++i )
	{
		if( !boost::filesystem::is_directory( *i ) && wiotools::getSuffix( i->leaf() ) == ".so" ) // linux!
		{
			void* handle = dlopen( i->path().native_file_string().c_str(), RTLD_NOW );

			if( !handle )
			{
				WLogger::getLogger()->addLogMessage( "Couldn't load shared object: " + i->path().native_file_string(), "Module Loader", LL_ERROR );
				WLogger::getLogger()->addLogMessage( std::string( dlerror() ), "Module Loader", LL_ERROR );
				continue;
			}
		
			// try to get module prototype instance
			typedef boost::shared_ptr< WModule > ( *createInstanceFunc )( void );
			createInstanceFunc createInstance = reinterpret_cast< createInstanceFunc >( dlsym( handle, "createModuleInstance" ) );
			
			if( !createInstance )
			{
				WLogger::getLogger()->addLogMessage( "Couldn't load prototype creator from " + i->path().native_file_string(), "Module Loader", LL_ERROR );
				WLogger::getLogger()->addLogMessage( std::string( dlerror() ), "Module Loader", LL_ERROR );
				continue;
			}
			
			boost::shared_ptr< WModule > m = createInstance();
			
			if( !m )
			{
				WLogger::getLogger()->addLogMessage( "Couldn't aquire prototype of module from " + i->path().native_file_string(), "Module Loader", LL_ERROR );
				WLogger::getLogger()->addLogMessage( std::string( dlerror() ), "Module Loader", LL_ERROR );
				continue;
			}
			else
			{
				WLogger::getLogger()->addLogMessage( "Successfully loaded " + i->path().native_file_string(), "Module Loader", LL_INFO );
				ticket->get().insert( m );
				m_handles.push_back( handle );
			}
		}
	}
}
