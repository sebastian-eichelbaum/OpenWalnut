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

#include <clocale>
#include <cstddef>
#include <cstdio>

#include <sstream>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "core/common/exceptions/WOutOfBounds.h"
#include "core/dataHandler/exceptions/WDHIOFailure.h"
#include "core/dataHandler/exceptions/WDHNoSuchFile.h"
#include "core/dataHandler/WEEGValueMatrix.h"
#include "WPagerEEGLibeep.h"


WPagerEEGLibeep::WPagerEEGLibeep( std::string fileName )
    : WPagerEEG( fileName )
{
    // libeep needs the standard C locale to load float values from ASCII
    std::setlocale( LC_NUMERIC, "C" );

    // initialize
    std::FILE* file = std::fopen( getFilename().c_str(), "rb" );
    if( !file )
    {
        throw WDHNoSuchFile( getFilename() + " could not be opened" );
    }

    int status;
    m_eeg = eep_init_from_file( getFilename().c_str(), file, &status );
    if( status != CNTERR_NONE || !m_eeg )
    {
        std::ostringstream stream;
        stream << getFilename() << " could not be initialized. Libeep status code: " << status;
        throw WDHIOFailure( stream.str() );
    }

    m_nbChannels = eep_get_chanc( m_eeg );
    m_nbSamples = eep_get_samplec( m_eeg );

    m_scales = std::vector< double >( m_nbChannels );
    for( size_t channelID = 0; channelID < m_nbChannels; ++channelID )
    {
        m_scales[channelID] = eep_get_chan_scale( m_eeg, channelID );
    }
}

WPagerEEGLibeep::~WPagerEEGLibeep()
{
    // cleanup
    int status = eep_fclose( m_eeg );
    if( status != CNTERR_NONE )
    {
        std::ostringstream stream;
        stream << getFilename() << " could not be closed. Libeep status code: " << status;
        throw WDHIOFailure( stream.str() );
    }
}

std::size_t WPagerEEGLibeep::getNumberOfSegments() const
{
    return 1;
}

std::size_t WPagerEEGLibeep::getNumberOfChannels() const
{
    return m_nbChannels;
}

std::size_t WPagerEEGLibeep::getNumberOfSamples( std::size_t segmentID ) const
{
    if( segmentID != 0 )
    {
        std::ostringstream stream;
        stream << getFilename() << " has no segment number " << segmentID;
        throw WOutOfBounds( stream.str() );
    }

    return m_nbSamples;
}

boost::shared_ptr< WEEGValueMatrix > WPagerEEGLibeep::getValues( std::size_t segmentID, std::size_t start, std::size_t length ) const
{
    if( segmentID != 0 )
    {
        std::ostringstream stream;
        stream << getFilename() << " has no segment number " << segmentID;
        throw WOutOfBounds( stream.str() );
    }

    if( start + length > m_nbSamples )
    {
        std::ostringstream stream;
        stream << "Could not read sample number " << start + length - 1 << " of file " << getFilename()
               << ", it only has " << m_nbSamples << " samples";
        throw WOutOfBounds( stream.str() );
    }

    int status = eep_seek( m_eeg, DATATYPE_EEG, start, 0 );
    if( status != CNTERR_NONE )
    {
        std::ostringstream stream;
        stream << "Seek error on " << getFilename() << ". Libeep status code: " << status;
        throw WDHIOFailure( stream.str() );
    }

    sraw_t* buffer = new sraw_t[CNTBUF_ARRAYSIZE( m_eeg, length )];
    status = eep_read_sraw( m_eeg, DATATYPE_EEG, buffer, length );
    if( status != CNTERR_NONE )
    {
        std::ostringstream stream;
        stream << "Read error on " << getFilename() << ". Libeep status code: " << status;
        throw WDHIOFailure( stream.str() );
    }

    boost::shared_ptr< WEEGValueMatrix > values( new WEEGValueMatrix( m_nbChannels, std::vector< double >( length ) ) );
    for( size_t channelID = 0; channelID < m_nbChannels; ++channelID )
    {
        for( size_t sampleID = 0; sampleID < length; ++sampleID )
        {
            (*values)[channelID][sampleID] = buffer[sampleID * m_nbChannels + channelID] * m_scales[channelID];
        }
    }
    delete[] buffer;

    return values;
}

double WPagerEEGLibeep::getSamplingRate() const
{
    return eep_get_rate( m_eeg );
}

std::string WPagerEEGLibeep::getChannelUnit( std::size_t channelID ) const
{
    if( channelID >= m_nbChannels )
    {
        std::ostringstream stream;
        stream << getFilename() << " has no channel number " << channelID;
        throw WOutOfBounds( stream.str() );
    }

    return eep_get_chan_unit( m_eeg, channelID );
}

std::string WPagerEEGLibeep::getChannelLabel( std::size_t channelID ) const
{
    if( channelID >= m_nbChannels )
    {
        std::ostringstream stream;
        stream << getFilename() << " has no channel number " << channelID;
        throw WOutOfBounds( stream.str() );
    }

    return eep_get_chan_label( m_eeg, channelID );
}
