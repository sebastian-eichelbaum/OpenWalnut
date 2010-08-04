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

#ifndef WVALUESETHISTOGRAM_H
#define WVALUESETHISTOGRAM_H

#include <map>
#include <list>
#include <utility>

#include <boost/shared_ptr.hpp>
#include <boost/scoped_array.hpp>
#include <boost/shared_array.hpp>

#include "../../common/WHistogram.h"
#include "../WValueSet.h"

/**
 * Used to find the occurrence frequencies of values in a value set. It implements a classical histogram but allows easy modification of bucket
 * sizes without unnecessary recalculation of the whole histogram. This histogram uses right-open intervals for counting, which is why there
 * always is a bucket at the end from max to infinity which holds all the max values.
 *
 * \note This histogram is different from from WValueSetHistogram which is a generic histogram class.
 */
class WValueSetHistogram: public WHistogram
{
friend class WValueSetHistogramTest;
public:
    /**
     * Constructor. Creates the histogram for the specified value set.
     *
     * \param valueSet source of the data for the histogram
     * \param buckets the number of buckets to use. If not specified, 1000 is used as default. Must be larger than 1.
     */
    explicit WValueSetHistogram( boost::shared_ptr< WValueSetBase > valueSet, size_t buckets = 1000 );

    /**
     * Constructor. Creates the histogram for the specified value set.
     *
     * \param valueSet source of the data for the histogram
     * \param buckets the number of buckets to use. If not specified, 1000 is used as default. Must be larger than 1.
     */
    explicit WValueSetHistogram( const WValueSetBase& valueSet, size_t buckets = 1000 );

    /**
     * Copy constructor. If another interval size is given the histogram gets matched to it using the initial bucket data.
     * \note this does not deep copy the m_initialBuckets and m_mappedBuckets array as these are shared_array instances.
     *
     * \param histogram another WValueSetHistogram
     * \param buckets the new number of buckets. Must be larger than 1 if specified.
     */
    WValueSetHistogram( const WValueSetHistogram& histogram, size_t buckets = 0 );

    /**
     * Destructor.
     */
    ~WValueSetHistogram();

    /**
     * Copy assignment. Copies the contents of the specified histogram to this instance.
     *
     * \param other the other instance
     *
     * \return this instance with the contents of the other one.
     * \note this does not deep copy the m_initialBuckets and m_mappedBuckets array as these are shared_array instances.
     */
    WValueSetHistogram& operator=( const WValueSetHistogram& other );

    /**
     * Get the count of the bucket.
     *
     * \param index which buckets count is to be returned; starts with 0 which is the bucket containing the smallest values.
     *
     * \return elements in the bucket.
     */
    virtual size_t operator[]( size_t index ) const;

    /**
     * Get the count of the bucket. Testing if the position is valid.
     *
     * \param index which buckets count is to be returned; starts with 0 which is the bucket containing the smallest values.
     *
     * \return elements in the bucket
     */
    virtual size_t at( size_t index ) const;

    /**
     * Returns the number of buckets in the histogram with the actual mapping.
     *
     * \return number of buckets
     */
    virtual size_t size() const;

    /**
     * Return the size of one bucket.
     *
     * \param index the width for this bucket is queried.
     *
     * \return the size of a bucket.
     */
    virtual double getBucketSize( size_t index = 0 ) const;

    /**
     * Returns the actual interval associated with the given index. The interval is open, meaning that
     * getIntervalForIndex( i ).second == getIntervalForIndex( i + 1 ).first but does not belong anymore to the interval itself but every value
     * smaller than getIntervalForIndex( i ).second.
     *
     * \param index the intex
     *
     * \return the open interval.
     */
    virtual std::pair< double, double > getIntervalForIndex( size_t index ) const;

protected:
    /**
     * Return the initial buckets.
     *
     * \return m_initialBuckets
     */
    boost::shared_array< size_t > getInitialBuckets() const;

    /**
     * Return the number of initial buckets.
     *
     * \return m_nInitialBuckets
     */
    size_t getNInitialBuckets() const;

    /**
     * Return the size of one initial bucket.
     *
     * \return m_bucketSize
     */
    double getInitialBucketSize() const;

    /**
     * increment the value by one, contains the logic to find the element place in the array.
     * Should only be used in the constructor i.e. while iterating over WValueSet.
     *
     * \param value value to increment
     */
    virtual void insert( double value );

private:

    /**
     * Size of one bucket in the initial histogram.
     */
    double m_initialBucketSize;

    /**
     * Pointer to all initial buckets of the histogram.
     */
    boost::shared_array< size_t > m_initialBuckets;

    /**
     * Number of buckets in the initial histogram.
     */
    size_t m_nInitialBuckets;

    /**
     * Pointer to all initial buckets of the histogram.
     */
    boost::shared_array< size_t > m_mappedBuckets;

    /**
     * Tracks the number of a buckets in the mapped histogram.
     */
    size_t m_nMappedBuckets;

    /**
     * Size of one bucket in the mapped histogram.
     */
    double m_mappedBucketSize;
};

/**
 * Write a histogram in string representation to the given output stream.
 */
std::ostream& operator<<( std::ostream& out, const WValueSetHistogram& h );

#endif  // WVALUESETHISTOGRAM_H

