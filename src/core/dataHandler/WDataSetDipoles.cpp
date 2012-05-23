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

#include <vector>

#include "WDataSetDipoles.h"

// prototype instance as singleton
boost::shared_ptr< WPrototyped > WDataSetDipoles::m_prototype = boost::shared_ptr< WPrototyped >();


WDataSetDipoles::WDataSetDipoles()
{
}

WDataSetDipoles::WDataSetDipoles( WPosition dipPos, std::vector<float> mags, std::vector<float> times )
{
    WAssert( mags.size() == times.size(), "There has to be a magnitude for every time and vice versa." );
    for( size_t id = 0; id < times.size() - 1; ++id )
    {
        WAssert( times[id] < times[id+1], "Times need to be ascending." );
    }
    addDipole( dipPos, mags, times );
}

WDataSetDipoles::~WDataSetDipoles()
{
}

boost::shared_ptr< WPrototyped > WDataSetDipoles::getPrototype()
{
    if( !m_prototype )
    {
        m_prototype = boost::shared_ptr< WPrototyped >( new WDataSetDipoles() );
    }

    return m_prototype;
}

size_t WDataSetDipoles::addDipole( WPosition dipPos, std::vector<float> mags, std::vector<float> times )
{
    Dipole dipole;
    dipole.m_dipolePosition = dipPos;
    dipole.m_magnitudes = mags;
    dipole.m_times = times;
    m_dipoles.push_back( dipole );
    return m_dipoles.size() - 1;
}

WPosition WDataSetDipoles::getPosition( size_t dipoleId )
{
    return m_dipoles[dipoleId].m_dipolePosition;
}

size_t WDataSetDipoles::getNumberOfDipoles()
{
    return m_dipoles.size();
}

float WDataSetDipoles::getMagnitude( float time, size_t dipoleId )
{
    std::vector<float>& times = m_dipoles[dipoleId].m_times;
    std::vector<float>& magnitudes = m_dipoles[dipoleId].m_magnitudes;

    if( time < times[0] || time > times.back() )
    {
        return 0;
    }
    else
    {
        size_t upperBoundId;
        for( size_t id = 0; id < times.size(); ++id )
        {
            if( time < times[id] )
            {
                upperBoundId = id - 1;
                break;
            }
        }
        float scale = ( time - times[upperBoundId-1] ) / ( times[upperBoundId] - times[upperBoundId-1] );
        float magnitude = magnitudes[upperBoundId-1] + scale * ( magnitudes[upperBoundId] - magnitudes[upperBoundId-1] );
        return magnitude;
    }
}
