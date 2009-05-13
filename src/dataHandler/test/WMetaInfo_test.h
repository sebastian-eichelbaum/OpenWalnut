//---------------------------------------------------------------------------
//
// Project: OpenWalnut
//
// Copyright 2009 SomeCopyrightowner
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

#ifndef WMETAINFO_TEST_H
#define WMETAINFO_TEST_H

#include <cxxtest/TestSuite.h>

#include "../WMetaInfo.h"

class WMetaInfoTest : public CxxTest::TestSuite
{
public:
    void testInstantiation( void )
    {
        TS_ASSERT_THROWS_NOTHING( WMetaInfo metaInfo );
    }
    void testGetAndSetName( void )
    {
        WMetaInfo metaInfo;
        TS_ASSERT_THROWS_NOTHING( metaInfo.setName( "TestName" ) );
        TS_ASSERT_THROWS_NOTHING( metaInfo.getName() );
        TS_ASSERT_EQUALS( metaInfo.getName(), "TestName" )
    }
    void testGetAndSetFileName( void )
    {
        WMetaInfo metaInfo;
        TS_ASSERT_THROWS_NOTHING( metaInfo.setFileName( "TestFileName" ) );
        TS_ASSERT_THROWS_NOTHING( metaInfo.getFileName() );
        TS_ASSERT_EQUALS( metaInfo.getFileName(), "TestFileName" )
    }
};

#endif  // WMETAINFO_TEST_H
