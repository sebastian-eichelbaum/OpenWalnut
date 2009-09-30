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

#include "WLoaderFibers.h"
#include "../WDataHandler.h"
#include "../../common/WStringUtils.hpp"

WLoaderFibers::WLoaderFibers( std::string fname, boost::shared_ptr< WDataHandler > dataHandler )
    : WLoader( fname, dataHandler )
{
}

WLoaderFibers::~WLoaderFibers() throw()
{
}

void WLoaderFibers::operator()() throw()
{
    std::ifstream in( m_fileName.c_str(), std::ifstream::in | std::ifstream::binary );
    try
    {
        if( !in || in.bad() )
        {
            throw WDHIOFailure( "Load broken file '" + m_fileName + "'" );
        }
        readHeader( &in );
        if( !datasetTypeIs( "POLYDATA" ) )
        {
            throw WDHException( "Invalid VTK DATASET type: DATASET != POLYDATA not implemented" );
        }
        if( !isBinary() )
        {
            throw WDHException( "VTK files in ASCII format is not yet supported" );
        }
        readPoints( &in );
    }
    catch( WDHException e )
    {
        in.close();
        std::cerr << "Error :: DataHandler :: Abort loading VTK file due to: " << e.what() << std::endl;
    }
    in.close();
}

void WLoaderFibers::readHeader( std::ifstream* ifs ) throw( WDHIOFailure )
{
    std::string line;
    try
    {
        for( int i = 0; i < 4; ++i )  // strip first four lines
        {
            std::getline( *ifs, line );
            m_header.push_back( line );
        }
    }
    catch( std::ios_base::failure e )
    {
        throw WDHIOFailure( "Reading first 4 lines of '" + m_fileName + "': " + e.what() );
    }

    assert( m_header.size() == 4 );
}

bool WLoaderFibers::datasetTypeIs( const std::string& type ) const
{
    assert( m_header.size() == 4 );
    std::string lastLine = m_header.back();
    std::vector< std::string > tokens = string_utils::tokenize( lastLine );
    assert( tokens.size() >= 2 );
    return tokens[1] == type;
}

bool WLoaderFibers::isBinary() const
{
    assert( m_header.size() == 4 );
    std::string thirdLine = string_utils::toUpper( string_utils::trim( m_header[2] ) );
    if( thirdLine != "BINARY" )
    {
        assert( thirdLine == "ASCII" );
    }
    return "BINARY" == thirdLine;
}

void WLoaderFibers::readPoints( std::ifstream* ifs )
{
}
