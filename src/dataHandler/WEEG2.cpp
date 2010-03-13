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

#include <sstream>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "../common/exceptions/WOutOfBounds.h"
#include "exceptions/WDHException.h"
#include "io/WPagerEEG.h"
#include "WEEGChannelInfo.h"
#include "WEEG2Segment.h"
#include "WEEG2.h"


WEEG2::WEEG2( boost::shared_ptr< WPagerEEG > pager )
{
    if( !pager )
    {
        throw WDHException( "Couldn't construct new EEG: pager invalid" );
    }

    m_segments.reserve( pager->getNumberOfSegments() );
    for( std::size_t segmentID = 0; segmentID < pager->getNumberOfSegments(); ++segmentID )
    {
        m_segments.push_back( boost::shared_ptr< WEEG2Segment >( new WEEG2Segment( pager, segmentID ) ) );
    }

    m_channelInfos.reserve( pager->getNumberOfChannels() );
    for( std::size_t channelID = 0; channelID < pager->getNumberOfChannels(); ++channelID )
    {
        m_channelInfos.push_back( boost::shared_ptr< WEEGChannelInfo >( new WEEGChannelInfo( pager, channelID ) ) );
    }
}

std::size_t WEEG2::getNumberOfSegments() const
{
    return m_segments.size();
}

std::size_t WEEG2::getNumberOfChannels() const
{
    return m_channelInfos.size();
}

boost::shared_ptr< WEEG2Segment > WEEG2::getSegment( std::size_t segmentID ) const
{
    if( segmentID >= m_segments.size() )
    {
        std::ostringstream stream;
        stream << "The EEG has no segment number " << segmentID;
        throw WOutOfBounds( stream.str() );
    }

    return m_segments[segmentID];
}

boost::shared_ptr< WEEGChannelInfo > WEEG2::getChannelInfo( std::size_t channelID ) const
{
    if( channelID >= m_channelInfos.size() )
    {
        std::ostringstream stream;
        stream << "The EEG has no channel number " << channelID;
        throw WOutOfBounds( stream.str() );
    }

    return m_channelInfos[channelID];
}
