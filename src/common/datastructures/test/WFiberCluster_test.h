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

#ifndef WFIBERCLUSTER_TEST_H
#define WFIBERCLUSTER_TEST_H

#include <cxxtest/TestSuite.h>

#include "../WFiberCluster.h"

/**
 * Unit test the WFiberCluster class
 */
class WFiberClusterTest : public CxxTest::TestSuite
{
public:
    /**
     * When merging two clusters the indices are merged and the second cluster
     * becomes empty.
     */
    void testMerge( void )
    {
        WFiberCluster a( 1 );
        WFiberCluster b;
        size_t mydata[] = { 16, 2, 77, 29 }; // NOLINT
        std::list< size_t > data( mydata, mydata + sizeof( mydata ) / sizeof( size_t ) );
        b.m_memberIndices = data;
        a.merge( b );
        TS_ASSERT( b.empty() );
        size_t mxdata[] = { 1, 16, 2, 77, 29 }; // NOLINT
        std::list< size_t > xdata( mxdata, mxdata + sizeof( mxdata ) / sizeof( size_t ) );
        WFiberCluster expected;
        expected.m_memberIndices = xdata;
        TS_ASSERT_EQUALS( expected, a );
    }
};

#endif  // WFIBERCLUSTER_TEST_H
