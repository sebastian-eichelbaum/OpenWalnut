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
     * A temporary holder for some value.
     */
    bool m_testTemporary1;

    /**
     * A temporary holder for some value.
     */
    bool m_testTemporary2;

    /**
     * Helper function which simply sets the value above to true. It is used to test some conditions here.
     */
    void setTemporary1()
    {
        m_testTemporary1 = true;
    }

    /**
     * Helper function which simply sets the value above to true. It is used to test some conditions here.
     */
    void setTemporary2()
    {
        m_testTemporary2 = true;
    }

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
        TS_ASSERT( p->m_properties.getReadTicket()->get().size() == 3 );

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
        TS_ASSERT( p->m_properties.getReadTicket()->get().size() == 3 );

        // clear
        TS_ASSERT_THROWS_NOTHING( p->clear() );
        TS_ASSERT( p->m_properties.getReadTicket()->get().size() == 0 );

        // multiple clear should not cause any error
        TS_ASSERT_THROWS_NOTHING( p->clear() );
    }

    /**
     * Test the removeProperty() method
     */
    void testRemove( void )
    {
        WException::disableBacktrace(); // in tests, turn of backtrace globally

        boost::shared_ptr< WProperties > p = boost::shared_ptr< WProperties >( new WProperties( "hey", "you" ) );

        // add some new properties
        boost::shared_ptr< WPropertyBase > p1 = p->addProperty( "1", "test1", true );
        boost::shared_ptr< WPropertyBase > p2 = p->addProperty( "2", "test2", 1 );
        boost::shared_ptr< WPropertyBase > p3 = p->addProperty( "3", "test3", 1.0 );

        // this should have created 3 props
        TS_ASSERT( p->m_properties.getReadTicket()->get().size() == 3 );

        // remove a property
        TS_ASSERT_THROWS_NOTHING( p->removeProperty( p2 ) );
        TS_ASSERT( p->m_properties.getReadTicket()->get().size() == 2 );

        // remove a prop which is not in the list
        TS_ASSERT_THROWS_NOTHING( p->removeProperty( p2 ) );
        TS_ASSERT( p->m_properties.getReadTicket()->get().size() == 2 );
    }

    /**
     * Tests whether the properties children can be set by the WProperties::set call using a other WProperties instance
     */
    void testRecursiveSetByProperty( void )
    {
        WException::disableBacktrace(); // in tests, turn of backtrace globally

        // some props we can use as target
        boost::shared_ptr< WProperties > t = boost::shared_ptr< WProperties >( new WProperties( "hey", "you" ) );

        // add some new properties
        WPropBool tp1 = t->addProperty( "p1", "", true );
        WPropInt tp2 = t->addProperty( "p2", "", 1 );
        WPropGroup tg1 = t->addPropertyGroup( "g1", "" );
        WPropDouble tp3 = tg1->addProperty( "p3", "", 1.0 );
        WPropDouble tp4 = t->addProperty( "p4", "", 10.0 );

        // create a group we can use as source
        boost::shared_ptr< WProperties > s = boost::shared_ptr< WProperties >( new WProperties( "hey", "you" ) );

        // add some new properties
        WPropBool sp1 = s->addProperty( "p1", "", false );
        WPropInt sp2 = s->addProperty( "p2__", "", 10 ); // NOTE: the name is different
        WPropGroup sg1 = s->addPropertyGroup( "g1", "" );
        WPropDouble sp3 = sg1->addProperty( "p3", "", 2.0 );
        WPropInt sp4 = s->addProperty( "p4", "", 2 );

        // let us set t using the values in s
        t->set( s );

        // lets check the values:
        // tp1 should be set to the value of sp1
        TS_ASSERT( tp1->get() == sp1->get() );
        // tp2 should be untouched as no corresponding property exists in s
        TS_ASSERT( tp2->get() == 1 );
        // the child of the group g1 should be set to sp3
        TS_ASSERT( tp3->get() == sp3->get() );
        // tp4 must not be sp4 even if the names match. The type is a mismatch
        TS_ASSERT( tp4->get() == 10.0 );
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
        boost::shared_ptr< WPropertyBase > p2 = p->addProperty( "2", "test2", 1 );
        boost::shared_ptr< WPropertyBase > p3 = psub->addProperty( "3", "test3", 1.0 );
        boost::shared_ptr< WPropertyBase > p4 = psub->addProperty( "4", "test4", std::string( "hello" ) );

        // insert a prop with the same name as a sub property
        TS_ASSERT_THROWS( p->addProperty( "heySub", "test1", true ), WPropertyNotUnique );

        /////////////
        // exists

        // try to find a property of a group in the parent: should fail
        TS_ASSERT( !p->existsProperty( "3" ) );
        TS_ASSERT( !p->existsProperty( "4" ) );
        TS_ASSERT( psub->existsProperty( "3" ) );
        TS_ASSERT( psub->existsProperty( "4" ) );
        TS_ASSERT( !psub->existsProperty( "1" ) );
        TS_ASSERT( !psub->existsProperty( "2" ) );

        // search it with the proper name:
        TS_ASSERT( p->existsProperty( "heySub/3" ) );
        TS_ASSERT( !p->existsProperty( "heySub/1" ) );

        /////////////
        // find

        // search it with the proper name:
        TS_ASSERT( p3 == p->findProperty( "heySub/3" ) );
        TS_ASSERT( p4 == p->findProperty( "heySub/4" ) );

        // ensure nothing is found if wrong name is specified
        TS_ASSERT( boost::shared_ptr< WPropertyBase >() == p->findProperty( "heySub/1" ) );

        /////////////
        // get

        TS_ASSERT_THROWS_NOTHING( p->getProperty( "heySub/3" ) );
        TS_ASSERT_THROWS_NOTHING( p->getProperty( "heySub/4" ) );

        // ensure nothing is found if wrong name is specified
        TS_ASSERT_THROWS( p->getProperty( "heySub/1" ), WPropertyUnknown );
    }

    /**
     * Tests the cloning functionality.
     */
    void testClone()
    {
        /////////////////////
        // Clone

        boost::shared_ptr< WProperties > orig = boost::shared_ptr< WProperties >( new WProperties( "hey", "you" ) );
        boost::shared_ptr< WProperties > clone = orig->clone()->toPropGroup();

        // test that toPropGroup worked and both are different
        TS_ASSERT( clone.get() );
        TS_ASSERT( orig != clone );

        /////////////////////
        // Conditions

        // is there a new condition? This has to be the case, this mainly situated in WPropertyBase
        TS_ASSERT( orig->getUpdateCondition() != clone->getUpdateCondition() );

        // update of property list does not modify the original
        clone->addProperty( "1", "test1", 1.0 );
        TS_ASSERT( clone->m_properties.getReadTicket()->get().size() == 1 );
        TS_ASSERT( orig->m_properties.getReadTicket()->get().size() == 0 );

        // does the condition fire on add?
        // first, register some callbacks to test it
        m_testTemporary1 = false;
        m_testTemporary2 = false;
        orig->getUpdateCondition()->subscribeSignal( boost::bind( &WPropertiesTest::setTemporary1, this ) );
        clone->getUpdateCondition()->subscribeSignal( boost::bind( &WPropertiesTest::setTemporary2, this ) );

        // add a bool property -> conditions fired?
        clone->addProperty( "2", "test2", false );

        // the first should not fire, but the condition of the clone
        TS_ASSERT( m_testTemporary1 == false );
        TS_ASSERT( m_testTemporary2 == true );

        // the same thing but vice versa
        m_testTemporary1 = false;
        m_testTemporary2 = false;
        orig->addProperty( "1", "test1", false );
        // this time, the first should fire but not the second
        TS_ASSERT( m_testTemporary2 == false );
        TS_ASSERT( m_testTemporary1 == true );

        /////////////////////
        // cloned list

        // the clone now contains some properties -> clone it again and check the list of contained properties
        boost::shared_ptr< WProperties > cloneClone = clone->clone()->toPropGroup();

        // same size?
        TS_ASSERT( clone->m_properties.getReadTicket()->get().size() == 2 );
        TS_ASSERT( cloneClone->m_properties.getReadTicket()->get().size() == 2 );

        WProperties::PropertySharedContainerType::ReadTicket t = clone->getProperties();

        // iterate the original and check that there exists a cloned property in the cloned one
        for( WProperties::PropertyConstIterator iter = t->get().begin(); iter != t->get().end(); ++iter )
        {
            // ensure there is a corresponding property in cloneClone
            boost::shared_ptr< WPropertyBase > p = cloneClone->findProperty( ( *iter )->getName() );
            TS_ASSERT( p ); // found?
            TS_ASSERT( p != ( *iter ) ); // is it really a clone? (the cloning functionality of WPropertyVariable is tested separately
        }
    }
};

#endif  // WPROPERTIES_TEST_H


