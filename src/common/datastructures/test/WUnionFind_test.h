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

#ifndef WUNIONFIND_TEST_H
#define WUNIONFIND_TEST_H

#include <set>
#include <vector>

#include <cxxtest/TestSuite.h>

#include "../WUnionFind.h"

/**
 * Unit tests the WUnionFind datastructure.
 */
class WUnionFindTest : public CxxTest::TestSuite
{
public:
    /**
     * The union always ensure that the new canonical element is the biggest
     * index.
     */
    void testUnionMergesToBiggerIndex( void )
    {
        WUnionFind uf( 5 );
        uf.merge( 4, 0 );
        size_t data[] = { 0, 1, 2, 3, 0 }; // NOLINT
        std::vector< size_t > expected( data, data + 5 );
        TS_ASSERT_EQUALS( uf.m_component, expected );
        uf.merge( 1, 3 );
        expected[1] = 3;
        TS_ASSERT_EQUALS( uf.m_component, expected );
    }

    /**
     * Ensure that only the maximal set is returned, and nothing else.
     */
    void testMaxSet( void )
    {
        WUnionFind uf( 10 );
        uf.merge( 0, 1 );
        for( int i = 2; i < 6; ++i )
        {
            uf.merge( i, i + 1 );
        }
        size_t data[] = { 2, 3, 4, 5, 6 };
        std::set< size_t > expected( data, data + 5 );
        TS_ASSERT_EQUALS( *uf.getMaxSet(), expected );
    }
};

#endif  // WUNIONFIND_TEST_H
