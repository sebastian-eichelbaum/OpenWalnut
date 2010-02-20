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

#include "WModule.h"
#include "exceptions/WModuleUninitialized.h"
#include "exceptions/WModuleAlreadyAssociated.h"
#include "exceptions/WModuleSignalSubscriptionFailed.h"
#include "../common/WLogger.h"
#include "../common/WThreadedRunner.h"
#include "WKernel.h"
#include "WModuleFactory.h"
#include "WModuleTypes.h"
#include "WModuleInputConnector.h"
#include "WModuleOutputConnector.h"
#include "WBatchLoader.h"

#include "../modules/data/WMData.h"

#include "WModuleContainer.h"

WModuleContainer::WModuleContainer( std::string name, std::string description ):
    WModule(),
    m_name( name ),
    m_description( description )
{
    WLogger::getLogger()->addLogMessage( "Constructing module container." , "ModuleContainer (" + getName() + ")", LL_INFO );
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
    WLogger::getLogger()->addLogMessage( "Adding module \"" + module->getName() + "\" to container." ,
            "ModuleContainer (" + getName() + ")", LL_INFO );

    if ( !module->isInitialized()() )
    {
        std::ostringstream s;
        s << "Could not add module \"" << module->getName() << "\" to container \"" + getName() + "\". Reason: module not initialized.";

        throw WModuleUninitialized( s.str() );
    }

    // already associated with this container?
    if ( module->getAssociatedContainer() == shared_from_this() )
    {
        return;
    }

    // is this module already associated?
    if ( module->isAssociated()() )
    {
        module->getAssociatedContainer()->remove( module );
    }

    // get write lock
    boost::unique_lock<boost::shared_mutex> lock = boost::unique_lock<boost::shared_mutex>( m_moduleSetLock );
    m_modules.insert( module );
    lock.unlock();
    module->setAssociatedContainer( boost::shared_static_cast< WModuleContainer >( shared_from_this() ) );
    WLogger::getLogger()->addLogMessage( "Associated module \"" + module->getName() + "\" with container." , "ModuleContainer (" + getName() + ")",
            LL_INFO );

    // now module->isUsable() is true
    // -> so run it

    // connect default ready/error notifiers
    boost::shared_lock<boost::shared_mutex> slock = boost::shared_lock<boost::shared_mutex>( m_errorNotifiersLock );
    for ( std::list< t_ModuleErrorSignalHandlerType >::iterator iter = m_errorNotifiers.begin(); iter != m_errorNotifiers.end(); ++iter)
    {
        module->subscribeSignal( WM_ERROR, ( *iter ) );
    }
    slock = boost::shared_lock<boost::shared_mutex>( m_associatedNotifiersLock );
    for ( std::list< t_ModuleGenericSignalHandlerType >::iterator iter = m_associatedNotifiers.begin(); iter != m_associatedNotifiers.end(); ++iter)
    {
        // call associated notifier
        ( *iter )( module );
    }
    slock = boost::shared_lock<boost::shared_mutex>( m_readyNotifiersLock );
    for ( std::list< t_ModuleGenericSignalHandlerType >::iterator iter = m_readyNotifiers.begin(); iter != m_readyNotifiers.end(); ++iter)
    {
        module->subscribeSignal( WM_READY, ( *iter ) );
    }
    slock.unlock();

    // add the modules progress to local progress combiner
    m_progress->addSubProgress( module->getRootProgressCombiner() );

    // run it
    if ( run )
    {
        module->run();
    }
}

void WModuleContainer::remove( boost::shared_ptr< WModule > module )
{
    WLogger::getLogger()->addLogMessage( "Removing module \"" + module->getName() + "\" from container." , "ModuleContainer (" + getName() + ")",
            LL_DEBUG );

    if ( module->getAssociatedContainer() != shared_from_this() )
    {
        return;
    }

    // stop module
    WLogger::getLogger()->addLogMessage( "Waiting for module \"" + module->getName() + "\" to finish." , "ModuleContainer (" + getName() + ")",
            LL_DEBUG );
    module->wait( true );

    // get write lock
    boost::unique_lock<boost::shared_mutex> lock = boost::unique_lock<boost::shared_mutex>( m_moduleSetLock );
    m_modules.erase( module );
    lock.unlock();
    module->setAssociatedContainer( boost::shared_ptr< WModuleContainer >() );

    // TODO(ebaum): remove signal subscriptions
    // TODO(ebaum): remove progress from combiner
    // TODO(ebaum): flat or deep removal? What to do with associated modules?
}

WModuleContainer::DataModuleListType WModuleContainer::getDataModules()
{
    DataModuleListType l;

    boost::shared_lock<boost::shared_mutex> slock = boost::shared_lock<boost::shared_mutex>( m_moduleSetLock );

    // iterate module list
    for( std::set< boost::shared_ptr< WModule > >::iterator iter = m_modules.begin(); iter != m_modules.end(); ++iter )
    {
        // is this module a data module?
        if ( ( *iter )->getType() == MODULE_DATA )
        {
            boost::shared_ptr< WMData > dm = boost::shared_static_cast< WMData >( *iter );

            // now check the contained dataset ( isTexture and whether it is ready )
            if ( dm->isReady()() )
            {
                l.insert( dm );
            }
        }
    }
    slock.unlock();

    // now sort the list using the sorter

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

    // read lock
    slock = boost::shared_lock<boost::shared_mutex>( m_moduleSetLock );
    for( std::set< boost::shared_ptr< WModule > >::iterator listIter = m_modules.begin(); listIter != m_modules.end(); ++listIter )
    {
        WLogger::getLogger()->addLogMessage( "Waiting for module \"" + ( *listIter )->getName() + "\" to finish." ,
                "ModuleContainer (" + getName() + ")", LL_INFO );
        ( *listIter )->wait( true );
    }
    slock.unlock();

    // get write lock
    boost::unique_lock<boost::shared_mutex> lock = boost::unique_lock<boost::shared_mutex>( m_moduleSetLock );
    m_modules.clear();
    lock.unlock();
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

boost::shared_ptr< WModule > WModuleContainer::applyModule( boost::shared_ptr< WModule > applyOn, std::string what, bool tryOnly )
{
    boost::shared_ptr< WModule >prototype = boost::shared_ptr< WModule >();
    if ( tryOnly )
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
    if ( applyOn->isAssociated()() && ( applyOn->getAssociatedContainer() != shared_from_this() ) )
    {
        throw WModuleAlreadyAssociated( "The specified module \"" + applyOn->getName() + "\" is associated with another container." );
    }

    // create a new initialized instance of the module
    boost::shared_ptr< WModule > m = WModuleFactory::getModuleFactory()->create( prototype );

    // add it
    add( m, true );
    applyOn->isReady().wait();
    m->isReady().wait();

    // get offered outputs
    std::set<boost::shared_ptr<WModuleInputConnector> > ins = m->getInputConnectors();
    // get offered inputs
    std::set<boost::shared_ptr<WModuleOutputConnector> > outs = applyOn->getOutputConnectors();

    // TODO(ebaum): search best matching instead of simply connecting both
    if ( !ins.empty() && !outs.empty() )
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

