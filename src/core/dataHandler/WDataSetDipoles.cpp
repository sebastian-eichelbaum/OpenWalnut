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


WDataSetDipoles::WDataSetDipoles() :
    m_maxMagnitude( 0.0f )
{
}

WDataSetDipoles::WDataSetDipoles( WPosition dipPos, std::vector<float> mags, std::vector<float> times,
                                  size_t firstTimeStep, size_t lastTimeStep ) :
    m_maxMagnitude( 0.0f )
{
    WAssert( mags.size() == times.size(), "There has to be a magnitude for every time and vice versa." );
    for( size_t id = 0; id < times.size() - 1; ++id )
    {
        WAssert( times[id] < times[id+1], "Times need to be ascending." );
    }
    addDipole( dipPos, mags, times, firstTimeStep, lastTimeStep );
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

size_t WDataSetDipoles::addDipole( WPosition dipPos, std::vector<float> mags, std::vector<float> times,
                                   size_t firstTimeStep, size_t lastTimeStep )
{
    Dipole dipole;
    dipole.m_dipolePosition = dipPos;
    dipole.m_magnitudes = mags;
    dipole.m_times = times;
    dipole.m_firstTimeStep = firstTimeStep;
    dipole.m_lastTimeStep = lastTimeStep;
    m_dipoles.push_back( dipole );

    for( size_t id = 0u; id < mags.size(); ++id )
    {
        if( mags[id] > m_maxMagnitude )
        {
            m_maxMagnitude = mags[id];
        }
    }

    return m_dipoles.size() - 1;
}

WPosition WDataSetDipoles::getPosition( size_t dipoleId )
{
    return m_dipoles[dipoleId].m_dipolePosition;
}

float WDataSetDipoles::getStartTime( size_t dipoleId ) const
{
    return m_dipoles[dipoleId].m_times[m_dipoles[dipoleId].m_firstTimeStep];
}

float WDataSetDipoles::getEndTime( size_t dipoleId ) const
{
    return m_dipoles[dipoleId].m_times[m_dipoles[dipoleId].m_lastTimeStep];
}

std::vector<float> WDataSetDipoles::getTimes( size_t dipoleId ) const
{
    const Dipole& dipole = m_dipoles[dipoleId];
    const std::vector<float>::const_iterator& begin = dipole.m_times.begin();

    return std::vector<float>( begin + dipole.m_firstTimeStep, begin + ( dipole.m_lastTimeStep + 1u ) );
}

std::vector<float> WDataSetDipoles::getMagnitudes( size_t dipoleId ) const
{
    const Dipole& dipole = m_dipoles[dipoleId];
    const std::vector<float>::const_iterator& begin = dipole.m_magnitudes.begin();

    return std::vector<float>( begin + dipole.m_firstTimeStep, begin + ( dipole.m_lastTimeStep + 1u ) );
}

size_t WDataSetDipoles::getNumberOfDipoles()
{
    return m_dipoles.size();
}

float WDataSetDipoles::getMaxMagnitude() const
{
    return m_maxMagnitude;
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
        size_t upperBoundId = 1u;
        for( ; upperBoundId < times.size() - 1u; ++upperBoundId )
        {
            if( time < times[upperBoundId] )
            {
                break;
            }
        }
        float scale = ( time - times[upperBoundId-1] ) / ( times[upperBoundId] - times[upperBoundId-1] );
        float magnitude = magnitudes[upperBoundId-1] + scale * ( magnitudes[upperBoundId] - magnitudes[upperBoundId-1] );
        return magnitude;
    }
}
