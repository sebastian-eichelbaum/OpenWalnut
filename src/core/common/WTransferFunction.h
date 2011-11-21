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

#ifndef WTRANSFERFUNCTION_H
#define WTRANSFERFUNCTION_H

#include <iostream>
#include <vector>
#include <algorithm>

#include "WColor.h"
#include <osg/Vec4>
#include <osg/io_utils> // for the operator<< and operator>> for Vec4

#include "WExportCommon.h"

/**
 * A cass that stores a 1D transfer function which consits
 * of a linear interpolation of alpha and color values
 */
class WTransferFunction
{
    private:

        struct Entry
        {
            explicit Entry( double iso ) : iso( iso )
            {
            }

            bool operator <= ( const Entry &rhs ) const
            {
                return iso <= rhs.iso;
            }

            double iso;
        };

        //typedef Entry LessPred;
        struct ColorEntry : public Entry
    {
        ColorEntry( double iso, WColor color ) : Entry( iso ), color( color )
        {
        }

        bool operator==( const ColorEntry& rhs ) const
        {
            return iso == rhs.iso && color == rhs.color;
        }

        WColor color;
    };

        struct AlphaEntry : public Entry
    {
        AlphaEntry( double iso, double alpha ) : Entry( iso ), alpha( alpha )
        {
        }

        bool operator==( const AlphaEntry& rhs ) const
        {
            return iso == rhs.iso && alpha == rhs.alpha;
        }

        double alpha;
    };

        template<typename T>
            struct LessPred
            {
                explicit LessPred( double iso ) : iso( iso )
                {
                }

                bool operator()( const T& t )
                {
                    return iso < t.iso;
                }

                double iso;
            };

    public:
        WTransferFunction() : isomin( 0. ), isomax( 0. )
    {
    }

        WTransferFunction( const WTransferFunction &rhs )
            : colors( rhs.colors ), alphas( rhs.alphas ), isomin( rhs.isomin ), isomax( rhs.isomax ), histogram( rhs.histogram )
        {
        }

        WTransferFunction& operator=( const WTransferFunction &rhs )
        {
            this->colors = rhs.colors;
            this->alphas = rhs.alphas;
            this->isomin = rhs.isomin;
            this->isomax = rhs.isomax;
            this->histogram = rhs.histogram;
            return ( *this );
        }

        bool operator==( const WTransferFunction &rhs ) const;

        bool operator!=( const WTransferFunction &rhs ) const;

        ~WTransferFunction()
        {
        }

        size_t numAlphas() const
        {
            return alphas.size();
        }

        size_t numColors() const
        {
            return colors.size();
        }

        double getAlphaIsovalue( size_t i ) const
        {
            return alphas.at( i ).iso;
        }

        double getColorIsovalue( size_t i ) const
        {
            return colors.at( i ).iso;
        }

        double getAlpha( size_t i ) const
        {
            return alphas.at( i ).alpha;
        }

        const WColor& getColor( size_t i ) const
        {
            return colors.at( i ).color;
        }

        /**
         * insert a new color point
         */
        void addColor( double iso, const WColor& color );

        /**
         * insert a new alpha point
         */
        void addAlpha( double iso, double alpha );

        /**
         * set the histogram going along with the transfer function
         *
         * This should be changed in the future to be handled in a different
         * way. A good option would be to introduce an object encapsulating
         * a transfer function and histogram information.
         */
        void setHistogram( std::vector< double > & data )
        {
            histogram.swap( data );
        }

        /**
         * clears the histogram data so the gui won't show any
         */
        void removeHistogram()
        {
            histogram.clear();
        }

        const std::vector< double >& getHistogram() const
        {
            return histogram;
        }

        /**
         * sample/render the transfer function linearly between min and max in an RGBA texture.
         * \param width is the number of RGBA samples.
         * \param min the lowest value to be sampled
         * \param max the hightes value to be sampled
         * \post array contains the sampled data
         */
        void sample1DTransferFunction( unsigned char*array, int width, double min, double max ) const;
    private:

        std::vector<ColorEntry> colors;
        std::vector<AlphaEntry> alphas;

        double isomin;
        double isomax;

        std::vector< double > histogram;
};

#endif  // WTRANSFERFUNCTION_H
