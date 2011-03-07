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

#ifndef WPOLYNOMIALEQUATIONSOLVERS_H
#define WPOLYNOMIALEQUATIONSOLVERS_H

#include <complex>
#include <iostream>
#include <sstream>
#include <utility>

#include "../exceptions/WEquationHasNoRoots.h"
#include "../WLimits.h"


/**
 * Solves a quadratic equation: ax^2 + bx + c = 0 where a,b,c are real coefficient.
 *
 * \param a first coefficient
 * \param b second coefficient
 * \param c remainder
 *
 * \throw WEquationHasNoRoots In case there are no roots for this equation.
 *
 * \return Solutions to the equation above as complex numbers. If only one solution exists both
 * complex numbers are the same!
 */
template< typename T > typename std::pair< typename std::complex< T >, typename std::complex< T > > solveRealQuadraticEquation( T a, T b, T c );



template< typename T >
inline typename std::pair< typename std::complex< T >, typename std::complex< T > > solveRealQuadraticEquation( T a, T b, T c )
{
    typename std::pair< typename std::complex< T >, typename std::complex< T > > result( std::complex< T >( 0.0, 0.0 ), std::complex< T >( 0.0, 0.0 ) ); // NOLINT line length
    if( a != 1.0 ) // normalize
    {
        if( std::abs( a ) <= wlimits::DBL_EPS ) // a ≃ 0.0
        {
            if( std::abs( b ) <= wlimits::DBL_EPS ) // b ≃ 0.0
            {
                if( std::abs( c ) > wlimits::DBL_EPS ) // there is no solution
                {
                    std::stringstream ss;
                    ss << "The equation: " << a << "x^2 + " << b << "x + " << c << " = 0.0 has no solutions!";
                    throw WEquationHasNoRoots( ss.str() );
                }
                else // all coefficents are zero so we return 0.0 as a solution
                {
                    return result;
                }
            }
            else // Note equation degrades to linear equation:
            {
               result.first = std::complex< T >( -c / b, 0.0 );
                result.second = result.first;
                return result;
            }
        }
        else
        {
            b /= a;
            c /= a;
            a = 1.0;
        }
    }

    // equation is in normal form
    double discriminant = b * b - 4.0 * c;
    if( discriminant < 0.0 )
    {
        result.first = std::complex< T >( -b / 2.0, std::sqrt( std::abs( discriminant ) ) / 2.0 );
        result.second = std::complex< T >( -b / 2.0, -std::sqrt( std::abs( discriminant ) ) / 2.0 );
    }
    else if( discriminant > 0.0 )
    {
        result.first = std::complex< T >( -b / 2.0 + std::sqrt( discriminant ) / 2.0, 0.0 );
        result.second = std::complex< T >( -b / 2.0 - std::sqrt( discriminant ) / 2.0 , 0.0 );
    }
    else // discriminant ≃ 0.0
    {
        // NOTE: if b==-0.0 and discriminant==0.0 then result.first and result.second would be different. To prevent this we check if b ≃ 0.0
        if( std::abs( b ) <= wlimits::DBL_EPS ) // b ≃ 0.0
        {
            result.first = std::complex< T >( 0.0, 0.0 );
            result.second = result.first;
        }
        else
        {
            result.first = std::complex< T >( -b / 2, 0.0 );
            result.second = result.first;
        }
    }
    return result;
}

#endif  // WPOLYNOMIALEQUATIONSOLVERS_H
