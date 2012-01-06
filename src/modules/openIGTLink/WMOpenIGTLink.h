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

#ifndef WMOPENIGTLINK_H
#define WMOPENIGTLINK_H

#include <string>

#include <osg/Geode>

#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"

/**
 * This module provides an interface to OpenIGTLink, a protocol to exchange
 * data remotely over IP networks.
 *
 * The module can act as a source or sink and, thus, transmit or receive data
 * to and from servers or clients across the network.
 * Clients that support the protocol are most VTK-based applications, including
 * 3D Slicer, but there are interfaces for other tools such as MATLAB or proprietary
 * scanner software as well.
 *
 * For more information see http://www.na-mic.org/Wiki/index.php/OpenIGTLink
 *
 * \ingroup modules
 */
class WMOpenIGTLinkSender: public WModule
{
public:
    /**
     *
     */
    WMOpenIGTLinkSender();

    /**
     *
     */
    virtual ~WMOpenIGTLinkSender();

    /**
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Gives back a description of this module.
     * \return description to module.
     */
    virtual const std::string getDescription() const;

    /**
     * Due to the prototype design pattern used to build modules, this method returns a new instance of this method. NOTE: it
     * should never be initialized or modified in some other way. A simple new instance is required.
     *
     * \return the prototype used to create every module in OpenWalnut.
     */
    virtual boost::shared_ptr< WModule > factory() const;

    /**
     * Get the icon for this module in XPM format.
     *
     * \return The icon.
     */
    virtual const char** getXPMIcon() const;

protected:
    /**
     * Entry point after loading the module. Runs in separate thread.
     */
    virtual void moduleMain();

    /**
     * Initialize the connectors this module is using.
     */
    virtual void connectors();

    /**
     * Initialize the properties for this module.
     */
    virtual void properties();

    /**
     * Initialize requirements for this module.
     */
    virtual void requirements();


private:
    /**
     * input for data to send
     */
    boost::shared_ptr < WModuleInputData < WDataSetScalar > > m_input;

    /**
     * output of received data
     */
    boost::shared_ptr < WModuleOutputData < WDataSetScalar > > m_output;

    /**
     * A condition used to notify about changes in several properties
     */
    boost::shared_ptr < WCondition > m_propCondition;

    /**
     * hostname to connect to
     */
    WPropString m_hostname;

    /**
     * port to listen on or to connect to
     */
    WPropInt m_port;

    WPropDouble    m_xPos; //!< x position of the slice

    WPropDouble    m_yPos; //!< y position of the slice

    WPropDouble    m_zPos; //!< z position of the slice

    /**
     * whether we connect to the remote host or just listen
     */
    //WPropBool m_propListenOnPort;

    /**
     * List of selections to pick whether to act as an TCPIP server or as an TCPIP client,
     * i.e., whether to listen on a port for incoming connections
     * or whether to connect actively to a remote host.
     */
    boost::shared_ptr <  WItemSelection >  m_propServerOrClientSelections;

    /**
     * Pick whether to act as an TCPIP server or as an TCPIP client,
     * i.e., whether to listen on a port for incoming connections
     * or whether to connect actively to a remote host.
     */
    WPropSelection m_propServerOrClient;

    /**
     * Activation status of the server
     */
    WPropBool m_propActive;


    /**
     * Check CRC
     */
    WPropBool m_propCheckCRC;

    /**
     * whether we connect to the remote host or just listen
     */
    //WPropBool m_propConnectToRemote;
};

#endif  // WMOPENIGTLINK_H
