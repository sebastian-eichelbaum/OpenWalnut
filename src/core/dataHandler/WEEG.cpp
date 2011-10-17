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

#include <string>

#include "../common/WPrototyped.h"

#include "WEEG.h"

// prototype instance as singleton
boost::shared_ptr< WPrototyped > WEEG::m_prototype = boost::shared_ptr< WPrototyped >();

WEEG::WEEG( const WEEGSegmentArray& data,
            const WEEGElectrodeLibrary& electrodeLib,
            const WEEGChannelLabels& channelLabels )
    : WRecording()
{
    assert( data.size() <= WRecording::MAX_RECORDING_SEGMENTS );
    assert( data.size() > 0 );  // ensure that ther is really data
    for( WEEGSegmentArray::const_iterator it1 = data.begin(); it1 != data.end(); ++it1 )
    {
        assert( it1->size() <= WRecording::MAX_RECORDING_CHANNELS );
        assert( it1->size() > 0 );  // ensure that ther is really data
        for( WEEGSegment::const_iterator it2 = it1->begin(); it2 != it1->end(); ++it2 )
        {
            assert( it2->size() <= WRecording::MAX_RECORDING_SAMPLES );
            assert( it2->size() > 0 );  // ensure that ther is really data
        }
    }
    m_segments = data;
    m_electrodeLibrary = electrodeLib;
    m_channelLabels = channelLabels;
}

WEEG::WEEG()
    : WRecording()
{
    // do nothing here. Only useful for prototypes.
}

bool WEEG::isTexture() const
{
    return false;
}

const std::string WEEG::getName() const
{
    return "WEEG";
}

const std::string WEEG::getDescription() const
{
    return "Contains data acquired using EEG.";
}

boost::shared_ptr< WPrototyped > WEEG::getPrototype()
{
    if( !m_prototype )
    {
        m_prototype = boost::shared_ptr< WPrototyped >( new WEEG() );
    }

    return m_prototype;
}


WEEGElectrodeObject::WEEGElectrodeObject( WPosition position ):
    m_position( position )
{
}

WPosition WEEGElectrodeObject::getPosition() const
{
    return m_position;
}
