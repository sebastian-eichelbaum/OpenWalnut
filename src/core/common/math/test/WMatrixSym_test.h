//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2013 OpenWalnut Community, BSV@Uni-Leipzig and CNCF@MPI-CBS
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

#ifndef WMATRIXSYM_TEST_H
#define WMATRIXSYM_TEST_H

#include <sstream>
#include <string>
#include <vector>

#include <cxxtest/TestSuite.h>
#include <cxxtest/ValueTraits.h>

#include "../../exceptions/WOutOfBounds.h"
#include "../../test/WTraitsBase.h"
#include "../WMatrixSym.h"

#ifdef CXXTEST_RUNNING
namespace CxxTest
{
CXXTEST_TEMPLATE_INSTANTIATION
/**
 * Enables better UnitTest OutPut if something fails with WFibers, so you see
 * immedeatly what is failing.
 */
class ValueTraits< WMatrixSym< double > > : public WTraitsBase
{
public:
    /**
     * Constructor for class allowing usable output of WMatrix in tests
     *
     * \param m the WMatrix to print
     */
    explicit ValueTraits( const WMatrixSym< double > &m )
    {
        std::stringstream tmp;
        tmp.precision( 5 );
        for( size_t row = 0; row < m.size(); row++ )
        {
          for( size_t col = row + 1; col < m.size(); col++ )
          {
              tmp << m( row, col ) << " ";
          }
        }
        m_s = tmp.str();
    }
};
}
#endif // CXXTEST_RUNNING

/**
 * Unit test this LookUp table class. All test performed on matrices with double as element type.
 */
class WMatrixSymTest : public CxxTest::TestSuite
{
public:
    /**
     * Only the Elements of the upper/lower sym. Matrix should be stored.
     */
    void testOperatorOn3x3Matrix( void )
    {
        WMatrixSymDBL t( 3 );
        TS_ASSERT_EQUALS( t.m_data.size(), 3 );
    }

    /**
     * Access to elements on main diagonal is forbidden. Then other acess
     * should be symmetric.
     */
    void testAccessOn3x3Matrix( void )
    {
        WMatrixSymDBL t( 3 );
        double mydata[] = { 1.6, 0.2, 7.7 }; // NOLINT
        std::vector< double > data( mydata, mydata + sizeof( mydata ) / sizeof( double ) );
        t.setData( data );
        TS_ASSERT_EQUALS( t( 1, 2 ), 7.7 );
        TS_ASSERT_EQUALS( t( 2, 1 ), 7.7 );
    }

    /**
     * If new elements are set via the setData() method then it has to be
     * checked if the dimension is valid for the number of elements which
     * are given.
     */
    void testSetDataWithInvalidLengthForDimension( void )
    {
        WMatrixSymDBL t( 4 );
        double mydata[] = { 1.6, 0.2, 7.7 }; // NOLINT
        std::vector< double > data( mydata, mydata + sizeof( mydata ) / sizeof( double ) );
        TS_ASSERT_THROWS_EQUALS( t.setData( data ), WOutOfBounds &e, std::string( e.what() ), "Data vector length: 3 doesn't fit to number of rows and cols: 4" ); // NOLINT line length
    }

    /**
     * Accessing diagonal elements is forbidden and an exception should be thrown
     */
    void testInvalidAccessOnMainDiagonal( void )
    {
        WMatrixSymDBL t( 4 );
        TS_ASSERT_THROWS_EQUALS( t( 0, 0 ), WOutOfBounds &e, std::string( e.what() ),
                "Invalid Element Access ( 0, 0 ). No diagonal elements or indices bigger than 4 are allowed." );
    }

    /**
     * Renders the matrix to a string, where each row is in a separate line.
     */
    void testToString( void )
    {
        WMatrixSymDBL t( 3 );
        double mydata[] = { 1.6, 0.2, 1/3.0 }; // NOLINT
        std::vector< double > data( mydata, mydata + sizeof( mydata ) / sizeof( double ) );
        t.setData( data );
        std::string expected = "0.000000000 1.600000000 0.200000000\n1.600000000 0.000000000 0.333333333\n0.200000000 0.333333333 0.000000000";
        TS_ASSERT_EQUALS( expected, t.toString() );
    }

    /**
     * There should be an output operator for symmetric matrices.
     */
    void testOutputStream( void )
    {
        WMatrixSymDBL t( 3 );
        double mydata[] = { 1.6, 0.2, 1/3.0 }; // NOLINT
        std::vector< double > data( mydata, mydata + sizeof( mydata ) / sizeof( double ) );
        t.setData( data );
        std::string expected = "0.00 1.60 0.20\n1.60 0.00 0.33\n0.20 0.33 0.00";
        std::stringstream ss;
        ss << std::setprecision( 2 ) << std::fixed;
        ss << t;
        TS_ASSERT_EQUALS( expected, ss.str() );
    }

    /**
     * There should be an input operator for symmetric matrices, reading data into a given matrix.
     */
    void testInputStream( void )
    {
        WMatrixSymDBL expected( 3 );
        double mydata[] = { 1.6, 0.2, 0.3 }; // NOLINT
        std::vector< double > data( mydata, mydata + sizeof( mydata ) / sizeof( double ) );
        expected.setData( data );
        std::stringstream ss( "0.0 1.6 0.2\n1.6 0.0 0.3\n0.2 0.3 0.0" );
        WMatrixSymDBL actual( 3 );
        ss >> actual;
        TS_ASSERT_EQUALS( expected, actual );
    }
};

#endif  // WMATRIXSYM_TEST_H
