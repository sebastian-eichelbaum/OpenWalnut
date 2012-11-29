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
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "core/dataHandler/WEEG.h"
#include "core/dataHandler/WSubject.h"
#include "core/common/WAssert.h"
#include "core/common/WException.h"
#include "core/common/WLogger.h"
#include "core/common/WStringUtils.h"

#include "WReaderBiosig.h"

WReaderBiosig::WReaderBiosig( std::string fileName )
    : WReaderEEG( fileName ),
      m_columns( 0 ),
      m_rows( 0 )
{
}

void WReaderBiosig::fillSegmentColumnBased( std::vector<std::vector<double> >* segment, biosig_data_type* data )
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

void WReaderBiosig::fillSegmentRowBased( std::vector<std::vector<double> >* segment, biosig_data_type* data )
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

boost::shared_ptr< WDataSet > WReaderBiosig::load()
{
    WAssert( m_fname.substr( m_fname.size() - 4 ) == ".edf", "We expect only EDF for the biosig loader so far." );

    hd =  sopen( m_fname.c_str(), "r", 0 );

#if( BIOSIG_VERSION >= 10400 )
    switch( serror2( hd ) )
#else
    switch( B4C_ERRNUM )
#endif
    {
        case B4C_NO_ERROR:
            break;
        case B4C_FORMAT_UNKNOWN:
            throw WException( std::string( "BIOSIG: Unknown format!" ) );
            break;
        case B4C_FORMAT_UNSUPPORTED:
            throw WException( std::string( "BIOSIG: Unsupported format!" ) );
            break;
        case B4C_UNSPECIFIC_ERROR:
            throw WException( std::string( "BIOSIG: Unspecific error occured!" ) );
            break;
        default:
            throw WException( std::string( "BIOSIG: An error occured! The type is not known to OpenWalnut biosig loader." ) );
    }

    if( hd->NRec == -1 )
    {
        throw WException( std::string( "Unknown number of blocks in file loaded by biosig!" ) );
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


    WEEGElectrodeLibrary lib = extractElectrodePositions();

    if( hd->NS != lib.size() )
        throw WDHException( std::string( "Contents of edf and elc files are not compatible: Different number of channels." ) );

    WEEGChannelLabels labels( hd->NS );
    for( unsigned int i = 0; i < hd->NS; ++i )
    {
//         std::cout << "BIOSIG Channel Label    : " << hd->CHANNEL[i].Label << std::endl;
        labels[i].first = hd->CHANNEL[i].Label;
        // TODO(wiebel): set second channel
    }

    boost::shared_ptr< WEEG > eeg = boost::shared_ptr< WEEG >( new WEEG( segments, lib, labels ) );
    eeg->setFilename( m_fname );


    wlog::info( "BIOSIG loading done." );
    return eeg;
}
