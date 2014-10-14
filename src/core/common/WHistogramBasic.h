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

#ifndef WHISTOGRAMBASIC_H
#define WHISTOGRAMBASIC_H

#include <cstddef>  // for std::size_t
#include <utility>
#include <vector>

#include "WHistogram.h"


/**
 * Container which associate values with (uniform width) bins (aka intervals or buckets). This class implements a very simple and easy to use
 * generic histogram with uniform bucket sizes.
 */
class WHistogramBasic: public WHistogram
{
public:
    /**
     * Default constructor. Creates an empty histogram covering the specified min and max values with the specified number of buckets.
     *
     * \param min the smallest value
     * \param max the largest value
     * \param buckets the number of buckets
     */
    WHistogramBasic( double min, double max, std::size_t buckets = 1000 );

    /**
     * Copy constructor. Creates a deep copy of the specified histogram.
     *
     * \param hist the histogram to copy.
     */
    WHistogramBasic( const WHistogramBasic& hist );

    /**
     * Default destructor.
     */
    ~WHistogramBasic();

    /**
     * Get the count of the specified bucket.
     *
     * \param index which buckets count is to be returned; starts with 0 which is the bucket containing the smallest values.
     *
     * \return elements in the bucket.
     */
    virtual std::size_t operator[]( std::size_t index ) const;

    /**
     * Get the count of the specified bucket. Testing if the position is valid.
     *
     * \param index which buckets count is to be returned; starts with 0 which is the bucket containing the smallest values.
     *
     * \return elements in the bucket
     */
    virtual std::size_t at( std::size_t index ) const;

    /**
     * Return the size of one specific bucket.
     *
     * \param index the width for this bucket is queried.
     *
     * \return the size of a bucket.
     */
    virtual double getBucketSize( std::size_t index = 0 ) const;

    /**
     * Returns the actual interval associated with the given index. The interval is open, meaning that
     * getIntervalForIndex( i ).second == getIntervalForIndex( i + 1 ).first but does not belong anymore to the interval itself but every value
     * smaller than getIntervalForIndex( i ).second.
     *
     * \param index the intex
     *
     * \return the open interval.
     */
    virtual std::pair< double, double > getIntervalForIndex( std::size_t index ) const;

    /**
     * Computes the number of inserted values so far.
     *
     * \return Number of values so far.
     */
    std::size_t valuesSize() const;

    /**
     * Inserts a given value within the given range (min, max) into exactly one bin and increment its size.
     *
     * \param value Value to insert.
     */
    virtual void insert( double value );

protected:
private:
    /**
     * Bins to associate with the values. Each bin has the width of m_intervalWidth;
     */
    std::vector< std::size_t > m_bins;

    /**
     * The width of an interval is precomputed to save performance.
     */
    double m_intervalWidth;
};

#endif  // WHISTOGRAMBASIC_H
