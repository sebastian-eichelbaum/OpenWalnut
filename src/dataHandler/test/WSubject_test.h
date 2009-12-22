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

#ifndef WSUBJECT_TEST_H
#define WSUBJECT_TEST_H

#include <string>
#include <cxxtest/TestSuite.h>

#include "../WSubject.h"
#include "../WDataSet.h"

/**
 * The tests for our subject class.
 */
class WSubjectTest : public CxxTest::TestSuite
{
public:
    /**
     * Test instantiation of objects of WSubject class
     */
    void testInstantiation( void )
    {
        TS_ASSERT_THROWS_NOTHING( WSubject() );
    }

    /**
     * Test instantiation of objects of WSubject class with name
     */
    void testInstantiationWithName( void )
    {
        WPersonalInformation testInfo( WPersonalInformation::createDummyInformation() );
        TS_ASSERT_THROWS_NOTHING( WSubject( testInfo ) );
    }

    /**
     * Test whether we have intialized the object correctly.
     */
    void TestStandardConstructor()
    {
        WSubject dummySubject;
        TS_ASSERT_EQUALS( 0, dummySubject.m_dataSets.size() );
        TS_ASSERT_EQUALS( 0, dummySubject.m_personalInfo.getSubjectID() );
    }

    /**
     * Test whether we have put the info where it belongs and intialized the rest.
     */
    void TestConstructorWithInfo()
    {
        WPersonalInformation testInfo( WPersonalInformation::createDummyInformation() );
        WSubject dummySubject( testInfo );
        TS_ASSERT_EQUALS( testInfo, dummySubject.m_personalInfo );
    }


    /**
     * Test whether we can retrieve the right info with getName function.
     */
    void testGetName()
    {
        WPersonalInformation testInfo( WPersonalInformation::createDummyInformation() );
        testInfo.setSubjectID( 1 );
        testInfo.setLastName( "Testname" );
        WSubject dummySubject( testInfo );
        TS_ASSERT_EQUALS( testInfo.getLastName()+",  " , dummySubject.getName() );
    }

    /**
     * Test adding of data sets.
     */
    void testAddDataSet()
    {
        boost::shared_ptr< WDataSet > dummyDataSet;
        dummyDataSet = boost::shared_ptr< WDataSet >( new WDataSet );
        std::string fileName = "Hallo";
        dummyDataSet->setFileName( fileName );

        WSubject dummySubject;
        dummySubject.addDataSet( dummyDataSet );
        TS_ASSERT_EQUALS( 1, dummySubject.m_dataSets.size() );
        TS_ASSERT_EQUALS( fileName, dummySubject.m_dataSets[0]->getFileName() );
        TS_ASSERT_EQUALS( dummyDataSet, dummySubject.m_dataSets[0] );
    }

    /**
     * Test retrieving data sets.
     */
    void testGetDataSet()
    {
        boost::shared_ptr< WDataSet > dummyDataSet;
        dummyDataSet = boost::shared_ptr< WDataSet >( new WDataSet );
        std::string fileName = "Hallo";
        dummyDataSet->setFileName( fileName );

        WSubject dummySubject;
        dummySubject.addDataSet( dummyDataSet );
        TS_ASSERT_EQUALS( dummyDataSet, dummySubject.getDataSet( 0 ) );
    }

    /**
     * Test retrieving data sets with [].
     */
    void testAcessOperator()
    {
        boost::shared_ptr< WDataSet > dummyDataSet;
        dummyDataSet = boost::shared_ptr< WDataSet >( new WDataSet );
        std::string fileName = "Hallo";
        dummyDataSet->setFileName( fileName );

        WSubject dummySubject;
        dummySubject.addDataSet( dummyDataSet );
        TS_ASSERT_EQUALS( dummyDataSet, dummySubject[0] );
    }

    /**
     * Test getting number of datasets.
     */
    void testGetNumberOfDataSet()
    {
        boost::shared_ptr< WDataSet > dummyDataSet;
        dummyDataSet = boost::shared_ptr< WDataSet >( new WDataSet );
        std::string fileName = "Hallo";
        dummyDataSet->setFileName( fileName );

        WSubject dummySubject;
        TS_ASSERT_EQUALS( 0, dummySubject.getNumberOfDataSets() );
        dummySubject.addDataSet( dummyDataSet );
        TS_ASSERT_EQUALS( 1, dummySubject.getNumberOfDataSets() );
        dummySubject.addDataSet( dummyDataSet );
        TS_ASSERT_EQUALS( 2, dummySubject.getNumberOfDataSets() );
        dummySubject.addDataSet( dummyDataSet );
        TS_ASSERT_EQUALS( 3, dummySubject.getNumberOfDataSets() );
    }
};

#endif  // WSUBJECT_TEST_H
