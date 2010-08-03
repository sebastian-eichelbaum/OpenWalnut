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

#include <algorithm>
#include <iomanip>
#include <numeric>

#include "WAssert.h"
#include "WHistogram.h"
#include "WLimits.h"
#include "WLogger.h"

WHistogram::WHistogram( double min, double max, size_t size )
    : m_bins( size, 0 ),
      m_min( min ),
      m_max( max )
{
    if( min > max )
    {
        std::swap( m_min, m_max );
    }
    WAssert( m_bins.size() > 0, "Error: A histogram with a size of 0 does not make any sense" );
    m_intervalWidth = std::abs( m_min - m_max ) / m_bins.size();
}

WHistogram::~WHistogram()
{
}

void WHistogram::insert( double value )
{
    if( value > m_max || value < m_min )
    {
        wlog::warn( "WHistogram" ) << std::scientific << std::setprecision( 16 ) << "Inserted value out of bounds, thread: "
                                   << value << " as min, resp. max: " << m_min << "," << m_max;
        return; // value = ( value > m_max ? m_max : m_min );
    }

    if( value == m_max )
    {
        value = m_max - wlimits::DBL_EPS;
    }

    m_bins.at( static_cast< size_t >( std::abs( value - m_min ) / m_intervalWidth ) )++;
}

size_t WHistogram::binSize() const
{
    return m_bins.size();
}

size_t WHistogram::valuesSize() const
{
    return std::accumulate( m_bins.begin(), m_bins.end(), 0 );
}

size_t WHistogram::operator[]( size_t index ) const
{
    if( index >= m_bins.size() )
    {
        wlog::error( "WHistogram" ) << index << "th interval is not available, there are only: " << m_bins.size();
        return 0;
    }
    return m_bins[ index ];
}
