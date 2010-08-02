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

#include <map>
#include <list>
#include <utility>

#include <boost/shared_ptr.hpp>
#include <boost/scoped_array.hpp>
#include <boost/shared_array.hpp>

#include "../WValueSet.h"

/**
 * Used to track (later: display) the occurrence frequencies of values in a value set.
 **/
class WHistogram
{
    private:
        /**
         * The smalest value in the ValueSet
         **/
        double m_minimum;

        /**
         * The biggest value in the ValueSet
         **/
        double m_maximum;

        /**
         * Size of one bucket in the initial histogram.
         **/
        double m_bucketSize;

        /**
         * Pointer to all initial buckets of the histogram.
         **/
        boost::shared_array< unsigned int > m_initialBuckets;

        /**
         * Number of buckets in the initial histogram.
         **/
        unsigned int m_nInitialBuckets;

        /**
         * Pointer to all the buckets in the mapped histogram.
         **/
        boost::scoped_array< unsigned int > m_mappedBuckets;

        /**
         * Tracks the number of a buckets in the mapped histogram.
         **/
        unsigned int m_nMappedBuckets;

        /**
         * To calculate the new buckets
         *
         * \param intervalSize the size of one bucket
         **/
        void calculateMapping( double intervalSize );

        /**
         * increment the value by one, contains the logic to find the element place in the array.
         * Should only be used in the constructor ie. while iterating over WValueSet.
         *
         * \param value value to increment
         **/
        void increment( double value );

    protected:
        /**
         * Return the initial buckets.
         *
         * \return m_initialBuckets
         **/
        boost::shared_array< unsigned int > getInitialBuckets() const;

        /**
         * Return the number of initial buckets.
         *
         * \return m_nInitialBuckets
         **/
        unsigned int getNInitialBuckets() const;

        /**
         * Return the size of one initial bucket.
         *
         * \return m_bucketSize
         **/
        double getBucketSize() const;

    public:
        /**
         * Constructor.
         *
         * \param valueSet source of the data for the histogram
         * \param nBuckets number of buckets this histogram should display
         **/
        explicit WHistogram( boost::shared_ptr< WValueSetBase > valueSet );
        explicit WHistogram( const WValueSetBase& valueSet );

        /**
         * Copy constructor. If another interval size is given setInterval() is called and a
         * the mapped histogram is calculated.
         *
         * \param histogram another WHisogram
         * \param intervalSize the size of one bucket in the mapped histogram
         **/
        explicit WHistogram( const WHistogram& histogram, double intervalSize = 0.0 );

        /**
         * Destructor.
         **/
        ~WHistogram();

        /**
         * Set the new intervall size.
         *
         * \param intervalSize size of the interval for each mapped bucket.
         *
         * \return size of the new (mapped) histogram.
         **/
        unsigned int setInterval( double intervalSize );

        /**
         * Get the size of the bucket.
         *
         * \param index which buckets size is to be returned, starts with 0 which is the bucket
         * containing the smalest values.
         *
         * \return elements in the bucket.
         **/
        unsigned int operator[]( unsigned int index );

        /**
         * Get the size of the bucket. Testing if the position is valid.
         *
         * \param index which buckets size is to be returned, starts with 0 which is the bar with
         * the smalest values
         *
         * \return elements in the bucket
         **/
        unsigned int at( unsigned int index );

        /**
         * Returns the number of bars in the histogram with the actual mapping.
         *
         * \return number of buckets
         **/
        unsigned int size() const;

        /**
         * Retruns the minimum value in the value set.
         *
         * \return minimum
         **/
        double getMin() const;

        /**
         * Retruns the maximum value in the value set.
         *
         * \return maximum
         **/
        double getMax() const;

        /**
         * to test if the histogram works
         **/
        void test();
};

#endif  // WHISTOGRAM_H

