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

#include <cstring> // memset()

#include "../../common/WAssert.h"
#include "../../common/WLimits.h"

#include "WValueSetHistogram.h"

WValueSetHistogram::WValueSetHistogram( boost::shared_ptr< WValueSetBase > valueSet )
{
    // calculate min max
    m_minimum = wlimits::MAX_DOUBLE;
    m_maximum = wlimits::MIN_DOUBLE;
    double minDistance = wlimits::MAX_DOUBLE;
    for( size_t i = 0; i != valueSet->size(); ++i )
    {
        double tmp = valueSet->getScalarDouble( i );
        m_maximum = m_maximum < tmp ? tmp : m_maximum;
        m_minimum = m_minimum > tmp ? tmp : m_minimum;

        if( m_minimum != tmp && m_minimum != wlimits::MAX_DOUBLE )
        {
            minDistance = tmp - m_minimum < minDistance ? tmp - m_minimum : minDistance;
        }
        if( m_maximum != tmp && m_maximum != wlimits::MIN_DOUBLE )
        {
            minDistance = m_maximum - tmp < minDistance ? m_maximum - tmp : minDistance;
        }
    }

    // create base histogram
    m_nInitialBuckets = ( ( m_maximum - m_minimum ) / minDistance ) + 1;
    m_bucketSize = minDistance;
    unsigned int* initialBuckets = new unsigned int[m_nInitialBuckets];
    // initialize array to zero
    memset( initialBuckets, 0, m_nInitialBuckets * sizeof( unsigned int ) );
    //*initialBuckets = { 0 }; // this should works with C++0x (instead memset), TEST IT!
    m_initialBuckets = boost::shared_array< unsigned int >( initialBuckets );

    m_nMappedBuckets = 0;

    for( size_t i = 0; i < valueSet->size(); ++i )
    {
        double tmp = valueSet->getScalarDouble( i );
        increment( tmp );
    }
}

WValueSetHistogram::WValueSetHistogram( const WValueSetBase& valueSet )
{
    // calculate min max
    m_minimum = wlimits::MAX_DOUBLE;
    m_maximum = wlimits::MIN_DOUBLE;
    double minDistance = wlimits::MAX_DOUBLE;
    for( size_t i = 0; i != valueSet.size(); ++i )
    {
        double tmp = valueSet.getScalarDouble( i );
        m_maximum = m_maximum < tmp ? tmp : m_maximum;
        m_minimum = m_minimum > tmp ? tmp : m_minimum;

        if( m_minimum != tmp && m_minimum != wlimits::MAX_DOUBLE )
        {
            minDistance = tmp - m_minimum < minDistance ? tmp - m_minimum : minDistance;
        }
        if( m_maximum != tmp && m_maximum != wlimits::MIN_DOUBLE )
        {
            minDistance = m_maximum - tmp < minDistance ? m_maximum - tmp : minDistance;
        }
    }

    // create base histogram
    m_nInitialBuckets = ( ( m_maximum - m_minimum ) / minDistance ) + 1;
    m_bucketSize = minDistance;
    unsigned int* initialBuckets = new unsigned int[m_nInitialBuckets];
    // initialize array to zero
    memset( initialBuckets, 0, m_nInitialBuckets * sizeof( unsigned int ) );
    //*initialBuckets = { 0 }; // this should works with C++0x (instead memset), TEST IT!
    m_initialBuckets = boost::shared_array< unsigned int >( initialBuckets );

    m_nMappedBuckets = 0;

    for( size_t i = 0; i < valueSet.size(); ++i )
    {
        double tmp = valueSet.getScalarDouble( i );
        increment( tmp );
    }
}

WValueSetHistogram::WValueSetHistogram( const WValueSetHistogram& histogram, double intervalSize )
{
    // copy constructor
    m_nInitialBuckets = histogram.getNInitialBuckets();
    m_initialBuckets = boost::shared_array< unsigned int >( histogram.getInitialBuckets() );
    m_bucketSize = histogram.getBucketSize();

    m_nMappedBuckets = 0;

    m_minimum = histogram.getMin();
    m_maximum = histogram.getMax();

    if( intervalSize != 0.0 )
    {
        WAssert( intervalSize > 0.0, "WValueSetHistogram::WValueSetHistogram : intervalSize has to be greater then zero." );
        calculateMapping( intervalSize );
    }
}

WValueSetHistogram::~WValueSetHistogram()
{
}

boost::shared_array< unsigned int > WValueSetHistogram::getInitialBuckets() const
{
    return m_initialBuckets;
}

unsigned int WValueSetHistogram::getNInitialBuckets() const
{
    return m_nInitialBuckets;
}

double WValueSetHistogram::getBucketSize() const
{
    return m_bucketSize;
}

void WValueSetHistogram::increment( double value )
{
    WAssert( m_bucketSize > 0.0, "WValueSetHistogram::increment() : m_bucketSize to small." );
    unsigned int index = static_cast<unsigned int>( value / m_bucketSize );
    m_initialBuckets[index]++;
}

unsigned int WValueSetHistogram::setInterval( double intervalSize )
{
    if( m_bucketSize == intervalSize )
    {
        if( m_mappedBuckets )
        {
            m_mappedBuckets.reset();
            m_nMappedBuckets = 0;
        }
    }
    else
    {
        calculateMapping( intervalSize );
    }
    return m_nMappedBuckets;
}

void WValueSetHistogram::calculateMapping( double intervalSize )
{
    unsigned int ratio = static_cast<unsigned int>( intervalSize / m_bucketSize );
    WAssert( ratio > 1, "WValueSetHistogram::calculateMapping() : intervalSize has to be greater then the original size." );
    // number of elements in the new mapped histogram = division + (round up)
    m_nMappedBuckets = m_nInitialBuckets / ratio + ( m_nInitialBuckets % ratio > 0 ? 1 : 0 );

    if( m_mappedBuckets )
    {
        m_mappedBuckets.reset();
    }

    unsigned int* mappedBuckets = new unsigned int[m_nMappedBuckets];
    memset( mappedBuckets, 0, m_nMappedBuckets * sizeof( unsigned int ) );
    //*mappedBuckets = { 0 }; // works with C++0x
    boost::scoped_array< unsigned int > scoped( mappedBuckets );
    m_mappedBuckets.swap( scoped );
    unsigned int index = 0;
    for( unsigned int i = 0; i != m_nInitialBuckets; ++i )
    {
        if( i % ratio == 0 && i != 0 )
        {
            index++;
        }
        m_mappedBuckets[index] += m_initialBuckets[i];
    }
}

unsigned int WValueSetHistogram::operator[]( unsigned int index )
{
    unsigned int value = 0;
    if( m_mappedBuckets )
    {
        index = m_mappedBuckets[index];
    }
    else
    {
        value = m_initialBuckets[index];
    }
    return value;
}

unsigned int WValueSetHistogram::at( unsigned int index )
{
    unsigned int value = 0;
    if( m_mappedBuckets )
    {
        WAssert( index < m_nMappedBuckets, "WValueSetHistogram::at() : index out of range." );
        value = m_mappedBuckets[index];
    }
    else
    {
        WAssert( index < m_nInitialBuckets, "WValueSetHistogram::at() : index out of range." );
        value = m_initialBuckets[index];
    }
    return value;
}

unsigned int WValueSetHistogram::size() const
{
    return (m_mappedBuckets ? m_nMappedBuckets : m_nInitialBuckets);
}

double WValueSetHistogram::getMin() const
{
    return m_minimum;
}

double WValueSetHistogram::getMax() const
{
    return m_maximum;
}

