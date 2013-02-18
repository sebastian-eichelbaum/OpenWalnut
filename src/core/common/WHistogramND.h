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

#ifndef WHISTOGRAMND_H
#define WHISTOGRAMND_H

#include <utility>

#include <boost/array.hpp>

/**
 * This template should handly arbitrary N-dimensional histograms.
 *
 * \tparam N specifies the dimensionality
 * \tparam T specifies the type of data. Normally this should be double or float.
 */
template< std::size_t N, typename T = double >
/**
 * Interface for an N-dimensional histogram.
 */
class WHistogramND // NOLINT
{
public:
    /**
     * Shorthand for N-dimensional indices, counter, etc.
     */
    typedef boost::array< size_t, N > SizeArray;

    /**
     * Shorthand for N-dimensional values of type T.
     */
    typedef boost::array< T, N > TArray; // e.g. DoubleArray for T == double

    /**
     * Default constructor. Creates an empty N-dimensional histogram covering the specified min and max values with the specified number of buckets.
     *
     * \param min the smallest value(s) in each dimension
     * \param max the largest value(s) in each dimension
     * \param buckets the number of buckets in each direction (may be non uniform).
     */
    WHistogramND( TArray min, TArray max, SizeArray buckets );

    /**
     * Copy constructor. Creates a deep copy of the specified ND histogram.
     *
     * \param hist the HistogramND to copy.
     */
    WHistogramND( const WHistogramND& hist );

    /**
     * Default destructor.
     */
    virtual ~WHistogramND();

    /**
     * Get the count of the specified bucket.
     *
     * \param index in each dimension
     *
     * \return elements in the bucket.
     */
    virtual size_t operator()( SizeArray index ) const = 0;

    /**
     * Returns the number of buckets in the HistogramND with the actual mapping.
     *
     * \return number of buckets
     */
    virtual size_t size() const;

    /**
     * Returns the minimum value(s).
     *
     * \return minimum
     */
    virtual TArray getMinima() const;

    /**
     * Returns the maximum value(s).
     *
     * \return maximum
     */
    virtual TArray getMaxima() const;

    /**
     * Return the measure of one specific bucket. For one dimensional Histograms this is the width of the bucket, for two
     * dimensions this is the area, for three dims this is the volume, etc.
     *
     * \param index the measure for this bucket is queried.
     *
     * \return the size of a bucket.
     */
    virtual T getBucketSize( SizeArray index ) const = 0;

    /**
     * Returns the actual (right-open) interval in each dimension associated with the given index.
     *
     * \param index for this bucket the intervals will be returned
     *
     * \return the right-open interval in each dimension.
     */
    virtual boost::array< std::pair< T, T >, N > getIntervalForIndex( SizeArray index ) const = 0;

protected:
    /**
     * Default constructor is protected to allow subclassing constructors not to use initialization lists which would
     * imply C++11 or GNU++11 style initializers for boost::array members. To workaround, reset() member function is
     * provided.
     */
    WHistogramND();

    /**
     * Initializes all members. This exists because to circumvent boost::array initializers in c'tor init lists and
     * reduces code duplication, as it is used in this abstract class as well as in its base classes.
     *
     * \param min Minimal values in each dimension.
     * \param max Maximal values in each dimension.
     * \param buckets \#buckets in each dimension.
     */
    void reset( TArray min, TArray max, SizeArray buckets );

    /**
     * The smallest value in each dimension.
     */
    TArray m_min;

    /**
     * The biggest value in each dimension.
     */
    TArray m_max;

    /**
     * The number of buckets.
     */
    SizeArray m_buckets;

    /**
     * Total number of buckets.
     */
    size_t m_nbBuckets;

private:
};

template< std::size_t N, typename T >
WHistogramND< N, T >::WHistogramND()
{
}

template< std::size_t N, typename T >
WHistogramND< N, T >::WHistogramND( TArray min, TArray max, SizeArray buckets )
{
    reset( min, max, buckets );
}

template< std::size_t N, typename T >
void WHistogramND< N, T >::reset( TArray min, TArray max, SizeArray buckets )
{
    m_min = min;
    m_max = max;

    WAssert( min.size() == max.size(), "Error, WHistogram initialized with wrong dimensionality" );
    for( size_t i = 0; i < min.size(); ++i )
    {
        WAssert( min[i] <= max[i], "Error, WHistogram has at least one dimension where max is smaller than min" );
    }

    m_buckets = buckets;
    m_nbBuckets = 1;
    for( typename SizeArray::const_iterator cit = buckets.begin(); cit != buckets.end(); ++cit )
    {
        m_nbBuckets *= *cit;
    }
}

template< std::size_t N, typename T >
WHistogramND< N, T >::~WHistogramND()
{
}

template< std::size_t N, typename T >
WHistogramND< N, T >::WHistogramND( const WHistogramND& other )
{
    m_min = other.m_min;
    m_max = other.m_max;
    m_buckets = other.m_buckets;
    m_nbBuckets = other.m_nbBuckets;
}

template< std::size_t N, typename T >
size_t WHistogramND< N, T >::size() const
{
    return m_nbBuckets;
}

template< std::size_t N, typename T >
typename WHistogramND< N, T >::TArray WHistogramND< N, T >::getMinima() const
{
    return m_min;
}

template< std::size_t N, typename T >
typename WHistogramND< N, T >::TArray WHistogramND< N, T >::getMaxima() const
{
    return m_max;
}

#endif  // WHISTOGRAMND_H
