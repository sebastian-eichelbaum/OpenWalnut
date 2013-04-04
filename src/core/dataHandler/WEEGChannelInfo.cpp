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
#include <string>

#include <boost/shared_ptr.hpp>

#include "../common/exceptions/WOutOfBounds.h"
#include "../common/math/linearAlgebra/WPosition.h"
#include "WEEGPositionsLibrary.h"
#include "WEEGChannelInfo.h"
#include "exceptions/WDHException.h"
#include "io/WPagerEEG.h"


WEEGChannelInfo::WEEGChannelInfo( std::size_t channelID,
                                  boost::shared_ptr< WPagerEEG > pager,
                                  boost::shared_ptr< WEEGPositionsLibrary > positionsLibrary )
{
    if( !pager )
    {
        throw WDHException( std::string( "Couldn't construct new EEG channel info: pager invalid" ) );
    }

    if( !positionsLibrary )
    {
        throw WDHException( std::string( "Couldn't construct new EEG channel info: positions library invalid" ) );
    }

    if( channelID >= pager->getNumberOfChannels() )
    {
        std::ostringstream stream;
        stream << "The EEG has no channel number " << channelID;
        throw WOutOfBounds( stream.str() );
    }

    m_unit = pager->getChannelUnit( channelID );
    m_label = pager->getChannelLabel( channelID );

    try
    {
        m_position = positionsLibrary->getPosition( m_label );
        m_hasPosition = true;
    }
    catch( const WOutOfBounds& )
    {
        m_hasPosition = false;
    }
}

std::string WEEGChannelInfo::getUnit() const
{
    return m_unit;
}

std::string WEEGChannelInfo::getLabel() const
{
    return m_label;
}

WPosition WEEGChannelInfo::getPosition() const throw( WDHException )
{
    if( m_hasPosition )
    {
        return m_position;
    }
    else
    {
        throw WDHException( std::string( "The position of this electrode is unknown." ) );
    }
}
