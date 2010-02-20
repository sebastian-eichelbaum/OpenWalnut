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
#include <string>
#include <vector>

#include <boost/lexical_cast.hpp>

#include "../../common/WLogger.h"
#include "WLoaderEEG.h"


WLoaderEEG::WLoaderEEG( std::string fileName ) throw( WDHIOFailure )
    : WLoader( fileName )
{
}

WEEGElectrodeLibrary WLoaderEEG::extractElectrodePositions()
{
    namespace su = string_utils;

    std::string elcFileName = m_fileName;
    elcFileName.resize( elcFileName.size() - 3 ); // drop suffix
    elcFileName += "elc"; // add new suffix

    std::ifstream ifs;
    ifs.open( elcFileName.c_str(), std::ifstream::in );
    if( !ifs || ifs.bad() )
    {
        WLogger::getLogger()->addLogMessage( "Try load broken file '" + elcFileName + "'", "EEG Loader", LL_ERROR );
        throw std::runtime_error( "Problem during reading file. Probably file not found." );
    }

    std::string line = "";

    while(  ifs.good() && line.substr( 0, 16 ) != "NumberPositions=" )  // go to number of positions
    {
        std::getline( ifs, line );
        if( !ifs.good() )
        {
            WLogger::getLogger()->addLogMessage( "Unexpected end of file: " + elcFileName, "EEG Loader", LL_ERROR );
        }
    }
    std::vector< std::string > tokens = su::tokenize( line );
    size_t numPositions = boost::lexical_cast< size_t >( tokens.at( 1 ) );

    while( ifs.good() && line.substr( 0, 9 ) != "Positions" )  // go to line before start of positions
    {
        std::getline( ifs, line );
        if( !ifs.good() )
        {
            WLogger::getLogger()->addLogMessage( "Unexpected end of file: " + elcFileName, "EEG Loader", LL_ERROR );
        }
    }

    size_t posCounter = 0;
    WEEGElectrodeLibrary elecPos;
    while( posCounter != numPositions && ifs.good() && line.substr( 0, 9 ) != "Labels" )  // run through all positions
    {
        std::getline( ifs, line );
        if( !ifs.good() )
        {
            WLogger::getLogger()->addLogMessage( "Unexpected end of file: " + elcFileName, "EEG Loader", LL_ERROR );
        }
        else
        {
            ++posCounter;
            std::vector< std::string > lineTokens = su::tokenize( line, ":" );
            std::string label = lineTokens.at( 0 );
            label = su::rTrim( label );
//                 std::cout << "Loading positions: " << label << std::endl;
            std::vector< std::string > posTokens = su::tokenize( lineTokens.at( 1 ) );
            double posX = boost::lexical_cast< double >( posTokens.at( 1 ) );
            double posY = boost::lexical_cast< double >( posTokens.at( 2 ) );
            double posZ = boost::lexical_cast< double >( posTokens.at( 3 ) );
            wmath::WPosition pos( posX, posY, posZ );
            elecPos.push_back( WEEGElectrodeObject( pos ) );
//                 std::cout << "Loading positions: " << pos << std::endl;
        }
    }

    std::getline( ifs, line );
    assert( elecPos.size() == numPositions );
    assert( line.substr( 0, 6 )  == "Labels" );

    return elecPos;
}