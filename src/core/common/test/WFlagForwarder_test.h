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

#ifndef WFLAGFORWARDER_TEST_H
#define WFLAGFORWARDER_TEST_H

#include <cxxtest/TestSuite.h>

#include "../WFlagForwarder.h"
#include "../WFlag.h"
#include "../WConditionOneShot.h"

/**
 * Test WFlagForwarder.
 */
class WFlagForwarderTest : public CxxTest::TestSuite
{
public:
    /**
     * Add some flags and test whether the value gets propagated properly.
     */
    void testPropagation( void )
    {
        // create some flags
        boost::shared_ptr< WFlag< int > > flagSource( new WFlag< int >( new WConditionOneShot(), 5 ) );
        boost::shared_ptr< WFlag< int > > flagTarget1( new WFlag< int >( new WConditionOneShot(), 10 ) );
        boost::shared_ptr< WFlag< int > > flagTarget2( new WFlag< int >( new WConditionOneShot(), 15 ) );

        // create the forwarder
        WFlagForwarder< int >* f = new WFlagForwarder< int >( flagSource );
        f->forward( flagTarget1 );
        f->forward( flagTarget2 );

        // now all flags should have value 5
        TS_ASSERT( flagSource->get() == 5 );
        TS_ASSERT( flagTarget1->get() == 5 );
        TS_ASSERT( flagTarget2->get() == 5 );

        // set some value to the source
        flagSource->set( 50 );

        // now all flags should have value 50
        TS_ASSERT( flagSource->get() == 50 );
        TS_ASSERT( flagTarget1->get() == 50 );
        TS_ASSERT( flagTarget2->get() == 50 );

        // changing the value of one target flag should not affect the others:
        flagTarget2->set( 100 );
        TS_ASSERT( flagSource->get() == 50 );
        TS_ASSERT( flagTarget1->get() == 50 );
        TS_ASSERT( flagTarget2->get() == 100 );
    }
};

#endif  // WFLAGFORWARDER_TEST_H


