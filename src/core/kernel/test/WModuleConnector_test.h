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
#include <string>

#include <boost/shared_ptr.hpp>

#include <cxxtest/TestSuite.h>

#include "../WModuleConnector.h"
#include "../WModuleInputData.h"
#include "../WModuleOutputData.h"
#include "../WModuleInputConnector.h"
#include "../WModuleOutputConnector.h"
#include "../WModule.h"
#include "../../common/WSegmentationFault.h"
#include "../../common/WTransferable.h"
#include "../../common/WPrototyped.h"
#include "../../common/WLogger.h"
#include "../exceptions/WModuleConnectorInitFailed.h"
#include "../exceptions/WModuleConnectionFailed.h"
#include "../exceptions/WModuleConnectorsIncompatible.h"
#include "../exceptions/WModuleException.h"
#include "../exceptions/WModuleConnectorUnconnected.h"

/**
 * Test class used to test data transfer and compatibility checks.
 */
class WTestTransferableBase: public WTransferable
{
friend class WModuleConnectorTest;

public:
    /**
     * Constructor.
     */
    WTestTransferableBase(): WTransferable()
    {
        // do nothing here
        m_data = 0;
    };

    /**
     * Gets the name of this prototype.
     *
     * \return the name.
     */
    virtual const std::string getName() const
    {
        return "WTestTransferableBase";
    }

    /**
     * Gets the description for this prototype.
     *
     * \return the description
     */
    virtual const std::string getDescription() const
    {
        return "Test class for testing transfer of data.";
    }

    /**
     * Returns a prototype instantiated with the true type of the deriving class.
     *
     * \return the prototype.
     */
    static boost::shared_ptr< WPrototyped > getPrototype()
    {
        return boost::shared_ptr< WPrototyped >( new WTestTransferableBase() );
    }

    /**
     * Gives the magic int.
     *
     * \return the currently set data
     */
    int get() const
    {
        return m_data;
    }

    /**
     * Sets the new int.
     *
     * \param i the int used for testing.
     */
    void set( int i )
    {
        m_data = i;
    }

protected:
    /**
     * The data.
     */
    int m_data;

private:
};

/**
 * Derived test class used to test data transfer and compatibility checks, especially the inheritance checks.
 */
class WTestTransferableDerived: public WTestTransferableBase
{
friend class WModuleConnectorTest;

public:
    /**
     * Constructor.
     */
    WTestTransferableDerived(): WTestTransferableBase()
    {
    };

    /**
     * Gets the name of this prototype.
     *
     * \return the name.
     */
    virtual const std::string getName() const
    {
        return "WTestTransferableDerived";
    }

    /**
     * Gets the description for this prototype.
     *
     * \return the description
     */
    virtual const std::string getDescription() const
    {
        return "Test class for testing transfer of data.";
    }

    /**
     * Returns a prototype instantiated with the true type of the deriving class.
     *
     * \return the prototype.
     */
    static boost::shared_ptr< WPrototyped > getPrototype()
    {
        return boost::shared_ptr< WPrototyped >( new WTestTransferableDerived() );
    }

protected:
private:
};

/**
 * Class implementing a simple module since WModuleConnector itself is not usable for proper
 * testing itself because it is has pure virtual methods, i.e. is abstract.
 */
class WModuleImpl: public WModule
{
friend class WModuleConnectorTest;

public:
    /**
     * Constructor.
     *
     * \param n a string to test with (sets initial value).
     */
    explicit WModuleImpl( std::string n="?" ): WModule()
    {
        this->n = n;
    }

    /**
     * Destructor.
     */
    virtual ~WModuleImpl()
    {
    }

    /**
     * Create instance of this module class.
     *
     * \return new instance of this module.
     */
    virtual boost::shared_ptr< WModule > factory() const
    {
        return boost::shared_ptr< WModule >( new WModuleImpl() );
    }

    /**
     * Returns name of this module.
     *
     * \return the name of this module.
     */
    virtual const std::string getName() const
    {
        return "testmodule";
    }

    /**
     * Returns description of module.
     *
     * \return the description.
     */
    const std::string getDescription() const
    {
        return "testdesc";
    }

    /**
     * Set up connectors.
     */
    virtual void connectors()
    {
        m_input = boost::shared_ptr< WModuleInputData< WTestTransferableBase > >(
                new WModuleInputData< WTestTransferableBase > ( shared_from_this(), "in1", "desc1" )
        );
        // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
        addConnector( m_input );

        m_output = boost::shared_ptr< WModuleOutputData< WTestTransferableBase > >(
                new WModuleOutputData< WTestTransferableBase > ( shared_from_this(), "out1", "desc2" )
        );
        // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
        addConnector( m_output );

        // now, the same with the derived class as type
        m_inputDerived = boost::shared_ptr< WModuleInputData< WTestTransferableDerived > >(
                new WModuleInputData< WTestTransferableDerived > ( shared_from_this(), "in2", "desc1" )
        );
        // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
        addConnector( m_inputDerived );

        m_outputDerived = boost::shared_ptr< WModuleOutputData< WTestTransferableDerived > >(
                new WModuleOutputData< WTestTransferableDerived > ( shared_from_this(), "out2", "desc2" )
        );
        // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
        addConnector( m_outputDerived );
    }

protected:
    /**
     * temporary name string
     */
    std::string n;

    // required since pure virtual
    virtual void moduleMain()
    {
        // Since the modules run in a separate thread: such loops are possible
        while( !m_shutdownFlag() )
        {
            // do fancy stuff
            sleep( 1 );
        }
    }

    /**
     * Notifier called whenever a connection got established.
     */
    virtual void notifyConnectionEstablished( boost::shared_ptr< WModuleConnector > /*here*/,
                                              boost::shared_ptr< WModuleConnector > /*there*/ )
    {
        // std::cout << "connection established between " << n << ":" << here->getCanonicalName() << " and "
        //           << there->getCanonicalName() << std::endl;
    }

    /**
     * Notifier called whenever a connection got closed.
     */
    virtual void notifyConnectionClosed( boost::shared_ptr< WModuleConnector > /*here*/,
                                              boost::shared_ptr< WModuleConnector > /*there*/ )
    {
        // std::cout << "connection closed between " << n << ":" <<  here->getCanonicalName() << " and "
        //           <<  there->getCanonicalName() << std::endl;
    }

    /**
     * Notifier called whenever a changed data was propagated to one of this modules connectors.
     *
     * param input  the local connector receiving the event.
     * \param output the remote connector propagating the event.
     */
    virtual void notifyDataChange( boost::shared_ptr< WModuleConnector > /*input */,
                                   boost::shared_ptr< WModuleConnector > output )
    {
        // just copy the data and add one
        boost::shared_ptr< WModuleOutputData< WTestTransferableBase > > o =
            boost::shared_dynamic_cast< WModuleOutputData< WTestTransferableBase > >( output );
        if( !o.get() )
        {
            return;
        }

        boost::shared_ptr< WTestTransferableBase > ds = o->getData();
        if( ds.get() )
        {
            data = ds->get() + 1;
        }

        // std::cout << "change to " << data << " in " << input->getCanonicalName() << " from " << output->getCanonicalName()
        //          << std::endl;
    }

private:
    /**
     * The data lastly submitted.
     */
    int data;

    /**
     * Input connection.
     */
    boost::shared_ptr< WModuleInputData< WTestTransferableBase > > m_input;

    /**
     * Input connection with a derived class as transferable.
     */
    boost::shared_ptr< WModuleInputData< WTestTransferableDerived > > m_inputDerived;

    /**
     * Output connection.
     */
    boost::shared_ptr< WModuleOutputData< WTestTransferableBase > > m_output;

    /**
     * Output connection with a derived class as transferable
     */
    boost::shared_ptr< WModuleOutputData< WTestTransferableDerived > > m_outputDerived;
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
     * Setup logger and other stuff for each test.
     */
    void setUp()
    {
        WLogger::startup();
    }

    /**
     * Simple module to test with.
     */
    boost::shared_ptr< WModuleImpl > m1;

    /**
     * Simple module to test with.
     */
    boost::shared_ptr< WModuleImpl > m2;

    /**
     * Simple module to test with.
     */
    boost::shared_ptr< WModuleImpl > m3;

    /**
     * Initialized the test modules.
     */
    void createModules( void )
    {
        // init 3 separate test modules
        m1 = boost::shared_ptr< WModuleImpl >( new WModuleImpl( "m1" ) );
        m2 = boost::shared_ptr< WModuleImpl >( new WModuleImpl( "m2" ) );
        m3 = boost::shared_ptr< WModuleImpl >( new WModuleImpl( "m3" ) );
    }

    /**
     * Initializes modules. This is normally done by the module container.
     */
    void initModules( void )
    {
        m1->initialize();
        m2->initialize();
        m3->initialize();
    }

    /**
     * Initialize some connections.
     */
    void initConnections( void )
    {
        // connect output with input (cyclic)
        m1->m_output->connect( m2->m_input );
        m1->m_input->connect( m2->m_output );
    }

    /**
     * Test whether modules can be created without exception and proper initialization of connection lists.
     */
    void testModuleCreation( void )
    {
        TS_ASSERT_THROWS_NOTHING( createModules() );

        // check whether there are NO connectors.
        // The constructor should now create connectors since shared_ptr are needed -> init in constructor leads to exception
        // (it is enough to test one of them)
        TS_ASSERT( m1->m_inputConnectors.size() == 0 );
        TS_ASSERT( m1->m_outputConnectors.size() == 0 );
    }

    /**
     * Test whether modules can be initialized without problems.
     */
    void testModuleInitialization( void )
    {
        createModules();

        TS_ASSERT_THROWS_NOTHING( initModules() );

        // now there should be 1 everywhere
        TS_ASSERT( m1->m_inputConnectors.size() == 2 );
        TS_ASSERT( m1->m_outputConnectors.size() == 2 );
        TS_ASSERT( m2->m_inputConnectors.size() == 2 );
        TS_ASSERT( m2->m_outputConnectors.size() == 2 );
        TS_ASSERT( m3->m_inputConnectors.size() == 2 );
        TS_ASSERT( m3->m_outputConnectors.size() == 2 );

        // now we have 3 properly initialized modules?
        TS_ASSERT( m1->isInitialized()() );
        TS_ASSERT( m2->isInitialized()() );
        TS_ASSERT( m3->isInitialized()() );
    }

    /**
     * Test whether module initialization is robust against double init.
     */
    void testModuleTwiceInitialization( void )
    {
        WException::disableBacktrace();

        createModules();
        initModules();

        // try initializing twice
        TS_ASSERT_THROWS( m1->initialize(), WModuleConnectorInitFailed );
        TS_ASSERT( m1->isInitialized()() );
    }

    /**
     * Test whether automatic compatibility check works.
     */
    void testModuleConnectorCompatibility( void )
    {
        WException::disableBacktrace();

        createModules();
        initModules();

        // connect input with input and output with output should fail
        TS_ASSERT_THROWS( m1->m_input->connect( m2->m_input ), WModuleConnectorsIncompatible );
        TS_ASSERT_THROWS( m1->m_output->connect( m2->m_output ), WModuleConnectorsIncompatible );

        // there should be nothing connected.
        TS_ASSERT( m1->m_output->m_connected.size() == 0 );
        TS_ASSERT( m1->m_input->m_connected.size() == 0 );
        TS_ASSERT( m2->m_output->m_connected.size() == 0 );
        TS_ASSERT( m2->m_input->m_connected.size() == 0 );
    }

    /**
     * Test whether automatic type compatibility check works.
     */
    void testModuleConnectorTypeCompatibility( void )
    {
        WException::disableBacktrace();

        createModules();
        initModules();

        TS_ASSERT( m1->m_input->m_connected.size() == 0 );
        TS_ASSERT( m1->m_output->m_connected.size() == 0 );
        TS_ASSERT( m1->m_inputDerived->m_connected.size() == 0 );
        TS_ASSERT( m1->m_outputDerived->m_connected.size() == 0 );

        // connect an input with base type to output of derived type
        TS_ASSERT_THROWS_NOTHING( m1->m_input->connect( m2->m_outputDerived ) );
        TS_ASSERT( m1->m_input->m_connected.size() == 1 );
        TS_ASSERT( m2->m_outputDerived->m_connected.size() == 1 );

        // connect an input of derived type with output of base type
        TS_ASSERT_THROWS( m1->m_output->connect( m2->m_inputDerived ), WModuleConnectorsIncompatible );
        TS_ASSERT( m1->m_output->m_connected.size() == 0 );
        TS_ASSERT( m1->m_inputDerived->m_connected.size() == 0 );
    }

    /**
     * Test whether connection works properly
     */
    void testModuleConnection( void )
    {
        createModules();
        initModules();

        TS_ASSERT_THROWS_NOTHING( initConnections() );

        // check that every connector has a connection count of 1
        TS_ASSERT( m1->m_output->m_connected.size() == 1 );
        TS_ASSERT( m1->m_input->m_connected.size() == 1 );
        TS_ASSERT( m2->m_output->m_connected.size() == 1 );
        TS_ASSERT( m2->m_input->m_connected.size() == 1 );
    }

    /**
     * Test whether connecting twice is not possible.
     */
    void testModuleTwiceConnection( void )
    {
        createModules();
        initModules();
        initConnections();

        // try to connect twice
        TS_ASSERT_THROWS_NOTHING( m1->m_output->connect( m2->m_input ) );
        TS_ASSERT_THROWS_NOTHING( m1->m_input->connect( m2->m_output ) );
        TS_ASSERT( m1->m_output->m_connected.size() == 1 );
        TS_ASSERT( m1->m_input->m_connected.size() == 1 );
        TS_ASSERT( m2->m_output->m_connected.size() == 1 );
        TS_ASSERT( m2->m_input->m_connected.size() == 1 );
    }

    /**
     * Test whether the connection can properly be disconnected.
     */
    void testModuleDisconnect( void )
    {
        createModules();
        initModules();
        initConnections();

        // Disconnect something not connected
        TS_ASSERT_THROWS_NOTHING( m1->m_output->disconnect( m1->m_input ) );
        TS_ASSERT( m1->m_output->m_connected.size() == 1 );
        TS_ASSERT( m1->m_input->m_connected.size() == 1 );

        // Disconnect a connected
        TS_ASSERT_THROWS_NOTHING( m1->m_output->disconnect( m2->m_input ) );
        TS_ASSERT( m1->m_output->m_connected.size() == 0 );
        TS_ASSERT( m1->m_input->m_connected.size() == 1 );
        TS_ASSERT( m2->m_output->m_connected.size() == 1 );
        TS_ASSERT( m2->m_input->m_connected.size() == 0 );
    }

    /**
     * Test whether all connections can be removed in one step.
     */
    void testModuleDisconnectAll( void )
    {
        createModules();
        initModules();
        initConnections();

        // connect m3
        TS_ASSERT_THROWS_NOTHING( m3->m_input->connect( m2->m_output ) );

        // now m2->out should have 2 connections
        TS_ASSERT( m2->m_output->m_connected.size() == 2 );
        TS_ASSERT( m3->m_input->m_connected.size() == 1 );

        // remove both connections
        m2->m_output->disconnectAll();
        TS_ASSERT( m2->m_output->m_connected.size() == 0 );
        TS_ASSERT( m1->m_input->m_connected.size() == 0 );
        TS_ASSERT( m3->m_input->m_connected.size() == 0 );
    }

    /**
     * Test whether module clean up is working properly.
     */
    void testModuleCleanup( void )
    {
        createModules();
        initModules();
        initConnections();

        TS_ASSERT_THROWS_NOTHING( m1->cleanup() );
        TS_ASSERT( m1->m_inputConnectors.size() == 0 );
        TS_ASSERT( m1->m_outputConnectors.size() == 0 );
    }

    /**
     * Tests the propagation of data.
     */
    void testModulePropagateDataChange( void )
    {
        createModules();
        initModules();
        initConnections();

        // set some data, propagate change
        boost::shared_ptr< WTestTransferableBase > data = boost::shared_ptr< WTestTransferableBase >( new WTestTransferableBase() );
        int d = 5;
        data->set( d );
        TS_ASSERT_THROWS_NOTHING( m1->m_output->updateData( data ) );

        // got the data transferred?
        TS_ASSERT( m1->m_output->getData()->get() == d );
        TS_ASSERT( m2->m_input->getData()->get() == d );
        TS_ASSERT( m2->data == d + 1 );
    }

    /**
     * Tests several cases of unset data.
     */
    void testModuleInvalidData( void )
    {
        WException::disableBacktrace();

        createModules();
        initModules();
        initConnections();

        // try to get data from an unconnected connector
        TS_ASSERT( !m3->m_input->getData().get() );

        // try to get uninitialized data -> should return an "NULL" Pointer
        TS_ASSERT( m2->m_input->getData() == boost::shared_ptr< WTestTransferableBase >() );
    }
};

#endif  // WMODULECONNECTOR_TEST_H

