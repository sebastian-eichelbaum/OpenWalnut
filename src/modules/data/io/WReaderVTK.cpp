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

#include <stdint.h>

#include <fstream>
#include <iosfwd>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "core/common/WAssert.h"
#include "core/common/WIOTools.h"
#include "core/common/WLimits.h"
#include "core/common/WLogger.h"
#include "core/common/WStringUtils.h"
#include "core/dataHandler/exceptions/WDHIOFailure.h"
#include "core/dataHandler/exceptions/WDHNoSuchFile.h"
#include "core/dataHandler/exceptions/WDHParseError.h"
#include "core/dataHandler/WDataSet.h"
#include "core/dataHandler/WDataSetScalar.h"
#include "core/dataHandler/WGridRegular3D.h"
#include "WReaderVTK.h"

WReaderVTK::WReaderVTK( std::string fname )
    : WReader( fname )
{
}

WReaderVTK::~WReaderVTK() throw()
{
}

boost::shared_ptr< WDataSet > WReaderVTK::read()
{
    m_ifs = boost::shared_ptr< std::ifstream >( new std::ifstream() );
    m_ifs->open( m_fname.c_str(), std::ifstream::in | std::ifstream::binary );
    if( !m_ifs || m_ifs->bad() )
    {
        throw WDHIOFailure( std::string( "internal error while opening" ) );
    }
    readHeader();
    readStructuredPoints(); //< this is the only grid supported so far
    readPointData();

    // boost::shared_ptr< WDataSet > fibers = boost::shared_ptr< WDataSetFibers >( new WDataSetFibers( m_points,
    //                                                                                                       m_fiberStartIndices,
    //                                                                                                       m_fiberLengths,
    //                                                                                                       m_pointFiberMapping ) );
    // fibers->setFilename( m_fname );

    m_ifs->close();
    return newDataSet;
}

void WReaderVTK::readHeader()
{
    for( int i = 0; i < 4; ++i )  // strip first four lines
    {
        m_header.push_back( getLine( "reading first 4 lines (aka header)" ) );
    }

    // check if the header may be valid for the .fib format
    if( ( m_header.at( 0 ) != "# vtk DataFile Version 3.0" )
            && ( m_header.at( 0 ) != "# vtk DataFile Version 2.0" ) )
    {
        wlog::error( "WReaderVTK" ) << "Unsupported format version string in VTK file: "
            << m_fname
            << ", "
            << m_header.at( 0 );
    }
    if( m_header.at( 1 ).size() > 256 )
    {
        wlog::warn( "WReaderVTK" ) << "Invalid header size of VTK file: "
            << m_fname
            << ", max. 256 but got: "
            << m_header.at( 1 ).size();
    }
    namespace su = string_utils;
    if( ( su::toUpper( su::trim( m_header.at( 2 ) ) ) != "BINARY" )
            &&( su::toUpper( su::trim( m_header.at( 2 ) ) ) != "ASCII" ) )
    {
        wlog::error( "WReaderVTK" ) << "VTK files in '" << m_header.at( 2 ) << "' format are not yet supported. Must be BINARY or ASCII";
    }

    this->isAscii = ( su::toUpper( su::trim( m_header.at( 2 ) ) ) == "ASCII" );

    if(  su::tokenize( m_header.at( 3 ) ).size() < 2 || su::toUpper( su::tokenize( m_header.at( 3 ) )[1] ) != "STRUCTURED_POINTS" )
    {
        wlog::error( "WReaderVTK" ) << "Cannot read this VTK DATASET type: " << su::tokenize( m_header.back() )[1];
    }
}

void WReaderVTK::readStructuredPoints()
{
    std::string line = getLine( "reading DIMENSIONS" );
    std::vector< std::string > dimensions_line = string_utils::tokenize( line );
    if( (  dimensions_line.size() != 4 ) && string_utils::toLower( dimensions_line.at( 0 ) ) != "dimensions" )
    {
        throw WDHParseError( std::string( "invalid DIMENSIONS token: " + line + ", expected DIMENSIONS." ) );
    }

    size_t dimensions[ 3 ];
    dimensions[ 0 ] = getLexicalCast< size_t >( dimensions_line.at( 1 ), "" );
    dimensions[ 1 ] = getLexicalCast< size_t >( dimensions_line.at( 2 ), "" );
    dimensions[ 2 ] = getLexicalCast< size_t >( dimensions_line.at( 3 ), "" );

    line = getLine( "reading ORIGIN" );
    std::vector< std::string > origin_line = string_utils::tokenize( line );
    if( ( origin_line.size() != 4 ) && string_utils::toLower( origin_line.at( 0 ) ) != "origin" )
    {
        throw WDHParseError( std::string( "invalid ORIGIN token: " + line + ", expected ORIGIN." ) );
    }

    double origin[ 3 ];
    origin[ 0 ] = getLexicalCast< float >( origin_line.at( 1 ), "" );
    origin[ 1 ] = getLexicalCast< float >( origin_line.at( 2 ), "" );
    origin[ 2 ] = getLexicalCast< float >( origin_line.at( 3 ), "" );

    line = getLine( "reading SPACING" );
    std::vector< std::string > spacings_line = string_utils::tokenize( line );
    if( ( spacings_line.size() != 4 ) && string_utils::toLower( spacings_line.at( 0 ) ) != "spacing" )
    {
        throw WDHParseError( std::string( "invalid DIMENSIONS token: " + line + ", expected DIMENSIONS." ) );
    }
    double spacings[ 3 ];
    spacings[ 0 ] = getLexicalCast< float >( spacings_line.at( 1 ), "" );
    spacings[ 1 ] = getLexicalCast< float >( spacings_line.at( 2 ), "" );
    spacings[ 2 ] = getLexicalCast< float >( spacings_line.at( 3 ), "" );

    // FIXME: setting a reasonable transform (in my case a scaling of 0.00025 for a 200x200x200 grid)
    // FIXME: does not show the data set, so this is disabled for now
    WGridTransformOrtho transform( spacings[ 0 ], spacings[ 1 ], spacings[ 2 ] ); // NOLINT since this is not transform from STL <algorithm>, but just a identifier
    transform.translate( origin ); // this is unclear in the codumentation of OpenWalnut
    this->newGrid.reset(
            new WGridRegular3D( dimensions[ 0 ], dimensions[ 1 ], dimensions[ 2 ], transform ) );
}

// we currently do not handle the name flag but should set the name of the data set somewhere
void WReaderVTK::readScalars( size_t nbPoints, const std::string& /*name*/ )
{
    int pos = m_ifs->tellg(); // remember where we are for binary files

    std::string line = getLine( "LookupTableOrData" );
    std::vector<std::string> tokens = string_utils::tokenize( line );
    if( string_utils::toUpper( tokens.at( 0 ) ) == "LOOKUP_TABLE" )
    {
        line = getLine( "Data" );
        tokens = string_utils::tokenize( line );
        pos = m_ifs->tellg(); // remember where we are for binary files
    }

    boost::shared_ptr<std::vector< float > > data( new std::vector< float >( nbPoints ) );
    if( this->isAscii )
    {
        wlog::debug( "WReaderVTK" ) << "Reading ASCII";
        size_t point = 0;
        while( point < nbPoints )
        {
            for( size_t token = 0; token < tokens.size(); ++token )
            {
                ( *data )[ point++ ] = getLexicalCast< float >( tokens.at( token ), "invalid data point" );
                if( point > nbPoints )
                {
                    throw WDHException( std::string( "Too many points in file" ) );
                }
            }
            if( point < nbPoints )
            {
                line = getLine( "Data Loop" );
                tokens = string_utils::tokenize( line );
            }
        }
    }
    else
    {
        if( pos != m_ifs->tellg() )
        {
            m_ifs->seekg( pos, std::ios::beg );
        }
        wlog::debug( "WReaderVTK" ) << "Reading BINARY";
        m_ifs->read(  reinterpret_cast<char*>(  &( *data )[ 0 ] ), data->size() * sizeof( ( *data )[ 0 ] ) );
        // do endianess stuff!!! switchByteOrderOfArray(  data, data->size() );
    }

    boost::shared_ptr< WValueSetBase> vs(  new WValueSet< float >(  0, 1, data, W_DT_FLOAT ) );
    this->newDataSet.reset( new WDataSetScalar( vs, this->newGrid ) );

    // line = getLine( "also eat the remaining newline after lines declaration" );
    // WAssert( std::string( "" ) == line, "Found characters in file where nothing was expected." );
}

void WReaderVTK::readVectors( size_t nbPoints, const std::string& /*name*/ )
{
    int pos = m_ifs->tellg(); // remember where we are for binary files

    // some files even have a lookup-table tag for vector-valued data
    std::string line = getLine( "LookupTableOrData" );
    std::vector< std::string > tokens = string_utils::tokenize( line );
    if( string_utils::toUpper( tokens.at( 0 ) ) == "LOOKUP_TABLE" )
    {
        line = getLine( "Data" );
        tokens = string_utils::tokenize( line );
        pos = m_ifs->tellg(); // remember where we are for binary files
    }

    // We assume that all VTK fields store 3-dimensional vectors.
    // There are other fields around, but there is no official VTK documentation dealing
    // with 2D vectors and no unique interpretation of the data
    const size_t dimension = 3;
    boost::shared_ptr< std::vector< float > > data( new std::vector< float >( nbPoints * dimension ) );
    if( this->isAscii )
    {
        wlog::debug( "WReaderVTK" ) << "Reading ASCII";
        size_t point = 0;
        while( point < nbPoints * dimension )
        {
            for( size_t token = 0; token < tokens.size(); ++token )
            {
                ( *data )[ point++ ] = getLexicalCast< float >( tokens.at( token ), "invalid data point" );
                if( point > nbPoints * dimension)
                {
                    throw WDHException( std::string( "Too many points in file" ) );
                }
            }
            if( point < nbPoints * dimension )
            {
                line = getLine( "Data Loop" );
                tokens = string_utils::tokenize( line );
            }
        }
    }
    else
    {
        if( pos != m_ifs->tellg() )
        {
            m_ifs->seekg( pos, std::ios::beg );
        }
        wlog::debug( "WReaderVTK" ) << "Reading BINARY";
        m_ifs->read(  reinterpret_cast<char*>(  &( *data )[ 0 ] ), data->size() * dimension * sizeof( ( *data )[ 0 ] ) );
        // do endianess stuff!!! switchByteOrderOfArray(  data, data->size() * dimension );
    }

    boost::shared_ptr< WValueSetBase> vs(  new WValueSet< float >(  1, 3, data, W_DT_FLOAT ) );
    this->newDataSet.reset( new WDataSetScalar( vs, this->newGrid ) );

    // line = getLine( "also eat the remaining newline after lines declaration" );
    // WAssert( std::string( "" ) == line, "Found characters in file where nothing was expected." );
}

void WReaderVTK::readTensors( size_t nbPoints, const std::string& /*name*/ )
{
    int pos = m_ifs->tellg(); // remember where we are for binary files

    // some files even have a lookup-table tag for vector-valued data
    std::string line = getLine( "LookupTableOrData" );
    std::vector< std::string > tokens = string_utils::tokenize( line );
    if( string_utils::toUpper( tokens.at( 0 ) ) == "LOOKUP_TABLE" )
    {
        line = getLine( "Data" );
        tokens = string_utils::tokenize( line );
        pos = m_ifs->tellg(); // remember where we are for binary files
    }

    // We assume that all VTK fields store 3-dimensional vectors.
    // There are other fields around, but there is no official VTK documentation dealing
    // with 2D vectors and no unique interpretation of the data
    const size_t dimension = 9;
    boost::shared_ptr< std::vector< float > > data( new std::vector< float >( nbPoints * dimension ) );
    if( this->isAscii )
    {
        wlog::debug( "WReaderVTK" ) << "Reading ASCII";
        size_t point = 0;
        while( point < nbPoints * dimension )
        {
            for( size_t token = 0; token < tokens.size(); ++token )
            {
                ( *data )[ point++ ] = getLexicalCast< float >( tokens.at( token ), "invalid data point" );
                if( point > nbPoints * dimension)
                {
                    throw WDHException( std::string( "Too many points in file" ) );
                }
            }
            if( point < nbPoints * dimension )
            {
                line = getLine( "Data Loop" );
                tokens = string_utils::tokenize( line );
            }
        }
    }
    else
    {
        if( pos != m_ifs->tellg() )
        {
            m_ifs->seekg( pos, std::ios::beg );
        }
        wlog::debug( "WReaderVTK" ) << "Reading BINARY";
        m_ifs->read(  reinterpret_cast<char*>(  &( *data )[ 0 ] ), data->size() * dimension * sizeof( ( *data )[ 0 ] ) );
        // do endianess stuff!!! switchByteOrderOfArray(  data, data->size() * dimension );
    }

    boost::shared_ptr< WValueSetBase> vs(  new WValueSet< float >(  2, 3, data, W_DT_FLOAT ) );
    this->newDataSet.reset( new WDataSetScalar( vs, this->newGrid ) );

    // line = getLine( "also eat the remaining newline after lines declaration" );
    // WAssert( std::string( "" ) == line, "Found characters in file where nothing was expected." );
}


void WReaderVTK::readPointData()
{
    std::string line = getLine( "POINT_DATA declaration" );

    std::vector< std::string > tokens = string_utils::tokenize( line );
    if( tokens.size() != 2 || string_utils::toUpper( tokens.at( 0 ) ) != "POINT_DATA" )
    {
        throw WDHException( std::string( "Invalid SCALARS tag" ) );
    }

    size_t nbPoints = getLexicalCast< size_t >( tokens.at( 1 ), "parsing number of points" );


    line = getLine( "SCALARS declaration" );

    tokens = string_utils::tokenize( line );


    std::string name = "Default";
    if( tokens.size() >= 2 )
    {
        name = tokens.at( 1 );
    }
    if( tokens.size() == 3 && string_utils::toUpper( tokens.at( 0 ) ) == "SCALARS" )
    {
        readScalars( nbPoints, name );
    }
    else if( tokens.size() == 3 && string_utils::toUpper( tokens.at( 0 ) ) == "VECTORS" )
    {
        readVectors( nbPoints, name );
    }
    else if( tokens.size() == 3 && string_utils::toUpper( tokens.at( 0 ) ) == "TENSORS" )
    {
        readTensors( nbPoints, name );
    }
    else
    {
        throw WDHException( std::string( "Invalid SCALARS or VECTORS tag" ) );
    }
}

std::string WReaderVTK::getLine( const std::string& desc )
{
    std::string line;
    try
    {
        // we use '\n' as line termination under every platform so our files (which are most likely to be generated on Unix systems)
        // can be read from all platforms not having those line termination symbols like e.g. windows ('\r\n').
        std::getline( *m_ifs, line, '\n' );
    }
    catch( const std::ios_base::failure &e )
    {
        throw WDHIOFailure( std::string( "IO error while " + desc + " of VTK fiber file: " + m_fname + ", " + e.what() ) );
    }
    if( !m_ifs->good() )
    {
        throw WDHParseError( std::string( "Unexpected end of VTK fiber file: " + m_fname ) );
    }
    return line;
}
