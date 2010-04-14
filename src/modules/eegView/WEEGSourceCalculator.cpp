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

#include <vector>

#include <boost/shared_ptr.hpp>

#include "../../common/math/WPosition.h"
#include "../../dataHandler/WEEG2.h"
#include "../../dataHandler/WEEGChannelInfo.h"
#include "../../dataHandler/exceptions/WDHException.h"
#include "WEEGEvent.h"
#include "WEEGSourceCalculator.h"


WEEGSourceCalculator::WEEGSourceCalculator( const boost::shared_ptr< const WEEG2 > eeg )
    : m_eeg( eeg )
{
    // it is possible to calculate some time position independend data
    // structures here (like the lead matrix)

    m_hasPosition.reserve( m_eeg->getNumberOfChannels() );
    for( std::size_t channelID = 0; channelID < m_eeg->getNumberOfChannels(); ++channelID )
    {
        try
        {
            m_eeg->getChannelInfo( channelID )->getPosition();
            m_hasPosition.push_back( true );
        }
        catch( const WDHException& )
        {
            m_hasPosition.push_back( false );
        }
    }
}

wmath::WPosition WEEGSourceCalculator::calculate( const boost::shared_ptr< const WEEGEvent > event ) const
{
    const std::vector< double >& values = event->getValues();
    double min = 1.0 / 0.0;
    double sum = 0.0;
    for( std::size_t channelID = 0; channelID < values.size(); ++channelID )
    {
        if( m_hasPosition[channelID] )
        {
            if( values[channelID] < min )
            {
                min = values[channelID];
            }
            sum += values[channelID];
        }
    }

    sum -= values.size() * min;

    wmath::WPosition source;
    for( std::size_t channelID = 0; channelID < values.size(); ++channelID )
    {
        if( m_hasPosition[channelID] )
        {
            source += ( values[channelID] - min ) / sum * m_eeg->getChannelInfo( channelID )->getPosition();
        }
    }

    return source;
}
