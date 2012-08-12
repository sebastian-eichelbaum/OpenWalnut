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

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

#include "WAssert.h"
#include "WTransferFunction.h"

bool WTransferFunction::operator==( const WTransferFunction &rhs ) const
{
    if( m_histogram.size() != rhs.m_histogram.size() )
    {
        return false;
    }
    {
        std::vector< double >::const_iterator ait1 = m_histogram.begin();
        std::vector< double >::const_iterator ait2 = rhs.m_histogram.begin();
        for( ;
                ait1 != m_histogram.end();
                ++ait1, ++ait2 )
        {
            if( *ait1 != *ait2 )
            {
                return false;
            }
        }
    }


    if( m_colors.size() != rhs.m_colors.size() || m_alphas.size() != rhs.m_alphas.size() )
    {
        return false;
    }

    if( m_isomin != rhs.m_isomin && m_isomax != rhs.m_isomax )
    {
        return false;
    }

    std::vector< ColorEntry >::const_iterator it1 = m_colors.begin();
    std::vector< ColorEntry >::const_iterator it2 = rhs.m_colors.begin();
    for( ;
            it1 != m_colors.end();
            ++it1, ++it2 )
    {
        if( !( *it1 == *it2 ) )
        {
            return false;
        }
    }

    std::vector< AlphaEntry >::const_iterator ait1 = m_alphas.begin();
    std::vector< AlphaEntry >::const_iterator ait2 = rhs.m_alphas.begin();
    for( ;
            ait1 != m_alphas.end();
            ++ait1, ++ait2 )
    {
        if( !( *ait1 == *ait2 ) )
        {
            return false;
        }
    }

    return true;
}

bool WTransferFunction::operator!=( const WTransferFunction &rhs ) const
{
    return !( ( *this ) == rhs );
}

namespace
{
    //! linear blend between two colors in rgb space if ta = 1.-tb
    WColor blend( const WColor&a, double ta, const WColor &b, double tb )
    {
        return WColor(
                ta*a[ 0 ]+tb*b[ 0 ],
                ta*a[ 1 ]+tb*b[ 1 ],
                ta*a[ 2 ]+tb*b[ 2 ], 1. );
    }

    //! linear blend between two variables
    double ablend( const double a, const double ta, const double b, const double tb )
    {
        return a*ta + b*tb;
    }
} // namespace


void WTransferFunction::sample1DTransferFunction( unsigned char*array, int width, double min, double max ) const
{
    if( m_colors.size() < 1 ) return;
    if( m_alphas.size() < 1 ) return;

    std::vector< ColorEntry >::const_iterator c1 = m_colors.begin();
    std::vector< ColorEntry >::const_iterator c2 = c1+1;

    std::vector< AlphaEntry >::const_iterator a1 = m_alphas.begin();
    std::vector< AlphaEntry >::const_iterator a2 = a1+1;

    for( int i = 0; i < width; ++i )
    {
        WColor color;
        double iso = ( double )i/( double )width * ( max-min ) + min;

        if( iso <= m_isomin )
        {
            color = m_colors.begin()->color;
            color[ 3 ] = m_alphas.begin()->alpha;
        }
        else if( iso >= m_isomax )
        {
            color = m_colors.back().color;
            color[ 3 ] = m_alphas.back().alpha;
        }
        else
        {
            while( c2 != m_colors.end() && iso > c2->iso )
            {
                WAssert( c2 != m_colors.end(), "Corruption of internal data structure." );
                c1++;
                c2++;
                WAssert( c1 != m_colors.end(), "Corruption of internal data structure." );
            }

            while( a2 != m_alphas.end() && iso > a2->iso )
            {
                WAssert( a2 != m_alphas.end(), "Corruption of internal data structure." );
                a1++;
                a2++;
                WAssert( a1 != m_alphas.end(), "Corruption of internal data structure." );
            }

            if( c2 == m_colors.end() )
            {
                color = c1->color;
            }
            else
            {
                double colorParameter = ( iso - c1->iso )/( c2->iso - c1->iso );
                color = blend( c1->color, 1.-colorParameter, c2->color, colorParameter );
            }
            if( a2 == m_alphas.end() )
            {
                color[ 3 ] = a1->alpha;
            }
            else
            {
                double alphaParameter = ( iso - a1->iso )/( a2->iso - a1->iso );
                color[ 3 ] = ablend( a1->alpha, 1.-alphaParameter, a2->alpha, alphaParameter );
            }
        }
        for( int j = 0; j < 4; ++j )
        {
            array[ 4*i + j ] = color[ j ]*255.;
        }
    }
}


void WTransferFunction::addColor( double iso, const WColor& color )
{
    if( m_colors.size() == 0 )
    {
        m_colors.push_back( ColorEntry( iso, color ) );
    }
    else
    {
        std::vector<ColorEntry>::iterator e = find_if( m_colors.begin(), m_colors.end(), LessPred<ColorEntry>( iso ) );
        m_colors.insert( e, ColorEntry( iso, color ) );
    }

    if( m_alphas.size() >= 1 )
    {
        m_isomin = std::min( m_colors.front().iso, m_alphas.front().iso );
        m_isomax = std::max( m_colors.back().iso, m_alphas.back().iso );
    }
    else
    {
        m_isomin = m_colors.front().iso;
        m_isomax = m_colors.back().iso;
    }
}

void WTransferFunction::addAlpha( double iso, double alpha )
{
    if( m_alphas.size() == 0 )
    {
        m_alphas.push_back( AlphaEntry( iso, alpha ) );
    }
    else
    {
        std::vector<AlphaEntry>::iterator e = find_if( m_alphas.begin(), m_alphas.end(), LessPred<AlphaEntry>( iso ) );
        m_alphas.insert( e, AlphaEntry( iso, alpha ) );
    }

    if( m_colors.size() >= 1 )
    {
        m_isomin = std::min( m_colors.front().iso, m_alphas.front().iso );
        m_isomax = std::max( m_colors.back().iso, m_alphas.back().iso );
    }
    else
    {
        m_isomin = m_alphas.front().iso;
        m_isomax = m_alphas.back().iso;
    }
}

WTransferFunction WTransferFunction::createFromRGBA( unsigned char const * const rgba, size_t size )
{
    // we create a linear match to the transfer funciton given by rgba by scanning the
    // alpha and color values in two passes.
    // each pass starts at the left of the picture and moves right looking for a good match of
    // the line between start point and end point to the given function in between. The error is
    // normalized to a per-sample basis. If the maximum error is below MIN_ERROR_THRESHOLD, the
    // line is accepted and the next segment is analyzed.
    const double MIN_ERROR_THRESHOLD = 5.0;
    WTransferFunction rgbatf;
    std::vector < float > values( size );

    // copy channel
    for( size_t i = 0; i < size; ++i )
    {
        values[ i ] =  static_cast<double>( rgba[ i*4+3 ] );
    }

    // add first and last alpha
    rgbatf.addAlpha( 0.0, values[ 0 ]/255. );
    rgbatf.addAlpha( 1.0, values[ size-1 ]/255. );

    std::vector < float > errors( size );

    size_t seed = 0;
    while( seed < size-1 )
    {
        // start at first pixel and fit a line to the data
        size_t to = seed+1;
        while( to < size )
        {
            double error = 0.0;
            double incline =  ( values[ to ] - values[ seed ] )/( to-seed );
            for( size_t j = seed+1; j < to; ++j )
            {
                error +=  std::sqrt( std::pow( values[ j ] - values[ seed ] - incline * ( j-seed ), 2 ) );
            }
            errors[ to ] =  error/( to-seed ); // compute square error per pixel length of line
            ++to;
        }
        size_t minElement = size-1;
        double minerror = errors[ minElement ];
        for( to = size-1; to > seed; --to )
        {
            if( errors[ to ] <  minerror )
            {
                minElement = to;
                minerror = errors[ to ];
                if( minerror < MIN_ERROR_THRESHOLD )
                {
                    break;
                }
            }
        }
        if( minElement < size-1 )
        {
            rgbatf.addAlpha( ( double )minElement/( double )( size-1 ), values[ minElement ]/255. );
        }
        seed = minElement;
    }


    // same for color
    // add first and last color
    rgbatf.addColor( 0.0, WColor( rgba[ 0*4+0 ]/255.f, rgba[ 0*4+1 ]/255.f, rgba[ 0*4+2 ]/255.f, 0.f ) );
    rgbatf.addColor( 1.0, WColor( rgba[ ( size-1 )*4+0 ]/255.f, rgba[ ( size-1 )*4+1 ]/255.f, rgba[ ( size-1 )*4+2 ]/255.f, 0.f ) );

    // first try of code: use combined RGB errors

    seed = 0;
    while( seed < size-1 )
    {
        // start at first pixel and fit a line to the data
        size_t to = seed+1;
        while( to < size )
        {
            double error = 0.0;
            double inclineR =  ( rgba[ to*4+0 ] - rgba[ seed*4+0 ] )/( to-seed );
            double inclineG =  ( rgba[ to*4+1 ] - rgba[ seed*4+1 ] )/( to-seed );
            double inclineB =  ( rgba[ to*4+2 ] - rgba[ seed*4+2 ] )/( to-seed );

            for( size_t j = seed; j < to; ++j )
            {
                error +=  std::sqrt(
                        std::pow( rgba[ 4*j+0 ] - rgba[ 4*seed+0 ] - inclineR * ( j-seed ), 2 ) +
                        std::pow( rgba[ 4*j+1 ] - rgba[ 4*seed+1 ] - inclineG * ( j-seed ), 2 ) +
                        std::pow( rgba[ 4*j+2 ] - rgba[ 4*seed+2 ] - inclineB * ( j-seed ), 2 )
                        );
            }
            errors[ to ] =  error/( to-seed ); // compute square error per pixel length of line
            ++to;
        }

        size_t minElement = size-1;
        double minerror = errors[ size-1 ];
        // traverse from back
        for( to = size-2; to > seed; --to )
        {
            if( errors[ to ] <  minerror )
            {
                minElement = to;
                minerror = errors[ to ];
            }
            if( minerror < MIN_ERROR_THRESHOLD*2.0 ) //! the threshold here is larger than for alpha, becuase we compare all colors at once
            {
                break;
            }
        }
        if( minElement < size-1 )
        {
            rgbatf.addColor( ( double )minElement/( double )( size-1 ),
                WColor( rgba[ minElement*4+0 ]/255.f, rgba[ minElement*4+1 ]/255.f, rgba[ minElement*4+2 ]/255.f, 0.f ) );
        }
        seed = minElement;
    }

    std::cout <<  "New Transfer Function: " << rgbatf << "." <<  std::endl;
    return rgbatf;
}

std::ostream& operator << ( std::ostream& out, const WTransferFunction& tf )
{
    size_t numColors = tf.numColors();
    for( size_t i = 0; i < numColors; ++i )
    {
        double iso = tf.getColorIsovalue( i );
        WColor c = tf.getColor( i );
        out << "c:" << iso << ":" << c[ 0 ] << ":" << c[ 1 ] << ":" << c[ 2 ] << ";";
    }
    size_t numAlphas = tf.numAlphas();
    for( size_t i = 0; i < numAlphas; ++i )
    {
        double iso = tf.getAlphaIsovalue( i );
        double alpha = tf.getAlpha( i );
        out << "a:" << iso << ":" << alpha;
        if( i != numAlphas-1 )
        {
            out << ";";
        }
    }
    return out;
}
