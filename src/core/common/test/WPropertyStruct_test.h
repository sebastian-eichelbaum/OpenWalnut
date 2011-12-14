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

#ifndef WPROPERTYSTRUCT_TEST_H
#define WPROPERTYSTRUCT_TEST_H

#include <string>

#include <cxxtest/TestSuite.h>

#include "../WPropertyTypes.h"
#include "../WPropertyVariable.h"
#include "../WPropertyStruct.h"

/**
 * Test WPropertyStruct.
 */
class WPropertyStructTest : public CxxTest::TestSuite
{
public:
    /**
     * Test instantiation, also test name and description and type (from WPropertyBase)
     */
    void testInstantiation( void )
    {
        typedef WPropertyStruct< WPropInt, WPropBool > TestStruct;

        TestStruct* prop = new TestStruct( "Hallo", "Description Text" );
        TS_ASSERT( prop->size() == 2 );

        // although this is not a proper test, it fails compilation and therefore informs the programmer that he did something wrong
        BOOST_MPL_ASSERT( ( boost::is_same< TestStruct::TupleType, boost::tuple<  WPropInt, WPropBool > > ) );
        BOOST_MPL_ASSERT( ( boost::is_same< TestStruct::TypeVector,
                                            boost::mpl::vector<  WPropInt, WPropBool > > // NOLINT
                        ) );
    }

    /**
     * Test the set method
     */
    void testSet( void )
    {
        // do not test setting the properties here using one of the getProperty methods. Setting properties directly is tested in the appropriate
        // tests

        // we test getting/setting via string here

        // create the prop
        typedef WPropertyStruct< WPropInt, WPropBool >::SPtr TestStruct;
        TestStruct prop( new TestStruct::element_type( "Hallo", "Description Text" ) );

        // set some defaults
        prop->getProperty< 0 >()->set( 12 );
        prop->getProperty< 1 >()->set( true );

        // get as string
        std::string got = prop->getAsString();

        // change the value a little bit
        prop->getProperty< 0 >()->set( 111 );
        prop->getProperty< 1 >()->set( false );

        // set by string and check values
        prop->setAsString( got );

        TS_ASSERT( prop->getProperty< 0 >()->get() == 12 );
        TS_ASSERT( prop->getProperty< 1 >()->get() == true );

        // also test setting via property
        TestStruct prop2( new TestStruct::element_type( "Hallo2", "Description Text" ) );
        prop2->set( prop );
        TS_ASSERT( prop2->getProperty< 0 >()->get() == 12 );
        TS_ASSERT( prop2->getProperty< 1 >()->get() == true );
    }

    /**
     * Test getter
     */
    void testGet( void )
    {
        typedef WPropertyStruct< WPropInt, WPropBool > TestStruct;
        TestStruct prop( "Hallo", "Description Text" );

        // compile time test: this fails during compilation if something is wrong
        WPropInt i = prop.getProperty< 0 >();
        TS_ASSERT( i.get() );
        WPropBool b = prop.getProperty< 1 >();
        TS_ASSERT( b.get() );

        // get the second prop
        WPropertyBase::SPtr base = prop.getProperty( 1 );
        TS_ASSERT( base.get() );

        // this has to be a bool prop
        TS_ASSERT( base->toPropBool().get() );
    }
};

#endif  // WPROPERTYSTRUCT_TEST_H



