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

#include <cstddef>

#include <fstream>
#include <map>
#include <string>
#include <vector>

#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>

#include "core/common/math/linearAlgebra/WLinearAlgebra.h"
#include "core/common/WLogger.h"
#include "core/common/WStringUtils.h"
#include "core/dataHandler/exceptions/WDHIOFailure.h"
#include "core/dataHandler/exceptions/WDHNoSuchFile.h"
#include "core/dataHandler/exceptions/WDHParseError.h"
#include "core/dataHandler/WEEGPositionsLibrary.h"

#include "WReaderELC.h"

WReaderELC::WReaderELC( std::string fname )
    : WReader( fname )
{
}

boost::shared_ptr< WEEGPositionsLibrary > WReaderELC::read()
{
    std::ifstream ifs;
    ifs.open( m_fname.c_str(), std::ifstream::in );
    if( !ifs || ifs.bad() )
    {
        throw WDHNoSuchFile( std::string( "Problem loading file " + m_fname + ". Probably file not found." ) );
    }

    std::string line;
    while( ifs.good() && line.substr( 0, 16 ) != "NumberPositions=" )  // go to number of positions
    {
        std::getline( ifs, line );
        if( !ifs.good() )
        {
            throw WDHIOFailure( std::string( "Unexpected end of file " + m_fname ) );
        }
    }

    std::vector< std::string > tokens = string_utils::tokenize( line );
    std::size_t numPositions = boost::lexical_cast< std::size_t >( tokens.at( 1 ) );

    while( ifs.good() && line.substr( 0, 9 ) != "Positions" )  // go to line before start of positions
    {
        std::getline( ifs, line );
        if( !ifs.good() )
        {
            throw WDHIOFailure( std::string( "Unexpected end of file " + m_fname ) );
        }
    }

    std::size_t posCounter = 0;
    std::vector< WPosition > positions;
    positions.reserve( numPositions );
    while( posCounter != numPositions && ifs.good() && line.substr( 0, 6 ) != "Labels" )  // run through all positions
    {
        std::getline( ifs, line );
        if( !ifs.good() )
        {
            throw WDHIOFailure( std::string( "Unexpected end of file " + m_fname ) );
        }
        else
        {
            ++posCounter;
            std::vector< std::string > lineTokens = string_utils::tokenize( line, ":" );

            std::vector< std::string > posTokens = string_utils::tokenize( lineTokens.back() );
            double posX = boost::lexical_cast< double >( posTokens.at( posTokens.size() - 3 ) );
            double posY = boost::lexical_cast< double >( posTokens.at( posTokens.size() - 2 ) );
            double posZ = boost::lexical_cast< double >( posTokens.at( posTokens.size() - 1 ) );
            positions.push_back( WPosition( posX, posY, posZ ) );
        }
    }

    if( positions.size() != numPositions )
    {
        throw WDHParseError( std::string( "Could not find correct number of Positions regarding to NumberPositions statement in file " + m_fname ) );
    }

    while( ifs.good() && line.substr( 0, 6 )  != "Labels" )  // go to line before start of labels
    {
        std::getline( ifs, line );
        if( !ifs.good() )
        {
            throw WDHIOFailure( std::string( "Unexpected end of file " + m_fname ) );
        }
    }

    std::size_t labelCounter = 0;
    std::map< std::string, WPosition > positionsMap;
    while( labelCounter != numPositions && ifs.good() ) // run through all labels
    {
        std::getline( ifs, line );
        if( ifs.fail() )
        {
            throw WDHIOFailure( std::string( "Unexpected end of file " + m_fname ) );
        }
        else
        {
            std::vector< std::string > labelTokens = string_utils::tokenize( line );
            for( std::size_t i = 0; i < labelTokens.size(); ++i )
            {
                positionsMap[labelTokens[i]] = positions[labelCounter];
                ++labelCounter;
            }
        }
    }

    if( positionsMap.size() != numPositions )
    {
        throw WDHParseError( std::string( "Could not find correct number of Labels regarding to NumberPositions statement in file " + m_fname ) );
    }

    ifs.close();

    for( std::map< std::string, WPosition >::const_iterator iter = positionsMap.begin(); iter != positionsMap.end(); ++iter )
    {
        wlog::debug( "WReaderELC" ) << iter->first << ": " << iter->second;
    }

    return boost::shared_ptr< WEEGPositionsLibrary >( new WEEGPositionsLibrary( positionsMap ) );
}
