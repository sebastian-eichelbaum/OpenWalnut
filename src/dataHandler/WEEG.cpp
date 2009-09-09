//---------------------------------------------------------------------------
//
// Project: OpenWalnut
//
// Copyright 2009 SomeCopyrightowner
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

#include "WEEG.h"
#include "../common/WLimits.h"


WEEG::WEEG( boost::shared_ptr< WMetaInfo > metaInfo, const WEEGSegmentArray& data )
    : WRecording( metaInfo )
{
    assert( data.size() <= wlimits::MAX_RECORDING_SEGMENTS );
    assert( data.size() > 0 );  // ensure that ther is really data
    for( WEEGSegmentArray::const_iterator it1 = data.begin(); it1 != data.end(); ++it1 )
    {
        assert( it1->size() <= wlimits::MAX_RECORDING_CHANNELS );
        assert( it1->size() > 0 );  // ensure that ther is really data
        for( WEEGSegment::const_iterator it2 = it1->begin(); it2 != it1->end(); ++it2 )
        {
            assert( it2->size() <= wlimits::MAX_RECORDING_SAMPLES );
            assert( it2->size() > 0 );  // ensure that ther is really data
        }
    }
    m_segments = data;
}
