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

#include <set>
#include <string>
#include <sstream>

#include <boost/shared_ptr.hpp>

#include "WModuleInputConnector.h"
#include "WModuleOutputConnector.h"
#include "WModuleConnectorSignals.h"
#include "exceptions/WModuleSignalUnknown.h"
#include "exceptions/WModuleConnectorInitFailed.h"

#include "WModule.h"

WModule::WModule():
    WThreadedRunner(),
     m_initialized( false )
{
    // initialize members
    m_properties = boost::shared_ptr<WProperties>( new WProperties() );
}

WModule::~WModule()
{
    // cleanup
    removeConnectors();
}

void WModule::addConnector( boost::shared_ptr<WModuleInputConnector> con )
{
    m_InputConnectors.insert( con );
}

void WModule::addConnector( boost::shared_ptr<WModuleOutputConnector> con )
{
    m_OutputConnectors.insert( con );
}

void WModule::removeConnectors()
{
    m_initialized = false;

    // remove connections and their signals
    for( std::set<boost::shared_ptr<WModuleInputConnector> >::iterator listIter = m_InputConnectors.begin();
         listIter != m_InputConnectors.end(); ++listIter )
    {
        ( *listIter )->disconnectAll();
    }
    for( std::set<boost::shared_ptr<WModuleOutputConnector> >::iterator listIter = m_OutputConnectors.begin();
         listIter != m_OutputConnectors.end(); ++listIter )
    {
        ( *listIter )->disconnectAll();
    }

    // clean up list
    // this should delete the connector since nobody else *should* have another shared_ptr to them
    m_InputConnectors.clear();
    m_OutputConnectors.clear();
}

void WModule::connectors()
{
}

void WModule::properties()
{
}


void WModule::initialize()
{
    // doing it twice is not allowed
    if ( isInitialized() )
    {
        // TODO(ebaum): is this really needed?
        std::ostringstream s;
        s << "Could not initialize connectors for Module " << getName() << ". Reason: already initialized.";

        throw WModuleConnectorInitFailed( s.str() );
    }

    connectors();

    m_initialized = true;
}

void WModule::cleanup()
{
    // currently just removes connectors
    removeConnectors();
}

const std::set<boost::shared_ptr<WModuleInputConnector> >& WModule::getInputConnectors() const
{
    return m_InputConnectors;
}

const std::set<boost::shared_ptr<WModuleOutputConnector> >& WModule::getOutputConnectors() const
{
    return m_OutputConnectors;
}

const t_GenericSignalHandlerType WModule::getSignalHandler( MODULE_CONNECTOR_SIGNAL signal )
{
    switch ( signal )
    {
        case CONNECTION_ESTABLISHED:
            return boost::bind( &WModule::notifyConnectionEstablished, this, _1, _2 );
        case CONNECTION_CLOSED:
            return boost::bind( &WModule::notifyConnectionClosed, this, _1, _2 );
        case DATA_CHANGED:
            return boost::bind( &WModule::notifyDataChange, this, _1, _2 );
        default:
            std::ostringstream s;
            s << "Could not subscribe to unknown signal. You need to implement this signal type explicitly in your module.";
            throw WModuleSignalUnknown( s.str() );
            break;
    }
}

bool WModule::isInitialized() const
{
    return m_initialized;
}

void WModule::notifyConnectionEstablished( boost::shared_ptr<WModuleConnector> /*here*/,
                                           boost::shared_ptr<WModuleConnector> /*there*/ )
{
    // By default this callback does nothing. Overwrite it in your module.
}

void WModule::notifyConnectionClosed( boost::shared_ptr<WModuleConnector> /*here*/,
                                      boost::shared_ptr<WModuleConnector> /*there*/ )
{
    // By default this callback does nothing. Overwrite it in your module.
}

void WModule::notifyDataChange( boost::shared_ptr<WModuleConnector> /*input*/,
                                boost::shared_ptr<WModuleConnector> /*output*/ )
{
    // By default this callback does nothing. Overwrite it in your module.
}

boost::shared_ptr<WProperties> WModule::getProperties()
{
    return m_properties;
}

void WModule::connectToGui()
{
}

