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
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>


#include "WLoaderBiosig.h"
#include "../WEEG.h"
#include "../WSubject.h"
#include "../../common/WException.h"
#include "../../common/WLogger.h"
#include "../../common/WStringUtils.h"

WLoaderBiosig::WLoaderBiosig( std::string fileName )
    : WLoader( fileName ),
      m_columns( 0 ),
      m_rows( 0 )
{
}

void WLoaderBiosig::fillSegmentColumnBased( std::vector<std::vector<double> >* segment, biosig_data_type* data )
{
    for( unsigned int i = 0; i < m_columns; ++i )
    {
        WEEGElectrode channel( 0 );
        for( unsigned int j = 0; j < m_rows; ++j )
        {
            channel.push_back( data[i*m_rows+j] );
        }
        segment->push_back( channel );
    }
}

void WLoaderBiosig::fillSegmentRowBased( std::vector<std::vector<double> >* segment, biosig_data_type* data )
{
    for( unsigned int j = 0; j < m_rows; ++j )
    {
        WEEGElectrode channel( 0 );
        for( unsigned int i = 0; i < m_columns; ++i )
        {
            channel.push_back( data[i*m_rows+j] );
        }
        segment->push_back( channel );
    }
}

WEEGElectrodeLibrary extractElectrodePositions( std::string elcFileName )
{
    namespace su = string_utils;

    std::ifstream ifs;
    ifs.open( elcFileName.c_str(), std::ifstream::in );
    if( !ifs || ifs.bad() )
    {
        WLogger::getLogger()->addLogMessage( "Try load broken file '" + elcFileName + "'", "BiosigLoader", LL_ERROR );
        throw std::runtime_error( "Problem during reading file. Probably file not found." );
    }

    std::string line = "";

    while(  ifs.good() && line.substr( 0, 16 ) != "NumberPositions=" )  // go to number of positions
    {
        std::getline( ifs, line );
        if( !ifs.good() )
        {
            WLogger::getLogger()->addLogMessage( "Unexpected end of file: " + elcFileName, "BiosigLoader", LL_ERROR );
        }
    }
    std::vector< std::string > tokens = su::tokenize( line );
    size_t numPositions = boost::lexical_cast< size_t >( tokens.at( 1 ) );

    while( ifs.good() && line.substr( 0, 9 ) != "Positions" )  // go to line before start of positions
    {
        std::getline( ifs, line );
        if( !ifs.good() )
        {
            WLogger::getLogger()->addLogMessage( "Unexpected end of file: " + elcFileName, "BiosigLoader", LL_ERROR );
        }
    }

    size_t posCounter = 0;
    WEEGElectrodeLibrary elecPos;
    while( posCounter != numPositions && ifs.good() && line.substr( 0, 9 ) != "Labels" )  // run through all positions
    {
        std::getline( ifs, line );
        if( !ifs.good() )
        {
            WLogger::getLogger()->addLogMessage( "Unexpected end of file: " + elcFileName, "BiosigLoader", LL_ERROR );
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


boost::shared_ptr< WDataSet > WLoaderBiosig::load()
{
    assert( m_fileName.substr( m_fileName.size() - 4 ) == ".edf" && "We expect only EDF so far." );
    std::string elcFileName = m_fileName;
    elcFileName.resize( elcFileName.size() - 3 ); // drop suffix
    elcFileName += "elc"; // add new suffix

    hd =  sopen( m_fileName.c_str(), "r", 0 );

    switch( B4C_ERRNUM )
    {
        case B4C_NO_ERROR:
            break;
        case B4C_FORMAT_UNKNOWN:
            throw WException( "BIOSIG: Unknown format!" );
            break;
        case B4C_FORMAT_UNSUPPORTED:
            throw WException( "BIOSIG: Unsupported format!" );
            break;
        case B4C_UNSPECIFIC_ERROR:
            throw WException( "BIOSIG: Unspecific error occured!" );
            break;
        default:
            throw WException( "BIOSIG: An error occured! The type is not known to OpenWalnut biosig loader." );
    }

    if( hd->NRec == -1 )
    {
        throw WException( "Unknown number of blocks in file loaded by biosig!" );
    }


    bool rowBasedChannels = hd->FLAG.ROW_BASED_CHANNELS;
    if( rowBasedChannels )
    {
        wlog::info( "BIOSIG channels stored as rows." );
    }
    else
    {
        wlog::info( "BIOSIG channels stored as cols." );
    }

    biosig_data_type* DATA = 0;
    size_t LEN = 1;
    size_t dummy = sread( DATA, 0, LEN, hd );
    wlog::info( "BIOSIG" ) << " DataSize " << dummy;
    size_t nbSamples = LEN*hd->SPR*hd->NS;
    m_rows =  hd->data.size[0];
    m_columns = hd->data.size[1];
    wlog::info( "BIOSIG" ) << " nbSamples " << nbSamples;


    std::vector<std::vector<double> > segment;
    if( rowBasedChannels )
    {
        fillSegmentRowBased( &segment, hd->data.block );
    }
    else
    {
        fillSegmentColumnBased( &segment, hd->data.block );
    }


    std::vector<std::vector<std::vector<double> > > segments( 0 );
    segments.push_back( segment );


    WEEGElectrodeLibrary lib = extractElectrodePositions( elcFileName );

    if( hd->NS != lib.size() )
        throw WDHException( "Contents of edf and elc files are not compatible: Different number of channels." );

    WEEGChannelLabels labels( hd->NS );
    for( unsigned int i = 0; i < hd->NS; ++i )
    {
//         std::cout << "BIOSIG Channel Label    : " << hd->CHANNEL[i].Label << std::endl;
        labels[i].first = hd->CHANNEL[i].Label;
        // TODO(wiebel): set second channel
    }

    boost::shared_ptr< WEEG > eeg = boost::shared_ptr< WEEG >( new WEEG( segments, lib, labels ) );
    eeg->setFileName( m_fileName );


    wlog::info( "BIOSIG loading done." );
    return eeg;
}
