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

#ifndef WPROPERTYVARIABLE_TEST_H
#define WPROPERTYVARIABLE_TEST_H

#include <string>

#include <cxxtest/TestSuite.h>

#include "../WPropertyVariable.h"
#include "../constraints/WPropertyConstraintMin.h"
#include "../constraints/WPropertyConstraintMax.h"

#include "../exceptions/WPropertyNotUnique.h"
#include "../exceptions/WPropertyUnknown.h"
#include "../exceptions/WPropertyNameMalformed.h"

/**
 * Test WPropertyVariable
 */
class WPropertyVariableTest : public CxxTest::TestSuite
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
        boost::shared_ptr< WPropertyVariable< bool > > p;
        TS_ASSERT_THROWS_NOTHING( p =  boost::shared_ptr< WPropertyVariable< bool > >( new WPropertyVariable< bool >( "hey", "you", false ) ) );

        // test names
        TS_ASSERT( p->getName() == "hey" );
        TS_ASSERT( p->getDescription() == "you" );
        TS_ASSERT( p->getType() == PV_BOOL );

        TS_ASSERT_THROWS_NOTHING( p.reset() );
    }

    /**
     * Tests that only properties with proper names can be created
     */
    void testNameConvention( void )
    {
        WException::disableBacktrace(); // in tests, turn of backtrace globally

        boost::shared_ptr< WPropertyVariable< bool > > p;
        TS_ASSERT_THROWS( p = boost::shared_ptr< WPropertyVariable< bool > >( new WPropertyVariable< bool >( "hey/you", "you", false ) ),
                          WPropertyNameMalformed );
    }

    /**
     * Tests the cloning functionality.
     */
    void testClone()
    {
        WException::disableBacktrace(); // in tests, turn of backtrace globally

        /////////////////////////
        // Create an original

        // create an int property
        boost::shared_ptr< WPropertyVariable< int > > p =
            boost::shared_ptr< WPropertyVariable< int > >( new WPropertyVariable< int >( "hey", "you", false ) );
        // add a min/max prop
        WPropertyVariable< int >::PropertyConstraintMin cmin = p->setMin( 0 );
        WPropertyVariable< int >::PropertyConstraintMax cmax = p->setMax( 9 );
        p->set( 5 );

        /////////////////////////
        // Clone it

        boost::shared_ptr< WPropertyVariable< int > > clone = p->clone()->toPropInt();

        // some rudimentary tests (from WPropertyBase)
        TS_ASSERT( clone );
        TS_ASSERT( clone->getType() == PV_INT );
        TS_ASSERT( clone->getName() == p->getName() );
        TS_ASSERT( clone->getDescription() == p->getDescription() );
        TS_ASSERT( clone->getPurpose() == p->getPurpose() );

        // equal value?
        TS_ASSERT( p->get() == clone->get() );

        // different conditions?
        TS_ASSERT( p->getContraintsChangedCondition() != clone->getContraintsChangedCondition() );
        TS_ASSERT( p->getUpdateCondition() != clone->getUpdateCondition() );        // this is from WPropertyBase

        // cloned constraints?
        TS_ASSERT( p->getMin() != clone->getMin() ); // the constraints of course need to be cloned too
        TS_ASSERT( p->getMax() != clone->getMax() ); // the constraints of course need to be cloned too

        TS_ASSERT( p->getMin()->getMin() == clone->getMin()->getMin()  ); // but their values need to be the same. This somehow tests the clone
                                                                          // feature of WPropertyConstrainMin and Max
        TS_ASSERT( p->getMax()->getMax() == clone->getMax()->getMax()  ); // but their values need to be the same. This somehow tests the clone
                                                                          // feature of WPropertyConstrainMin and Max

        // check independence of both update conditions
        p->getUpdateCondition()->subscribeSignal( boost::bind( &WPropertyVariableTest::setTemporary1, this ) );
        clone->getUpdateCondition()->subscribeSignal( boost::bind( &WPropertyVariableTest::setTemporary2, this ) );

        // set the value of the clone -> fire condition of clone but not of original
        m_testTemporary1 = false;
        m_testTemporary2 = false;
        clone->set( 4 );
        TS_ASSERT( !m_testTemporary1 );
        TS_ASSERT( m_testTemporary2 );

        // and test vice versa
        m_testTemporary1 = false;
        m_testTemporary2 = false;
        p->set( 2 );
        TS_ASSERT( m_testTemporary1 );
        TS_ASSERT( !m_testTemporary2 );

        // do the same for constraints
        m_testTemporary1 = false;
        m_testTemporary2 = false;
        clone->removeConstraint( PC_MIN );
        TS_ASSERT( !m_testTemporary1 );
        TS_ASSERT( m_testTemporary2 );

        // and vice versa
        m_testTemporary1 = false;
        m_testTemporary2 = false;
        p->removeConstraint( PC_MIN );
        TS_ASSERT( m_testTemporary1 );
        TS_ASSERT( !m_testTemporary2 );
    }

    /**
     * Test min/max functionality, including tests for set(), accept() and ensureValidity.
     */
    void testMinMaxWithSetAndAccept()
    {
        WException::disableBacktrace(); // in tests, turn of backtrace globally

        // create an int property
        boost::shared_ptr< WPropertyVariable< int > > p =
            boost::shared_ptr< WPropertyVariable< int > >( new WPropertyVariable< int >( "hey", "you", false ) );

        // by default there should be no min/max property set. Only IF the property was created using a WProperties::addProperty.
        WPropertyVariable< int >::PropertyConstraintMin cmin = p->getMin();
        WPropertyVariable< int >::PropertyConstraintMax cmax = p->getMax();
        TS_ASSERT( !cmin );
        TS_ASSERT( !cmax );

        // does set() and accept work if no constraints are there?
        TS_ASSERT( p->set( 123 ) );
        TS_ASSERT( p->get() == 123 );
        TS_ASSERT( p->accept( 12345 ) );

        // add a min prop
        cmin = p->setMin( 10 );
        cmax = p->setMax( 15 );
        TS_ASSERT( cmin );
        TS_ASSERT( cmax );

        // compare that getMin/max returns the correct ones
        TS_ASSERT( cmin == p->getMin() );
        TS_ASSERT( cmax == p->getMax() );

        // try to set a valid value
        TS_ASSERT( p->set( 10 ) );
        TS_ASSERT( p->get() == 10 );

        // try to set an invalid value
        TS_ASSERT( !p->set( 9 ) );
        TS_ASSERT( p->get() == 10 );
        TS_ASSERT( !p->set( 16 ) );
        TS_ASSERT( p->get() == 10 );

        // add another min value. Is the first one removed?
        p->setMin( 5 );
        p->setMax( 20 );
        p->m_constraints->getReadTicket()->get().size();

        // try to set a valid value, which was invalid previously
        TS_ASSERT( p->set( 9 ) );
        TS_ASSERT( p->get() == 9 );
        TS_ASSERT( p->set( 16 ) );
        TS_ASSERT( p->get() == 16 );

        // finally, test ensureValidity
        // this function helps to restore a property to a valid state after a constraint change

        // currently, the state of p is valid. So ensureValidity should do nothing
        TS_ASSERT( p->ensureValidity( 10 ) );
        TS_ASSERT( p->get() == 16 );

        // change the min constraint so that 16 gets invalid
        TS_ASSERT( p->isValid() );
        p->setMin( 17 );
        TS_ASSERT( !p->isValid() );
        TS_ASSERT( p->get() == 16 );    // setting a new constraint should NOT modify the current value

        // use ensureValidity
        TS_ASSERT( p->ensureValidity( 18 ) );
        TS_ASSERT( p->get() == 18 );
        TS_ASSERT( p->isValid() );

        // what happens if the ensureValidity parameter itself is invalid? It should return false
        p->setMin( 19 );
        TS_ASSERT( !p->ensureValidity( 16 ) );    // 16 is invalid since minimum is 19
        TS_ASSERT( !p->isValid() );                // the value should stay invalid
        TS_ASSERT( p->get() == 18 );
    }

    /**
     * Tests constraint management. Especially add,replace,remove,count,getFirst.
     */
    void testConstraintManagement( void )
    {
        WException::disableBacktrace(); // in tests, turn of backtrace globally

        // create an int property
        boost::shared_ptr< WPropertyVariable< int > > p =
            boost::shared_ptr< WPropertyVariable< int > >( new WPropertyVariable< int >( "hey", "you", false ) );

        // register a condition callback
        p->getUpdateCondition()->subscribeSignal( boost::bind( &WPropertyVariableTest::setTemporary1, this ) );

        ////////////////////////////////////
        // add

        // add a constraint
        m_testTemporary1 = false;
        WPropertyVariable< int >::PropertyConstraintMin cmin =
            boost::shared_ptr< WPropertyConstraintMin< int > >( new  WPropertyConstraintMin< int >( 10 ) );
        p->addConstraint( cmin );
        TS_ASSERT( p->m_constraints->getReadTicket()->get().size() == 1 );
        TS_ASSERT( m_testTemporary1 );    // the update condition has to be fired on constraint updates

        ////////////////////////////////////
        // count, getFirst

        // count constraints
        m_testTemporary1 = false;
        TS_ASSERT( p->countConstraint( PC_MIN ) == 1 );
        TS_ASSERT( p->countConstraint( PC_MAX ) == 0 );

        // get first constraint should return the first constraint of a specified type
        TS_ASSERT( cmin == p->getFirstConstraint( PC_MIN ) );
        TS_ASSERT( !p->getFirstConstraint( PC_MAX ) );  // there is no max constraint
        TS_ASSERT( !m_testTemporary1 );      // these operations should not fire the condition

        ////////////////////////////////////
        // replace

        // replace a constraint
        m_testTemporary1 = false;
        WPropertyVariable< int >::PropertyConstraintMax cmax =
            boost::shared_ptr< WPropertyConstraintMax< int > >( new  WPropertyConstraintMax< int >( 15 ) );

        // replace non existent type
        TS_ASSERT_THROWS_NOTHING( p->replaceConstraint( cmax, PC_MAX ) );  // since there is no max constraint, replace acts like addConstraint
        TS_ASSERT( m_testTemporary1 );

        // replace existent type ( note: there is now a min and a max constraint )
        m_testTemporary1 = false;
        WPropertyVariable< int >::PropertyConstraintMax cmax2 =
            boost::shared_ptr< WPropertyConstraintMax< int > >( new  WPropertyConstraintMax< int >( 20 ) );
        p->replaceConstraint( cmax2, PC_MAX );
        TS_ASSERT( m_testTemporary1 );
        TS_ASSERT( cmax2 == p->getFirstConstraint( PC_MAX ) );

        ////////////////////////////////////
        // remove

        // removeConstraints should not fire the condition if nothing is removed
        m_testTemporary1 = false;
        p->removeConstraint( PC_NOTEMPTY );
        TS_ASSERT( !m_testTemporary1 );

        // remove max constraint
        m_testTemporary1 = false;
        TS_ASSERT( p->countConstraint( PC_MAX ) == 1 );
        p->removeConstraint( PC_MAX );
        TS_ASSERT( p->countConstraint( PC_MAX ) == 0 );
        TS_ASSERT( m_testTemporary1 );  // should have fired

        // remove min constraint with pointer
        m_testTemporary1 = false;
        TS_ASSERT( p->countConstraint( PC_MIN ) == 1 );
        p->removeConstraint( cmin );
        TS_ASSERT( p->countConstraint( PC_MIN ) == 0 );
        TS_ASSERT( m_testTemporary1 );  // should have fired
    }
};

#endif  // WPROPERTYVARIABLE_TEST_H

