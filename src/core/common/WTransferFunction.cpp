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

#include <cassert>

#include <iostream>
#include <algorithm>

//#include <osg/Vec4>
//#include <osg/io_utils> // for the operator<< and operator>> for Vec4

#include "WTransferFunction.h"

bool WTransferFunction::operator==( const WTransferFunction &rhs ) const
{
    if ( histogram.size() != rhs.histogram.size() )
    {
        return false;
    }
    {
        std::vector< double >::const_iterator ait1 = histogram.begin();
        std::vector< double >::const_iterator ait2 = rhs.histogram.begin();
        for ( ;
                ait1 != histogram.end();
                ++ait1, ++ait2 )
        {
            if ( *ait1 != *ait2 )
            {
                return false;
            }
        }
    }


    if ( colors.size() != rhs.colors.size() || alphas.size() != rhs.alphas.size() )
    {
        return false;
    }

    if ( isomin != rhs.isomin && isomax != rhs.isomax )
    {
        return false;
    }

    std::vector< ColorEntry >::const_iterator it1 = colors.begin();
    std::vector< ColorEntry >::const_iterator it2 = rhs.colors.begin();
    for ( ;
            it1 != colors.end();
            ++it1, ++it2 )
    {
        if ( !( *it1 == *it2 ) )
        {
            return false;
        }
    }

    std::vector< AlphaEntry >::const_iterator ait1 = alphas.begin();
    std::vector< AlphaEntry >::const_iterator ait2 = rhs.alphas.begin();
    for ( ;
            ait1 != alphas.end();
            ++ait1, ++ait2 )
    {
        if ( !( *ait1 == *ait2 ) )
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
    if ( colors.size() < 1 ) return;
    if ( alphas.size() < 1 ) return;

    std::vector< ColorEntry >::const_iterator c1 = colors.begin();
    std::vector< ColorEntry >::const_iterator c2 = c1+1;

    std::vector< AlphaEntry >::const_iterator a1 = alphas.begin();
    std::vector< AlphaEntry >::const_iterator a2 = a1+1;

    for ( int i = 0; i < width; ++i )
    {
        WColor color;
        double iso = ( double )i/( double )width * ( max-min ) + min;

        if ( iso <= isomin )
        {
            color = colors.begin()->color;
            color[ 3 ] = alphas.begin()->alpha;
        }
        else if ( iso >= isomax )
        {
            color = colors.back().color;
            color[ 3 ] = alphas.back().alpha;
        }
        else
        {
            while ( c2 != colors.end() && iso > c2->iso )
            {
                assert( c2 != colors.end() );
                c1++;
                c2++;
                assert( c1 != colors.end() );
            }

            while ( a2 != alphas.end() && iso > a2->iso )
            {
                assert( a2 != alphas.end() );
                a1++;
                a2++;
                assert( a1 != alphas.end() );
            }

            if ( c2 == colors.end() )
            {
                color = c1->color;
            }
            else
            {
                double colorParameter = ( iso - c1->iso )/( c2->iso - c1->iso );
                color = blend( c1->color, 1.-colorParameter, c2->color, colorParameter );
            }
            if ( a2 == alphas.end() )
            {
                color[ 3 ] = a1->alpha;
            }
            else
            {
                double alphaParameter = ( iso - a1->iso )/( a2->iso - a1->iso );
                color[ 3 ] = ablend( a1->alpha, 1.-alphaParameter, a2->alpha, alphaParameter );
            }
        }
        for ( int j = 0; j < 4; ++j )
        {
            array[ 4*i + j ] = color[ j ]*255.;
        }
    }
}


void WTransferFunction::addColor( double iso, const WColor& color )
{
    if ( colors.size() == 0 )
    {
        colors.push_back( ColorEntry( iso, color ) );
    }
    else
    {
        std::vector<ColorEntry>::iterator e = find_if( colors.begin(), colors.end(), LessPred<ColorEntry>( iso ) );
        colors.insert( e, ColorEntry( iso, color ) );
    }

    if ( alphas.size() >= 1 )
    {
        isomin = std::min( colors.front().iso, alphas.front().iso );
        isomax = std::max( colors.back().iso, alphas.back().iso );
    }
    else
    {
        isomin = colors.front().iso;
        isomax = colors.back().iso;
    }
}

void WTransferFunction::addAlpha( double iso, double alpha )
{
    if ( alphas.size() == 0 )
    {
        alphas.push_back( AlphaEntry( iso, alpha ) );
    }
    else
    {
        std::vector<AlphaEntry>::iterator e = find_if( alphas.begin(), alphas.end(), LessPred<AlphaEntry>( iso ) );
        alphas.insert( e, AlphaEntry( iso, alpha ) );
    }

    if ( colors.size() >= 1 )
    {
        isomin = std::min( colors.front().iso, alphas.front().iso );
        isomax = std::max( colors.back().iso, alphas.back().iso );
    }
    else
    {
        isomin = alphas.front().iso;
        isomax = alphas.back().iso;
    }
}


