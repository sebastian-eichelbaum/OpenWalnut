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

#ifndef WDATAHANDLER_TEST_H
#define WDATAHANDLER_TEST_H

#include <boost/shared_ptr.hpp>

#include <cxxtest/TestSuite.h>

#include "../../common/WLogger.h"
#include "../WSubject.h"
#include "../WDataHandler.h"

/**
 * Test important functionality of WDataHandler class
 */
class WDataHandlerTest : public CxxTest::TestSuite
{
public:

    /**
     * Setup logger and other stuff for each test.
     */
    void setUp()
    {
        WLogger::startup();
    }

    /**
     * Instatiation should throw nothing.
     */
    void testInstantiation()
    {
        TS_ASSERT_THROWS_NOTHING( WDataHandler() );
    }

    /**
     * Singleton getter should create an instance
     */
    void testSingleton()
    {
        TS_ASSERT_THROWS_NOTHING( WDataHandler::getDataHandler() );
        TS_ASSERT( WDataHandler::getDataHandler().get() );
    }

    /**
     * Test adding and iterating subjects
     */
    void testAddSubjects()
    {
        boost::shared_ptr< WDataHandler > dh = WDataHandler::getDataHandler();

        WPersonalInformation testInfo( WPersonalInformation::createDummyInformation() );
        testInfo.setSubjectID( 1 );
        testInfo.setLastName( "Testname" );

        WSubject* s = new WSubject( testInfo );
        TS_ASSERT_THROWS_NOTHING( dh->addSubject( boost::shared_ptr< WSubject >( s ) ) );
        TS_ASSERT_EQUALS( 2, dh->m_subjects.size() );   // note: this is 2 since the datahandler always provides a default subject

        // test iteration
        WDataHandler::SubjectSharedContainerType::ReadTicket a = dh->getSubjects();

        // iterate the list and find all textures
        int count = 0;
        for( WDataHandler::SubjectContainerType::const_iterator iter = a->get().begin(); iter != a->get().end(); ++iter )
        {
            count++;

            // the second one needs to be the added subject
            TS_ASSERT( ( count == 1 ) || ( s == ( *iter ).get() ) );
        }

        TS_ASSERT( count == 2 );
    }

    /**
     * Test the remove and clean functionality.
     */
    void testRemoveSubjects()
    {
        boost::shared_ptr< WDataHandler > dh = WDataHandler::getDataHandler();

        WPersonalInformation testInfo( WPersonalInformation::createDummyInformation() );
        testInfo.setSubjectID( 2 );
        testInfo.setLastName( "Testname2" );

        boost::shared_ptr< WSubject > s = boost::shared_ptr< WSubject >( new WSubject( testInfo ) );
        dh->addSubject( s );

        // now there should be 3 subjects (one from testAddSubjects, the above added one and the default subject)
        TS_ASSERT_EQUALS( 3, dh->m_subjects.size() );   // note: this is 2 since the datahandler always provides a default subject
        dh->removeSubject( s );
        TS_ASSERT_EQUALS( 2, dh->m_subjects.size() );   // note: this is 2 since the datahandler always provides a default subject
        dh->clear();
        TS_ASSERT_EQUALS( 0, dh->m_subjects.size() );   // note: this is 2 since the datahandler always provides a default subject
    }
};

#endif  // WDATAHANDLER_TEST_H

