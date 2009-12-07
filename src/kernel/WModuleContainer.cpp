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
#include <string>
#include <sstream>

#include "WModule.h"
#include "exceptions/WModuleUninitialized.h"
#include "exceptions/WModuleAlreadyAssociated.h"
#include "exceptions/WModuleSignalSubscriptionFailed.h"
#include "../common/WLogger.h"
#include "WKernel.h"
#include "WModuleFactory.h"

#include "WModuleContainer.h"

WModuleContainer::WModuleContainer( std::string name, std::string description ):
    boost::enable_shared_from_this< WModuleContainer >(),
    m_name( name ),
    m_description( description )
{
    WLogger::getLogger()->addLogMessage( "Constructing module container." , "ModuleContainer (" + m_name + ")", LL_INFO );
    // initialize members
}

WModuleContainer::~WModuleContainer()
{
    // cleanup
}

void WModuleContainer::add( boost::shared_ptr< WModule > module, bool run )
{
    WLogger::getLogger()->addLogMessage( "Adding module \"" + module->getName() + "\" to container." , "ModuleContainer (" + m_name + ")", LL_INFO );

    if ( !module->isInitialized()() )
    {
        std::ostringstream s;
        s << "Could not add module \"" << module->getName() << "\" to container \"" + m_name + "\". Reason: module not initialized.";

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
    module->setAssociatedContainer( shared_from_this() );
    WLogger::getLogger()->addLogMessage( "Associated module \"" + module->getName() + "\" with container." , "ModuleContainer (" + m_name + ")",
            LL_INFO );

    // now module->isUsable() is true
    // -> so run it

    // connect default ready/error notifiers
    boost::shared_lock<boost::shared_mutex> slock = boost::shared_lock<boost::shared_mutex>( m_readyNotifiersLock );
    for ( std::list< t_ModuleGenericSignalHandlerType >::iterator iter = m_readyNotifiers.begin(); iter != m_readyNotifiers.end(); ++iter)
    {
        module->subscribeSignal( READY, ( *iter ) );
    }
    slock.unlock();
    slock = boost::shared_lock<boost::shared_mutex>( m_errorNotifiersLock );
    for ( std::list< t_ModuleErrorSignalHandlerType >::iterator iter = m_errorNotifiers.begin(); iter != m_errorNotifiers.end(); ++iter)
    {
        module->subscribeSignal( ERROR, ( *iter ) );
    }
    slock.unlock();

    // TODO(ebaum,schurade): this should be removes some days
    module->connectToGui();

    // run it
    if ( run )
    {
        module->run();
    }
}

void WModuleContainer::remove( boost::shared_ptr< WModule > module )
{
    WLogger::getLogger()->addLogMessage( "Removing module \"" + module->getName() + "\" from container." , "ModuleContainer (" + m_name + ")",
            LL_DEBUG );

    if ( module->getAssociatedContainer() != shared_from_this() )
    {
        return;
    }

    // stop module
    WLogger::getLogger()->addLogMessage( "Waiting for module \"" + module->getName() + "\" to finish." , "ModuleContainer (" + m_name + ")",
            LL_DEBUG );
    module->wait( true );

    // get write lock
    boost::unique_lock<boost::shared_mutex> lock = boost::unique_lock<boost::shared_mutex>( m_moduleSetLock );
    m_modules.erase( module );
    lock.unlock();
    module->setAssociatedContainer( boost::shared_ptr< WModuleContainer >() );

    // TODO(ebaum): flat or deep removal? What to do with associated modules?
}

void WModuleContainer::stop()
{
    WLogger::getLogger()->addLogMessage( "Stopping modules." , "ModuleContainer (" + m_name + ")", LL_INFO );

    // read lock
    boost::shared_lock<boost::shared_mutex> slock = boost::shared_lock<boost::shared_mutex>( m_moduleSetLock );
    for( std::set< boost::shared_ptr< WModule > >::iterator listIter = m_modules.begin(); listIter != m_modules.end(); ++listIter )
    {
        WLogger::getLogger()->addLogMessage( "Waiting for module \"" + ( *listIter )->getName() + "\" to finish." ,
                "ModuleContainer (" + m_name + ")", LL_INFO );
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
        case READY:
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
        case ERROR:
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

boost::shared_ptr< WModule > WModuleContainer::applyModule( boost::shared_ptr< WModule > applyOn, std::string what )
{
    return applyModule( applyOn, WModuleFactory::getModuleFactory()->getPrototypeByName( what ) );
}

boost::shared_ptr< WModule > WModuleContainer::applyModule( boost::shared_ptr< WModule > applyOn, boost::shared_ptr< WModule > prototype )
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

    // TODO(ebaum): do actual connection stuff
    WLogger::getLogger()->addLogMessage( "Combining modules \"" + applyOn->getName() + "\" and \"" + m->getName() + "\" not yet implemented.",
            "ModuleContainer (" + m_name + ")", LL_WARNING );

    return m;
}

