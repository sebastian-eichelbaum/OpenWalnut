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
#include <typeinfo>

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/function.hpp>

#include "../common/WThreadedRunner.h"
#include "WModuleConnectorSignals.h"
#include "WModuleSignals.h"

#include "../dataHandler/WDataSet.h"
#include "../dataHandler/WDataSetSingle.h"
#include "../dataHandler/WValueSet.hpp"

#include "../common/WProperties.h"

class WModuleConnector;
class WModuleInputConnector;
class WModuleOutputConnector;
class WModuleContainer;
class WModuleFactory;

/**
 * Class representing a single module of OpenWalnut.
 * \ingroup kernel
 */
class WModule: public WThreadedRunner,
               public boost::enable_shared_from_this<WModule>
{
friend class WModuleConnector;  // requires access to notify members
friend class WModuleFactory;    // for proper creation of module instaces, the factory needs access to protected functions.
                                // (especially initialize)
friend class WModuleContainer;  // for proper management of m_container WModuleContainer needs access.

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
    boost::shared_ptr<WProperties> getProperties() const;

    /**
     * Determines whether the module instance is properly initialized.
     *
     * \return true if properly initialized.
     */
    bool isInitialized() const;

    /**
     * Checks whether the module instance is ready to be used. This is the case if isInitialized && isAssociated.
     * 
     * \return isInitialized && isAssociated
     */
    bool isUseable() const;

     /**
      * Checks whether this module is associated with an container.
      * 
      * \return true if associated.
      */
    bool isAssociated() const;

     /**
      * The container this module is associated with.
      * 
      * \return the container.
      */
    boost::shared_ptr< WModuleContainer > getAssociatedContainer() const;

    /**
     * Due to the prototype design pattern used to build modules, this method returns a new instance of this method. NOTE: it
     * should never be initialized or modified in some other way. A simple new instance is required.
     * 
     * \return the prototype used to create every module in OpenWalnut.
     */
    virtual boost::shared_ptr< WModule > factory() const = 0;

    /**
     * Takes all the relevant GUI signals and connects them to own member functions.
     * NOTE: this is only temporal. See ticket 142.
     */
    virtual void connectToGui();

   /** 
     * Connects a specified notify function with a signal this module instance is offering.
     * 
     * \exception WModuleSignalSubscriptionFailed thrown if the signal can't be connected.
     *
     * \param signal the signal to connect to.
     * \param notifier the notifier function to bind.
     *
     * \return connection descriptor.
     */
    virtual boost::signals2::connection subscribeSignal( MODULE_SIGNAL signal, t_ModuleGenericSignalHandlerType notifier );
    virtual boost::signals2::connection subscribeSignal( MODULE_SIGNAL signal, t_ModuleErrorSignalHandlerType notifier );

    /**
     * Returns a set of prototypes compatible with this module's connectors.
     * 
     * \return set of prototypes.
     */
    virtual std::set< boost::shared_ptr< WModule > > getCompatibles();

protected:

    /**
     * Entry point after loading the module. Runs in separate thread.
     */
    virtual void moduleMain() = 0;

    /**
     * Thread entry point. Calls moduleMain and sends error notification if needed.
     */
    void threadMain();

     /**
      * Sets the container this module is associated with.
      * 
      * \param container the container to associate with.
      */
    void setAssociatedContainer( boost::shared_ptr< WModuleContainer > container );

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

    /**
     * Completely disconnects all connected connectors of this module.
     */
    void disconnectAll();

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

    // **************************************************************************************************************************
    //
    // Members
    //
    // **************************************************************************************************************************

    /**
     * the property object for the module
     */
    boost::shared_ptr< WProperties > m_properties;

    /**
     * True if everything is initialized and ready to be used.
     */
    bool m_initialized;

    /**
     * The container this module belongs to.
     */
    boost::shared_ptr< WModuleContainer > m_container;

    /**
     * Set of input connectors associated with this module.
     * NOTE: we need a thread safe list implementation!
     */
    std::set<boost::shared_ptr<WModuleInputConnector> > m_inputConnectors;

    /**
     * Set of output connectors associated with this module.
     */
    std::set<boost::shared_ptr<WModuleOutputConnector> > m_outputConnectors;

    /**
     * Call this whenever your module is ready and can react on property changes.
     */
    void ready();

private:

     /**
     * Lock for m_inputConnectors.
     */
    // boost::shared_mutex m_inputConnectorsLock;

    /**
     * Lock for m_outputConnectors.
     */
    // boost::shared_mutex m_outputConnectorsLock;

    /**
     * Signal fired whenever a module main thread is ready.
     */
    t_ModuleGenericSignalType signal_ready;

    /**
     * Signal fired whenever a module main thread throws an exception/error.
     */
    t_ModuleErrorSignalType signal_error;
};

#endif  // WMODULE_H

