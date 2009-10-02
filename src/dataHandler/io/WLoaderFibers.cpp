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
#include <cassert>
#include <fstream>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

#include "WLoaderFibers.h"
#include "WIOTools.hpp"
#include "../WDataHandler.h"
#include "../../common/WStringUtils.hpp"
#include "../../math/WPosition.h"

WLoaderFibers::WLoaderFibers( std::string fname, boost::shared_ptr< WDataHandler > dataHandler ) throw( WDHIOFailure )
    : WLoader( fname, dataHandler )
{
    m_ifs = new std::ifstream( fname.c_str(), std::ifstream::in | std::ifstream::binary );
    if( !( *m_ifs ) || m_ifs->bad() )
    {
        throw WDHIOFailure( "Load broken file '" + m_fileName + "'" );
    }
}

WLoaderFibers::~WLoaderFibers() throw()
{
    if( m_ifs )
    {
        m_ifs->close();
    }
    delete( m_ifs );
}

void WLoaderFibers::operator()() throw()
{
    try
    {
        readHeader();
        readPoints();
    }
    catch( WDHException e )
    {
        m_ifs->close();
        // TODO(math): we should print the file name also, since knowing that
        // the file was malformated, doesn't give you the hint that there
        // could be thousands of them
        std::cerr << "Error :: DataHandler :: Abort loading VTK file due to: " << e.what() << std::endl;
    }
}

void WLoaderFibers::readHeader() throw( WDHIOFailure, WDHException )
{
    std::string line;
    try
    {
        for( int i = 0; i < 4; ++i )  // strip first four lines
        {
            std::getline( *m_ifs, line );
            if( !m_ifs->good() )
            {
                throw WDHException( "Unexpected end of file: " + m_fileName );
            }
            m_header.push_back( line );
        }
    }
    catch( const std::ios_base::failure &e )
    {
        throw WDHIOFailure( "Reading first 4 lines of '" + m_fileName + "': " + e.what() );
    }

    // check if the header may be valid for the .fib format
    if( m_header.at(0) != "# vtk DataFile Version 3.0" )
    {
        throw WDHIOFailure( "Unsupported format version string: " + m_header.at( 0 ) );
    }
    if( m_header.at(1).size() > 256 )
    {
        // TODO(math): This should be just a warning
        throw WDHException( "VTK header too big: " + boost::lexical_cast< std::string >( m_header.at( 1 ).size() ) );
    }
    namespace su = string_utils;
    if( su::toUpper( su::trim( m_header.at( 2 ) ) ) != "BINARY" )
    {
        throw WDHException( "VTK files in '" + m_header.at( 2 ) + "' format are not yet supported" );
    }
    if(  su::tokenize( m_header.at( 3 ) ).size() < 2 ||
         su::toUpper( su::tokenize( m_header.at( 3 ) )[1] ) != "POLYDATA" )
    {
        throw WDHException( "Invalid VTK DATASET type: " + su::tokenize( m_header.back() )[1] );
    }
}

void WLoaderFibers::readPoints()
{
    std::string line;
    try
    {
        std::getline( *m_ifs, line );
    }
    catch( const std::ios_base::failure &e )
    {
        throw WDHIOFailure( "Error reading POINTS declaration '" + m_fileName + "': " + e.what() );
    }
    namespace su = string_utils;
    size_t numPoints = 0;
    std::vector< std::string > tokens = su::tokenize( line );
    if( tokens.size() < 3 || su::toLower( tokens.at( 2 ) ) != "float" )
    {
        throw WDHException( "Invalid VTK POINTS declaration: " + line );
    }
    try
    {
        numPoints = boost::lexical_cast< size_t >( tokens.at( 1 ) );
    }
    catch( const boost::bad_lexical_cast &e )
    {
        throw WDHException( "Invalid number of points: " + tokens.at( 1 ) );
    }

    m_points.reserve( numPoints );
    float *pointData = new float[ 3 * numPoints ];
    m_ifs->read( reinterpret_cast< char* >( pointData ), 3 * sizeof( float ) * numPoints );

    // all 4 bytes of each float are in wrong order we need to reorder them
    wiotools::switchByteOrderOfArray( pointData, 3 * numPoints );

    for( size_t i = 0; i < numPoints; ++i )
    {
        m_points.push_back( wmath::WPosition( pointData[i * 3],
                                              pointData[i * 3 + 1],
                                              pointData[i * 3 + 2] ) );
    }
    delete[] pointData;
}
