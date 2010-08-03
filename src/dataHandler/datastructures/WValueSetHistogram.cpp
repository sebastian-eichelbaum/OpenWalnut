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

WValueSetHistogram::WValueSetHistogram( boost::shared_ptr< WValueSetBase > valueSet ):
    m_minimum( valueSet->getMinimumValue() ),
    m_maximum( valueSet->getMaximumValue() )
{
    // create base histogram
    m_nInitialBuckets = 100;
    m_initialBucketSize = ( m_maximum - m_minimum ) / static_cast< double >( m_nInitialBuckets );
    WAssert( m_initialBucketSize > 0.0, "WValueSetHistogram::WValueSetHistogram() : m_initialBucketSize to small." );

    // create and initialize array to zero which finally contains the counts
    size_t* initialBuckets = new size_t[ m_nInitialBuckets ];
    memset( initialBuckets, 0, m_nInitialBuckets * sizeof( size_t ) );
    // *initialBuckets = { 0 }; // this should works with C++0x (instead memset), TEST IT!

    // the array can be shared among several instances of WValueSetHistogram.
    m_initialBuckets = boost::shared_array< size_t >( initialBuckets );

    // no mapping applied yet. Initial and mapped are equal
    m_nMappedBuckets = m_nInitialBuckets;
    m_mappedBuckets = m_initialBuckets;
    m_mappedBucketSize = m_initialBucketSize;

    // and finally create the histogram
    for( size_t i = 0; i < valueSet->size(); ++i )
    {
        double tmp = valueSet->getScalarDouble( i );
        insert( tmp );
    }
}
/*
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
    size_t* initialBuckets = new size_t[m_nInitialBuckets];
    // initialize array to zero
    memset( initialBuckets, 0, m_nInitialBuckets * sizeof( size_t ) );
    //*initialBuckets = { 0 }; // this should works with C++0x (instead memset), TEST IT!
    m_initialBuckets = boost::shared_array< size_t >( initialBuckets );

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
    m_initialBuckets = boost::shared_array< size_t >( histogram.getInitialBuckets() );
    m_bucketSize = histogram.getBucketSize();

    m_nMappedBuckets = 0;

    m_minimum = histogram.getMin();
    m_maximum = histogram.getMax();

    if( intervalSize != 0.0 )
    {
        WAssert( intervalSize > 0.0, "WValueSetHistogram::WValueSetHistogram : intervalSize has to be greater then zero." );
        calculateMapping( intervalSize );
    }
}*/

WValueSetHistogram::~WValueSetHistogram()
{
}

boost::shared_array< size_t > WValueSetHistogram::getInitialBuckets() const
{
    return m_initialBuckets;
}

size_t WValueSetHistogram::getNInitialBuckets() const
{
    return m_nInitialBuckets;
}

double WValueSetHistogram::getInitialBucketSize() const
{
    return m_initialBucketSize;
}

double WValueSetHistogram::getBucketSize() const
{
    return m_mappedBucketSize;
}

void WValueSetHistogram::insert( double value )
{
    size_t index = static_cast< size_t >( ( value - m_minimum ) / static_cast< double >( m_mappedBucketSize ) );
    m_mappedBuckets[ index ]++;
}

void WValueSetHistogram::calculateMapping( double intervalSize )
{
    /*
    size_t ratio = static_cast<size_t>( intervalSize / m_bucketSize );
    WAssert( ratio > 1, "WValueSetHistogram::calculateMapping() : intervalSize has to be greater then the original size." );
    // number of elements in the new mapped histogram = division + (round up)
    m_nMappedBuckets = m_nInitialBuckets / ratio + ( m_nInitialBuckets % ratio > 0 ? 1 : 0 );

    if( m_mappedBuckets )
    {
        m_mappedBuckets.reset();
    }

    size_t* mappedBuckets = new size_t[m_nMappedBuckets];
    memset( mappedBuckets, 0, m_nMappedBuckets * sizeof( size_t ) );
    // *mappedBuckets = { 0 }; // works with C++0x
    boost::scoped_array< size_t > scoped( mappedBuckets );
    m_mappedBuckets.swap( scoped );
    size_t index = 0;
    for( size_t i = 0; i != m_nInitialBuckets; ++i )
    {
        if( i % ratio == 0 && i != 0 )
        {
            index++;
        }
        m_mappedBuckets[index] += m_initialBuckets[i];
    }*/
}

size_t WValueSetHistogram::operator[]( size_t index )
{
    // if no mapping has been applied, mappedBuckets points to the initial bucket
    return m_mappedBuckets[ index ];
}

size_t WValueSetHistogram::at( size_t index )
{
    WAssert( index < m_nMappedBuckets, "WValueSetHistogram::at() : index out of range." );

    // if no mapping has been applied, mappedBuckets points to the initial bucket
    return m_mappedBuckets[ index ];
}

size_t WValueSetHistogram::size() const
{
    return m_nMappedBuckets;
}

double WValueSetHistogram::getMinimum() const
{
    return m_minimum;
}

double WValueSetHistogram::getMaximum() const
{
    return m_maximum;
}

