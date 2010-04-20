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
#include <stdexcept>
#include <string>
#include <vector>

#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>

#include "../../common/WAssert.h"
#include "../../common/WIOTools.h"
#include "../../common/WStringUtils.h"
#include "../exceptions/WDHException.h"
#include "../exceptions/WDHIOFailure.h"
#include "WReader.h"
#include "WReaderLookUpTableVTK.h"

WReaderLookUpTableVTK::WReaderLookUpTableVTK( std::string fname )
    : WReader( fname )
{
}

void WReaderLookUpTableVTK::readTable( boost::shared_ptr< std::vector< double > > table ) const
{
    WAssert( table->size() == 0, "Non-zero size indicates an error, since the vector will be filled IN HERE." );

    // code mainly taken from WLoaderFibers.cpp, and adjusted since I don't
    // know how to code this DRY. Any suggestions?
    std::ifstream ifs;
    ifs.open( m_fname.c_str(), std::ifstream::in | std::ifstream::binary );
    WAssert( ifs && !ifs.bad(), "" );

    std::vector< std::string > header;
    std::string line;
    try
    {
        for( int i = 0; i < 4; ++i )  // strip first four lines
        {
            std::getline( ifs, line );
            if( !ifs.good() )
            {
                throw WDHException( "Unexpected end of file: " + m_fname );
            }
            header.push_back( line );
        }
    }
    catch( const std::ios_base::failure &e )
    {
        throw WDHIOFailure( "Reading first 4 lines of '" + m_fname + "': " + e.what() );
    }
    WAssert( header[0] == "# vtk DataFile Version 3.0", "Wrong string in file header found." );
    WAssert( header[1] == "DXtLookUpTable from OpenWalnut", "Wrong string in file header found." );
    WAssert( header[2] == "BINARY", "Wrong string in file header found." );
    WAssert( header[3] == "FIELD DXtLookUpTable 1", "Wrong string in file header found." );

    try
    {
        std::getline( ifs, line ); // something like this: "DISTANCES 15879430 1 float" expected
    }
    catch( const std::ios_base::failure &e )
    {
        throw WDHIOFailure( "Error reading DISTANCES field '" + m_fname + "': " + e.what() );
    }
    namespace su = string_utils;
    size_t numDistances = 0;
    std::vector< std::string > tokens = su::tokenize( line );
    if( tokens.size() != 4 || su::toLower( tokens.at( 3 ) ) != "float" )
    {
        throw WDHException( "Invalid DISTANCES declaration: " + line );
    }
    try
    {
        numDistances = boost::lexical_cast< size_t >( tokens.at( 1 ) );
    }
    catch( const boost::bad_lexical_cast &e )
    {
        throw WDHException( "Invalid number of distances: " + tokens.at( 1 ) );
    }

    float *data = new float[ numDistances ];
    try
    {
        ifs.read( reinterpret_cast< char* >( data ), sizeof( float ) * numDistances );
    }
    catch( const std::ios_base::failure &e )
    {
        throw WDHIOFailure( "Error reading distances in VTK DISTANCES field '" + m_fname + "': " + e.what() );
    }

    // all 4 bytes of each float are in wrong order we need to reorder them
    wiotools::switchByteOrderOfArray( data, numDistances );

    for( size_t i = 0; i < numDistances; ++i )
    {
        table->push_back( static_cast< double >( data[ i ] ) );
    }

    delete[] data;
}
