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

#ifndef WPERSONALINFORMATION_TEST_H
#define WPERSONALINFORMATION_TEST_H

#include <cxxtest/TestSuite.h>

#include "../WPersonalInformation.h"

/**
 * Tests for WPersonalInformation
 */
class WPersonalInformationTest : public CxxTest::TestSuite
{
public:
    /**
     * Test creation
     */
    void testCreation( void )
    {
        // The following would not compile: WPersonalInformation info;
        WPersonalInformation info( WPersonalInformation::createDummyInformation() );
    }

    /**
     * Test setting subjectID
     */
    void testSetSubjectID( void )
    {
        WPersonalInformation info( WPersonalInformation::createDummyInformation() );
        info.setSubjectID( 4711 );
        TS_ASSERT_EQUALS( 4711, info.m_subjectID );
    }

    /**
     * Test getting subjectID
     */
    void testGetSubjectID( void )
    {
        WPersonalInformation info( WPersonalInformation::createDummyInformation() );
        info.m_subjectID = 4711;
        TS_ASSERT_EQUALS( 4711, info.getSubjectID() );
    }

    /**
     * Test equality operator
     */
    void testEquality( void )
    {
        WPersonalInformation info( WPersonalInformation::createDummyInformation() );
        info.m_subjectID = 4711;
        info.m_lastName = "hallo";
        WPersonalInformation info2( WPersonalInformation::createDummyInformation() );
        info2.m_subjectID = 4711;
        info2.m_lastName = "hallo";
        TS_ASSERT_EQUALS( info == info2, true );
        info2.m_lastName = "hallo2";
        TS_ASSERT_EQUALS( info == info2, false );
    }

    /**
     * Test inequality operator
     */
    void testInequality( void )
    {
        WPersonalInformation info( WPersonalInformation::createDummyInformation() );
        info.m_subjectID = 4711;
        info.m_lastName = "hallo";
        WPersonalInformation info2( WPersonalInformation::createDummyInformation() );
        info2.m_subjectID = 4711;
        info2.m_lastName = "hallo2";
        TS_ASSERT_EQUALS( info != info2, true );
        info2.m_lastName = "hallo";
        TS_ASSERT_EQUALS( info != info2, false );
    }
};

#endif  // WPERSONALINFORMATION_TEST_H
