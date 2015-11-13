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

#ifndef WGRIDTRANSFORMORTHO_TEST_H
#define WGRIDTRANSFORMORTHO_TEST_H

#include <cstdio>
#include <sstream>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <cxxtest/TestSuite.h>

#include "../../common/exceptions/WPreconditionNotMet.h"

#include "../WGridTransformOrtho.h"

/**
 * Tests the WGridTransform class.
 */
class WGridTransformTest : public CxxTest::TestSuite
{
public:
    /**
     * Test if all data fields get initialized correctly. Constructors should throw
     * a WPreconditionNotMet exception if any input values are invalid.
     */
    void testInstantiation()
    {
        {
            TS_ASSERT_THROWS_NOTHING( WGridTransformOrtho v );
            WGridTransformOrtho v;
            TS_ASSERT_EQUALS( v.getOffsetX(), 1.0 );
            TS_ASSERT_EQUALS( v.getOffsetY(), 1.0 );
            TS_ASSERT_EQUALS( v.getOffsetZ(), 1.0 );
            compareVectors( v.getUnitDirectionX(), WVector3d( 1.0, 0.0, 0.0 ), 0.0001 );
            compareVectors( v.getUnitDirectionY(), WVector3d( 0.0, 1.0, 0.0 ), 0.0001 );
            compareVectors( v.getUnitDirectionZ(), WVector3d( 0.0, 0.0, 1.0 ), 0.0001 );
            compareVectors( v.getDirectionX(), WVector3d( 1.0, 0.0, 0.0 ), 0.0001 );
            compareVectors( v.getDirectionY(), WVector3d( 0.0, 1.0, 0.0 ), 0.0001 );
            compareVectors( v.getDirectionZ(), WVector3d( 0.0, 0.0, 1.0 ), 0.0001 );
            compareVectors( v.getOrigin(), WVector3d( 0.0, 0.0, 0.0 ), 0.0001 );
        }
        {
            TS_ASSERT_THROWS_NOTHING( WGridTransformOrtho v( 2.2, 3.3, -1.0 ) );
            TS_ASSERT_THROWS( WGridTransformOrtho v( 0.0, 0.0, 1.0 ), WPreconditionNotMet );
            TS_ASSERT_THROWS( WGridTransformOrtho v( 0.0, 2.0, 1.0 ), WPreconditionNotMet );
            TS_ASSERT_THROWS( WGridTransformOrtho v( 1.0, 1.0, 0.0 ), WPreconditionNotMet );
        }
        {
            WGridTransformOrtho v( 2.2, 3.3, -1.0 );
            TS_ASSERT_EQUALS( v.getOffsetX(), 2.2 );
            TS_ASSERT_EQUALS( v.getOffsetY(), 3.3 );
            TS_ASSERT_EQUALS( v.getOffsetZ(), 1.0 );
            compareVectors( v.getUnitDirectionX(), WVector3d( 1.0, 0.0, 0.0 ), 0.0001 );
            compareVectors( v.getUnitDirectionY(), WVector3d( 0.0, 1.0, 0.0 ), 0.0001 );
            compareVectors( v.getUnitDirectionZ(), WVector3d( 0.0, 0.0, -1.0 ), 0.0001 );
            compareVectors( v.getDirectionX(), WVector3d( 2.2, 0.0, 0.0 ), 0.0001 );
            compareVectors( v.getDirectionY(), WVector3d( 0.0, 3.3, 0.0 ), 0.0001 );
            compareVectors( v.getDirectionZ(), WVector3d( 0.0, 0.0, -1.0 ), 0.0001 );
            compareVectors( v.getOrigin(), WVector3d( 0.0, 0.0, 0.0 ), 0.0001 );
        }
        {
            WMatrix< double > mat( 4, 4 );
            mat.makeIdentity();
            mat( 0, 0 ) = 2.2;
            mat( 1, 1 ) = 3.3;
            mat( 2, 2 ) = 0.0;

            TS_ASSERT_THROWS( WGridTransformOrtho v( mat ), WPreconditionNotMet );
        }
        {
            WMatrix< double > mat( 4, 4 );
            mat.makeIdentity();
            mat( 0, 0 ) = 2.2;
            mat( 1, 0 ) = 0.1;
            mat( 1, 1 ) = 3.3;
            mat( 2, 2 ) = 1.0;

            TS_ASSERT_THROWS( WGridTransformOrtho v( mat ), WPreconditionNotMet );
        }
        {
            WMatrix< double > mat( 4, 4 );
            mat.makeIdentity();
            mat( 0, 0 ) = 2.0;
            mat( 1, 0 ) = 2.0;
            mat( 1, 1 ) = 3.0;
            mat( 0, 1 ) = -3.0;
            mat( 2, 2 ) = 4.4;
            mat( 0, 3 ) = 1.0;
            mat( 1, 3 ) = 2.0;
            mat( 2, 3 ) = 0.5;

            WGridTransformOrtho v( mat );
            TS_ASSERT_EQUALS( v.getOffsetX(), sqrt( 8.0 ) );
            TS_ASSERT_EQUALS( v.getOffsetY(), sqrt( 18.0 ) );
            TS_ASSERT_EQUALS( v.getOffsetZ(), 4.4 );
            TS_ASSERT_DELTA( length( v.getUnitDirectionX() - WVector3d( 0.5 * sqrt( 2.0 ), 0.5 * sqrt( 2.0 ), 0.0 ) ), 0.0, 1e-13 );
            TS_ASSERT_DELTA( length( v.getUnitDirectionY() - WVector3d( -0.5 * sqrt( 2.0 ), 0.5 * sqrt( 2.0 ), 0.0 ) ), 0.0, 1e-13 );
            compareVectors( v.getUnitDirectionZ(), WVector3d( 0.0, 0.0, 1.0 ), 0.0001 );
            compareVectors( v.getDirectionX(), WVector3d( 2.0, 2.0, 0.0 ), 0.0001 );
            compareVectors( v.getDirectionY(), WVector3d( -3.0, 3.0, 0.0 ), 0.0001 );
            compareVectors( v.getDirectionZ(), WVector3d( 0.0, 0.0, 4.4 ), 0.0001 );
            compareVectors( v.getOrigin(), WVector3d( 1.0, 2.0, 0.5 ), 0.0001 );
        }
    }

    /**
     * Different constructors should not yield differently initialized
     * data fields.
     */
    void testCompareConstructors()
    {
        WMatrix< double > mat( 4, 4 );
        mat.makeIdentity();
        mat( 0, 0 ) = 2.2;
        mat( 1, 1 ) = 3.3;
        mat( 2, 2 ) = 4.4;

        WGridTransformOrtho t1( mat );
        WGridTransformOrtho t2( 2.2, 3.3, 4.4 );

        TS_ASSERT_EQUALS( t1.getOffsetX(), t2.getOffsetX() );
        TS_ASSERT_EQUALS( t1.getOffsetY(), t2.getOffsetY() );
        TS_ASSERT_EQUALS( t1.getOffsetZ(), t2.getOffsetZ() );

        compareVectors( t1.getDirectionX(), t2.getDirectionX(), 0.0001 );
        compareVectors( t1.getDirectionY(), t2.getDirectionY(), 0.0001 );
        compareVectors( t1.getDirectionZ(), t2.getDirectionZ(), 0.0001 );

        compareVectors( t1.getOrigin(), t2.getOrigin(), 0.0001 );
    }

    /**
     * Test transformation from grid space to world space.
     */
    void testTransformationToWorldSpace()
    {
        {
            // test identity transform
            WVector3d v( -7.64, 8.73, -0.0063 );
            WGridTransformOrtho t;

            compareVectors( v, t.positionToWorldSpace( v ), 0.0001 );
            compareVectors( v, t.directionToWorldSpace( v ), 0.0001 );
        }

        {
            WGridTransformOrtho t( 2.2, 3.3, 4.4 );
            WVector3d v( 1.0, 1.0, 1.0 );

            compareVectors( WVector3d( 2.2, 3.3, 4.4 ), t.positionToWorldSpace( v ), 0.0001 );
            compareVectors( WVector3d( 2.2, 3.3, 4.4 ), t.directionToWorldSpace( v ), 0.0001 );
        }

        {
            WMatrix< double > mat( 4, 4 );
            mat.makeIdentity();
            mat( 0, 0 ) = 2.2;
            mat( 1, 1 ) = 3.3;
            mat( 2, 2 ) = 4.4;
            mat( 0, 3 ) = 1.0;
            mat( 1, 3 ) = 2.0;
            mat( 2, 3 ) = 0.5;

            WGridTransformOrtho t( mat );
            WVector3d v( 1.0, 1.0, 1.0 );

            compareVectors( WVector3d( 3.2, 5.3, 4.9 ), t.positionToWorldSpace( v ), 0.0001 );
            compareVectors( WVector3d( 2.2, 3.3, 4.4 ), t.directionToWorldSpace( v ), 0.0001 );
        }
        {
            WMatrix< double > mat( 4, 4 );
            mat.makeIdentity();
            mat( 0, 0 ) = 2.0;
            mat( 1, 0 ) = 2.0;
            mat( 1, 1 ) = 3.0;
            mat( 0, 1 ) = -3.0;
            mat( 2, 2 ) = 4.4;
            mat( 0, 3 ) = 1.0;
            mat( 1, 3 ) = 2.0;
            mat( 2, 3 ) = 0.5;

            WGridTransformOrtho t( mat );
            WVector3d v( 1.0, 1.0, 1.0 );

            WVector3d w = t.positionToWorldSpace( v );
            TS_ASSERT_DELTA( 0.0, w[ 0 ], 0.0001 );
            TS_ASSERT_DELTA( 7.0, w[ 1 ], 0.0001 );
            TS_ASSERT_DELTA( 4.9, w[ 2 ], 0.0001 );
            compareVectors( WVector3d( -1.0, 5.0, 4.4 ), t.directionToWorldSpace( v ), 0.0001 );
        }
    }

    /**
     * Test transformation from world space to grid space.
     */
    void testTransformationToGridSpace()
    {
        {
            // test identity transform
            WVector3d v( -7.64, 8.73, -0.0063 );
            WGridTransformOrtho t;

            compareVectors( v, t.positionToGridSpace( v ), 0.0001 );
            compareVectors( v, t.directionToGridSpace( v ), 0.0001 );
        }

        {
            WGridTransformOrtho t( 2.2, 3.3, 4.4 );
            WVector3d v( 2.2, 3.3, 4.4 );

            compareVectors( WVector3d( 1.0, 1.0, 1.0 ), t.positionToGridSpace( v ), 0.0001 );
            compareVectors( WVector3d( 1.0, 1.0, 1.0 ), t.directionToGridSpace( v ), 0.0001 );
        }

        {
            WMatrix< double > mat( 4, 4 );
            mat.makeIdentity();
            mat( 0, 0 ) = 2.2;
            mat( 1, 1 ) = 3.3;
            mat( 2, 2 ) = 4.4;
            mat( 0, 3 ) = 1.0;
            mat( 1, 3 ) = 2.0;
            mat( 2, 3 ) = 0.5;

            WGridTransformOrtho t( mat );

            compareVectors( WVector3d( 1.0, 1.0, 1.0 ), t.positionToGridSpace( WVector3d( 3.2, 5.3, 4.9 ) ), 0.0001 );
            compareVectors( WVector3d( 1.0, 1.0, 1.0 ), t.directionToGridSpace( WVector3d( 2.2, 3.3, 4.4 ) ), 0.0001 );
        }
        {
            WMatrix< double > mat( 4, 4 );
            mat.makeIdentity();
            mat( 0, 0 ) = 2.0;
            mat( 1, 0 ) = 2.0;
            mat( 1, 1 ) = 3.0;
            mat( 0, 1 ) = -3.0;
            mat( 2, 2 ) = 4.4;
            mat( 0, 3 ) = 1.0;
            mat( 1, 3 ) = 2.0;
            mat( 2, 3 ) = 0.5;

            WGridTransformOrtho t( mat );

            TS_ASSERT_DELTA( length( WVector3d( 1.0, 1.0, 1.0 )  // NOLINT
                               - t.positionToGridSpace( WVector3d( 0.0, 7.0, 4.9 ) ) ), 0.0, 1e-13 );
            TS_ASSERT_DELTA( length( WVector3d( 1.0, 1.0, 1.0 )  // NOLINT
                               - t.directionToGridSpace( WVector3d( -1.0, 5.0, 4.4 ) ) ), 0.0, 1e-13 );
        }
    }

private:
    /**
     * Compares two vectors, element by element.
     *
     * \param v1 The first vector.
     * \param v2 The second vector.
     * \param delta The maximum absolute difference between the elements of the vectors.
     */
    void compareVectors( WVector3d const& v1, WVector3d const& v2, double delta ) const
    {
        TS_ASSERT_DELTA( v1[ 0 ], v2[ 0 ], delta );
        TS_ASSERT_DELTA( v1[ 1 ], v2[ 1 ], delta );
        TS_ASSERT_DELTA( v1[ 2 ], v2[ 2 ], delta );
    }
};

#endif  // WGRIDTRANSFORMORTHO_TEST_H
