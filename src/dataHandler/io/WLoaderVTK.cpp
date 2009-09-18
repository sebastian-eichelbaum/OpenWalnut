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

#include <cassert>
#include <fstream>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "WLoaderVTK.h"
#include "../WDataHandler.h"
#include "../../common/WStringUtils.hpp"

WLoaderVTK::WLoaderVTK( std::string fname, boost::shared_ptr< WDataHandler > dataHandler )
    : WLoader( fname ),
      m_dataHandler( dataHandler )
{
}

void WLoaderVTK::operator()()
{
    readHeader();
    if( !datasetTypeIs( "POLYDATA" ) )
    {
        throw WDHException( "Loading VTK files with DATASET != POLYDATA not implemented" );
    }
    if( !isBinary() )
    {
        throw WDHException( "Loading VTK files in ASCII format is not yet supported" );
    }
}

void WLoaderVTK::readHeader() throw( WDHIOFailure )
{
    std::ifstream in( m_fileName.c_str(), std::ifstream::in | std::ifstream::binary );
    if( !in || in.bad() )
    {
        throw WDHIOFailure( "Invalid file or filename: " + m_fileName );
    }

    std::string line;
    try
    {
        for( int i = 0; i < 4; ++i )  // strip first four lines
        {
            std::getline( in, line );
            m_header.push_back( line );
        }
    }
    catch( std::ios_base::failure e )
    {
        throw WDHIOFailure( "Error while reading first four lines of " + m_fileName + ": " + e.what() );
    }

    assert( m_header.size() == 4 );
    in.close();
}

bool WLoaderVTK::datasetTypeIs( const std::string& type ) const
{
    assert( m_header.size() == 4 );
    std::string lastLine = m_header.back();
    std::vector< std::string > tokens = string_utils::tokenize( lastLine );
    assert( tokens.size() >= 2 );
    return tokens[1] == type;
}

bool WLoaderVTK::isBinary() const
{
    assert( m_header.size() == 4 );
    std::string thirdLine = string_utils::toUpper( string_utils::trim( m_header[2] ) );
    return "BINARY" == thirdLine;
}
