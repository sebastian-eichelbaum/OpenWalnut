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

#ifndef WMODULEINPUTCONNECTOR_H
#define WMODULEINPUTCONNECTOR_H

#include <string>

#include "WModule.h"
#include "WModuleConnector.h"
#include "WModuleConnectorSignals.h"

/**
 * Class implementing input connection functionality between modules.
 */
class WModuleInputConnector: public WModuleConnector
{
public:

    /**
     * Constructor.
     * 
     * \param module the module which is owner of this connector.
     * \param name The name of this connector.
     * \param description Short description of this connector.
     */    
    WModuleInputConnector( boost::shared_ptr<WModule> module, std::string name="", std::string description="" );

    /**
     * Destructor.
     */
    virtual ~WModuleInputConnector();

    /** 
     * Checks whether the specified connector is an output connector.
     * 
     * \param con the connector to check against.
     * 
     * \return true if compatible.
     */
    virtual bool connectable( boost::shared_ptr<WModuleConnector> con );

protected:

    /** 
     * Connect additional signals.
     * 
     * \param con the connector that requests connection.
     * 
     */
    virtual void connectSignals( boost::shared_ptr<WModuleConnector> con );

    /** 
     * Disconnect all signals subscribed by this connector from "con".
     * 
     * \param con the connector that gets disconnected.
     */
    virtual void disconnectSignals( boost::shared_ptr<WModuleConnector> con );

    /** 
     * Gets called when the data on this input connector changed.
     * 
     * \param input the input connector receiving the change.
     * \param output the output connector sending the change notification.
     */
    virtual void notifyDataChange( boost::shared_ptr<WModuleConnector> input,
                                   boost::shared_ptr<WModuleConnector> output );

private:

    /** 
     * Signal for "DATA_CHANGED" Events. We use a separate signal here (instead of using the signal send by the connected output)
     * since the output can not determine the receiver when signalling. So we use an own signal handler and signal to "forward"
     * the message and complete the information with a this-pointer.
     */
    t_GenericSignalType signal_DataChanged;

    /** 
     * Connection for Data Changed signal of the connected output connector.
     */
    boost::signals2::connection m_DataChangedConnection;
};

#endif  // WMODULEINPUTCONNECTOR_H

