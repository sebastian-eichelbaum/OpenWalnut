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

#ifndef WNOSUCHDATASETEXCEPTION_TEST_H
#define WNOSUCHDATASETEXCEPTION_TEST_H

#include <string>

#include <cxxtest/TestSuite.h>

#include "../WNoSuchDataSetException.h"

/**
 * UnitTests the WNoSuchDataSetException class
 */
class WNoSuchDataSetExceptionTest : public CxxTest::TestSuite
{
public:
    /**
     * An instantiation should never throw an exception.
     */
    void testInstantiation( void )
    {
        TS_ASSERT_THROWS_NOTHING( WNoSuchDataSetException e() );
        TS_ASSERT_THROWS_NOTHING( WNoSuchDataSetException e( "Some message" ) );
    }

    /**
     * Getting the message means every trace element should be returned.
     */
    void testGetMessage( void )
    {
        WNoSuchDataSetException e( "Dummy exception" );
        e.m_trace.push_back( "first" );
        e.m_trace.push_back( "second" );
        std::string expected = "Dummy exception\n\ntrace: first\ntrace: second";
        TS_ASSERT_EQUALS( expected, e.getTrace() );
        WNoSuchDataSetException f;
        TS_ASSERT_EQUALS( std::string(), f.getTrace() );
    }
};

#endif  // WNOSUCHDATASETEXCEPTION_TEST_H
