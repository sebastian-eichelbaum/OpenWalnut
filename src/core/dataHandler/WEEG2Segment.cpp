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
#include "WEEG2Segment.h"
#include "WEEGValueMatrix.h"
#include "WRecording.h"
#include "exceptions/WDHException.h"
#include "io/WPagerEEG.h"

WEEG2Segment::WEEG2Segment( std::size_t segmentID, boost::shared_ptr< WPagerEEG > pager )
    : m_segmentID( segmentID ),
      m_pager( pager )
{
    if( !m_pager )
    {
        throw WDHException( std::string( "Couldn't construct new EEG segment: pager invalid" ) );
    }

    if( m_segmentID >= m_pager->getNumberOfSegments() )
    {
        std::ostringstream stream;
        stream << "The EEG has no segment number " << m_segmentID;
        throw WOutOfBounds( stream.str() );
    }

    m_nbSamples = m_pager->getNumberOfSamples( m_segmentID );
    if( m_nbSamples <= 0 || WRecording::MAX_RECORDING_SAMPLES < m_nbSamples )
    {
        throw WDHException( std::string( "Couldn't construct new EEG segment: invalid number of samples" ) );
    }
}

std::size_t WEEG2Segment::getNumberOfSamples() const
{
    return m_nbSamples;
}

boost::shared_ptr< WEEGValueMatrix > WEEG2Segment::getValues( std::size_t start, std::size_t length ) const
{
    // No test whether start and length are valid - this should be done only
    // one time by the pager.
    return m_pager->getValues( m_segmentID, start, length );
}
