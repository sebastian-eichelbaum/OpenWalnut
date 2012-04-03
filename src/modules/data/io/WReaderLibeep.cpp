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

extern "C"
{
    #include <libeep/cnt/cnt.h>
}

#include <locale.h>

#include <string>

#include "core/common/WLogger.h"
#include "core/dataHandler/WEEG.h"
#include "core/dataHandler/exceptions/WDHNoSuchFile.h"

#include "WReaderLibeep.h"

WReaderLibeep::WReaderLibeep( std::string fileName )
    : WReaderEEG( fileName )
{
}

boost::shared_ptr< WDataSet > WReaderLibeep::load()
{
    wlog::debug( "Libeep Reader" ) << "Opening " << m_fname;

    // libeep needs the standard C locale to load float values from ASCII
    setlocale( LC_NUMERIC, "C" );

    // initialize
    FILE* file = fopen( m_fname.c_str(), "rb" );
    if( !file )
    {
        throw WDHNoSuchFile( m_fname + " could not be opened" );
    }
    int status;
    eeg_t* eeg = eep_init_from_file( m_fname.c_str(), file, &status );
    if( status != CNTERR_NONE || !eeg )
    {
        std::ostringstream stream;
        stream << m_fname << " could not be initialized. Libeep status code: " << status;
        throw WDHIOFailure( stream.str() );
    }

    // read data
    unsigned int numberOfChannels = eep_get_chanc( eeg );
    slen_t numberOfSamples = eep_get_samplec( eeg );

    if( numberOfSamples > 8192 )
    {
        // limit maximum size of the dataset
        // TODO(cornimueller): Don't load EEG data as a whole, use blocks instead
        // TODO(wiebel): Don't load EEG data as a whole, use blocks instead
        numberOfSamples = 8192;
    }

    sraw_t* buffer = new sraw_t[CNTBUF_ARRAYSIZE( eeg, numberOfSamples )];
    eep_read_sraw( eeg, DATATYPE_EEG, buffer, numberOfSamples );

    WEEGChannelLabels channelLabels( numberOfChannels );
    WEEGSegment segment( numberOfChannels );
    for( unsigned int channel = 0; channel < numberOfChannels; ++channel )
    {
        channelLabels[channel].first = eep_get_chan_label( eeg, channel );

        double scale = eep_get_chan_scale( eeg, channel );
        WEEGElectrode electrode( numberOfSamples );
        for( slen_t sample = 0; sample < numberOfSamples; ++sample )
        {
            electrode[sample] = buffer[sample * numberOfChannels + channel] * scale;
        }
        segment[channel] = electrode;
    }

    // cleanup
    delete[] buffer;
    eep_fclose( eeg );

    WEEGElectrodeLibrary electrodeLibrary = extractElectrodePositions();

    if( electrodeLibrary.size() != numberOfChannels )
    {
        std::ostringstream stream;
        stream << "Contents of cnt and elc files are not compatible: The cnt has " << numberOfChannels
               << " channels, but the elc has " << electrodeLibrary.size() << " channels.";
        throw WDHException( stream.str() );
    }

    // construct WEEG object and return it
    boost::shared_ptr< WEEG > out( new WEEG(
        WEEGSegmentArray( 1, segment ),
        electrodeLibrary,
        channelLabels
    ) );
    out->setFilename( m_fname );
    return out;
}
