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

#include <cmath> // test() -> log() ?
//#include <limits> // std::numeric_limits<double>
#include <cstring> // memset(), remove when using C++0x and {0} to initiate the arrays to zero
#include <iostream> // test() -> std::cout ?
//#include <map>
//#include <list>
//#include <utility>

#include "WAssert.h"
#include "WLimits.h"

#include "WHistogram.h"

WHistogram::WHistogram( boost::shared_ptr< WValueSetBase > valueSet, unsigned int nBuckets )
{
    m_nInitialBuckets = nBuckets;
    m_initialBuckets = new unsigned int[m_nInitialBuckets];
    m_mappedBuckets = 0;

    // initiate array to zero
    memset( m_initialBuckets, 0, m_nInitialBuckets * sizeof( unsigned int ) );
    //*m_initialBuckets = {0}; // this should works with C++0x (instead memset), TEST IT!

    // calculate min max
    m_minimum = wlimits::MAX_DOUBLE; //std::numeric_limits<double>::max(); // double min ?
    m_maximum = wlimits::MIN_DOUBLE; //std::numeric_limits<double>::min(); // double max ?
    for( size_t i = 0; i < valueSet->size(); ++i )
    {
        double tmp = valueSet->getScalarDouble( i );
        m_maximum = m_maximum < tmp ? tmp : m_maximum;
        m_minimum = m_minimum > tmp ? tmp : m_minimum;
    }

    // create base histogram
    m_bucketSize = ( m_maximum - m_minimum ) / static_cast<double>( m_nInitialBuckets ); // rounding ? static_cast needed ?
    for( size_t i = 0; i < valueSet->size(); ++i )
    {
        double tmp = valueSet->getScalarDouble( i );
        increment( tmp );
    }
}

WHistogram::WHistogram( const WHistogram& histogram )
{
    // copy constructor
    m_nInitialBuckets = histogram.getNInitialBuckets();
    m_initialBuckets = new unsigned int[m_nInitialBuckets];
    memcpy( m_initialBuckets, histogram.getInitialBuckets(), m_nInitialBuckets * sizeof( unsigned int ) );
    m_bucketSize = histogram.getBucketSize();

    m_mappedBuckets = 0;
    m_nMappedBuckets = 0;

    m_minimum = histogram.getMin();
    m_maximum = histogram.getMax();
}

WHistogram::~WHistogram()
{
    delete[] m_initialBuckets;
    delete[] m_mappedBuckets;
}

unsigned int* WHistogram::getInitialBuckets() const
{
    return m_initialBuckets;
}

unsigned int WHistogram::getNInitialBuckets() const
{
    return m_nInitialBuckets;
}

double WHistogram::getBucketSize() const
{
    return m_bucketSize;
}

void WHistogram::increment( double value )
{
    WAssert( m_bucketSize > 0.0, "bucket size to small." ); // ? test & output correct ?
    unsigned int index = static_cast<unsigned int>( value / m_bucketSize ); // round down correctly?
    ( *( m_initialBuckets + index ) )++;
}

unsigned int WHistogram::setInterval( double intervalSize )
{
    calculateMapping( intervalSize );
    return m_nMappedBuckets;
}

void WHistogram::calculateMapping( double intervalSize )
{
    unsigned int ratio = static_cast<unsigned int>( intervalSize / m_bucketSize );
    WAssert( ratio > 1, "the new interval size has to be greater than the original size." );
    // number of elements in the new mapped histogram = division + (round up)
    m_nMappedBuckets = m_nInitialBuckets / ratio + ( m_nInitialBuckets % ratio > 0 ? 1 : 0 );

    if( m_mappedBuckets )
    {
        delete[] m_mappedBuckets;
        m_mappedBuckets = 0;
    }

    m_mappedBuckets = new unsigned int[m_nMappedBuckets];
    memset( m_mappedBuckets, 0, m_nMappedBuckets * sizeof( unsigned int ) );
    //*m_mappedBuckets = {0}; // works with C++0x
    unsigned int index = 0;
    for( unsigned int i = 0; i != m_nInitialBuckets; ++i )
    {
        if( i % ratio == 0 && i != 0 )
        {
            index++;
        }
        *( m_mappedBuckets + index ) += *( m_initialBuckets + i );
    }
}

unsigned int WHistogram::operator[]( unsigned int position )
{
    unsigned int* ptr = 0;
    if( m_mappedBuckets )
    {
        ptr = m_mappedBuckets;
    }
    else
    {
        ptr = m_initialBuckets;
    }
    return *(ptr + position);
}

unsigned int WHistogram::at( unsigned int index )
{
    WAssert( index > m_nMappedBuckets, "WHistogram: index out of range." );
    unsigned int* ptr = 0;
    if( m_mappedBuckets )
    {
        ptr = m_mappedBuckets;
    }
    else
    {
        ptr = m_initialBuckets;
    }

    return *(m_mappedBuckets + index);
}

unsigned int WHistogram::size() const
{
    return (m_mappedBuckets ? m_nMappedBuckets : m_nInitialBuckets);
}

double WHistogram::getMin() const
{
    return m_minimum;
}

double WHistogram::getMax() const
{
    return m_maximum;
}

void WHistogram::test()
{
    unsigned int* ptr;
    unsigned int histSize;
    if( m_mappedBuckets )
    {
        ptr = m_mappedBuckets;
        histSize = m_nMappedBuckets;
    }
    else
    {
        ptr = m_initialBuckets;
        histSize = m_nInitialBuckets;
    }

    // unsigned int rangeStart= 0;
    // std::list< std:pair< WBucket*, unsigned int > >::iterator iter;
    // for(iter = mapping.begin(); iter != mapping.end(); iter++ )
    // {
    //     std::cout << iter->second << ": " << rangeStart << " - " << rangeStart+uniformInterval;
    //     rangeStart += uniformInterval;
    // }

    for( unsigned int y = 10; y != 0; --y )
    {
        std::cout << "|";
        unsigned int* h_ptr = ptr;
        for( unsigned int x = 0; x != histSize; ++x )
        {
            if( log( static_cast<double>( *h_ptr ) ) > static_cast<double>( (y-1)*2 ) )
            {
                std::cout << " #";
            }
            else
            {
                std::cout << "  ";
            }
            h_ptr++;
        }
        std::cout << " |\n";
    }
    std::cout << "-";
    for(unsigned int i = 0; i != histSize; ++i)
    {
        std::cout << "--";
    }
    std::cout << "--\n";
}

