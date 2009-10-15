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

#ifndef WGRIDREGULAR3D_TEST_H
#define WGRIDREGULAR3D_TEST_H

#include <cxxtest/TestSuite.h>

#include "../WGridRegular3D.h"
#include "../../math/WVector3D.h"

using wmath::WVector3D;
using wmath::WPosition;

namespace CxxTest
{
CXXTEST_TEMPLATE_INSTANTIATION
class ValueTraits<WVector3D>
{
    char _s[256];

public:
    explicit ValueTraits( const WVector3D &m )
    {
        snprintf( _s, sizeof( _s ), "WVector3D( %.18f %.18f %.18f )", m[0], m[1], m[2] );
    }
    const char *asString() const
    {
        return _s;
    }
};
}

/**
 * Tests the WGridRegular3D class.
 */
class WGridRegular3DTest : public CxxTest::TestSuite
{
public:
    /**
     * Called before every test.
     */
    void setUp( void )
    {
        m_delta = 1e-14;
    }

    /**
     * Ensure that nothing is thrown when an instance is created.
     */
    void testInstantiation( void )
    {
        TS_ASSERT_THROWS_NOTHING( WGridRegular3D grid( 3, 3, 3, 1., 1., 1. ) );
        TS_ASSERT_THROWS_NOTHING( WGridRegular3D grid( 0., 0., 0., 3, 3, 3, 1., 1., 1. ) );
        TS_ASSERT_THROWS_NOTHING( WGridRegular3D grid( 0., 0., 0., 3, 3, 3,
                WVector3D( 3., 1., 2. ), WVector3D( 1., 3., 2. ), WVector3D( 1., 2., 3. ) ) );
    }

    /**
     * After instantiation there should be the requested number of positions.
     */
    void testSize( void )
    {
        WGridRegular3D grid( 3, 3, 3, 1., 1., 1. );
        TS_ASSERT_EQUALS( grid.size(), 27 );

        WGridRegular3D grid2( 0., 0., 0., 3, 3, 3, 1., 1., 1. );
        TS_ASSERT_EQUALS( grid2.size(), 27 );

        WGridRegular3D grid3( 0., 0., 0., 3, 3, 3,
                WVector3D( 3., 1., 2. ), WVector3D( 1., 3., 2. ), WVector3D( 1., 2., 3. ) );
        TS_ASSERT_EQUALS( grid3.size(), 27 );
    }

    /**
     * getNbCoords should return the samples prescribed by the use of the constructor
     */
    void testGetNbCoords( void )
    {
        size_t x = 3;
        size_t y = 4;
        size_t z = 5;
        WGridRegular3D grid( x, y, z, 1., 1., 1. );
        TS_ASSERT_EQUALS( grid.getNbCoordsX(), x );
        TS_ASSERT_EQUALS( grid.getNbCoordsY(), y );
        TS_ASSERT_EQUALS( grid.getNbCoordsZ(), z );
    }

    /**
     * getOffset should return the scalar offsets prescribed by the use of the
     * constructor
     */
    void testGetScalarOffset( void )
    {
        double x = 1.2;
        double y = 3.4;
        double z = 5.6;
        WGridRegular3D grid( 10, 10, 10, x, y, z );
        TS_ASSERT_EQUALS( grid.getOffsetX(), x );
        TS_ASSERT_EQUALS( grid.getOffsetY(), y );
        TS_ASSERT_EQUALS( grid.getOffsetZ(), z );
    }

    /**
     * getOffset should return the vector offsets prescribed by the use of the
     * constructor
     */
    void testGetVectorOffset( void )
    {
        WVector3D x( 3., 1., 2. );
        WVector3D y( 2., 6., 4. );
        WVector3D z( 3., 6., 9. );
        WGridRegular3D grid( 0., 0., 0., 10, 10, 10, x, y, z );
        TS_ASSERT_DELTA( grid.getOffsetX(), x.norm(), m_delta );
        TS_ASSERT_DELTA( grid.getOffsetY(), y.norm(), m_delta );
        TS_ASSERT_DELTA( grid.getOffsetZ(), z.norm(), m_delta );
    }

    /**
     * getOrigin should return the origin prescribed by the use of the constructor
     * or (0,0,0) when using the second constructor.
     */
    void testGetOrigin( void )
    {
        WVector3D zeroOrigin( 0., 0., 0. );
        WGridRegular3D grid( 10, 10, 10, 1., 1., 1. );
        TS_ASSERT_EQUALS( grid.getOrigin(), zeroOrigin );

        WVector3D origin( 1.2, 3.4, 5.6 );
        WGridRegular3D grid2( origin[0], origin[1], origin[2], 10, 10, 10, 1., 1., 1. );
        TS_ASSERT_EQUALS( grid2.getOrigin(), origin );
    }

    /**
     * getPosition should return the correct position for scalar offsets
     */
    void testGetPositionScalarOffset( void )
    {
        unsigned int nX = 10, nY = 11, nZ = 12;
        unsigned int iX = 8, iY = 9, iZ = 5;
        unsigned int i = iX + iY * nX + iZ * nX * nY;

        double orX = 1.2;
        double orY = 3.4;
        double orZ = 5.6;

        double ofX = 1.1;
        double ofY = 2.2;
        double ofZ = 3.3;

        double x = orX + iX * ofX;
        double y = orY + iY * ofY;
        double z = orZ + iZ * ofZ;

        WPosition expected( x, y, z );
        WGridRegular3D grid( orX, orY, orZ, nX, nY, nZ, ofX, ofY, ofZ );

        TS_ASSERT_DELTA( grid.getPosition( iX, iY, iZ )[0], expected[0], m_delta );
        TS_ASSERT_DELTA( grid.getPosition( iX, iY, iZ )[1], expected[1], m_delta );
        TS_ASSERT_DELTA( grid.getPosition( iX, iY, iZ )[2], expected[2], m_delta );
        TS_ASSERT_DELTA( grid.getPosition( i )[0], expected[0], m_delta );
        TS_ASSERT_DELTA( grid.getPosition( i )[1], expected[1], m_delta );
        TS_ASSERT_DELTA( grid.getPosition( i )[2], expected[2], m_delta );
    }

    /**
     * getPosition should return the correct position for vector offsets
     */
    void testGetPositionVectorOffset( void )
    {
        unsigned int nX = 10, nY = 11, nZ = 12;
        unsigned int iX = 8, iY = 9, iZ = 5;
        unsigned int i = iX + iY * nX + iZ * nX * nY;

        double orX = 1.2;
        double orY = 3.4;
        double orZ = 5.6;

        WVector3D ofX( 3., 1., 2. );
        WVector3D ofY( 1., 3., 2. );
        WVector3D ofZ( 1., 2., 3. );

        WPosition expected = WPosition( orX, orY, orZ ) + iX * ofX + iY * ofY + iZ * ofZ;
        WGridRegular3D grid( orX, orY, orZ, nX, nY, nZ, ofX, ofY, ofZ );

        TS_ASSERT_DELTA( grid.getPosition( iX, iY, iZ )[0], expected[0], m_delta );
        TS_ASSERT_DELTA( grid.getPosition( iX, iY, iZ )[1], expected[1], m_delta );
        TS_ASSERT_DELTA( grid.getPosition( iX, iY, iZ )[2], expected[2], m_delta );
        TS_ASSERT_DELTA( grid.getPosition( i )[0], expected[0], m_delta );
        TS_ASSERT_DELTA( grid.getPosition( i )[1], expected[1], m_delta );
        TS_ASSERT_DELTA( grid.getPosition( i )[2], expected[2], m_delta );
    }

private:
    double m_delta;
};

#endif  // WGRIDREGULAR3D_TEST_H
