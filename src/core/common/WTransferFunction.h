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

#include <vector>

#include "WColor.h"

#include "WExportCommon.h"

/**
 * A cass that stores a 1D transfer function which consits
 * of a linear interpolation of alpha and color values
 */
class WTransferFunction
{
private:

    /**
     * Prototype for a storage element
     */
    struct Entry
    {
        /**
         * Default constructor
         * \param iso the iso value
         */
        explicit Entry( double iso ) : iso( iso )
        {
        }

        /**
         * comparison by isovalue
         * \param rhs entry to compare t
         */
        bool operator <= ( const Entry &rhs ) const
        {
            return iso <= rhs.iso;
        }

        double iso; //< the isovalue
    };

    /**
     * Color entries are linearly interpolated colors along isovalues
     */
    struct ColorEntry : public Entry
    {
        /** default constructor
         * \param iso the isovalue
         * \param color the color at the isovalue
         */
        ColorEntry( double iso, WColor color ) : Entry( iso ), color( color )
        {
        }

        /**
         * comparison operator to check for changes
         * \param rhs ColorEntry to compare to
         */
        bool operator==( const ColorEntry& rhs ) const
        {
            return iso == rhs.iso && color == rhs.color;
        }

        WColor color; //< holds the current color at isovalue Entry::iso
    };

    /**
     * Alpha entries represent linearly interpolated transparency values
     * along the isovalue scale. Alpha is in the range [ 0...1 ] where
     * 1 signifies entirely opaque ( which makes it more an opacity value instead
     * of alpha, but I use the commonly used language here )
     */
    struct AlphaEntry : public Entry
    {
        /** default constructor
         * \param iso the isovalue
         * \param alpha the alpha at the isovalue in the range from 0 = transparent to 1 = opaque
         */
         AlphaEntry( double iso, double alpha ) : Entry( iso ), alpha( alpha )
        {
        }

        /**
         * comparison operator to check for changes
         * \param rhs AlphaEntry to compare to
         */
        bool operator==( const AlphaEntry& rhs ) const
        {
            return iso == rhs.iso && alpha == rhs.alpha;
        }

        double alpha; //< holds the current alpha at isovalue Entry::iso
    };

    /**
     * Templatized comparison predicate for internal searching
     */
    template<typename T>
        struct LessPred
        {
            explicit LessPred( double iso ) : iso( iso )
            {
            }

            /** isovalue-based comparison */
            bool operator()( const T& t )
            {
                return iso < t.iso;
            }

            double iso; //< the isovalue to compare to
        };

public:
    /** default constructor of a meaningless transfer function */
    WTransferFunction() : isomin( 0. ), isomax( 0. )
    {
    }

    /** deep copy constructor */
    WTransferFunction( const WTransferFunction &rhs )
        : colors( rhs.colors ), alphas( rhs.alphas ), isomin( rhs.isomin ), isomax( rhs.isomax ), histogram( rhs.histogram )
    {
    }

    /** deep copy operator */
    WTransferFunction& operator=( const WTransferFunction &rhs )
    {
        this->colors = rhs.colors;
        this->alphas = rhs.alphas;
        this->isomin = rhs.isomin;
        this->isomax = rhs.isomax;
        this->histogram = rhs.histogram;
        return ( *this );
    }

    /**
     * \returns true if this object contains exactly the same
     * data as rhs
     * \param rhs object to compare with
     */
    bool operator==( const WTransferFunction &rhs ) const;

    /**
     * \returns negated result of operator==
     */
    bool operator!=( const WTransferFunction &rhs ) const;

    /** default destuctor */
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

    /**
     * Try to estimate a transfer function from an RGBA image.
     * \param rgba: values between 0 and 255 representing the red, green, and blue channel
     * \param size: number of color entries in rgba
     * \returns approximated transfer function
     */
    static WTransferFunction createFromRGBA( unsigned char const * const rgba, size_t size );
private:

    std::vector<ColorEntry> colors; //< sorted list of colors
    std::vector<AlphaEntry> alphas; //< sorted list of alpha values

    double isomin; //< smallest iso value
    double isomax; //< largest iso value

    std::vector< double > histogram; //< store a histogram. This is used for property-handling only
                                     // and does not change the transfer function at all.

    friend std::ostream& operator << ( std::ostream& out, const WTransferFunction& tf );
};

/**
 * Default output operator. Currently stores values the same way as it is done in the properties.
 * This code should only be used for debugging and you should not realy on the interface.
 * \param tf The transfer function to output
 * \param out The stream to which we write
 * \returns reference to \param out
 */
std::ostream& operator << ( std::ostream& out, const WTransferFunction& tf );

#endif  // WTRANSFERFUNCTION_H
