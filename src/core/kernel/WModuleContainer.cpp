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

#include <list>
#include <set>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <utility>

#include "../common/WLogger.h"
#include "../common/WThreadedRunner.h"
#include "WBatchLoader.h"
#include "WKernel.h"
#include "WModule.h"
#include "WModuleCombiner.h"
#include "WModuleFactory.h"
#include "WModuleInputConnector.h"
#include "WModuleOutputConnector.h"
#include "WModuleTypes.h"
#include "combiner/WApplyCombiner.h"
#include "exceptions/WModuleAlreadyAssociated.h"
#include "exceptions/WModuleSignalSubscriptionFailed.h"
#include "exceptions/WModuleUninitialized.h"
#include "WDataModule.h"

#include "WModuleContainer.h"

WModuleContainer::WModuleContainer( std::string name, std::string description ):
    WModule(),
    m_name( name ),
    m_description( description ),
    m_crashIfModuleCrashes( true )
{
    WLogger::getLogger()->addLogMessage( "Constructing module container." , "ModuleContainer (" + getName() + ")", LL_DEBUG );
    // initialize members
}

WModuleContainer::~WModuleContainer()
{
    // cleanup
}

void WModuleContainer::moduleMain()
{
    // do nothing here. The WModule class enforces us to overwrite this method here, but we do not need it.
    // Only set the ready flag.
    ready();
}

boost::shared_ptr< WModule > WModuleContainer::factory() const
{
    // this factory is not used actually.
    return boost::shared_ptr< WModule >( new WModuleContainer( getName(), getDescription() ) );
}

void WModuleContainer::add( boost::shared_ptr< WModule > module, bool run )
{
    if( !module )
    {
        // just ignore NULL Pointer
        return;
    }

    WLogger::getLogger()->addLogMessage( "Adding module \"" + module->getName() + "\" to container." ,
            "ModuleContainer (" + getName() + ")", LL_INFO );

    if( !module->isInitialized()() )
    {
        std::ostringstream s;
        s << "Could not add module \"" << module->getName() << "\" to container \"" + getName() + "\". Reason: module not initialized.";

        throw WModuleUninitialized( s.str() );
    }

    // already associated with this container?
    if( module->getAssociatedContainer() == shared_from_this() )
    {
        WLogger::getLogger()->addLogMessage( "Adding module \"" + module->getName() + "\" to container not needed. Its already inside." ,
            "ModuleContainer (" + getName() + ")", LL_INFO );
        return;
    }

    // is this module already associated?
    if( module->isAssociated()() )
    {
        module->getAssociatedContainer()->remove( module );
    }

    // get write lock
    ModuleSharedContainerType::WriteTicket wlock = m_modules.getWriteTicket();
    wlock->get().insert( module );
    wlock.reset();

    module->setAssociatedContainer( boost::shared_static_cast< WModuleContainer >( shared_from_this() ) );
    WLogger::getLogger()->addLogMessage( "Associated module \"" + module->getName() + "\" with container." , "ModuleContainer (" + getName() + ")",
            LL_INFO );

    // now module->isUsable() is true

    // Connect the error handler and all default handlers:
    ModuleSubscriptionsSharedType::WriteTicket subscriptionsLock = m_moduleSubscriptions.getWriteTicket();

    // connect the containers signal handler explicitly
    t_ModuleErrorSignalHandlerType func = boost::bind( &WModuleContainer::moduleError, this, _1, _2 );
    boost::signals2::connection signalCon = module->subscribeSignal( WM_ERROR, func );
    subscriptionsLock->get().insert( ModuleSubscription( module, signalCon ) );

    // connect default notifiers:
    boost::shared_lock<boost::shared_mutex> slock = boost::shared_lock<boost::shared_mutex>( m_errorNotifiersLock );
    for( std::list< t_ModuleErrorSignalHandlerType >::iterator iter = m_errorNotifiers.begin(); iter != m_errorNotifiers.end(); ++iter)
    {
        signalCon = module->subscribeSignal( WM_ERROR, ( *iter ) );
        subscriptionsLock->get().insert( ModuleSubscription( module, signalCon ) );
    }
    slock = boost::shared_lock<boost::shared_mutex>( m_associatedNotifiersLock );
    for( std::list< t_ModuleGenericSignalHandlerType >::iterator iter = m_associatedNotifiers.begin(); iter != m_associatedNotifiers.end(); ++iter)
    {
        // call associated notifier
        ( *iter )( module );
    }
    slock = boost::shared_lock<boost::shared_mutex>( m_connectorNotifiersLock );
    for( std::list< t_GenericSignalHandlerType >::iterator iter = m_connectorEstablishedNotifiers.begin();
                                                            iter != m_connectorEstablishedNotifiers.end(); ++iter )
    {
        // subscribe on each input
        for( InputConnectorList::const_iterator ins = module->getInputConnectors().begin(); ins != module->getInputConnectors().end(); ++ins )
        {
            signalCon = ( *ins )->subscribeSignal( CONNECTION_ESTABLISHED, ( *iter ) );
            subscriptionsLock->get().insert( ModuleSubscription( module, signalCon ) );
        }
    }
    for( std::list< t_GenericSignalHandlerType >::iterator iter = m_connectorClosedNotifiers.begin();
                                                            iter != m_connectorClosedNotifiers.end(); ++iter )
    {
        // subscribe on each input
        for( InputConnectorList::const_iterator ins = module->getInputConnectors().begin(); ins != module->getInputConnectors().end(); ++ins )
        {
            signalCon = ( *ins )->subscribeSignal( CONNECTION_CLOSED, ( *iter ) );
            subscriptionsLock->get().insert( ModuleSubscription( module, signalCon ) );
        }
    }
    slock = boost::shared_lock<boost::shared_mutex>( m_readyNotifiersLock );
    for( std::list< t_ModuleGenericSignalHandlerType >::iterator iter = m_readyNotifiers.begin(); iter != m_readyNotifiers.end(); ++iter)
    {
        signalCon = module->subscribeSignal( WM_READY, ( *iter ) );
        subscriptionsLock->get().insert( ModuleSubscription( module, signalCon ) );
    }
    slock.unlock();

    // free the subscriptions lock
    subscriptionsLock.reset();

    // add the modules progress to local progress combiner
    m_progress->addSubProgress( module->getRootProgressCombiner() );

    // run it
    if( run )
    {
        module->run();
    }
}

void WModuleContainer::remove( boost::shared_ptr< WModule > module )
{
    // simple flat removal.

    WLogger::getLogger()->addLogMessage( "Removing module \"" + module->getName() + "\" from container." , "ModuleContainer (" + getName() + ")",
            LL_DEBUG );

    if( module->getAssociatedContainer() != shared_from_this() )
    {
        return;
    }

    // remove connections inside this container
    module->disconnect();

    // remove progress combiner
    m_progress->removeSubProgress( module->getRootProgressCombiner() );

    // remove signal subscriptions to this containers default notifiers
    ModuleSubscriptionsSharedType::WriteTicket subscriptionsLock = m_moduleSubscriptions.getWriteTicket();

    // find all subscriptions for this module
    std::pair< ModuleSubscriptionsIterator, ModuleSubscriptionsIterator > subscriptions = subscriptionsLock->get().equal_range( module );
    for( ModuleSubscriptionsIterator it = subscriptions.first; it != subscriptions.second; ++it )
    {
        // disconnect subscription.
        ( *it ).second.disconnect();
    }
    // erase them
    subscriptionsLock->get().erase( subscriptions.first, subscriptions.second );
    subscriptionsLock.reset();

    // get write lock
    ModuleSharedContainerType::WriteTicket wlock = m_modules.getWriteTicket();
    wlock->get().erase( module );
    wlock.reset();

    module->setAssociatedContainer( boost::shared_ptr< WModuleContainer >() );

    // tell all interested about removal
    boost::shared_lock<boost::shared_mutex> slock = boost::shared_lock<boost::shared_mutex>( m_removedNotifiersLock );
    for( std::list< t_ModuleGenericSignalHandlerType >::iterator iter = m_removedNotifiers.begin(); iter != m_removedNotifiers.end(); ++iter)
    {
        // call associated notifier
        ( *iter )( module );
    }
    slock.unlock();
}

void WModuleContainer::removeDeep( boost::shared_ptr< WModule > module )
{
    WLogger::getLogger()->addLogMessage( "Deep removal of modules is not yet implemented.", "ModuleContainer (" + getName() + ")", LL_WARNING );

    // at least, remove the module itself
    remove( module );
}

WModuleContainer::DataModuleListType WModuleContainer::getDataModules()
{
    DataModuleListType l;

    // lock, unlocked if l looses focus
    ModuleSharedContainerType::ReadTicket lock = m_modules.getReadTicket();

    // iterate module list
    for( ModuleConstIterator iter = lock->get().begin(); iter != lock->get().end(); ++iter )
    {
        // is this module a data module?
        if( ( *iter )->getType() == MODULE_DATA )
        {
            boost::shared_ptr< WDataModule > dm = boost::shared_static_cast< WDataModule >( *iter );

            // now check the contained dataset ( isTexture and whether it is ready )
            if( dm->isReady()() )
            {
                l.insert( dm );
            }
        }
    }

    return l;
}

void WModuleContainer::stop()
{
    WLogger::getLogger()->addLogMessage( "Stopping pending threads." , "ModuleContainer (" + getName() + ")", LL_INFO );

    // read lock
    boost::shared_lock<boost::shared_mutex> slock = boost::shared_lock<boost::shared_mutex>( m_pendingThreadsLock );
    for( std::set< boost::shared_ptr< WThreadedRunner > >::iterator listIter = m_pendingThreads.begin(); listIter != m_pendingThreads.end();
            ++listIter )
    {
        ( *listIter )->wait( true );
    }
    slock.unlock();

    WLogger::getLogger()->addLogMessage( "Stopping modules." , "ModuleContainer (" + getName() + ")", LL_INFO );

    // lock, unlocked if l looses focus
    ModuleSharedContainerType::ReadTicket lock = m_modules.getReadTicket();

    for( ModuleConstIterator listIter = lock->get().begin(); listIter != lock->get().end(); ++listIter )
    {
        WLogger::getLogger()->addLogMessage( "Waiting for module \"" + ( *listIter )->getName() + "\" to finish." ,
                "ModuleContainer (" + getName() + ")", LL_INFO );
        ( *listIter )->wait( true );
        ( *listIter )->setAssociatedContainer( boost::shared_ptr< WModuleContainer >() );   // remove last refs to this container inside the module
    }
    lock.reset();

    // get write lock
    // lock, unlocked if l looses focus
    ModuleSharedContainerType::WriteTicket wlock = m_modules.getWriteTicket();
    wlock->get().clear();
}

const std::string WModuleContainer::getName() const
{
    return m_name;
}

const std::string WModuleContainer::getDescription() const
{
    return m_description;
}

void WModuleContainer::addDefaultNotifier( MODULE_SIGNAL signal, t_ModuleGenericSignalHandlerType notifier )
{
    boost::unique_lock<boost::shared_mutex> lock;
    switch (signal)
    {
        case WM_ASSOCIATED:
            lock = boost::unique_lock<boost::shared_mutex>( m_associatedNotifiersLock );
            m_associatedNotifiers.push_back( notifier );
            lock.unlock();
            break;
        case WM_READY:
            lock = boost::unique_lock<boost::shared_mutex>( m_readyNotifiersLock );
            m_readyNotifiers.push_back( notifier );
            lock.unlock();
            break;
        case WM_REMOVED:
            lock = boost::unique_lock<boost::shared_mutex>( m_removedNotifiersLock );
            m_removedNotifiers.push_back( notifier );
            lock.unlock();
            break;
        default:
            std::ostringstream s;
            s << "Could not subscribe to unknown signal.";
            throw WModuleSignalSubscriptionFailed( s.str() );
            break;
    }
}

void WModuleContainer::addDefaultNotifier( MODULE_SIGNAL signal, t_ModuleErrorSignalHandlerType notifier )
{
    boost::unique_lock<boost::shared_mutex> lock;
    switch (signal)
    {
        case WM_ERROR:
            lock = boost::unique_lock<boost::shared_mutex>( m_errorNotifiersLock );
            m_errorNotifiers.push_back( notifier );
            lock.unlock();
            break;
        default:
            std::ostringstream s;
            s << "Could not subscribe to unknown signal.";
            throw WModuleSignalSubscriptionFailed( s.str() );
            break;
    }
}

void WModuleContainer::addDefaultNotifier( MODULE_CONNECTOR_SIGNAL signal, t_GenericSignalHandlerType notifier )
{
    boost::unique_lock<boost::shared_mutex> lock;
    switch (signal)
    {
        case CONNECTION_ESTABLISHED:
            lock = boost::unique_lock<boost::shared_mutex>( m_connectorNotifiersLock );
            m_connectorEstablishedNotifiers.push_back( notifier );
            lock.unlock();
            break;
        case CONNECTION_CLOSED:
            lock = boost::unique_lock<boost::shared_mutex>( m_connectorNotifiersLock );
            m_connectorClosedNotifiers.push_back( notifier );
            lock.unlock();
            break;
        default:
            std::ostringstream s;
            s << "Could not subscribe to unknown signal.";
            throw WModuleSignalSubscriptionFailed( s.str() );
            break;
    }
}

boost::shared_ptr< WModule > WModuleContainer::applyModule( boost::shared_ptr< WModule > applyOn, std::string what, bool tryOnly )
{
    boost::shared_ptr< WModule >prototype = boost::shared_ptr< WModule >();
    if( tryOnly )
    {
        // isPrototypeAvailable returns the prototype or NULL if not found, but does not throw an exception
        prototype = WModuleFactory::getModuleFactory()->isPrototypeAvailable( what );
        if( !prototype )
        {
            return prototype;
        }
    }
    else
    {
        prototype = WModuleFactory::getModuleFactory()->getPrototypeByName( what );
    }

    return applyModule( applyOn, prototype );
}

boost::shared_ptr< WModule > WModuleContainer::applyModule( boost::shared_ptr< WModule > applyOn,
                                                                         boost::shared_ptr< WModule > prototype )
{
    // is this module already associated with another container?
    if( applyOn->isAssociated()() && ( applyOn->getAssociatedContainer() != shared_from_this() ) )
    {
        throw WModuleAlreadyAssociated( std::string( "The specified module \"" ) + applyOn->getName() +
                                        std::string( "\" is associated with another container." ) );
    }

    // create a new initialized instance of the module
    boost::shared_ptr< WModule > m = WModuleFactory::getModuleFactory()->create( prototype );

    // add it
    add( m, true );
    applyOn->isReadyOrCrashed().wait();
    m->isReadyOrCrashed().wait();

    // should we ignore the crash case? In general, a crashed module can be connected. The sense or non-sense of it is questionable but assume a
    // crashed module has set some data on its output and some other module needs it. -> so we ignore the case of crashed modules here.

    // get offered outputs
    WModule::InputConnectorList ins = m->getInputConnectors();
    // get offered inputs
    WModule::OutputConnectorList outs = applyOn->getOutputConnectors();

    // connect the first connectors. For a more sophisticated way of connecting modules, use ModuleCombiners.
    if( !ins.empty() && !outs.empty() )
    {
        ( *ins.begin() )->connect( ( *outs.begin() ) );
    }

    return m;
}

boost::shared_ptr< WBatchLoader > WModuleContainer::loadDataSets( std::vector< std::string > fileNames )
{
    // create thread which actually loads the data
    boost::shared_ptr< WBatchLoader > t = boost::shared_ptr< WBatchLoader >( new WBatchLoader( fileNames,
                boost::shared_static_cast< WModuleContainer >( shared_from_this() ) )
    );
    t->run();
    return t;
}

void WModuleContainer::loadDataSetsSynchronously( std::vector< std::string > fileNames )
{
    // create thread which actually loads the data
    boost::shared_ptr< WBatchLoader > t = boost::shared_ptr< WBatchLoader >( new WBatchLoader( fileNames,
                boost::shared_static_cast< WModuleContainer >( shared_from_this() ) )
    );
    t->run();
    t->wait();
}

void WModuleContainer::addPendingThread( boost::shared_ptr< WThreadedRunner > thread )
{
    boost::unique_lock<boost::shared_mutex> lock = boost::unique_lock<boost::shared_mutex>( m_pendingThreadsLock );
    m_pendingThreads.insert( thread );
    lock.unlock();
}

void WModuleContainer::finishedPendingThread( boost::shared_ptr< WThreadedRunner > thread )
{
    boost::unique_lock<boost::shared_mutex> lock = boost::unique_lock<boost::shared_mutex>( m_pendingThreadsLock );
    m_pendingThreads.erase( thread );
    lock.unlock();
}

void WModuleContainer::moduleError( boost::shared_ptr< WModule > module, const WException& exception )
{
    errorLog() << "Error in module \"" << module->getName() << "\". Forwarding to nesting container.";

    // simply forward it to the other signal handler
    signal_error( module, exception );

    if( m_crashIfModuleCrashes )
    {
        infoLog() << "Crash caused this container to shutdown.";
        requestStop();
        m_isCrashed( true );
    }
}

void WModuleContainer::setCrashIfModuleCrashes( bool crashIfCrashed )
{
    m_crashIfModuleCrashes = crashIfCrashed;
}

WModuleContainer::ModuleSharedContainerType::ReadTicket WModuleContainer::getModules() const
{
    return m_modules.getReadTicket();
}

WModuleContainer::ModuleVectorType WModuleContainer::getModules( std::string name ) const
{
    // get the list of all first.
    WModuleContainer::ModuleSharedContainerType::ReadTicket lock = getModules();

    // put results in here
    WModuleContainer::ModuleVectorType result;

    // handle each module
    for( ModuleConstIterator listIter = lock->get().begin(); listIter != lock->get().end(); ++listIter )
    {
        // check name
        if( name == ( *listIter )->getName() )
        {
            result.push_back( ( *listIter ) );
        }
    }

    return result;
}

WCombinerTypes::WCompatiblesList WModuleContainer::getPossibleConnections( boost::shared_ptr< WModule > module )
{
    WCombinerTypes::WCompatiblesList complist;

    if( !module )
    {
        // be nice in case of a null pointer
        return complist;
    }

    // read lock the container
    ModuleSharedContainerType::ReadTicket lock = m_modules.getReadTicket();

    // handle each module
    for( ModuleConstIterator listIter = lock->get().begin(); listIter != lock->get().end(); ++listIter )
    {
        WCombinerTypes::WOneToOneCombiners lComp = WApplyCombiner::createCombinerList< WApplyCombiner>( module, ( *listIter ) );

        if( lComp.size() != 0 )
        {
            complist.push_back( WCombinerTypes::WCompatiblesGroup( ( *listIter ), lComp ) );
        }
    }

    // sort the compatibles
    std::sort( complist.begin(), complist.end(), WCombinerTypes::compatiblesSort );

    return complist;
}

