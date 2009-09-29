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

#ifndef WMODULECONNECTOR_TEST_H
#define WMODULECONNECTOR_TEST_H

#include <iostream>

#include <boost/shared_ptr.hpp>

#include <cxxtest/TestSuite.h>

#include "../WModuleConnector.h"
#include "../WModuleOutputConnector.h"
#include "../WModuleInputConnector.h"
#include "../WModule.h"
#include "../../common/WSegmentationFault.h"
#include "../exceptions/WModuleConnectorInitFailed.h"

/** 
 * Class implementing a simple mod    // required since pure virtualule, since proper testing of WModuleConnector itself is not usable.
 */
class WModuleImpl: public WModule
{
friend class WModuleConnectorTest;

public:
    WModuleImpl(): WModule()
    {
    }

    virtual ~WModuleImpl()
    {
    }

    // required since pure virtual
    virtual const std::string getName() const
    {
        return "testmodule";
    }

    // required since pure virtual
    virtual const std::string getDescription() const
    {
        return "testdesc";
    }

    virtual void initializeConnectors()
    {
        m_Input= boost::shared_ptr<WModuleInputConnector>(
                new WModuleInputConnector( shared_from_this(), "in1", "desc1" )
        );
        // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
        addConnector( m_Input );
        
        m_Output= boost::shared_ptr<WModuleOutputConnector>(
                new WModuleOutputConnector( shared_from_this(), "out1", "desc2" )
        );
        // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
        addConnector( m_Output );

        WModule::initializeConnectors();
    }

protected:

    virtual void threadMain()
    {
        // Since the modules run in a separate thread: such loops are possible
        while ( !m_FinishRequested )
        {
            // do fancy stuff
            sleep( 1 );
        }
    }

    virtual void notifyConnectionEstablished( boost::shared_ptr<WModuleConnector> /*here*/,
                                              boost::shared_ptr<WModuleConnector> /*there*/ )
    {
        std::cout << "hallo est" << std::endl;
    }

    virtual void notifyConnectionClosed( boost::shared_ptr<WModuleConnector> /*here*/,
                                              boost::shared_ptr<WModuleConnector> /*there*/ )
    {
        std::cout << "hallo clo" << std::endl;
    }

    virtual void notifyDataChange( boost::shared_ptr<WModuleConnector> /*input*/,
                                   boost::shared_ptr<WModuleConnector> /*output*/ )
    {
        std::cout << "hallo change" << std::endl;
    }

private:

    boost::shared_ptr<WModuleInputConnector> m_Input;
    boost::shared_ptr<WModuleOutputConnector> m_Output;

};


/**
 * Tests the WModuleConnector class. We use WModuleConnector's direct derived classes WModuleInputConnector and
 * WModuleOutputConnector to test their common functionality implemented in WModuleConnector (which has pure virtual members -> so
 * can't be instantiated directly).
 */
class WModuleConnectorTest : public CxxTest::TestSuite
{
public:

    /** 
     * Tests the signal handler management.
     */
    void testModuleConnectors( void )
    {
        // install signal handler as early as possible
        //WSegmentationFault::installSignalHandler();

        // init 2 separate test modules
        boost::shared_ptr<WModuleImpl> m1 = boost::shared_ptr<WModuleImpl>( new WModuleImpl() );
        boost::shared_ptr<WModuleImpl> m2 = boost::shared_ptr<WModuleImpl>( new WModuleImpl() );

        // check whether there are NO connectors.
        // The constructor should now create connectors since shared_ptr are needed -> init in constructor leads to exception
        TS_ASSERT( m1->m_InputConnectors.size() == 0 );
        TS_ASSERT( m1->m_OutputConnectors.size() == 0 );
        TS_ASSERT( m2->m_InputConnectors.size() == 0 );
        TS_ASSERT( m2->m_OutputConnectors.size() == 0 );

        // init connectors
        // TODO(ebaum): replace this with the module container, since the module container should manage this
        // well actually this also tests the WModule::addConnector method and instantiation of WModuleInputConnector and
        // WModuleOutputConnector.
        TS_ASSERT_THROWS_NOTHING( m1->initializeConnectors() );
        TS_ASSERT_THROWS_NOTHING( m2->initializeConnectors() );

        // now there should be 1 everywhere
        TS_ASSERT( m1->m_InputConnectors.size() == 1 );
        TS_ASSERT( m1->m_OutputConnectors.size() == 1 );
        TS_ASSERT( m2->m_InputConnectors.size() == 1 );
        TS_ASSERT( m2->m_OutputConnectors.size() == 1 );

        // try initializing twice
        TS_ASSERT_THROWS( m1->initializeConnectors(), WModuleConnectorInitFailed );

        // now we have 2 properly initialized modules?
        TS_ASSERT( m1->isInitialized() );
        TS_ASSERT( m2->isInitialized() );

        // initialize connector
        






    }

    /** 
     * Test proper subscription to signals.
     */
    void testSignalSubscription( void )
    {
        TS_ASSERT( true );
    }

    /**
     * Connect/Disconnect Modules.
     */
    void testConnection( void )
    {
        TS_ASSERT( true );
    }

    /** 
     * Tests disconnection of multiple connections.
     */
    void testDisconnectAll( void )
    {
        TS_ASSERT( true );
    }

};

#endif  // WMODULECONNECTOR_TEST_H


