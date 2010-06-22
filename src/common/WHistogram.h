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

/**
 * Used to track (later: display) the occurrence frequencies of values in a value set.
 **/
class WHistogram
{
    /**
     * To compare the elements of the std::map used in WHistogram.
     **/
    class cmp
    {
        public:
            /**
             * Compares two double values.
             *
             * \param p1 first value
             * \param p2 second value
             *
             * \return the same bolean value as applying the less-than operator (a<b)
             **/
            bool operator()( double const& p1, double const& p2 ) const
            {
                return p1 < p2;
            }
    };

    /**
     * WBar is an element of WHistogram, where the range of monitored values equals 1.
     * Meaning WBar represents the amount of each specific value in the value set.
     **/
    class WBar
    {
        private:
            /**
             * Represents this bars height.
             **/
            unsigned int value;
            //unsigned int distance;
            //std::pair<int, int> range;

        public:
            /**
             * Default constructor.
             **/
            WBar() : value( 0 )
            {
            }

            /**
             * Clones this WBar from another WBar.
             *
             * \param bar WBar which serves as the origin of the height.
             **/
            explicit WBar( WBar const& bar ) : value( bar.value )
            {
            }


            /**
             * Destroys this Bar instance.
             **/
            ~WBar()
            {
            }

            /**
             * Postfix inkrement (x++) the bar by one.
             **/
            WBar& operator++( int ) // x++
            {
                WBar* tmp = this;
                tmp->value++;
                return *tmp;
            }

            /**
             * Prefix inkrement (++x) the bar by one.
             **/
            WBar& operator++() // ++x
            {
                WBar* tmp = this;
                tmp->value++;
                return *tmp;
            }

            /**
             * To access the height of the specific bar.
             *
             * \return the value of the bar.
             **/
            unsigned int getValue() const
            {
                return value;
            }
    };

    private:
        /**
         * Describes the uniform size of a bucket in the mapped histogram.
         **/
        unsigned int uniformInterval;

        /**
         * Pointer used to speed up operator[]( unsigned int ) if the parameter increases by 1
         * each call (supposed standard behaviour).
         **/
        std::pair< std::list<std::pair< const WBar*, unsigned int > >::iterator, unsigned int > mappingPointer;

        /**
         * Orderd sequence of WBar, which only account for specific values(the
         * double value).
         **/
        std::map< double, WBar, cmp > elements;

        /**
         * The Mapping, to gain bar's with a range greater than 1, is stored as
         * std::pair<WBar, unsigned int>.
         * Where the unsigned int is the the height of the specific bar.
         * The *WBar is the initial point (related to the std::map<> elements)
         * of the next bar.
         **/
        std::list< std::pair< const WBar*, unsigned int > > mapping;

        /**
         * Creates the mapping from the given range.
         **/
        void calculateMapping(); // for uniformInterval only

    public:
        /**
         * Default constructor.
         **/
        WHistogram();

        /**
         * Constructs a histogram with a given uniform intervall.
         *
         * \param interval the interval size if uniform intervals are used
         **/
        explicit WHistogram( unsigned int interval );

        /**
         * Destroys this Histogram instance.
         **/
        ~WHistogram();

        /**
         * Add an element to one of the WBar.
         *
         * \param value the bar that represents value in the histogram is increased bye one
         **/
        void add( double value );

        /**
         * Set and change the uniform interval.
         * Each time a new mapping is created.
         *
         * \param interval the new bucket size
         **/
        void setUniformInterval( unsigned int interval );

        /**
         * Sets intervals that are not uniform.
         * A new mapping is created each time.
         *
         * \param rangeList list that represents the size of each bucket starting with the lowest
         * value
         **/
        void setInterval( const std::list<unsigned int>& rangeList );

        /**
         * Get the height of a bar.
         *
         * \param position which bars height is to be returned, starts with 0 which is the bar with
         * the smalest values
         *
         * \return height of the bar i.e. size of the bucket
         **/
        unsigned int operator[]( unsigned int position ); // const;

        /**
         * Simple output to std::cout to test functionality.
         **/
        void test();
};

#endif  // WHISTOGRAM_H

