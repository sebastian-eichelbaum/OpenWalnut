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
#include <utility>

#include "WAssert.h"
#include "WHistogramBasic.h"
#include "WLimits.h"
#include "WLogger.h"

WHistogramBasic::WHistogramBasic( double min, double max, size_t buckets ):
    WHistogram( min, max, buckets ),
    m_bins( buckets, 0 ),
    m_intervalWidth( ( m_maximum - m_minimum ) / static_cast< double >( m_nbBuckets ) )
{
}

WHistogramBasic::WHistogramBasic( const WHistogramBasic& hist ):
    WHistogram( hist ),
    m_bins( hist.m_bins ),
    m_intervalWidth( hist.m_intervalWidth )

{
}

WHistogramBasic::~WHistogramBasic()
{
}

size_t WHistogramBasic::operator[]( size_t index ) const
{
    return m_bins[ index ];
}

size_t WHistogramBasic::at( size_t index ) const
{
    if( index >= m_bins.size() )
    {
        wlog::error( "WHistogramBasic" ) << index << "th interval is not available, there are only: " << m_bins.size();
        return 0;
    }
    return m_bins[ index ];
}

double WHistogramBasic::getBucketSize( size_t /* index */ ) const
{
    return m_intervalWidth;
}

std::pair< double, double > WHistogramBasic::getIntervalForIndex( size_t index ) const
{
    double first = m_minimum + m_intervalWidth * index;
    double second =  m_minimum + m_intervalWidth * ( index + 1 );
    return std::make_pair( first, second );
}

void WHistogramBasic::insert( double value )
{
    if( value > m_maximum || value < m_minimum )
    {
        wlog::warn( "WHistogramBasic" ) << std::scientific << std::setprecision( 16 ) << "Inserted value out of bounds, thread: "
                                        << value << " as min, resp. max: " << m_minimum << "," << m_maximum;
        return; // value = ( value > m_maximum ? m_maximum : m_minimum );
    }

    if( value == m_maximum )
    {
        value = m_maximum - wlimits::DBL_EPS;
    }

    m_bins.at( static_cast< size_t >( std::abs( value - m_minimum ) / m_intervalWidth ) )++;
}

size_t WHistogramBasic::valuesSize() const
{
    return std::accumulate( m_bins.begin(), m_bins.end(), 0 );
}

