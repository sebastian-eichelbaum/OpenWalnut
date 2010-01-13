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

#ifdef __linux__
#include <sys/prctl.h>
#endif

#include <set>
#include <string>
#include <sstream>

#include <boost/shared_ptr.hpp>

#include "WModuleInputConnector.h"
#include "WModuleOutputConnector.h"
#include "WModuleConnectorSignals.h"
#include "WModuleContainer.h"
#include "WModuleFactory.h"
#include "exceptions/WModuleSignalUnknown.h"
#include "exceptions/WModuleSignalSubscriptionFailed.h"
#include "exceptions/WModuleConnectorInitFailed.h"
#include "exceptions/WModuleUninitialized.h"
#include "../common/WException.h"
#include "../common/WLogger.h"
#include "../common/WCondition.h"
#include "../common/WConditionOneShot.h"
#include "../common/WProgressCombiner.h"

#include "WModule.h"

WModule::WModule():
    WThreadedRunner(),
    WPrototyped(),
    m_initialized( new WCondition(), false ),
    m_isAssociated( new WCondition(), false ),
    m_isUsable( new WCondition(), false ),
    m_isReady( new WConditionOneShot(), false ),
    m_readyProgress( boost::shared_ptr< WProgress >( new WProgress( "Initializing Module" ) ) ),
    m_moduleState()
{
    // initialize members
    m_properties = boost::shared_ptr< WProperties >( new WProperties() );
    m_container = boost::shared_ptr< WModuleContainer >();
    m_progress = boost::shared_ptr< WProgressCombiner >( new WProgressCombiner() );

    // add a progress indicator which finishes on "ready()"
    m_progress->addSubProgress( m_readyProgress );

    // our internal state consist out of two conditions: data changed and the exit flag from WThreadedRunner.
    m_moduleState.add( m_shutdownFlag.getCondition() );
}

WModule::~WModule()
{
    // cleanup
    removeConnectors();
}

void WModule::addConnector( boost::shared_ptr< WModuleInputConnector > con )
{
    m_inputConnectors.insert( con );
}

void WModule::addConnector( boost::shared_ptr< WModuleOutputConnector > con )
{
    m_outputConnectors.insert( con );
}

void WModule::disconnectAll()
{
    // TODO(ebaum): flat or deep removal? What to do with connected modules?

    // remove connections and their signals
    for( std::set<boost::shared_ptr< WModuleInputConnector > >::iterator listIter = m_inputConnectors.begin();
         listIter != m_inputConnectors.end(); ++listIter )
    {
        ( *listIter )->disconnectAll();
    }
    for( std::set<boost::shared_ptr< WModuleOutputConnector > >::iterator listIter = m_outputConnectors.begin();
         listIter != m_outputConnectors.end(); ++listIter )
    {
        ( *listIter )->disconnectAll();
    }
}

void WModule::removeConnectors()
{
    m_initialized( false );
    m_isUsable( m_initialized() && m_isAssociated() );

    // remove connections and their signals
    disconnectAll();

    // clean up list
    // this should delete the connector since nobody else *should* have another shared_ptr to them
    m_inputConnectors.clear();
    m_outputConnectors.clear();
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
    if ( isInitialized()() )
    {
        throw WModuleConnectorInitFailed( "Could not initialize connectors for Module " + getName() + ". Reason: already initialized." );
    }

    connectors();
    properties();

    m_initialized( true );
    m_isUsable( m_initialized() && m_isAssociated() );
}

void WModule::cleanup()
{
    // currently just removes connectors
    removeConnectors();
}

boost::shared_ptr< WModuleContainer > WModule::getAssociatedContainer() const
{
    return m_container;
}

void WModule::setAssociatedContainer( boost::shared_ptr< WModuleContainer > container )
{
    m_container = container;

    // true if the pointer is set
    m_isAssociated( m_container != boost::shared_ptr< WModuleContainer >() );
    m_isUsable( m_initialized() && m_isAssociated() );
}

const std::set<boost::shared_ptr< WModuleInputConnector > >& WModule::getInputConnectors() const
{
    return m_inputConnectors;
}

const std::set<boost::shared_ptr< WModuleOutputConnector > >& WModule::getOutputConnectors() const
{
    return m_outputConnectors;
}

boost::signals2::connection WModule::subscribeSignal( MODULE_SIGNAL signal, t_ModuleGenericSignalHandlerType notifier )
{
    switch (signal)
    {
        case WM_READY:
            return signal_ready.connect( notifier );
        default:
            std::ostringstream s;
            s << "Could not subscribe to unknown signal.";
            throw WModuleSignalSubscriptionFailed( s.str() );
            break;
    }
}

boost::signals2::connection WModule::subscribeSignal( MODULE_SIGNAL signal, t_ModuleErrorSignalHandlerType notifier )
{
    switch (signal)
    {
        case WM_ERROR:
            signal_error.connect( notifier );
        default:
            std::ostringstream s;
            s << "Could not subscribe to unknown signal.";
            throw WModuleSignalSubscriptionFailed( s.str() );
            break;
    }
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

std::set< boost::shared_ptr< WModule > > WModule::getCompatibles()
{
    return WModuleFactory::getModuleFactory()->getCompatiblePrototypes( shared_from_this() );
}

const WBoolFlag&  WModule::isInitialized() const
{
    return m_initialized;
}

const WBoolFlag& WModule::isAssociated() const
{
    return m_isAssociated;
}

const WBoolFlag& WModule::isUseable() const
{
    return m_isUsable;
    //return isInitialized() && isAssociated();
}

const WBoolFlag& WModule::isReady() const
{
    return m_isReady;
}

void WModule::notifyConnectionEstablished( boost::shared_ptr< WModuleConnector > /*here*/,
                                           boost::shared_ptr< WModuleConnector > /*there*/ )
{
    // By default this callback does nothing. Overwrite it in your module.
}

void WModule::notifyConnectionClosed( boost::shared_ptr< WModuleConnector > /*here*/,
                                      boost::shared_ptr< WModuleConnector > /*there*/ )
{
    // By default this callback does nothing. Overwrite it in your module.
}

void WModule::notifyDataChange( boost::shared_ptr< WModuleConnector > /*input*/,
                                boost::shared_ptr< WModuleConnector > /*output*/ )
{
    // By default this callback does nothing. Overwrite it in your module.
}

boost::shared_ptr< WProperties > WModule::getProperties() const
{
    return m_properties;
}

void WModule::ready()
{
    m_isReady( true );
    signal_ready( shared_from_this() );
}

void WModule::threadMain()
{
#ifdef __linux__
    prctl( PR_SET_NAME, ( "walnut (" + getName() + ")" ).c_str() );
#endif

    try
    {
        WLogger::getLogger()->addLogMessage( "Starting module main method.", "Module (" + getName() + ")", LL_INFO );

        // call main thread function
        moduleMain();
    }
    catch( const WException& e )
    {
        WLogger::getLogger()->addLogMessage( "WException. Notifying.", "Module (" + getName() + ")", LL_ERROR );

        // ensure proper exception propagation
        signal_error( shared_from_this(), e );
    }
    catch( const std::exception& e )
    {
        WLogger::getLogger()->addLogMessage( "Exception. Notifying.", "Module (" + getName() + ")", LL_ERROR );

        // convert these exceptions to WException
        WException ce = WException( e );
        signal_error( shared_from_this(), ce );
    }
}

wlog::WStreamedLogger WModule::infoLog() const
{
    return wlog::info( getName() );
}

wlog::WStreamedLogger WModule::errorLog() const
{
    return wlog::error( getName() );
}

wlog::WStreamedLogger WModule::debugLog() const
{
    return wlog::debug( getName() );
}

wlog::WStreamedLogger WModule::warnLog() const
{
    return wlog::warn( getName() );
}
