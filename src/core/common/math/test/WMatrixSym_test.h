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

#ifndef WMATRIXSYM_TEST_H
#define WMATRIXSYM_TEST_H

#include <string>
#include <vector>

#include <cxxtest/TestSuite.h>

#include "../../exceptions/WOutOfBounds.h"
#include "../WMatrixSym.h"

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

    void testToString( void )
    {
        WMatrixSymDBL t( 3 );
        double mydata[] = { 1.6, 0.2, 1/3.0 }; // NOLINT
        std::vector< double > data( mydata, mydata + sizeof( mydata ) / sizeof( double ) );
        t.setData( data );
        std::string expected = "0.0 1.600 0.200\n1.600 0.0 0.333\n0.200 0.333 0.0";
        TS_ASSERT_EQUALS( expected, t.toString() );
    }
};

#endif  // WMATRIXSYM_TEST_H
