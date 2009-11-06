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

#include "../common/WLogger.h"

#include "../modules/data/WMData.hpp"
#include "../modules/navSlices/WMNavSlices.h"
#include "../modules/coordinateSystem/WMCoordinateSystem.h"
#include "../modules/fiberDisplay/WMFiberDisplay.h"
#include "../modules/fiberCulling/WMFiberCulling.h"
#include "../modules/fiberClustering/WMFiberClustering.h"
#include "../modules/marchingCubes/WMMarchingCubes.h"
#include "../modules/eegTest/WMEEGTest.h"

#include "exceptions/WPrototypeUnknown.h"

#include "WModuleFactory.h"

// factory instance as singleton
boost::shared_ptr< WModuleFactory > WModuleFactory::m_instance = boost::shared_ptr< WModuleFactory >();

WModuleFactory::WModuleFactory()
{
    // initialize members
}

WModuleFactory::~WModuleFactory()
{
    // cleanup
}

void WModuleFactory::load()
{
    // load modules
    WLogger::getLogger()->addLogMessage( "Loading Modules", "ModuleFactory", LL_INFO );

    // operation must be exclusive
    boost::unique_lock< boost::shared_mutex > lock = boost::unique_lock< boost::shared_mutex >( m_prototypesLock );

    // currently the prototypes are added by hand. This will be done automatically later.
    m_prototypes.insert( boost::shared_ptr< WModule >( new WMNavSlices() ) );
    m_prototypes.insert( boost::shared_ptr< WModule >( new WMFiberDisplay() ) );
    m_prototypes.insert( boost::shared_ptr< WModule >( new WMFiberCulling() ) );
    m_prototypes.insert( boost::shared_ptr< WModule >( new WMFiberClustering() ) );
    m_prototypes.insert( boost::shared_ptr< WModule >( new WMCoordinateSystem() ) );
    m_prototypes.insert( boost::shared_ptr< WModule >( new WMEEGTest() ) );
    m_prototypes.insert( boost::shared_ptr< WModule >( new WMMarchingCubes() ) );

    lock.unlock();

    // for this a read lock is sufficient
    boost::shared_lock< boost::shared_mutex > slock = boost::shared_lock< boost::shared_mutex >( m_prototypesLock );

    // initialize every module in the set
    for( std::set< boost::shared_ptr< WModule > >::iterator list_iter = m_prototypes.begin(); list_iter != m_prototypes.end();
            ++list_iter )
    {
        WLogger::getLogger()->addLogMessage( "Loading module: " + ( *list_iter )->getName(), "ModuleFactory", LL_DEBUG );
        ( *list_iter )->initialize();
    }

    slock.unlock();
}

boost::shared_ptr< WModule > WModuleFactory::create( boost::shared_ptr< WModule > prototype )
{
    // for this a read lock is sufficient
    boost::shared_lock< boost::shared_mutex > slock = boost::shared_lock< boost::shared_mutex >( m_prototypesLock );

    // ensure this one is a prototype and nothing else
    if ( m_prototypes.count( prototype ) == 0 )
    {
        throw WPrototypeUnknown( "Could not clone module " + prototype->getName() + " since it is no prototype." );
    }

    slock.unlock();

    // call prototypes factory function
    boost::shared_ptr< WModule > clone = boost::shared_ptr< WModule >( prototype->factory() );
    clone->initialize();

    return clone;
}

boost::shared_ptr< WModuleFactory > WModuleFactory::getModuleFactory()
{
    if ( !m_instance )
    {
        m_instance = boost::shared_ptr< WModuleFactory >( new WModuleFactory() );
    }

    return m_instance;
}

