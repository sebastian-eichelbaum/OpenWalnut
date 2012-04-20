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

#include <algorithm>
#include <set>
#include <string>
#include <sstream>

#include <boost/shared_ptr.hpp>

#include "WModuleInputConnector.h"
#include "WModuleOutputConnector.h"
#include "WModuleInputData.h"
#include "WModuleOutputData.h"
#include "WModuleConnectorSignals.h"
#include "WModuleContainer.h"
#include "WModuleFactory.h"
#include "WModuleMetaInformation.h"
#include "exceptions/WModuleConnectorInitFailed.h"
#include "exceptions/WModuleConnectorNotFound.h"
#include "exceptions/WModuleUninitialized.h"
#include "exceptions/WModuleRequirementNotMet.h"
#include "../common/WException.h"
#include "../common/exceptions/WNameNotUnique.h"
#include "../common/exceptions/WSignalUnknown.h"
#include "../common/exceptions/WSignalSubscriptionFailed.h"
#include "../common/WLogger.h"
#include "../common/WCondition.h"
#include "../common/WConditionOneShot.h"
#include "../common/WConditionSet.h"
#include "../common/WPathHelper.h"
#include "../common/WProgressCombiner.h"
#include "../common/WPredicateHelper.h"

#include "WModule.h"

WModule::WModule():
    WThreadedRunner(),
    WPrototyped(),
    m_initialized( new WCondition(), false ),
    m_isAssociated( new WCondition(), false ),
    m_isUsable( new WCondition(), false ),
    m_isReady( new WConditionOneShot(), false ),
    m_isReadyOrCrashed( new WConditionSet(), false ),
    m_isRunning( new WCondition(), false ),
    m_readyProgress( boost::shared_ptr< WProgress >( new WProgress( "Initializing Module" ) ) ),
    m_moduleState(),
    m_localPath( WPathHelper::getSharePath() )
{
    // initialize members
    m_properties = boost::shared_ptr< WProperties >( new WProperties( "Properties", "Module's properties" ) );
    m_infoProperties = boost::shared_ptr< WProperties >( new WProperties( "Informational Properties", "Module's information properties" ) );
    m_infoProperties->setPurpose( PV_PURPOSE_INFORMATION );

    m_runtimeName = m_properties->addProperty( "Name", "The name of the module defined by the user. This is, by default, the module name but "
                                                       "can be changed by the user to provide some kind of simple identification upon many modules.",
                                                       std::string( "" ), false );

    m_active = m_properties->addProperty( "active", "Determines whether the module should be activated.", true, true );
    m_active->getCondition()->subscribeSignal( boost::bind( &WModule::activate, this ) );

    // the isReadyOrCrashed condition set needs to be set up here
    WConditionSet* cs = static_cast< WConditionSet* >( m_isReadyOrCrashed.getCondition().get() ); // NOLINT
    cs->setResetable( true, false );
    cs->add( m_isReady.getCondition() );
    cs->add( m_isCrashed.getCondition() );

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
}

void WModule::addConnector( boost::shared_ptr< WModuleInputConnector > con )
{
    size_t c = std::count_if( m_inputConnectors.begin(), m_inputConnectors.end(),
                              WPredicateHelper::Name< boost::shared_ptr< WModuleInputConnector > >( con->getName() )
    );
    // well ... we want it to be unique in both:
    c += std::count_if( m_outputConnectors.begin(), m_outputConnectors.end(),
                        WPredicateHelper::Name< boost::shared_ptr< WModuleOutputConnector > >( con->getName() )
    );

    // if there already is one ... exception
    if( c )
    {
        throw WNameNotUnique( std::string( "Could not add the connector " + con->getCanonicalName() + " since names must be unique." ) );
    }

    m_inputConnectors.push_back( con );
}

void WModule::addConnector( boost::shared_ptr< WModuleOutputConnector > con )
{
    size_t c = std::count_if( m_inputConnectors.begin(), m_inputConnectors.end(),
                              WPredicateHelper::Name< boost::shared_ptr< WModuleInputConnector > >( con->getName() )
    );
    // well ... we want it to be unique in both:
    c += std::count_if( m_outputConnectors.begin(), m_outputConnectors.end(),
                        WPredicateHelper::Name< boost::shared_ptr< WModuleOutputConnector > >( con->getName() )
    );

    // if there already is one ... exception
    if( c )
    {
        throw WNameNotUnique( std::string( "Could not add the connector " + con->getCanonicalName() + " since names must be unique." ) );
    }

    m_outputConnectors.push_back( con );
}

void WModule::disconnect()
{
    // remove connections and their signals
    for( InputConnectorList::iterator listIter = m_inputConnectors.begin();
         listIter != m_inputConnectors.end(); ++listIter )
    {
        ( *listIter )->disconnectAll();
    }
    for( OutputConnectorList::iterator listIter = m_outputConnectors.begin();
         listIter != m_outputConnectors.end(); ++listIter )
    {
        ( *listIter )->disconnectAll();
    }
}

WCombinerTypes::WDisconnectList WModule::getPossibleDisconnections()
{
    WCombinerTypes::WDisconnectList discons;

    // iterate inputs
    for( InputConnectorList::iterator listIter = m_inputConnectors.begin(); listIter != m_inputConnectors.end(); ++listIter )
    {
        // get all connections of the current connector:
        WCombinerTypes::WDisconnectGroup g = WCombinerTypes::WDisconnectGroup( ( *listIter )->getName(),
                                                                               ( *listIter )->getPossibleDisconnections() );

        if( g.second.size() )
        {
            discons.push_back( g );
        }
    }

    // iterate outputs
    for( OutputConnectorList::iterator listIter = m_outputConnectors.begin(); listIter != m_outputConnectors.end(); ++listIter )
    {
        // get all connections of the current connector:
        WCombinerTypes::WDisconnectGroup g = WCombinerTypes::WDisconnectGroup( ( *listIter )->getName(),
                                                                               ( *listIter )->getPossibleDisconnections() );

        if( g.second.size() )
        {
            discons.push_back( g );
        }
    }

    return discons;
}

void WModule::removeConnectors()
{
    m_initialized( false );
    m_isUsable( m_initialized() && m_isAssociated() );

    // remove connections and their signals, this is flat removal. The module container can do deep removal
    disconnect();

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

void WModule::requirements()
{
}

void WModule::activate()
{
}

std::string WModule::deprecated() const
{
    return "";
}

WModuleMetaInformation::ConstSPtr WModule::getMetaInformation() const
{
    return m_meta;
}

void WModule::initialize()
{
    // doing it twice is not allowed
    if( isInitialized()() )
    {
        throw WModuleConnectorInitFailed( std::string( "Could not initialize connectors for Module " ) + getName() +
                                          std::string( ". Reason: already initialized." ) );
    }

    // set the module name as default runtime name
    m_runtimeName->set( getName() );

    // initialize module meta information
    m_meta = WModuleMetaInformation::SPtr( new WModuleMetaInformation( shared_from_this() ) );

    // initialize connectors and properties
    requirements();
    connectors();
    properties();

    // now, the module is initialized but not necessarily usable (if not associated with a container)
    m_initialized( true );
    m_isUsable( m_initialized() && m_isAssociated() );

    // also set thread name
    setThreadName( getName() );
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

MODULE_TYPE WModule::getType() const
{
    return MODULE_ARBITRARY;
}

const WModule::InputConnectorList& WModule::getInputConnectors() const
{
    return m_inputConnectors;
}

const WModule::OutputConnectorList& WModule::getOutputConnectors() const
{
    return m_outputConnectors;
}

boost::shared_ptr< WModuleInputConnector > WModule::findInputConnector( std::string name )
{
    // simply search
    for( InputConnectorList::const_iterator listIter = m_inputConnectors.begin();
         listIter != m_inputConnectors.end(); ++listIter )
    {
        // try the canonical name
        if( ( name == ( *listIter )->getCanonicalName() ) || ( name == ( *listIter )->getName() ) )
        {
            return ( *listIter );
        }
    }

    return boost::shared_ptr< WModuleInputConnector >();
}

boost::shared_ptr< WModuleInputConnector > WModule::getInputConnector( std::string name )
{
    boost::shared_ptr< WModuleInputConnector > p = findInputConnector( name );

    if( !p )
    {
        throw WModuleConnectorNotFound( std::string( "The connector \"" ) + name +
                                        std::string( "\" does not exist in the module \"" ) + getName() + std::string( "\"." ) );
    }

    return p;
}

boost::shared_ptr< WModuleOutputConnector > WModule::findOutputConnector( std::string name )
{
    // simply search
    for( OutputConnectorList::const_iterator listIter = m_outputConnectors.begin();
         listIter != m_outputConnectors.end(); ++listIter )
    {
        // try the canonical name
        if( ( name == ( *listIter )->getCanonicalName() ) || ( name == ( *listIter )->getName() ) )
        {
            return ( *listIter );
        }
    }

    return boost::shared_ptr< WModuleOutputConnector >();
}

boost::shared_ptr< WModuleOutputConnector > WModule::getOutputConnector( std::string name )
{
    boost::shared_ptr< WModuleOutputConnector > p = findOutputConnector( name );

    if( !p )
    {
        throw WModuleConnectorNotFound( std::string( "The connector \"" ) + name +
                                        std::string( "\" does not exist in the module \"" ) + getName() +
                                        std::string( "\"." ) );
    }

    return p;
}

boost::shared_ptr< WModuleConnector > WModule::findConnector( std::string name )
{
    // simply search both
    boost::shared_ptr< WModuleConnector > p = findInputConnector( name );
    if( p ) // found?
    {
        return p;
    }

    // search in output list
    return findOutputConnector( name );
}

boost::shared_ptr< WModuleConnector > WModule::getConnector( std::string name )
{
    boost::shared_ptr< WModuleConnector > p = findConnector( name );

    if( !p )
    {
        throw WModuleConnectorNotFound( std::string( "The connector \"" ) + name +
                                        std::string( "\" does not exist in the module \"" ) + getName() +
                                        std::string( "\"." ) );
    }

    return p;
}

boost::signals2::connection WModule::subscribeSignal( MODULE_SIGNAL signal, t_ModuleGenericSignalHandlerType notifier )
{
    switch( signal )
    {
        case WM_READY:
            return signal_ready.connect( notifier );
        default:
            std::ostringstream s;
            s << "Could not subscribe to unknown signal.";
            throw WSignalSubscriptionFailed( s.str() );
            break;
    }
}

boost::signals2::connection WModule::subscribeSignal( MODULE_SIGNAL signal, t_ModuleErrorSignalHandlerType notifier )
{
    switch( signal)
    {
        case WM_ERROR:
            return signal_error.connect( notifier );
        default:
            std::ostringstream s;
            s << "Could not subscribe to unknown signal.";
            throw WSignalSubscriptionFailed( s.str() );
            break;
    }
}

const t_GenericSignalHandlerType WModule::getSignalHandler( MODULE_CONNECTOR_SIGNAL signal )
{
    switch( signal )
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
            throw WSignalUnknown( s.str() );
            break;
    }
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

const WBoolFlag& WModule::isReadyOrCrashed() const
{
    return m_isReadyOrCrashed;
}

const WBoolFlag& WModule::isRunning() const
{
    return m_isRunning;
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

boost::shared_ptr< WProperties > WModule::getInformationProperties() const
{
    return m_infoProperties;
}

boost::shared_ptr< WProgressCombiner > WModule::getRootProgressCombiner()
{
    return m_progress;
}

const char** WModule::getXPMIcon() const
{
    // return empty 1x1 icon by default.
    static const char * o_xpm[] =
        {
            "1 1 1 1",
            "   c None",
            " "
        };
    return o_xpm;
}

void WModule::ready()
{
    m_isReady( true );
    m_readyProgress->finish();
    signal_ready( shared_from_this() );
}

const WRequirement* WModule::checkRequirements() const
{
    // simply iterate all requirements and return the first found that is not fulfilled
    for( Requirements::const_iterator i = m_requirements.begin(); i != m_requirements.end(); ++i )
    {
        if( !( *i )->isComplied() )
        {
            return *i;
        }
    }

    return NULL;
}

void WModule::threadMain()
{
    WLogger::getLogger()->addLogMessage( "Starting module main method.", "Module (" + getName() + ")", LL_INFO );

    // check requirements
    const WRequirement* failedReq = checkRequirements();
    if( failedReq )
    {
        throw WModuleRequirementNotMet( failedReq );
    }

    // call main thread function
    m_isRunning( true );
    moduleMain();

    // NOTE: if there is any exception in the module thread, WThreadedRunner calls onThreadException for us. We can then disconnect the
    // module and call our own error notification mechanism.

    // remove all pending connections. This is important as connections that still exists after module deletion can cause segfaults when they get
    // disconnected in the connector destructor.
    disconnect();
    m_isRunning( false );
}

void WModule::onThreadException( const WException& e )
{
    // use our own error callback which includes the exact module pointer which caused the problem
    signal_error( shared_from_this(), e );

    // ensure the module is properly disconnected
    disconnect();

    // module is not running anymore.
    m_isRunning( false );

    // let WThreadedRunner do the remaining tasks.
    handleDeadlyException( e, "Module (" + getName() +")" );
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

void WModule::setLocalPath( boost::filesystem::path path )
{
    m_localPath = path;
}

boost::filesystem::path WModule::getLocalPath() const
{
    return m_localPath;
}

void WModule::setLibPath( boost::filesystem::path path )
{
    m_libPath = path;
}

boost::filesystem::path WModule::getLibPath() const
{
    return m_libPath;
}

void WModule::setPackageName( std::string name )
{
    m_packageName = name;
}

std::string WModule::getPackageName() const
{
    return m_packageName;
}

bool WModule::isDeprecated() const
{
    return !deprecated().empty();
}

std::string WModule::getDeprecationMessage() const
{
    return deprecated();
}

