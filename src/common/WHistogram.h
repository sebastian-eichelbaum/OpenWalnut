//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2010 OpenWalnut Community, BSV@Uni-Leipzig and CNCF@MPI-CBS
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

class WHistogram
{
    /**
     * To compare the elements of the std::map used in WHistogram.
     **/
    class cmp
    {
        public:
            bool operator()( double const& p1, double const& p2) const
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
            unsigned int value;
            //unsigned int distance;
            //std::pair<int, int> range;

        public:
            /**
             * Creates empty bar.
             **/
            WBar() : value(0)
            {
            }

            /**
             * Creates bar.
             **/
            WBar(WBar const& other) : value(other.value)
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
            WBar& operator++(int) // x++
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
             * Returns the value of the bar.
             **/
            unsigned int getValue() const
            {
                return value;
            }
    };

    private:
        unsigned int uniformInterval;
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
         * Creates empty histogram.
         **/
        WHistogram();

        /**
         * Create a histogram with a given uniform intervall.
         **/
        WHistogram( unsigned int );

        /**
         * Destroys this Histogram instance.
         **/
        ~WHistogram();

        /**
         * Add an element to one of the WBar.
         **/
        void add( double );

        /**
         * Set uniform interval.
         **/
        void setUniformInterval( unsigned int );

        /**
         * Seting Intervals that are not uniform.
         **/
        void setInterval( const std::list<unsigned int>& );

        /**
         * Get the height of a bar.
         **/
        unsigned int operator[]( unsigned int );// const;

        // To test the functionality
        void test();
};

#endif // WHISTOGRAM_H

