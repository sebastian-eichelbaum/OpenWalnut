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

#include <iosfwd>
#include <vector>

#include "WColor.h"



/**
 * A cass that stores a 1D transfer function which consits
 * of a linear interpolation of alpha and color values
 */
class WTransferFunction // NOLINT problem with macro
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
         * \returns true if this->iso <=  rhs.iso
         */
        bool operator <= ( const Entry &rhs ) const
        {
            return iso <= rhs.iso;
        }

        /** the isovalue */
        double iso;
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
         * \returns true if rhs equals this
         */
        bool operator==( const ColorEntry& rhs ) const
        {
            return iso == rhs.iso && color == rhs.color;
        }

        /** holds the current color at isovalue Entry::iso */
        WColor color;
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
         * \returns true if rhs is equal to this
         */
        bool operator==( const AlphaEntry& rhs ) const
        {
            return iso == rhs.iso && alpha == rhs.alpha;
        }

        /** holds the current alpha at isovalue Entry::iso */
        double alpha;
    };

    /**
     * Templatized comparison predicate for internal searching
     */
    template<typename T>
        struct LessPred
        {
            /** constructs a predicate that compares for less than iso
             * \param iso: used iso value
             */
            explicit LessPred( double iso ) : iso( iso )
            {
            }

            /** isovalue-based comparison
             * \param t the object to compare to
             * \returns true if iso is less than t.iso
             */
            bool operator()( const T& t )
            {
                return iso < t.iso;
            }

            /** the isovalue to compare to */
            double iso;
        };

public:
    /** default constructor of a meaningless transfer function */
    WTransferFunction() : isomin( 0. ), isomax( 0. )
    {
    }

    /** deep copy constructor
     * \param rhs the value to histogram
     */
    WTransferFunction( const WTransferFunction &rhs )
        : colors( rhs.colors ), alphas( rhs.alphas ), isomin( rhs.isomin ), isomax( rhs.isomax ), histogram( rhs.histogram )
    {
    }

    /** deep copy operator
     * \param rhs the value to copy
     * \returns reference to current object
     * \returns reference to current object
     */
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
     * \param rhs the value to compare with
     */
    bool operator!=( const WTransferFunction &rhs ) const;

    /** default destuctor */
    ~WTransferFunction()
    {
    }

    /**
     * \returns the number of alpha points
     */
    size_t numAlphas() const
    {
        return alphas.size();
    }

    /**
     * \returns the number of color points
     */
    size_t numColors() const
    {
        return colors.size();
    }

    /**
     * \param i the index of the point to query
     * \returns the alpha values position/isovalue at index i
     */
    double getAlphaIsovalue( size_t i ) const
    {
        return alphas.at( i ).iso;
    }


    /**
     * \param i the index of the point to query
     * \returns the color values position/isovalue at index i
     */
    double getColorIsovalue( size_t i ) const
    {
        return colors.at( i ).iso;
    }

    /**
     * \param i the index to query
     * \returns the alpha value at index i
     */
    double getAlpha( size_t i ) const
    {
        return alphas.at( i ).alpha;
    }

    /**
     * \param i the index to query
     * \returns the color at index i
     */
    const WColor& getColor( size_t i ) const
    {
        return colors.at( i ).color;
    }

    /**
     * insert a new color point
     * \param iso the new iso value
     * \param color the new color at the given iso value
     */
    void addColor( double iso, const WColor& color );

    /**
     * insert a new alpha point
     * \param iso the new iso value
     * \param alpha the new alpha value at the given iso value
     */
    void addAlpha( double iso, double alpha );

    /**
     * set the histogram going along with the transfer function
     *
     * This should be changed in the future to be handled in a different
     * way. A good option would be to introduce an object encapsulating
     * a transfer function and histogram information.
     *
     * \param data the histogram data between isomin and isomax
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

    /**
     * returns the histogram
     * \returns a reference to the internal representation of the histogram
     */
    const std::vector< double >& getHistogram() const
    {
        return histogram;
    }

    /**
     * sample/render the transfer function linearly between min and max in an RGBA texture.
     * \param array pointer to an allocated data structure
     * \param width is the number of RGBA samples.
     * \param min the lowest value to be sampled
     * \param max the hightes value to be sampled
     * \post array contains the sampled data
     * \pre array is allocated and has space for width elements
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
    /** sorted list of colors */
    std::vector<ColorEntry> colors;

    /** sorted list of alpha values */
    std::vector<AlphaEntry> alphas;

    /** the smallest used iso value */
    double isomin;
    /** the largest used iso value */
    double isomax;

    /** stores a histogram. This is used for property-handling only
     * and does not change the transfer function at all.
     */
    std::vector< double > histogram;

    friend std::ostream& operator << ( std::ostream& out, const WTransferFunction& tf );
};

/**
 * Default output operator. Currently stores values the same way as it is done in the properties.
 * This code should only be used for debugging and you should not realy on the interface.
 * \param tf The transfer function to output
 * \param out The stream to which we write
 * \returns reference to out
 */
std::ostream& operator << ( std::ostream& out, const WTransferFunction& tf );

#endif  // WTRANSFERFUNCTION_H
