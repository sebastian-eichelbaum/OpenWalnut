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
#include <complex>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <boost/array.hpp>

#include "../exceptions/WOutOfBounds.h"
#include "../WAssert.h"
#include "../WLimits.h"
#include "../WStringUtils.h"
#include "WLine.h"
#include "WPolynomialEquationSolvers.h"
#include "linearAlgebra/WPosition.h"
#include "linearAlgebra/WVectorFixed.h"

WLine::WLine( const std::vector< WPosition > &points )
    : WMixinVector< WPosition >( points )
{
}

WLine::WLine()
    : WMixinVector< WPosition >()
{
}

const WPosition& midPoint( const WLine& line )
{
    if( line.empty() )
    {
        throw WOutOfBounds( std::string( "There is no midpoint for an empty line." ) );
    }
    return line[( line.size() - 1 ) / 2];
}

void WLine::reverseOrder()
{
    std::reverse( begin(), end() );
}

double pathLength( const WLine& line )
{
    double len = 0;
    // incase of size() <= 1 the for loop will not run!
    for( size_t i = 1; i < line.size(); ++i )
    {
        len += length( line[i - 1] - line[i] );
    }
    return len;
}

void WLine::resampleByNumberOfPoints( size_t numPoints )
{
    WLine newLine;
    newLine.reserve( numPoints );
    if( size() != numPoints && size() > 1 && numPoints > 0 )
    {
        const double pathL = pathLength( *this );
        double newSegmentLength = pathL / ( numPoints - 1 );
        const double delta = newSegmentLength * 1.0e-10; // 1.0e-10 which represents the precision is choosen by intuition
        double remainingLength = 0.0;
        newLine.push_back( front() );
        for( size_t i = 0; i < ( size() - 1 ); ++i )
        {
            remainingLength += length( at( i ) - at( i + 1 ) );
            while( ( remainingLength > newSegmentLength ) || std::abs( remainingLength - newSegmentLength ) < delta )
            {
                remainingLength -= newSegmentLength;
                // TODO(math): fix numerical issuses: newSegmentLength may be wrong => great offset by many intraSegment sample points
                //                                    remainingLength may be wrong => ...
                //                                    Take a look at the unit test testNumericalStabilityOfResampling
                WPosition newPoint = at( i + 1 ) + remainingLength * normalize( at( i ) - at( i + 1 ) );
                newLine.push_back( newPoint );
                // std::cout << "line size so far" << newLine.size() << " lenght so far: " << newLine.pathLength() << std::endl;
                // std::cout << numPoints - newLine.size() << std::endl;
            }
        }
        // using string_utils::operator<<;
        // std::cout << "this: " << *this << std::endl << "new:  " << newLine << std::endl;
        // std::cout << "|remL - newSegL|: " << std::abs( remainingLength - newSegmentLength ) << std::endl;
        // std::cout << std::setprecision( 35 ) << "remainingLength: " << remainingLength << " newSegmentLength: " << newSegmentLength << std::endl;
        // std::cout << "this size: " << size() << " new size: " << newLine.size() << std::endl;
    }
    else if( size() == 1 && size() < numPoints )
    {
        for( size_t i = 0; i < numPoints; ++i )
        {
            newLine.push_back( front() );
        }
    }
    if( size() != numPoints )
    {
        this->WMixinVector< WPosition >::operator=( newLine );
    }
    // Note if the size() == 0, then the resampled tract is also of length 0
}

void WLine::removeAdjacentDuplicates()
{
    if( empty() )
    {
        return;
    }

    // Note: We cannot use std::remove for that since it allows only unary predicates to identify
    // elements which are to be removed
    WLine newLine;
    newLine.reserve( size() );
    newLine.push_back( front() );
    for( const_iterator cit = begin()++; cit != end(); ++cit )
    {
        if( length( *cit - newLine.back() ) > wlimits::DBL_EPS )
        {
            newLine.push_back( *cit );
        }
    }
    this->WMixinVector< WPosition >::operator=( newLine );
}

void WLine::resampleBySegmentLength( double newSegmentLength )
{
    // eliminate duplicate points following next to another
    removeAdjacentDuplicates();

    if( empty() || size() == 1 )
    {
        return;
    }
    WLine newLine;
    newLine.push_back( front() );
    for( size_t i = 1; i < size(); )
    {
        if( length( newLine.back() - ( *this )[i] ) > newSegmentLength )
        {
            const WPosition& pred = ( *this )[i - 1];
            if( pred == newLine.back() )
            {
                // Then there is no triangle and the old Segment Length is bigger as the new segment
                // length
                newLine.push_back( newLine.back() + normalize( ( *this )[i] - pred ) * newSegmentLength );
                continue;
            }
            else // this is the general case, and the point we search is inbetween the pred and the current position
            {
                // we compute the three coefficents describing the quadradic equation of the point of intersection of
                // the circle with radius newSegmentLength and the segmend: pred and ( *this )[i].
                // alpha * x^2 + beta * x + gamma = 0
                double alpha = ( ( *this )[i][0] - pred[0] ) * ( ( *this )[i][0] - pred[0] ) +
                               ( ( *this )[i][1] - pred[1] ) * ( ( *this )[i][1] - pred[1] ) +
                               ( ( *this )[i][2] - pred[2] ) * ( ( *this )[i][2] - pred[2] );

                double beta = 2.0 * ( ( *this )[i][0] - pred[0] ) * ( pred[0] - newLine.back()[0] ) +
                              2.0 * ( ( *this )[i][1] - pred[1] ) * ( pred[1] - newLine.back()[1] ) +
                              2.0 * ( ( *this )[i][2] - pred[2] ) * ( pred[2] - newLine.back()[2] );

                double gamma = ( pred[0] - newLine.back()[0] ) * ( pred[0] - newLine.back()[0] ) +
                               ( pred[1] - newLine.back()[1] ) * ( pred[1] - newLine.back()[1] ) +
                               ( pred[2] - newLine.back()[2] ) * ( pred[2] - newLine.back()[2] ) - newSegmentLength * newSegmentLength;

                typedef std::pair< std::complex< double >, std::complex< double > > ComplexPair;
                ComplexPair solution = solveRealQuadraticEquation( alpha, beta, gamma );
                // NOTE: if those asserts fire, then this algo is wrong and produces wrong results, and I've to search to bug!
                WAssert( std::imag( solution.first ) == 0.0, "Invalid quadratic equation while computing resamplingBySegmentLength" );
                WAssert( std::imag( solution.second ) == 0.0, "Invalid quadratic equation while computing resamplingBySegmentLength" );
                WPosition pointOfIntersection;
                if( std::real( solution.first ) > 0.0 )
                {
                    pointOfIntersection = pred + std::real( solution.first ) * ( ( *this )[i] - pred );
                }
                else
                {
                    pointOfIntersection = pred + std::real( solution.second ) * ( ( *this )[i] - pred );
                }
                newLine.push_back( pointOfIntersection );
            }
        }
        ++i;
    }
    if( length( newLine.back() - ( *this )[size() - 1] ) > newSegmentLength / 2.0 )
    {
        WVector3d direction = normalize( ( *this )[size() - 1] - newLine.back() );
        newLine.push_back( newLine.back() + direction * newSegmentLength );
    }
    this->WMixinVector< WPosition >::operator=( newLine );
}

int equalsDelta( const WLine& line, const WLine& other, double delta )
{
    size_t pts = ( std::min )( other.size(), line.size() ); // This ( std::min ) thing compiles also under Win32/Win64
    size_t diffPos = 0;
    bool sameLines = true;
    for( diffPos = 0; ( diffPos < pts ) && sameLines; ++diffPos )
    {
        for( int x = 0; x < 3; ++x ) // since WLine uses WPosition as elements there are 3 components per position
        {
            sameLines = sameLines && ( std::abs( line[diffPos][x] - other[diffPos][x] ) <= delta );
        }
    }
    if( sameLines && ( line.size() == other.size() ) )
    {
        return -1;
    }
    if( !sameLines )
    {
        return diffPos - 1;
    }
    return diffPos;
}

double maxSegmentLength( const WLine& line )
{
    double result = 0.0;
    if( line.empty() || line.size() == 1 )
    {
        return result;
    }
    for( size_t i = 0; i < line.size() - 1; ++i )
    {
        result = std::max( result, static_cast< double >( length( line[i] - line[i+1] ) ) );
    }
    return result;
}

void WLine::unifyDirectionBy( const WLine& other )
{
    const size_t numBasePoints = 4;
    boost::array< WPosition, numBasePoints > m;
    boost::array< WPosition, numBasePoints > n;

    double distance = 0.0;
    double inverseDistance = 0.0;
    for( size_t i = 0; i < numBasePoints; ++i )
    {
        m[i] = other.at( ( other.size() - 1 ) * static_cast< double >( i ) / ( numBasePoints - 1 ) );
        n[i] = at( ( size() - 1 ) * static_cast< double >( i ) / ( numBasePoints - 1 ) );
        distance += length2( m[i] - n[i] );
        inverseDistance += length2( m[i] - at( ( size() - 1 ) * static_cast< double >( numBasePoints - 1 - i ) / ( numBasePoints - 1 ) ) );
    }
    distance /= static_cast< double >( numBasePoints );
    inverseDistance /= static_cast< double >( numBasePoints );

    if( inverseDistance < distance )
    {
        this->reverseOrder();
    }
}

WBoundingBox computeBoundingBox( const WLine& line )
{
    WBoundingBox result;
    for( WLine::const_iterator cit = line.begin(); cit != line.end(); ++cit )
    {
        result.expandBy( *cit );
    }
    return result;
}
