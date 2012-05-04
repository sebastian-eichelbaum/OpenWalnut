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

#include "WDataSetDipole.h"

// prototype instance as singleton
boost::shared_ptr< WPrototyped > WDataSetDipole::m_prototype = boost::shared_ptr< WPrototyped >();


WDataSetDipole::WDataSetDipole()
{
}

WDataSetDipole::WDataSetDipole( WPosition dipPos, std::vector<float> mags, std::vector<float> times )
    : m_dipolePosition( dipPos ), m_magnitudes( mags ), m_times( times )
{
    WAssert( mags.size() == times.size(), "There has to be a magnitude for every time and vice versa." );
    for( size_t id = 0; id < times.size() - 1; ++id )
    {
        WAssert( times[id] < times[id+1], "Times need to be ascending." );
    }
}

WDataSetDipole::~WDataSetDipole()
{
}

boost::shared_ptr< WPrototyped > WDataSetDipole::getPrototype()
{
    if( !m_prototype )
    {
        m_prototype = boost::shared_ptr< WPrototyped >( new WDataSetDipole() );
    }

    return m_prototype;
}

WPosition WDataSetDipole::getPosition()
{
    return m_dipolePosition;
}

float WDataSetDipole::getMagnitude( float time )
{
    if( time < m_times[0] || time > m_times.back() )
    {
        return 0;
    }
    else
    {
        size_t upperBoundId;
        for( size_t id = 0; id < m_times.size(); ++id )
        {
            if( time < m_times[id] )
            {
                upperBoundId = id - 1;
                break;
            }
        }
        float scale = ( time - m_times[upperBoundId-1] ) / ( m_times[upperBoundId] - m_times[upperBoundId-1] );
        float magnitude = m_magnitudes[upperBoundId-1] + scale * ( m_magnitudes[upperBoundId] - m_magnitudes[upperBoundId-1] );
        return magnitude;
    }
}
