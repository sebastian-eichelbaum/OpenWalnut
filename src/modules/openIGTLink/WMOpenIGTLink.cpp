//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV-Leipzig and CNCF-CBS
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

#include <string>

#include "core/kernel/WKernel.h"
#include "modules/emptyIcon.xpm" // Please put a real icon here.

#include "core/dataHandler/WDataSetScalar.h"
#include "core/kernel/WSelectionManager.h"

#include "WMOpenIGTLink.h"
#include "WIGTLinkRemote.h"

#define PORT_SLICER_DEFAULT 18944

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMOpenIGTLinkSender )

WMOpenIGTLinkSender::WMOpenIGTLinkSender():
    WModule()
{
}

WMOpenIGTLinkSender::~WMOpenIGTLinkSender()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMOpenIGTLinkSender::factory() const
{
    // See "src/modules/template/" for an extensively documented example.
    return boost::shared_ptr< WModule >( new WMOpenIGTLinkSender() );
}

const char** WMOpenIGTLinkSender::getXPMIcon() const
{
    return emptyIcon_xpm; // Please put a real icon here.
}
const std::string WMOpenIGTLinkSender::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "OpenIGTLinkSender";
}

const std::string WMOpenIGTLinkSender::getDescription() const
{
    return "OpenIGT Link";
}

void WMOpenIGTLinkSender::connectors()
{
    m_input =  WModuleInputData < WDataSetScalar > ::createAndAdd( shared_from_this(), "input", "A data set to transmit over the network" );
    m_output = WModuleOutputData < WDataSetScalar > ::createAndAdd( shared_from_this(), "output", "A data set received from the network" );
    WModule::connectors();
}

void WMOpenIGTLinkSender::properties()
{
    m_propCondition =  boost::shared_ptr < WCondition > ( new WCondition() );

    m_propActive =  m_properties->addProperty( "Active",
            "Set to true to start the connection or to start the server."
            "To avoid random connections, an update of the remote IP address"
            " and the remote or local port is only performed whenever the active flag changes.",
            false, m_propCondition );

    m_propServerOrClientSelections = boost::shared_ptr < WItemSelection > ( new WItemSelection() );
    m_propServerOrClientSelections->addItem( "Server", "TCP/IP server listening for incoming connections." );
    m_propServerOrClientSelections->addItem( "Client", "TCP/IP client activenly making a connection to the remote host." );
    m_propServerOrClient =  m_properties->addProperty( "Act as:", "Server or Client.",
            m_propServerOrClientSelections->getSelectorFirst(), m_propCondition );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_propServerOrClient );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_propServerOrClient );

    m_propCheckCRC = m_properties->addProperty( "Check CRC", "Enable to perform CRC on incoming packets. Disable to ignore the checksum",
            true, m_propCondition );

    m_hostname = m_properties->addProperty( "Hostname", "Remote host name to connect to. Enter a host name or an IP address.",
            std::string( "127.0.0.1" ), m_propCondition );
    m_port = m_properties->addProperty( "Port", "Local port to open or remote port to connect to. The default Slicer3D port is 18944.",
            PORT_SLICER_DEFAULT, m_propCondition );
    m_port->setMin( 1 );
    m_port->setMax( 65535 );

    // The slice positions. We use these as a transform that is sent over the network. Maybe we should send it as a position instead?
    m_xPos    = m_properties->addProperty( WKernel::getRunningKernel()->getSelectionManager()->getPropSagittalPos() );
    m_yPos    = m_properties->addProperty( WKernel::getRunningKernel()->getSelectionManager()->getPropCoronalPos() );
    m_zPos    = m_properties->addProperty( WKernel::getRunningKernel()->getSelectionManager()->getPropAxialPos() );

    WModule::properties();
}

void WMOpenIGTLinkSender::requirements()
{
}

void WMOpenIGTLinkSender::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );
    m_moduleState.add( m_xPos->getUpdateCondition() );
    m_moduleState.add( m_yPos->getUpdateCondition() );
    m_moduleState.add( m_zPos->getUpdateCondition() );

    ready();
    debugLog() << "Module is now ready.";

    boost::shared_ptr < WIGTLinkRemote > remote;

    if( m_propActive->get() )
    {
        //if( m_propConnectToRemote->get() )
        if( m_propServerOrClient->get() == 0 )
        {
            debugLog() << "listen on local port";
            remote.reset( new WIGTLinkRemote );
            remote->createSocketAndWaitForConnection( m_port->get() );
                            }
        //else if( m_propListenOnPort->get() )
        else // client
        {
            debugLog() << "connect to remote host";
            remote.reset( new WIGTLinkRemote );
            try
            {
                remote->createSocketAndConnect( m_hostname->get(), m_port->get() );
            }
            catch( const std::exception& e )
            {
                infoLog() << "Connection failed";
                infoLog() << e.what();
                // TODO(mario): issue gui warning and possible interaction?
            }
            remote.reset();
        }
    }
    else
    {
        debugLog() << "no active connection";
    }

    if( remote )
    {
        remote->setCheckCRC( m_propCheckCRC->get() );
        m_moduleState.add( remote->receiversCondition );
        m_moduleState.add( remote->statusCondition );
        remote->run();
    }

    debugLog() << "Entering main loop";
    while( !m_shutdownFlag() )
    {
        debugLog() << "Waiting ...";
        m_moduleState.wait();
        debugLog() << "Awaken!";

        if( m_shutdownFlag() )
        {
           break;
        }

        if( m_propActive->changed( true ) )
            // we ignore the following changes as we only allow changes when the server becomes active or inactive
            // || m_hostname->changed( true ) || m_port->changed( true )
            // || m_propConnectToRemote->changed( true ) || m_propListenOnPort->changed( true ) )
        {
            if( remote )
            {
                m_moduleState.remove( remote->receiversCondition );
                m_moduleState.remove( remote->statusCondition );
                debugLog() << "Shutting down the server.";
                remote->wait( true );

                // first, remove the remote server object
                remote.reset();
                debugLog() << "waiting one second.";

                // wait for an appropriate time for operation system to close connections and allowing access to the ports, again
                sleep( 1 ); // wait 1 second
            }

            debugLog() << "reengage server.";
            if( m_propActive->get() )
            {
                //if( m_propConnectToRemote->get() )
                if( m_propServerOrClient->get() == 0 )
                {
                    debugLog() << "listen on local port";
                    remote.reset( new WIGTLinkRemote );
                    remote->createSocketAndWaitForConnection( m_port->get() );
                                    }
                //else if( m_propListenOnPort->get() )
                else // client
                {
                    debugLog() << "connect to remote host";
                    remote.reset( new WIGTLinkRemote );
                    try
                    {
                        remote->createSocketAndConnect( m_hostname->get(), m_port->get() );
                    }
                    catch( const std::exception& e )
                    {
                        infoLog() << "Connection failed";
                        infoLog() << e.what();
                        // TODO(mario): issue gui warning and possible interaction?
                        m_propActive->set( false ); // set active to disabled as this is what we are at that point
                    }
                    remote.reset();
                }
            }
            else
            {
                debugLog() << "no active connection";
            }

            if( remote )
            {
                remote->setCheckCRC( m_propCheckCRC->get() );
                m_moduleState.add( remote->receiversCondition );
                m_moduleState.add( remote->statusCondition );
                remote->run();
                debugLog() << "server running.";
            }
        }

        if( remote )
        {
            if( m_propCheckCRC->changed( true ) )
            {
                remote->setCheckCRC( m_propCheckCRC->get() );
            }

            debugLog() << "checking receive queue:";
            remote->receiversMutex.lock();
            if( !remote->receiveQueue.empty() )
            {
                debugLog() << "something in queue for us";
                WDataSetScalarSPtr ds = remote->receiveQueue.front();
                remote->receiveQueue.pop();
                if( ds )
                {
                    debugLog() << "received data set";
                    m_output->updateData( ds );
                }
                else
                {
                    debugLog() << "received empty data set";
                }
            }
            else
            {
                debugLog() << "Queue empty";
            }
            remote->receiversMutex.unlock();
        }

        if( remote && m_input->updated() )
        {
            debugLog() << "prepare sending new data";
            WDataSetScalarSPtr dataSet = m_input->getData();
            bool dataValid = ( dataSet );

            if( dataValid )
            {
                debugLog() << "sending new data";
                remote->sendImageData( dataSet ); // TODO(mario): try to make this async
            }
        }

        // if the navigation slices changed, we send an update to whoevert is listening
        if( ( m_xPos->changed( true ) || m_yPos->changed( true ) || m_zPos->changed( true ) ) && remote )
        {
            WMatrix<double> a( 4, 4 );
            a( 0, 0 ) = 1.0; a( 0, 1 ) = 0.0; a( 0, 2 ) = 0.0; a( 0, 3 ) = m_xPos->get(); // NOLINT
            a( 1, 0 ) = 0.0; a( 1, 1 ) = 1.0; a( 1, 2 ) = 0.0; a( 1, 3 ) = m_yPos->get(); // NOLINT
            a( 2, 0 ) = 0.0; a( 2, 1 ) = 0.0; a( 2, 2 ) = 1.0; a( 2, 3 ) = m_zPos->get(); // NOLINT
            a( 3, 0 ) = 0.0; a( 3, 1 ) = 0.0; a( 3, 2 ) = 0.0; a( 3, 3 ) = 1.0;           // NOLINT

            remote->sendTransform( "Coordinage System", a );
        }
    }

    debugLog() << "about to shut down";
    if( remote )
    {
        debugLog() << "about to shut down: waiting for connection to close.";
        remote->wait( true );
        debugLog() << "about to shut down: connection closed";
    }

    debugLog() << "module shutting down.";
}

