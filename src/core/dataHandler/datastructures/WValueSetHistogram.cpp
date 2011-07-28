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
#include <cstring> // memset()
#include <numeric>
#include <string>
#include <utility>

#include "../../common/WAssert.h"
#include "../../common/WLimits.h"
#include "../../common/exceptions/WOutOfBounds.h"
#include "../WDataHandlerEnums.h"

#include "WValueSetHistogram.h"

WValueSetHistogram::WValueSetHistogram( boost::shared_ptr< WValueSetBase > valueSet, size_t buckets ):
    WHistogram( valueSet->getMinimumValue(), valueSet->getMaximumValue(), buckets )
{
    buildHistogram( *valueSet );
}

WValueSetHistogram::WValueSetHistogram( const WValueSetBase& valueSet, size_t buckets ):
    WHistogram( valueSet.getMinimumValue(), valueSet.getMaximumValue(), buckets )
{
    buildHistogram( valueSet );
}

WValueSetHistogram::WValueSetHistogram( boost::shared_ptr< WValueSetBase > valueSet, double min, double max, size_t buckets ):
    WHistogram( min, max, buckets )
{
    buildHistogram( *valueSet );
}

WValueSetHistogram::WValueSetHistogram( const WValueSetBase& valueSet, double min, double max, size_t buckets ):
    WHistogram( min, max, buckets )
{
    buildHistogram( valueSet );
}

void WValueSetHistogram::buildHistogram( const WValueSetBase& valueSet )
{
    m_nbTotalElements = 0;

    // create base histogram
    WAssert( m_nbBuckets > 1, "WValueSetHistogram::buildHistogram : number of buckets needs to be larger than 1." );
    m_nInitialBuckets = m_nbBuckets - 1;
    m_initialBucketSize = ( m_maximum - m_minimum ) / static_cast< double >( m_nInitialBuckets );
    WAssert( m_initialBucketSize > 0.0, "WValueSetHistogram::buildHistogram() : m_initialBucketSize to small." );

    // NOTE: as all the intervals are right-open, we need an additional slot in our array for the last interval [m_maximum,\infinity). For the
    // calculation of interval sizes, the value must not be incremented
    m_nInitialBuckets++;

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
    for( size_t i = 0; i < valueSet.size(); ++i )
    {
        double tmp = valueSet.getScalarDouble( i );
        insert( tmp );
    }

    m_nbTotalElements = valueSet.size();
}

WValueSetHistogram::WValueSetHistogram( const WValueSetHistogram& histogram, size_t buckets ):
    WHistogram( histogram ),
    m_initialBucketSize( histogram.m_initialBucketSize ),
    m_initialBuckets( histogram.m_initialBuckets ),
    m_nInitialBuckets( histogram.m_nInitialBuckets ),
    m_mappedBuckets( histogram.m_mappedBuckets ),
    m_nMappedBuckets( histogram.m_nMappedBuckets ),
    m_mappedBucketSize( histogram.m_mappedBucketSize ),
    m_nbTotalElements( histogram.m_nbTotalElements )
{
    // apply modification of the histogram bucket size?
    if( ( buckets == 0 ) || ( buckets == m_nMappedBuckets ) )
    {
        return;
    }

    WAssert( buckets > 1, "WValueSetHistogram::WValueSetHistogram : number of buckets needs to be larger than 1." );
    WAssert( buckets < m_nInitialBuckets,
                          "WValueSetHistogram::WValueSetHistogram : number of buckets needs to be smaller than the initial bucket count." );

    // number of elements in the new mapped histogram = division + (round up)
    m_nMappedBuckets = buckets - 1;
    m_mappedBucketSize = ( m_maximum - m_minimum ) / static_cast< double >( m_nMappedBuckets );

    // NOTE: as all the intervals are right-open, we need an additional slot in our array for the last interval [m_maximum,\infinity). For the
    // calculation of interval sizes, the value must not be incremented
    m_nMappedBuckets++;

    size_t ratio = static_cast<size_t>( m_nInitialBuckets / m_nMappedBuckets );

    m_mappedBuckets.reset();

    size_t* mappedBuckets = new size_t[ m_nMappedBuckets ];
    memset( mappedBuckets, 0, m_nMappedBuckets * sizeof( size_t ) );
    // *mappedBuckets = { 0 }; // works with C++0x
    m_mappedBuckets = boost::shared_array< size_t >( mappedBuckets );

    // map it
    size_t index = 0;
    for( size_t i = 0; i < m_nInitialBuckets; ++i )
    {
        if( ( i % ratio == 0 ) && ( i != 0 ) && ( i != m_nInitialBuckets - 1 ) )
        {
            index++;
        }
        m_mappedBuckets[ index ] += m_initialBuckets[i];
    }
}

WValueSetHistogram& WValueSetHistogram::operator=( const WValueSetHistogram& other )
{
    if( this != &other ) // protect against invalid self-assignment
    {
        m_minimum = other.m_minimum;
        m_maximum = other.m_maximum;
        m_initialBucketSize = other.m_initialBucketSize;
        m_nInitialBuckets = other.m_nInitialBuckets;
        m_nMappedBuckets = other.m_nMappedBuckets;
        m_mappedBucketSize = other.m_mappedBucketSize;

        // copy the initial/mapped buckets reference, no deep copy here!
        m_initialBuckets = other.m_initialBuckets;
        m_mappedBuckets =  other.m_mappedBuckets;
    }

    return *this;
}

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

double WValueSetHistogram::getBucketSize( size_t /* index */ ) const
{
    // ignore index as each bucket has the same width
    return m_mappedBucketSize;
}

void WValueSetHistogram::insert( double value )
{
    m_mappedBuckets[ getIndexForValue( value ) ]++;
}

size_t WValueSetHistogram::operator[]( size_t index ) const
{
    // if no mapping has been applied, mappedBuckets points to the initial bucket
    return m_mappedBuckets[ index ];
}

size_t WValueSetHistogram::at( size_t index ) const
{
    WAssert( index < m_nMappedBuckets, "WValueSetHistogram::at() : index out of range." );

    // if no mapping has been applied, mappedBuckets points to the initial bucket
    return m_mappedBuckets[ index ];
}

size_t WValueSetHistogram::size() const
{
    return m_nMappedBuckets;    // overwrite the WHistogram::size here as we have our own size.
}

size_t WValueSetHistogram::getTotalElementCount() const
{
    return m_nbTotalElements;
}

std::pair< double, double > WValueSetHistogram::getIntervalForIndex( size_t index ) const
{
    double first = m_minimum + m_mappedBucketSize * index;
    double second =  m_minimum + m_mappedBucketSize * ( index + 1 );
    return std::make_pair( first, second );
}

size_t WValueSetHistogram::accumulate( size_t startIndex, size_t endIndex ) const
{
    if( startIndex > endIndex )
    {
        std::swap( startIndex, endIndex );
    }

    // valid index?
    if( endIndex > size() )    // as endIndex is exclusive, it is allowed to equal size()
    {
        throw WOutOfBounds( std::string( "The specified endIndex is out of bounds." ) );
    }

    // unfortunately, shared_array can't be used for std::accumulate
    size_t acc = 0;
    while( startIndex != endIndex )
    {
        acc += m_mappedBuckets[ startIndex++ ];
    }

    return acc;
}

std::ostream& operator<<( std::ostream& out, const WValueSetHistogram& h )
{
    for( size_t i = 0; i < h.size() - 1; ++i )
    {
        std::pair< double, double > interval = h.getIntervalForIndex( i );
        // NOTE: the notation for open intervals is [a,b) or alternatively [a,b[.
        //out << i << " = [" << interval.first << ", " << interval.second << ") = " << h[ i ] << std::endl;
        out << interval.first << " " <<  interval.second << " " << std::min( h[ i ], size_t( 3000 ) ) << std::endl;
    }
    // the last interval is handled special
    //out << h.size() - 1 << " = [" << h.getIntervalForIndex( h.size() - 1 ).first << ", inf) = " << h[ h.size() - 1 ] << std::endl;
    out << h.getIntervalForIndex( h.size() - 1 ).first << " inf " << std::min( h[ h.size() - 1 ], size_t( 3000 ) ) << std::endl;

    return out;
}

