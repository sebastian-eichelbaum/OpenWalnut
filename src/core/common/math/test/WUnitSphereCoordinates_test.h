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

#ifndef WUNITSPHERECOORDINATES_TEST_H
#define WUNITSPHERECOORDINATES_TEST_H

#include <string>
#include <vector>
#include <algorithm>

#include <cxxtest/TestSuite.h>

#include "core/common/WLogger.h"
#include "core/common/math/WGeometryFunctions.h"
#include "core/common/math/WSymmetricSphericalHarmonic.h"

#include "../WUnitSphereCoordinates.h"

/**
 * Test class for the WUnitSphereCoordinates template.
 */
class WUnitSphereCoordinatesTest : public CxxTest::TestSuite
{
public:
    /**
     * Test implicit conversion between sphere and euclidean coordinates.
     */
    void testSphereToEuclideanConversion()
    {
        // 1. test case
        WVector3d n( 1.0, 0.0, 0.0 );
        WUnitSphereCoordinates< double > m( n );
        WVector3d n2(  m.getEuclidean() );
        for( size_t i = 0; i < 3; i++ )
        {
            TS_ASSERT_DELTA( n[ i ], n2[ i ], 0.001 );
        }

        // 2. test case
        n[ 0  ] = 1.0;
        n[ 1  ] = 0.5;
        n[ 2  ] = 1.0;
        m.setEuclidean( n );
        n2 = m.getEuclidean();
        n = normalize( n );
        for( size_t i = 0; i < 3; i++ )
        {
            TS_ASSERT_DELTA( n[ i ], n2[ i ], 0.001 );
        }
    }

private:
};

#endif  // WUNITSPHERECOORDINATES_TEST_H
