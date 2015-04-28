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

#include <iostream>
#include <fstream>
#include <string>

#include "core/common/WAssert.h"
#include "core/common/WLogger.h"
#include "core/common/WLogStream.h"

#include "WLoggerWrapper.h"

WLoggerWrapper::WLoggerWrapper()
{
}

WLoggerWrapper::WLoggerWrapper( WLogger* logger )
{
    WAssert( logger, "Null pointer" );
    m_logger = logger;
}

WLoggerWrapper::~WLoggerWrapper()
{
    removeAllFileStreams();
}

void WLoggerWrapper::removeFileStreamNumber( size_t i )
{
    m_logger->removeStream( m_fileStreamList[ i ].m_WLogStream );
    m_fileStreamList.erase( m_fileStreamList.begin() + i );
}

bool WLoggerWrapper::addFileStream( std::string filename )
{
    boost::shared_ptr< std::ofstream > fileStream( new std::ofstream( filename.c_str() ) );
    if( !fileStream )
    {
        return false;
    }
    FileStreamEntry newEntry;
    newEntry.m_filename = filename;
    newEntry.m_fileStream = fileStream;
    newEntry.m_WLogStream = WLogStream::SharedPtr( new WLogStream( *fileStream ) );
    m_fileStreamList.push_back( newEntry );
    m_logger->addStream( newEntry.m_WLogStream );
    return true;
}

bool WLoggerWrapper::removeFileStream( std::string filename )
{
    for( size_t i = 0; i < m_fileStreamList.size(); ++i )
    {
        if( filename == m_fileStreamList[ i ].m_filename )
        {
            removeFileStreamNumber( i );
            return true;
        }
    }
    return false;
}

void WLoggerWrapper::removeAllFileStreams()
{
    for( size_t i = 0; i < m_fileStreamList.size(); ++i )
    {
        removeFileStreamNumber( i );
    }
}

void WLoggerWrapper::error( std::string const& location, std::string const& message )
{
    m_logger->addLogMessage( message, location, LL_ERROR );
}

void WLoggerWrapper::warning( std::string const& location, std::string const& message )
{
    m_logger->addLogMessage( message, location, LL_WARNING );
}

void WLoggerWrapper::info( std::string const& location, std::string const& message )
{
    m_logger->addLogMessage( message, location, LL_INFO );
}

void WLoggerWrapper::debug( std::string const& location, std::string const& message )
{
    m_logger->addLogMessage( message, location, LL_DEBUG );
}

