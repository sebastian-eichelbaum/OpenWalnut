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

#ifndef WLINEARALGEBRAFUNCTIONS_TEST_H
#define WLINEARALGEBRAFUNCTIONS_TEST_H

#include <cxxtest/TestSuite.h>

#include "../WLinearAlgebraFunctions.h"
#include "../WMatrix.h"

using wmath::WMatrix;
/**
 * Tests for WMatrix.
 */
class WLinearAlgebraFunctionsTest : public CxxTest::TestSuite
{
public:
    // TODO(schurade): document and implement multMatrixWithVector3D and invertMatrix3x3 here.
    /**
     * This is a dummy
     */
    void testOneOftheFunctions( void )
    {
    }

    /**
     * Test the inversion of 4x4 matrices
     */
    void test4x4Inverse()
    {
        wmath::WMatrix<double> m( 4, 4 );

        for( size_t r = 0; r < 4; ++r)
        {
            for( size_t c = 0; c < 4; ++c )
            {
                m( c, r ) = r + c * 4 + 1;
            }
        }
        m( 2, 2 ) = 12;
        m( 3, 3 ) = 15;

        wmath::WMatrix<double> m_inv = wmath::invertMatrix4x4( m );

        wmath::WMatrix<double> id = wmath::WMatrix<double>( 4, 4 ).makeIdentity();

        wmath::WMatrix<double> m_m_inv = m * m_inv;

        TS_ASSERT( m_m_inv == id );
    }
};

#endif  // WLINEARALGEBRAFUNCTIONS_TEST_H
