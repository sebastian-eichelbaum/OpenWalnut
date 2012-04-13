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
#include <limits>
#include <string>
#include <vector>

#include "../common/WAssert.h"
#include "../common/WLimits.h"
#include "WDataSetTimeSeries.h"

// prototype instance as singleton
boost::shared_ptr< WPrototyped > WDataSetTimeSeries::m_prototype = boost::shared_ptr< WPrototyped >();

WDataSetTimeSeries::WDataSetTimeSeries( std::vector< boost::shared_ptr< WDataSetScalar const > > datasets,
                                        std::vector< float > times )
    : m_dataSets()
{
    WAssert( !datasets.empty(), "" );
    WAssert( datasets.size() == times.size(), "" );
    std::vector< boost::shared_ptr< WDataSetScalar const > >::iterator dit;
    std::vector< float >::iterator tit;
    boost::shared_ptr< WGridRegular3D > g = boost::shared_dynamic_cast< WGridRegular3D >( datasets.front()->getGrid() );
    WAssert( g, "" );
    dataType d = datasets.front()->getValueSet()->getDataType();
    m_minValue = datasets.front()->getMin();
    m_maxValue = datasets.front()->getMax();
    for( dit = datasets.begin(), tit = times.begin(); dit != datasets.end() && tit != times.end(); ++dit, ++tit )
    {
        WAssert( *dit, "" );
        WAssert( g == boost::shared_dynamic_cast< WGridRegular3D >( ( *dit )->getGrid() ), "" );
        WAssert( !wlimits::isnan( *tit ), "" );
        WAssert( d == ( *dit )->getValueSet()->getDataType(), "" );
        WAssert( ( *dit )->getValueSet()->dimension() == 1, "" );
        WAssert( ( *dit )->getValueSet()->order() == 0, "" );
        m_dataSets.push_back( TimeSlice( *dit, *tit ) );
        if( m_minValue > ( *dit )->getMin() )
        {
            m_minValue = ( *dit )->getMin();
        }
        if( m_maxValue < ( *dit )->getMax() )
        {
            m_maxValue = ( *dit )->getMax();
        }
    }
    std::sort( m_dataSets.begin(), m_dataSets.end(), TimeSliceCompare() );
    for( std::size_t k = 1; k < m_dataSets.size(); ++k )
    {
        if( m_dataSets[ k ].second == m_dataSets[ k - 1 ].second )
        {
            throw WException( std::string( "There are multiple time slices at the same point in time!" ) );
        }
    }
}

WDataSetTimeSeries::~WDataSetTimeSeries()
{
}

std::string const WDataSetTimeSeries::getName() const
{
    return std::string( "WDataSetTimeSeries" );
}

std::string const WDataSetTimeSeries::getDescription() const
{
    return std::string( "A time series." );
}

boost::shared_ptr< WPrototyped > WDataSetTimeSeries::getPrototype()
{
    if( !m_prototype )
    {
        m_prototype = boost::shared_ptr< WPrototyped >( new WDataSetTimeSeries() );
    }

    return m_prototype;
}

bool WDataSetTimeSeries::isTimeSlice( float time ) const
{
    std::vector< TimeSlice >::const_iterator f = std::lower_bound( m_dataSets.begin(), m_dataSets.end(), time, TimeSliceCompare() );
    return f != m_dataSets.end() && f->second == time;
}

float WDataSetTimeSeries::findNearestTimeSlice( float time ) const
{
    WAssert( !wlimits::isnan( time ), "" );
    if( time > getMaxTime() )
    {
        return getMaxTime();
    }
    float lb = getLBTimeSlice( time );
    float ub = getUBTimeSlice( time );
    return time - lb <= ub - time ? lb : ub;
}

boost::shared_ptr< WDataSetScalar const > WDataSetTimeSeries::getDataSetPtrAtTimeSlice( float time ) const
{
    std::vector< TimeSlice >::const_iterator f = std::lower_bound( m_dataSets.begin(), m_dataSets.end(), time, TimeSliceCompare() );
    if( f != m_dataSets.end() && f->second == time )
    {
        return f->first;
    }
    return boost::shared_ptr< WDataSetScalar const >();
}

boost::shared_ptr< WDataSetScalar const > WDataSetTimeSeries::calcDataSetAtTime( float time, std::string const& name ) const
{
    WAssert( !wlimits::isnan( time ), "" );
    if( time < getMinTime() || time > getMaxTime() )
    {
        return boost::shared_ptr< WDataSetScalar const >();
    }
    float lb = getLBTimeSlice( time );
    float ub = getUBTimeSlice( time );
    if( lb == time || ub == time )
    {
        // not optimal if the compiler does not inline
        return getDataSetPtrAtTimeSlice( time );
    }

    boost::shared_ptr< WValueSetBase > vs;
    switch( m_dataSets.front().first->getValueSet()->getDataType() )
    {
    case W_DT_UINT8:
        vs = calcInterpolatedValueSet< uint8_t >( lb, ub, time );
        break;
    case W_DT_INT8:
        vs = calcInterpolatedValueSet< int8_t >( lb, ub, time );
        break;
    case W_DT_UINT16:
        vs = calcInterpolatedValueSet< uint16_t >( lb, ub, time );
        break;
    case W_DT_INT16:
        vs = calcInterpolatedValueSet< int16_t >( lb, ub, time );
        break;
    case W_DT_UINT32:
        vs = calcInterpolatedValueSet< uint32_t >( lb, ub, time );
        break;
    case W_DT_SIGNED_INT:
        vs = calcInterpolatedValueSet< int32_t >( lb, ub, time );
        break;
    case W_DT_UINT64:
        vs = calcInterpolatedValueSet< uint64_t >( lb, ub, time );
        break;
    case W_DT_INT64:
        vs = calcInterpolatedValueSet< int64_t >( lb, ub, time );
        break;
    case W_DT_FLOAT:
        vs = calcInterpolatedValueSet< float >( lb, ub, time );
        break;
    case W_DT_DOUBLE:
        vs = calcInterpolatedValueSet< double >( lb, ub, time );
        break;
    default:
        throw WException( std::string( "Unsupported datatype in WDataSetTimeSeries::calcDataSetAtTime()" ) );
        break;
    }
    boost::shared_ptr< WDataSetScalar > ds( new WDataSetScalar( vs, m_dataSets.front().first->getGrid() ) );
    ds->setFilename( name );
    return ds;
}

bool WDataSetTimeSeries::TimeSliceCompare::operator() ( TimeSlice const& t0, TimeSlice const& t1 )
{
    return t0.second < t1.second;
}

bool WDataSetTimeSeries::TimeSliceCompare::operator() ( float const& t0, TimeSlice const& t1 )
{
    return t0 < t1.second;
}

bool WDataSetTimeSeries::TimeSliceCompare::operator() ( TimeSlice const& t0, float const& t1 )
{
    return t0.second < t1;
}

float WDataSetTimeSeries::getLBTimeSlice( float time ) const
{
    std::vector< TimeSlice >::const_iterator f = std::lower_bound( m_dataSets.begin(), m_dataSets.end(), time, TimeSliceCompare() );
    float t = -std::numeric_limits< float >::infinity();
    if( f != m_dataSets.end() && f->second == time )
    {
        return time;
    }
    if( f != m_dataSets.begin() )
    {
        --f;
        t = f->second;
    }
    return t;
}

float WDataSetTimeSeries::getUBTimeSlice( float time ) const
{
    std::vector< TimeSlice >::const_iterator g = std::upper_bound( m_dataSets.begin(), m_dataSets.end(), time, TimeSliceCompare() );
    float t = std::numeric_limits< float >::infinity();
    if( g != m_dataSets.end() )
    {
        t = g->second;
    }
    return t;
}

WDataSetTimeSeries::WDataSetTimeSeries()
    : m_dataSets()
{
}

double WDataSetTimeSeries::getMinValue()
{
    return m_minValue;
}

double WDataSetTimeSeries::getMaxValue()
{
    return m_maxValue;
}
