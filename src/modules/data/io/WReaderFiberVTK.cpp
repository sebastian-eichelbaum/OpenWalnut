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
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "core/common/WIOTools.h"
#include "core/common/WLimits.h"
#include "core/common/WAssert.h"
#include "core/common/WLogger.h"
#include "core/common/WStringUtils.h"
#include "core/dataHandler/WDataSetFibers.h"
#include "core/dataHandler/exceptions/WDHIOFailure.h"
#include "core/dataHandler/exceptions/WDHNoSuchFile.h"
#include "core/dataHandler/exceptions/WDHParseError.h"

#include "WReaderFiberVTK.h"

WReaderFiberVTK::WReaderFiberVTK( std::string fname )
    : WReader( fname )
{
}

WReaderFiberVTK::~WReaderFiberVTK() throw()
{
}

boost::shared_ptr< WDataSetFibers > WReaderFiberVTK::read()
{
    m_ifs = boost::shared_ptr< std::ifstream >( new std::ifstream() );
    m_ifs->open( m_fname.c_str(), std::ifstream::in | std::ifstream::binary );
    if( !m_ifs || m_ifs->bad() )
    {
        throw WDHIOFailure( std::string( "internal error while opening file" ) );
    }
    readHeader();
    readPoints();
    readLines();

    boost::shared_ptr< WDataSetFibers > fibers = boost::shared_ptr< WDataSetFibers >( new WDataSetFibers( m_points,
                                                                                                          m_fiberStartIndices,
                                                                                                          m_fiberLengths,
                                                                                                          m_pointFiberMapping ) );
    fibers->setFilename( m_fname );

    m_ifs->close();
    return fibers;
}

void WReaderFiberVTK::readHeader()
{
    for( int i = 0; i < 4; ++i )  // strip first four lines
    {
        m_header.push_back( getLine( "reading first 4 lines (aka header)" ) );
    }

    // check if the header may be valid for the .fib format
    if( m_header.at(0) != "# vtk DataFile Version 3.0" )
    {
        wlog::error( "WReaderFiberVTK" ) << "Unsupported format version string in VTK fiber file: "
                                         << m_fname
                                         << ", "
                                         << m_header.at( 0 );
    }
    if( m_header.at(1).size() > 256 )
    {
        wlog::warn( "WReaderFiberVTK" ) << "Invalid header size of VTK fiber file: "
                                           << m_fname
                                           << ", max. 256 but got: "
                                           << string_utils::toString( m_header.at( 1 ).size() );
    }
    namespace su = string_utils;
    if( su::toUpper( su::trim( m_header.at( 2 ) ) ) != "BINARY" )
    {
        wlog::error( "WReaderFiberVTK" ) << "VTK files in '" << m_header.at( 2 ) << "' format are not yet supported";
    }
    if(  su::tokenize( m_header.at( 3 ) ).size() < 2 || su::toUpper( su::tokenize( m_header.at( 3 ) )[1] ) != "POLYDATA" )
    {
        wlog::error( "WReaderFiberVTK" ) << "Invalid fiber VTK DATASET type: " << su::tokenize( m_header.back() )[1];
    }
}

void WReaderFiberVTK::readPoints()
{
    std::string line = getLine( "reading POINTS declaration" );

    std::vector< std::string > tokens = string_utils::tokenize( line );
    if( tokens.size() != 3 || string_utils::toLower( tokens.at( 2 ) ) != "float" )
    {
        throw WDHParseError( std::string( "Invalid POINTS declaration: " + line + ", expected float." ) );
    }

    size_t numPoints = getLexicalCast< size_t >( tokens.at( 1 ), "Invalid number of points" );

    float *pointData = new float[ 3 * numPoints ];
    m_ifs->read( reinterpret_cast< char* >( pointData ), 3 * sizeof( float ) * numPoints );

    switchByteOrderOfArray( pointData, 3 * numPoints ); // all 4 bytes of each float are in wrong order we need to reorder them

    m_points = boost::shared_ptr< std::vector< float > >( new std::vector< float >( pointData, pointData + 3 * numPoints ) );

    WAssert( m_points->size() % 3 == 0, "Number of floats for coordinates not dividable by three." );
    delete[] pointData;

    // since we know here the size of the vector we may allocate it right here
    m_pointFiberMapping = boost::shared_ptr< std::vector< size_t > >( new std::vector< size_t > );
    m_pointFiberMapping->reserve( numPoints );

    line = getLine( "also eat the remaining newline after points declaration" );
    WAssert( std::string( "" ) == line, "Found characters in file where nothing was expected." );
}

void WReaderFiberVTK::readLines()
{
    std::string line = getLine( "LINES declaration" );

    std::vector< std::string > tokens = string_utils::tokenize( line );
    if( tokens.size() != 3 || string_utils::toUpper( tokens.at( 0 ) ) != "LINES" )
    {
        throw WDHException( std::string( "Invalid VTK LINES declaration: " + line ) );
    }
    size_t numLines = getLexicalCast< size_t >( tokens.at( 1 ), "Invalid number of lines in LINES delclaration" );
    size_t linesSize = getLexicalCast< size_t >( tokens.at( 2 ), "Invalid size of lines in LINES delclaration" );

    uint32_t *lineData = new uint32_t[ linesSize ];
    m_ifs->read( reinterpret_cast<char*>( lineData ), linesSize * sizeof( uint32_t ) );

    switchByteOrderOfArray( lineData, linesSize );

    m_fiberStartIndices = boost::shared_ptr< std::vector< size_t > >( new std::vector< size_t > );
    m_fiberLengths = boost::shared_ptr< std::vector< size_t > >( new std::vector< size_t > );
    m_fiberStartIndices->reserve( numLines );
    m_fiberLengths->reserve( numLines );

    // now convert lines with point numbers to real fibers
    size_t linesSoFar = 0;
    size_t pos = 0;
    size_t posInVerts = 0;
    while( linesSoFar < numLines )
    {
        m_fiberStartIndices->push_back( posInVerts );
        size_t fiberLength = lineData[pos];
        m_fiberLengths->push_back( fiberLength );
        ++pos;
        for( size_t i = 0; i < fiberLength; ++i, ++pos, ++posInVerts )
        {
            m_pointFiberMapping->push_back( linesSoFar ); // space was reserved in the readPoints memeber function
        }
        ++linesSoFar;
    }

    delete[] lineData;

    line = getLine( "also eat the remaining newline after lines declaration" );
    WAssert( std::string( "" ) == line, "Found characters in file where nothing was expected." );
}

std::string WReaderFiberVTK::getLine( const std::string& desc )
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
