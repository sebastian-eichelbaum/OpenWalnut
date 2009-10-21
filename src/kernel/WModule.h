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

#ifndef WMODULE_H
#define WMODULE_H

#include <set>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include "../common/WThreadedRunner.h"
#include "WModuleConnectorSignals.h"

#include "../dataHandler/WDataSet.h"
#include "../dataHandler/WDataSetSingle.h"
#include "../dataHandler/WValueSet.hpp"

#include "WProperties.h"

class WModuleConnector;
class WModuleInputConnector;
class WModuleOutputConnector;

/**
 * Class representing a single module of OpenWalnut.
 * \ingroup kernel
 */
class WModule: public WThreadedRunner,
               public boost::enable_shared_from_this<WModule>
{
friend class WModuleConnector;  // requires access to notify members

public:

    /**
     * Constructs a new WModule instance
     */
    WModule();

    /**
     * Destructor.
     */
    virtual ~WModule();

    /**
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const = 0;

    /**
     * Gives back a description of this module.
     * \return description to module.
     */
    virtual const std::string getDescription() const = 0;

    /**
     * Gives back an input connectors.
     *
     * \return the input connectors.
     */
    const std::set<boost::shared_ptr<WModuleInputConnector> >& getInputConnectors() const;

    /**
     * Gives back a  output connectors.
     *
     * \return the output connectors.
     */
    const std::set<boost::shared_ptr<WModuleOutputConnector> >& getOutputConnectors() const;

    /**
     * Return a pointer to the properties object of the module
     */
    WProperties* getProperties();

    /**
     * Determines whether the module instance is properly initialized.
     *
     * \return true if properly initialized.
     */
     bool isInitialized() const;

    /**
     * Takes all the relevant GUI signals and connects them to own member functions.
     * NOTE: this is only temporal. See ticket 142.
     */
    virtual void connectToGui();

protected:

    /**
     * Entry point after loading the module. Runs in separate thread.
     */
    virtual void threadMain() = 0;

    // **************************************************************************************************************************
    //
    // Connector Management
    //
    // **************************************************************************************************************************

    /**
     * Initialize connectors in this function. This function must not be called multiple times for one module instance.
     * The module container manages calling those functions -> so just implement it.
     */
    virtual void connectors();

    /**
     * Initialize properties in this function. This function must not be called multiple times for one module instance.
     * The module container manages calling those functions -> so just implement it. Once initialized the number and type
     * of all properties should be set.
     */
    virtual void properties();


    /**
     * Manages connector initialization. Gets called by module container.
     *
     * \throw WModuleConnectorInitFailed if called multiple times.
     */
    void initialize();

    /**
     * Removes connectors and cleans up.
     */
    void cleanup();

    /**
     * Set of input connectors associated with this module.
     * NOTE: we need a thread safe list implementation!
     */
    std::set<boost::shared_ptr<WModuleInputConnector> > m_InputConnectors;

    /**
     * Set of output connectors associated with this module.
     */
    std::set<boost::shared_ptr<WModuleOutputConnector> > m_OutputConnectors;

    /**
     * Adds the specified connector to the list of inputs.
     *
     * \param con the connector.
     */
    void addConnector( boost::shared_ptr<WModuleInputConnector> con );

    /**
     * Adds the specified connector to the list of outputs.
     *
     * \param con the connector.
     */
    void addConnector( boost::shared_ptr<WModuleOutputConnector> con );

    /**
     * Removes all connectors properly. It disconnects the connectors and cleans the connectors list.
     */
    void removeConnectors();

    // **************************************************************************************************************************
    //
    // Signal handlers that HAVE to be in every module. By default they do nothing. You should overwrite them to get notified
    // with the corresponding signal
    //
    // **************************************************************************************************************************

    /**
     * Gives the signal handler function responsible for a given signal. Modules defining own signal handlers should overwrite
     * this function. This function is protected since boost::functions are callable, which is what is not wanted here. Just
     * signals should call them.
     *
     * \param signal the signal to get the handler for.
     *
     * \return the signal handler for "signal".
     */
    virtual const t_GenericSignalHandlerType getSignalHandler( MODULE_CONNECTOR_SIGNAL signal );

    /**
     * Gets called whenever a connector gets connected to the specified input.
     *
     * \param here the connector of THIS module that got connected to "there"
     * \param there the connector that has been connected with the connector "here" of this module.
     */
    virtual void notifyConnectionEstablished( boost::shared_ptr<WModuleConnector> here,
                                              boost::shared_ptr<WModuleConnector> there );
    /**
     * Gets called whenever a connection between a remote and local connector gets closed.
     *
     * \param here the connector of THIS module getting disconnected.
     * \param there the connector of the other module getting disconnected.
     */
    virtual void notifyConnectionClosed( boost::shared_ptr<WModuleConnector> here, boost::shared_ptr<WModuleConnector> there );

    /**
     * Gets called when the data on one input connector changed.
     *
     * \param input the input connector receiving the change.
     * \param output the output connector sending the change notification.
     */
    virtual void notifyDataChange( boost::shared_ptr<WModuleConnector> input,
                                   boost::shared_ptr<WModuleConnector> output );

     /**
     * the property object for the module
     */
    WProperties m_properties;

    /**
     * True if everything is initialized and ready to be used.
     */
    bool m_initialized;

private:

     /**
     * Lock for m_InputConnectors.
     */
    // boost::shared_mutex m_InputConnectorsLock;

    /**
     * Lock for m_OutputConnectors.
     */
    // boost::shared_mutex m_OutputConnectorsLock;
};

#endif  // WMODULE_H

