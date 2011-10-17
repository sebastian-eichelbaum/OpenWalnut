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

#ifndef WOSSIMHELPER_TEST_H
#define WOSSIMHELPER_TEST_H

#include <cxxtest/TestSuite.h>

#include "../WOSSIMHelper.h"

/**
 * Tests for WOSSIMHelper.
 */
class WOSSIMHelperTest : public CxxTest::TestSuite
{
public:
    /**
     * Test ComputeSVD function of WOSSIMHelper
     */
    void testComputeSVD( void )
    {
#ifdef OW_USE_OSSIM
        const size_t nbRows = 3, nbCols = 3;
        const double a = 1.2, b = 2.3, c = 3.4,
                     d = 4.5, e = 5.6, f = 6.7,
                     g = 3.4, h = 1.2, i = 7.0;
        WMatrix< double > A( nbRows, nbCols );

        A( 0, 0 ) = a;
        A( 0, 1 ) = b;
        A( 0, 2 ) = c;
        A( 1, 0 ) = d;
        A( 1, 1 ) = e;
        A( 1, 2 ) = f;
        A( 2, 0 ) = g;
        A( 2, 1 ) = h;
        A( 2, 2 ) = i;

        WMatrix< double > U( nbRows, nbCols );
        WMatrix< double > V( nbCols, nbCols );
        WValue< double > S( nbCols );

        WOSSIMHelper::computeSVD( A, U, V, S );

        WMatrix< double > Sm( nbRows, nbCols );
        Sm( 0, 0 ) = S[0];
        Sm( 1, 1 ) = S[1];
        Sm( 2, 2 ) = S[2];

        WMatrix<double> svd( U*Sm*V.transposed() );

        for( size_t row = 0; row < svd.getNbRows(); row++ )
        {
          for( size_t col = 0; col < svd.getNbCols(); col++ )
          {
            TS_ASSERT_DELTA( svd( row, col ), A( row, col ), 0.0001 );
          }
        }
#endif
    }
    /**
     * Test pseudoInverse function of WOSSIMHelper
     */
//     void testPseudoInverse( void )
//     {
//         const size_t nbRows = 3, nbCols = 3;
//         const double a = 1.2, b = 2.3, c = 3.4,
//                      d = 4.5, e = 5.6, f = 6.7,
//                      g = 3.4, h = 1.2, i = 7.0;
//         WMatrix< double > A( nbRows, nbCols );
//
//         A( 0, 0 ) = a;
//         A( 0, 1 ) = b;
//         A( 0, 2 ) = c;
//         A( 1, 0 ) = d;
//         A( 1, 1 ) = e;
//         A( 1, 2 ) = f;
//         A( 2, 0 ) = g;
//         A( 2, 1 ) = h;
//         A( 2, 2 ) = i;
//
//         WMatrix<double> Ainvers( WOSSIMHelper::pseudoInverse( A ) );
//         WMatrix<double> I( A*Ainvers );
//
//         for( size_t row = 0; row < I.getNbRows(); row++ )
//         {
//           for( size_t col = 0; col < I.getNbCols(); col++ )
//           {
//             if( row == col )
//             {
//               TS_ASSERT_DELTA( I( row, col ), 1.0, 0.0001 );
//             }
//             else
//             {
//               TS_ASSERT_DELTA( I( row, col ), 0.0, 0.0001 );
//             }
//           }
//         }
//     }
};

#endif  // WOSSIMHELPER_TEST_H
