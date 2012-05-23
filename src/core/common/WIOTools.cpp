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

#include <fstream>
#include <streambuf>
#include <string>

#include <boost/filesystem.hpp>

#include "exceptions/WFileNotFound.h"
#include "exceptions/WFileOpenFailed.h"
#include "WIOTools.h"

std::string readFileIntoString( const std::string& name )
{
    return readFileIntoString( boost::filesystem::path( name ) );
}

std::string readFileIntoString( const boost::filesystem::path& path )
{
    std::string filename = path.string();
    std::ifstream input( filename.c_str() );
    if( !input.is_open() )
    {
        throw WFileNotFound( std::string( "The file \"" ) + path.string() + std::string( "\" does not exist." ) );
    }

    // preallocate space for the string.
    std::string str;
    input.seekg( 0, std::ios::end );
    str.reserve( input.tellg() );
    input.seekg( 0, std::ios::beg );

    str.assign( ( std::istreambuf_iterator< char >( input ) ), std::istreambuf_iterator< char >() );

    input.close();
    return str;
}

void writeStringIntoFile( const std::string& name, const std::string& content )
{
    writeStringIntoFile( boost::filesystem::path( name ), content );
}

void writeStringIntoFile( const boost::filesystem::path& path, const std::string& content )
{
    std::ofstream outfile( path.string().c_str() );
    if( !outfile.is_open() )
    {
        throw WFileOpenFailed( "The file \"" +  path.string() + "\" could not be opened." );
    }

    outfile << content << std::flush;
    outfile.close();
}

boost::filesystem::path tempFileName()
{
    return tempFilename();
}

boost::filesystem::path tempFilename( boost::filesystem::path model )
{
    return boost::filesystem::temp_directory_path() / boost::filesystem::unique_path( model );
}

