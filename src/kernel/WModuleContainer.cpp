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

#include <string>
#include <sstream>

#include "WModule.h"
#include "exceptions/WModuleUninitialized.h"
#include "../common/WLogger.h"

#include "WModuleContainer.h"

WModuleContainer::WModuleContainer(): boost::enable_shared_from_this< WModuleContainer >()
{
    // initialize members
}

WModuleContainer::~WModuleContainer()
{
    // cleanup
}

void WModuleContainer::add( boost::shared_ptr< WModule > module )
{
    if ( !module->isInitialized() )
    {
        std::ostringstream s;
        s << "Could not add module " << module->getName() << " to container. Reason: module not initialized.";

        throw WModuleUninitialized( s.str() );
    }

    // already associated with this container?
    if ( module->getAssociatedContainer() == shared_from_this() )
    {
        return;
    }

    // is this module already associated?
    if ( module->isAssociated() )
    {
        module->getAssociatedContainer()->remove( module );
    }

    // get write lock
    boost::unique_lock<boost::shared_mutex> lock = boost::unique_lock<boost::shared_mutex>( m_moduleSetLock );
    m_modules.insert( module );
    lock.unlock();
    module->setAssociatedContainer( shared_from_this() );

    // now module->isUsable() is true
    // -> so run it
    module->run();
}

void WModuleContainer::remove( boost::shared_ptr< WModule > module )
{
    if ( module->getAssociatedContainer() != shared_from_this() )
    {
        return;
    }

    // stop module
    WLogger::getLogger()->addLogMessage( "Waiting for module " + module->getName() + " to finish." , "ModuleContainer", LL_DEBUG );
    module->wait( true );

    // get write lock
    boost::unique_lock<boost::shared_mutex> lock = boost::unique_lock<boost::shared_mutex>( m_moduleSetLock );
    m_modules.erase( module );
    lock.unlock();
    module->setAssociatedContainer( boost::shared_ptr< WModuleContainer >() );

    // TODO(ebaum): flat or deep removal? What to do with associated modules?
}

