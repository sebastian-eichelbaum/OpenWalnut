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

#ifndef WJOINCONTOURTREE_TEST_H
#define WJOINCONTOURTREE_TEST_H

#include <set>
#include <vector>

#include <cxxtest/TestSuite.h>

#include "../../../common/WLogger.h"

#include "../WJoinContourTree.h"

/**
 * Unit tests the Join Tree of the Contour Tree!
 */
class WJoinContourTreeTest : public CxxTest::TestSuite
{
public:

    /**
     * The construction of a Join Tree is done via a special index array.
     */
    void testbuildJoinTreeOnRegular2DGrid( void )
    {
        // Expected JoinTree for this example:
        /**
        // 15
        //  \       14
        //   13      \
        //    \       \
        //     12      \
        //      \       \
        //       11      \
        //        \       \   10
        //         \       \ /
        //          \       9
        //           \     /
        //            \   8
        //             \ /
        //              5
        //              |
        //              4
        //              |
        //              3
        //              |
        //              2
        //              |
        //              1
        //              |
        //              0
        //              |
        //             -1
        //              |
        //             -3
        */
        //                0  1  2  3  4  5   6  7   8   9  10  11  12  13  14  15
        size_t data[] = { 4, 9, 3, 3, 5, 1,  7, 2, 12, 13, 11, 14,  6,  8,  9, 11 }; // NOLINT
        std::vector< size_t > expectedJT( data, data + 16 );
        WJoinContourTree jt( m_dataset );
        jt.buildJoinTree();
        TS_ASSERT_EQUALS( jt.m_joinTree, expectedJT );
    }

    /**
     * All voxels enclosed by the biggest isosurface are contained in the biggest component
     * of the JoinTree above the given isovalue the in in the JoinTree.
     */
    void testGetVolumeVoxelsEnclosedByIsoSurfaceWithOutMerge( void )
    {
        size_t data[] = { 0, 4, 5, 1 }; // NOLINT
        std::set< size_t > expected( data, data + 4 );

        WJoinContourTree jt( m_dataset );
        jt.buildJoinTree();
        using string_utils::operator<<;
        TS_ASSERT_EQUALS( expected, *jt.getVolumeVoxelsEnclosedByIsoSurface( 8.3 ) );
    }

    /**
     * All voxels enclosed by the biggest isoSurface are contained in the biggest component
     * which may be created with some merges of the join tree.
     */
    void testGetVolumeVoxelsEnclosedByIsoSurfaceWithMerges( void )
    {
        size_t data[] = { 0, 4, 5, 1, 10, 11, 14, 15, 13, 9 }; // NOLINT
        std::set< size_t > expected( data, data + 10 );

        WJoinContourTree jt( m_dataset );
        jt.buildJoinTree();
        using string_utils::operator<<;
        TS_ASSERT_EQUALS( expected, *jt.getVolumeVoxelsEnclosedByIsoSurface( 4.0 ) );
    }

protected:
    /**
     * Creates an example dataset so I hope its easy to test the join tree.
     */
    void setUp()
    {
        WLogger::startup();

        // isovalues:           Point id's:
        //   2--- 4--- 8---14     12---13---14---15
        //   |    |    |    |      |    |    |    |
        //   |    |    |    |      |    |    |    |
        //   3--- 5---10--- 9      8--- 9---10---11
        //   |    |    |    |      |    |    |    |
        //   |    |    |    |      |    |    |    |
        //  13---12--- 1--- 0      4--- 5--- 6--- 7
        //   |    |    |    |      |    |    |    |
        //   |___ |___ |____|      |___ |___ |____|
        //  15   11   -1   -3      0    1    2    3

        boost::shared_ptr< WGridRegular3D > grid( new WGridRegular3D( 4, 4, 1 ) );
        double isoValuesData[] = { 15, 11, -1, -3, 13, 12, 1, 0, 3, 5, 10, 9, 2, 4, 8, 14 }; // NOLINT
        boost::shared_ptr< std::vector< double > > isoValues =
            boost::shared_ptr< std::vector< double > >( new std::vector< double >( isoValuesData, isoValuesData + 16 ) );
        boost::shared_ptr< WValueSet< double > > valueset( new WValueSet< double >( 0, 1, isoValues, W_DT_DOUBLE ) );
        m_dataset = boost::shared_ptr< WDataSetSingle >( new WDataSetSingle( valueset, grid ) );
    }

    /**
     * Tidy up things created during setUp
     */
    void tearDown( void )
    {
        m_dataset.reset();
    }

    boost::shared_ptr< WDataSetSingle > m_dataset; //!< Dataset which is used to create the join tree
};

#endif  // WJOINCONTOURTREE_TEST_H
