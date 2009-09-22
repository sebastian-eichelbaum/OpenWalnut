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

class WModuleConnector;
class WModuleInputConnector;
class WModuleOutputConnector;

/**
 * Class representing a single module of OpenWalnut.
 * \ingroup kernel
 */
class WModule: public WThreadedRunner
{
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

protected:

    /**
     * Entry point after loading the module. Runs in separate thread.
     */
    virtual void threadMain() = 0;

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

    // XXX corresponding removeConnector methods should not be needed


    // Signal handlers that HAVE to be in every module. By default they do nothing. You should overwrite them to get notified with
    // the corresponding signal

    /** 
     * Gets called whenever a connector gets connected to the specified input.
     * 
     * \param here the connector of THIS module that got connected to "there"
     * \param output the connector that has been connected with the connector "here" of this module.
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
     * \param input 
     */
    virtual void notifyDataChange( boost::shared_ptr<WModuleInputConnector> input,
                                   boost::shared_ptr<WModuleOutputConnector> output );

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

