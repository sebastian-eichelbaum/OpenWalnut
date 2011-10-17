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

#ifndef WPOLYNOMIALEQUATIONSOLVERS_TEST_H
#define WPOLYNOMIALEQUATIONSOLVERS_TEST_H

#include <iomanip>
#include <sstream>
#include <string>
#include <utility>

#include <cxxtest/TestSuite.h>

#include "../WPolynomialEquationSolvers.h"

/**
 * Testsuite for the WPolynomialEquationSolvers functions
 */
class WPolynomialEquationSolversTest : public CxxTest::TestSuite
{
public:
    /**
     * If there is no solution an exception should be thrown.
     */
    void testRealQuadraticEquationWithNoSolution( void )
    {
        TS_ASSERT_THROWS_EQUALS( solveRealQuadraticEquation( 0.0, 0.0, 1.0 ), const WEquationHasNoRoots &e,
                                 std::string( e.what() ), "The equation: 0x^2 + 0x + 1 = 0.0 has no solutions!" );
    }

    /**
     * x^2 = 0 has only one solution: 0.0.
     */
    void testRealQuadraticEquationWithOnlyOneSolution( void )
    {
        typedef std::pair< std::complex< double >, std::complex< double > > ComplexPair;
        ComplexPair actual = solveRealQuadraticEquation( 1.0, 0.0, 0.0 );
        TS_ASSERT_EQUALS( actual.first, std::complex< double >( 0.0, 0.0 ) );
        TS_ASSERT_EQUALS( actual.second, actual.first );
    }

    /**
     * x^2 - 1 = 0 has two solutions: 1.0 and -1.0.
     */
    void testRealQuadraticEquationWithTwoRealSolutions( void )
    {
        typedef std::pair< std::complex< double >, std::complex< double > > ComplexPair;
        ComplexPair actual = solveRealQuadraticEquation( 1.0, 0.0, -1.0 );
        ComplexPair expected = ComplexPair( std::complex< double >( -1.0, 0.0 ), std::complex< double >( 1.0, 0.0 ) );
        if(  actual.first != expected.first )
        {
            std::cout << std::endl << "We assume the order of the pair is swaped, but both solutions are right" << std::endl;
            TS_ASSERT_EQUALS( actual.first, expected.second );
            TS_ASSERT_EQUALS( actual.second, expected.first );
        }
        else
        {
            TS_ASSERT_EQUALS( actual.first, expected.first );
            TS_ASSERT_EQUALS( actual.second, expected.second );
        }
    }

    /**
     * x^2 + 1 = 0 has two solutions: i and -i.
     */
    void testRealQuadraticEquationWithTwoImaginarySolutions( void )
    {
        typedef std::pair< std::complex< double >, std::complex< double > > ComplexPair;
        ComplexPair actual = solveRealQuadraticEquation( 1.0, 0.0, 1.0 );
        ComplexPair expected = ComplexPair( std::complex< double >( 0.0, 1.0 ), std::complex< double >( 0.0, -1.0 ) );
        if(  actual.first != expected.first )
        {
            std::cout << std::endl << "We assume the order of the pair is swaped, but both solutions are right" << std::endl;
            TS_ASSERT_EQUALS( actual.first, expected.second );
            TS_ASSERT_EQUALS( actual.second, expected.first );
        }
        else
        {
            TS_ASSERT_EQUALS( actual.first, expected.first );
            TS_ASSERT_EQUALS( actual.second, expected.second );
        }
    }

    /**
     * This test numerical stability:
     * x^2 + 9999999999x -9e10 = 0, has solutions according to wolfram alpha:
     *    x_1 = -1.00000000079999999928x10^10
     *    x_2 = 8.99999999280000001224
     */
    void testRealQuadraticEquationToTestNumericalIssuesAndPrecisions( void )
    {
        typedef std::pair< std::complex< double >, std::complex< double > > ComplexPair;
        ComplexPair actual = solveRealQuadraticEquation( 1.0, 9999999999.0, -9.0e10 );
        ComplexPair expected = ComplexPair( std::complex< double >( -1.00000000079999999928e10, 0.0 ), std::complex< double >( 8.99999999280000001224, 0.0 ) ); // NOLINT line length
        double delta = 0.00000001; // This precision was found by trial and error
        if( ( ( std::abs( actual.first - expected.first ) > delta ) && ( std::abs( actual.first - expected.second ) > delta ) ) ||
            ( ( std::abs( actual.second - expected.second ) > delta ) && ( std::abs( actual.second - expected.first ) > delta ) ) )
        {
            std::stringstream ss;
            ss << std::fixed << std::setprecision( 16 ) << std::endl;
            ss << "Invalid solutions: " << std::endl;
            ss << std::real( actual.first ) << " i" << std::imag( actual.first ) << " :::: ";
            ss << std::real( actual.second ) << " i" << std::imag( actual.second ) << std::endl;
            ss << " were invalid. I expected: " << std::endl;
            ss << std::real( expected.first ) << " i" << std::imag( expected.first ) << " :::: ";
            ss << std::real( expected.second ) << " i" << std::imag( expected.second ) << std::endl;
            std::cout << ss.str();
            TS_FAIL( "Check your numerical stability and the order of the solutions found." );
        }
    }
};

#endif  // WPOLYNOMIALEQUATIONSOLVERS_TEST_H
