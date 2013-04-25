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

#ifndef WHISTOGRAM2D_H
#define WHISTOGRAM2D_H

#include <utility>

#include <boost/array.hpp>

#include <Eigen/Core>

#include <core/graphicsEngine/WGETexture.h>

#include "WHistogramND.h"

/**
 * Uniform two dimensional histogram for double values. The terms bin and bucket are interchangeable. For the first dimensional part often the
 * analouge X-dimension is used and for the second, Y-dimension.
 */
class WHistogram2D : public WHistogramND< 2 > // NOLINT
{
public:

    /**
     * Convenience type for a shared_ptr on this type.
     */
    typedef boost::shared_ptr< WHistogram2D > SPtr;

    /**
     * Creates a two dimensional histogram field, bounded by the given limits, containing the demanded number of buckets in each dimension.
     *
     * \param minX Minimal bound for X-values.
     * \param maxX Maximal bound for X-values.
     * \param minY Minimal bound for Y-values.
     * \param maxY Maximal bound for Y-values.
     * \param bucketsX Number of buckets in X direction.
     * \param bucketsY Number of buckets in Y direction.
     */
    WHistogram2D( double minX, double maxX, double minY, double maxY, size_t bucketsX, size_t bucketsY );

    /**
     * Cleans up!
     */
    ~WHistogram2D();

    /**
     * Copy constructor, performing a deep copy.
     *
     * \param other The other instance to copy from.
     */
    WHistogram2D( const WHistogram2D& other );

    /**
     * Get the count of the specified bucket.
     *
     * \param index in each dimension
     *
     * \return elements in the bucket.
     */
    virtual size_t operator()( SizeArray index ) const;

    /**
     * Convenience function to easier access the buckets for 2D.
     *
     * \param i X-index
     * \param j Y-index
     *
     * \return elements in the bucket.
     */
    virtual size_t operator()( size_t i, size_t j ) const;

    /**
     * Return the measure of one specific bucket. For one dimensional Histograms this is the width of the bucket, for two
     * dimensions this is the area, for three dims this is the volume, etc.
     *
     * \param index the measure for this bucket is queried.
     *
     * \return the size of a bucket.
     */
    virtual double getBucketSize( SizeArray index ) const;

    /**
     * Returns the actual (right-open) interval in each dimension associated with the given index.
     *
     * \param index for this bucket the intervals will be returned
     *
     * \return the right-open interval in each dimension.
     */
    virtual boost::array< std::pair< double, double >, 2 > getIntervalForIndex( SizeArray index ) const;

    /**
     * Given a value the corresponding bucket is determined and incremented by one.
     *
     * \param values The value to count into specific bucket.
     */
    void insert( TArray values );

    /**
     * Shorthand to overloaded insert function where each dimension can be overhanded separately.
     * \see insert()
     * \param x value for the first dimension.
     * \param y value for the second dimension.
     */
    void insert( double x, double y );

    /**
     * Copy-convert this into a texture.
     *
     * \return \c osg::ref_ptr to the two-dimensional texture.
     */
    WGETexture2D::RPtr getTexture();

protected:
private:
    /**
     * Shorthand for data structure storing bucket information. In 2D this is a matrix.
     */
    typedef Eigen::Matrix< size_t, Eigen::Dynamic, Eigen::Dynamic > BinType;

    /**
     * Storing the bucket counts, how often a value occurs.
     */
    BinType m_bins;

    /**
     * For each dimension this stores the uniform interval width.
     */
    TArray m_intervalWidth;
};

#endif  // WHISTOGRAM2D_H
