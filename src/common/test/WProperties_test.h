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

#ifndef WPROPERTIES_TEST_H
#define WPROPERTIES_TEST_H

#include <string>

#include <cxxtest/TestSuite.h>

#include "../WProperties.h"
#include "../exceptions/WPropertyNotUnique.h"
#include "../exceptions/WPropertyUnknown.h"
#include "../exceptions/WPropertyNameMalformed.h"

/**
 * Test WProperties
 */
class WPropertiesTest : public CxxTest::TestSuite
{
public:

    /**
     * Test instantiation, also test name and description and type (from WPropertyBase)
     */
    void testInstantiation( void )
    {
        boost::shared_ptr< WProperties > p;
        TS_ASSERT_THROWS_NOTHING( p =  boost::shared_ptr< WProperties >( new WProperties( "hey", "you" ) ) );

        // test names
        TS_ASSERT( p->getName() == "hey" );
        TS_ASSERT( p->getDescription() == "you" );
        TS_ASSERT( p->getType() == PV_GROUP );

        TS_ASSERT_THROWS_NOTHING( p.reset() );
    }

    /**
     * Test the add features, also tests the type of properties added
     */
    void testAdd( void )
    {
        WException::disableBacktrace(); // in tests, turn of backtrace globally

        boost::shared_ptr< WProperties > p = boost::shared_ptr< WProperties >( new WProperties( "hey", "you" ) );

        // add some new properties
        boost::shared_ptr< WPropertyBase > p1 = p->addProperty( "1", "test1", true );
        boost::shared_ptr< WPropertyBase > p2 = p->addProperty( "2", "test2", 1 );
        boost::shared_ptr< WPropertyBase > p3 = p->addProperty( "3", "test3", 1.0 );

        // add a malformed (name) property
        // The name is malformed since the "/" is used as group separator
        TS_ASSERT_THROWS( p->addProperty( "4/5", "test4", 1.0 ), WPropertyNameMalformed );

        // this should have created 3 props
        // NOTE: we can use the propAccess directly here since there is no multi--threading in this test
        TS_ASSERT( p->m_propAccess->get().size() == 3 );

        // ensure that it has created the correct types:
        TS_ASSERT( p1->getType() == PV_BOOL );
        TS_ASSERT( p2->getType() == PV_INT );
        TS_ASSERT( p3->getType() == PV_DOUBLE );

        // try to add another property with the same name ( regardless of actual type )
        TS_ASSERT_THROWS( p->addProperty( "1", "test1", 1.0 ), WPropertyNotUnique );
    }

    /**
     * Test the clear() method
     */
    void testClear( void )
    {
        WException::disableBacktrace(); // in tests, turn of backtrace globally

        boost::shared_ptr< WProperties > p = boost::shared_ptr< WProperties >( new WProperties( "hey", "you" ) );

        // add some new properties
        boost::shared_ptr< WPropertyBase > p1 = p->addProperty( "1", "test1", true );
        boost::shared_ptr< WPropertyBase > p2 = p->addProperty( "2", "test2", 1 );
        boost::shared_ptr< WPropertyBase > p3 = p->addProperty( "3", "test3", 1.0 );

        // this should have created 3 props
        // NOTE: we can use the propAccess directly here since there is no multi--threading in this test
        TS_ASSERT( p->m_propAccess->get().size() == 3 );

        // clear
        TS_ASSERT_THROWS_NOTHING( p->clear() );
        TS_ASSERT( p->m_propAccess->get().size() == 0 );

        // multiple clear should not cause any error
        TS_ASSERT_THROWS_NOTHING( p->clear() );
    }

    /**
     * Test the features to find and get properties.
     */
    void testGetAndExistsAndFind( void )
    {
        WException::disableBacktrace(); // in tests, turn of backtrace globally

        boost::shared_ptr< WProperties > p = boost::shared_ptr< WProperties >( new WProperties( "hey", "you" ) );

        // add some new properties
        boost::shared_ptr< WPropertyBase > p1 = p->addProperty( "1", "test1", true );
        boost::shared_ptr< WPropertyBase > p2 = p->addProperty( "2", "test2", 1 );
        boost::shared_ptr< WPropertyBase > p3 = p->addProperty( "3", "test3", 1.0 );

        /////////////
        // exists

        // now, try to check whether a property exists:
        TS_ASSERT( p->existsProperty( "1" ) );
        TS_ASSERT( !p->existsProperty( "shouldNotBeInTheList" ) );

        /////////////
        // find

        // same for find. Find does not throw an exception if the property does not exist! It simply returns it or NULL
        boost::shared_ptr< WPropertyBase > someProp;
        TS_ASSERT_THROWS_NOTHING( someProp = p->findProperty( "1" ) );
        // The property exists -> return value is not NULL
        TS_ASSERT( someProp );

        // now for an unexisting one
        TS_ASSERT_THROWS_NOTHING( someProp = p->findProperty( "shouldNotBeInTheList" ) );
        // The property exists -> return value is not NULL
        TS_ASSERT( !someProp );

        /////////////
        // get

        // the getProperty method throws an exception if the property has not been found.

        // this one exists -> no exception
        TS_ASSERT_THROWS_NOTHING( someProp = p->getProperty( "1" ) );
        TS_ASSERT( someProp );

        // this one does not exist
        TS_ASSERT_THROWS( someProp = p->getProperty( "shouldNotBeInTheList" ), WPropertyUnknown );
    }

    /**
     * Test the recursive search mechanism.
     */
    void testGetAndExistsAndFindRecursive( void )
    {
        boost::shared_ptr< WProperties > p = boost::shared_ptr< WProperties >( new WProperties( "hey", "you" ) );
        boost::shared_ptr< WProperties > psub = p->addPropertyGroup( "heySub", "you" );

        // add some new properties
        boost::shared_ptr< WPropertyBase > p1 = p->addProperty( "1", "test1", true );
        boost::shared_ptr< WPropertyBase > p2 = psub->addProperty( "1", "test2", 1 );
        boost::shared_ptr< WPropertyBase > p3 = psub->addProperty( "2", "test3", 1.0 );

        // insert a prop with the same name as a sub property
        TS_ASSERT_THROWS( p->addProperty( "heySub", "test1", true ), WPropertyNotUnique );

        /////////////
        // exists

        // try to find a property of a group in the parent: should fail
        TS_ASSERT( !p->existsProperty( "2" ) );
        TS_ASSERT( psub->existsProperty( "2" ) );
        // search it with the proper name:
        //TS_ASSERT( p->existsProperty( "1/1" ) );

        /////////////
        // find

        /////////////
        // get
    }
};

#endif  // WPROPERTIES_TEST_H


