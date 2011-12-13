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

#ifndef WHISTOGRAM_H
#define WHISTOGRAM_H

#include <utility>

#include "WExportCommon.h"

/**
 * Container which associate values with (uniform width) bins (aka intervals or buckets). This class implements the abstract interface and
 * therefore builds the base class for all histogram classes. The interface also allows programming histogram of different bucket sizes.
 */
class OWCOMMON_EXPORT WHistogram // NOLINT
{
public:
    /**
     * Default constructor. Creates an empty histogram covering the specified min and max values with the specified number of buckets.
     *
     * \param min the smallest value
     * \param max the largest value
     * \param buckets the number of buckets
     */
    WHistogram( double min, double max, size_t buckets = 1000 );

    /**
     * Copy constructor. Creates a deep copy of the specified histogram.
     *
     * \param hist the histogram to copy.
     */
    WHistogram( const WHistogram& hist );

    /**
     * Default destructor.
     */
    virtual ~WHistogram();

    /**
     * Get the count of the specified bucket.
     *
     * \param index which buckets count is to be returned; starts with 0 which is the bucket containing the smallest values.
     *
     * \return elements in the bucket.
     */
    virtual size_t operator[]( size_t index ) const = 0;

    /**
     * Get the count of the specified bucket. Testing if the position is valid.
     *
     * \param index which buckets count is to be returned; starts with 0 which is the bucket containing the smallest values.
     *
     * \return elements in the bucket
     */
    virtual size_t at( size_t index ) const = 0;

    /**
     * Returns the number of buckets in the histogram with the actual mapping.
     *
     * \return number of buckets
     */
    virtual size_t size() const;

    /**
     * Returns the minimum value.
     *
     * \return minimum
     */
    virtual double getMinimum() const;

    /**
     * Returns the maximum value.
     *
     * \return maximum
     */
    virtual double getMaximum() const;

    /**
     * Return the size of one specific bucket.
     *
     * \param index the width for this bucket is queried.
     *
     * \return the size of a bucket.
     */
    virtual double getBucketSize( size_t index = 0 ) const = 0;

    /**
     * Returns the actual interval associated with the given index. The interval is open, meaning that
     * getIntervalForIndex( i ).second == getIntervalForIndex( i + 1 ).first but does not belong anymore to the interval itself but every value
     * smaller than getIntervalForIndex( i ).second.
     *
     * \param index the intex
     *
     * \return the open interval.
     */
    virtual std::pair< double, double > getIntervalForIndex( size_t index ) const = 0;

protected:
    /**
     * The smallest value
     */
    double m_minimum;

    /**
     * The biggest value
     */
    double m_maximum;

    /**
     * The number of buckets.
     */
    double m_nbBuckets;

private:
};

#endif  // WHISTOGRAM_H
