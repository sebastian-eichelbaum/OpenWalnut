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

#include <string>

#include <cxxtest/TestSuite.h>

#include "../WMetaInfo.h"

class WMetaInfoTest : public CxxTest::TestSuite
{
public:
    /**
     * As normal an instantiation should never throw anything.
     */
    void testInstantiation( void )
    {
        TS_ASSERT_THROWS_NOTHING( WMetaInfo metaInfo() );
    }

    /**
     * The name returned is the same which is used internally
     */
    void testGetName( void )
    {
        WMetaInfo metaInfo;
        metaInfo.m_name = std::string( "TestName" );
        TS_ASSERT_EQUALS( metaInfo.getName(), "TestName" );
    }

    /**
     * When the name will be reset, the one used internally is equal.
     */
    void testSetName( void )
    {
        WMetaInfo metaInfo;
        TS_ASSERT_THROWS_NOTHING( metaInfo.setName( "TestName" ) );
        TS_ASSERT_EQUALS( metaInfo.m_name, "TestName" );
    }

    /**
     * The file name returned is the same which is used internally
     */
    void testGetFileName( void )
    {
        WMetaInfo metaInfo;
        metaInfo.m_fileName = std::string( "TestFileName" );
        TS_ASSERT_EQUALS( metaInfo.getFileName(), "TestFileName" );
    }

    /**
     * When the file name will be reset, the one used internally is equal.
     */
    void testSetFileName( void )
    {
        WMetaInfo metaInfo;
        TS_ASSERT_THROWS_NOTHING( metaInfo.setFileName( "TestFileName" ) );
        TS_ASSERT_EQUALS( metaInfo.m_fileName, "TestFileName" );
    }
};

#endif  // WMETAINFO_TEST_H
