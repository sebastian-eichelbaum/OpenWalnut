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
#include <iostream>
#include <set>
#include <string>
#include <typeinfo>
#include <vector>

#include "../common/WLogger.h"
#include "combiner/WApplyCombiner.h"
#include "exceptions/WPrototypeNotUnique.h"
#include "exceptions/WPrototypeUnknown.h"
#include "WModule.h"
#include "WModuleCombiner.h"
#include "WModuleFactory.h"

// factory instance as singleton
boost::shared_ptr< WModuleFactory > WModuleFactory::m_instance = boost::shared_ptr< WModuleFactory >();

WModuleFactory::WModuleFactory():
    m_prototypes(),
    m_moduleLoader()
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
    PrototypeSharedContainerType::WriteTicket l = m_prototypes.getWriteTicket();

    // Load the dynamic modules here:
    m_moduleLoader.load( l );

    // initialize every module in the set
    std::set< std::string > names;  // helper to find duplicates
    PrototypeContainerIteratorType listIter = l->get().begin();
    while( listIter != l->get().end() )
    {
        WLogger::getLogger()->addLogMessage( "Initializing module prototype: \"" + ( *listIter )->getName() + "\"", "ModuleFactory", LL_DEBUG );

        // that should not happen. Names should not occur multiple times since they are unique
        if( names.count( ( *listIter )->getName() ) )
        {
            WLogger::getLogger()->addLogMessage( std::string( "Module \"" + ( *listIter )->getName() +
                                                               "\" is not unique. Modules have to have a unique name. Ignoring this module." ),
                                                 "ModuleFactory", LL_ERROR );
            // we remove the module from the prototype list
            l->get().erase( listIter++ );
            continue;
        }
        else
        {
            names.insert( ( *listIter )->getName() );
            initializeModule( ( *listIter ) );
            ++listIter;
        }
    }
    WLogger::getLogger()->addLogMessage( "Loading Modules Done", "ModuleFactory", LL_INFO );
}

bool WModuleFactory::isPrototype( boost::shared_ptr< WModule > module )
{
    // for this a read lock is sufficient, gets unlocked if it looses scope
    PrototypeSharedContainerType::ReadTicket l = getModuleFactory()->m_prototypes.getReadTicket();
    return getModuleFactory()->checkPrototype( module, l );
}

bool WModuleFactory::checkPrototype( boost::shared_ptr< WModule > module, PrototypeSharedContainerType::ReadTicket ticket )
{
    return ( ticket->get().count( module ) != 0 );
}

boost::shared_ptr< WModule > WModuleFactory::create( boost::shared_ptr< WModule > prototype )
{
    wlog::debug( "ModuleFactory" ) << "Creating new instance of prototype \"" << prototype->getName() << "\".";

    // for this a read lock is sufficient, gets unlocked if it looses scope
    PrototypeSharedContainerType::ReadTicket l = m_prototypes.getReadTicket();

    // ensure this one is a prototype and nothing else
    if( !checkPrototype( prototype, l ) )
    {
        throw WPrototypeUnknown( std::string( "Could not clone module \"" + prototype->getName() + "\" since it is no prototype." ) );
    }

    // explicitly unlock
    l.reset();

    // call prototypes factory function
    boost::shared_ptr< WModule > clone = boost::shared_ptr< WModule >( prototype->factory() );
    clone->setLocalPath( prototype->getLocalPath() );   // prototype and clone have the same local path.
    initializeModule( clone );

    return clone;
}

void WModuleFactory::initializeModule( boost::shared_ptr< WModule > module )
{
    module->initialize();
}

boost::shared_ptr< WModuleFactory > WModuleFactory::getModuleFactory()
{
    if( !m_instance )
    {
        m_instance = boost::shared_ptr< WModuleFactory >( new WModuleFactory() );
    }

    return m_instance;
}


const boost::shared_ptr< WModule > WModuleFactory::isPrototypeAvailable( std::string name )
{
    // for this a read lock is sufficient, gets unlocked if it looses scope
    PrototypeSharedContainerType::ReadTicket l = m_prototypes.getReadTicket();

    // find first and only prototype (ensured during load())
    boost::shared_ptr< WModule > ret = boost::shared_ptr< WModule >();
    for( std::set< boost::shared_ptr< WModule > >::const_iterator listIter = l->get().begin(); listIter != l->get().end();
            ++listIter )
    {
        if( ( *listIter )->getName() == name )
        {
            ret = ( *listIter );
            break;
        }
    }

    return ret;
}

const boost::shared_ptr< WModule > WModuleFactory::getPrototypeByName( std::string name )
{
    boost::shared_ptr< WModule > ret = isPrototypeAvailable( name );

    // if not found -> throw
    if( ret == boost::shared_ptr< WModule >() )
    {
        throw WPrototypeUnknown( std::string( "Could not find prototype \"" + name + "\"." ) );
    }

    return ret;
}

const boost::shared_ptr< WModule > WModuleFactory::getPrototypeByInstance( boost::shared_ptr< WModule > instance )
{
    return getPrototypeByName( instance->getName() );
}

std::vector< WModule::ConstSPtr > WModuleFactory::getPrototypesByType( MODULE_TYPE type )
{
    std::vector< WModule::ConstSPtr > ret;

    // for this a read lock is sufficient, gets unlocked if it looses scope
    PrototypeSharedContainerType::ReadTicket l = m_prototypes.getReadTicket();

    // find first and only prototype (ensured during load())
    for( std::set< boost::shared_ptr< WModule > >::const_iterator listIter = l->get().begin(); listIter != l->get().end();
            ++listIter )
    {
        if( ( *listIter )->getType() == type )
        {
            ret.push_back( *listIter );
        }
    }

    return ret;
}

WModuleFactory::PrototypeSharedContainerType::ReadTicket WModuleFactory::getPrototypes() const
{
    return m_prototypes.getReadTicket();
}

WCombinerTypes::WCompatiblesList WModuleFactory::getCompatiblePrototypes( boost::shared_ptr< WModule > module )
{
    WCombinerTypes::WCompatiblesList compatibles;

    // for this a read lock is sufficient, gets unlocked if it looses scope
    PrototypeSharedContainerType::ReadTicket l = m_prototypes.getReadTicket();

    // has the module an output? If not, return.
    bool addModulesWithoutInput = !module;

    if( addModulesWithoutInput )
    {
        // First, add all modules with no input connector.
        for( PrototypeContainerConstIteratorType listIter = l->get().begin(); listIter != l->get().end();
                ++listIter )
        {
            // get connectors of this prototype
            WModule::InputConnectorList pcons = ( *listIter )->getInputConnectors();
            if(  pcons.size() == 0  )
            {
                // the modules which match every time need their own groups
                WCombinerTypes::WOneToOneCombiners lComp;

                // NOTE: it is OK here to use the variable module even if it is NULL as the combiner in this case only adds the specified module
                lComp.push_back( boost::shared_ptr< WApplyCombiner >( new WApplyCombiner( module, "", *listIter, "" ) ) );

                // add this list
                compatibles.push_back( WCombinerTypes::WCompatiblesGroup( ( *listIter ), lComp ) );
            }
        }
    }

    // if NULL was specified, only return all modules without any inputs
    if( module )
    {
        // go through every prototype
        for( PrototypeContainerConstIteratorType listIter = l->get().begin(); listIter != l->get().end();
             ++listIter )
        {
            WCombinerTypes::WOneToOneCombiners lComp = WApplyCombiner::createCombinerList< WApplyCombiner >( module, ( *listIter ) );

            // add the group
            if( lComp.size() != 0 )
            {
                compatibles.push_back( WCombinerTypes::WCompatiblesGroup( ( *listIter ), lComp ) );
            }
        }
    }

    // unlock. No locking needed for further steps.
    l.reset();

    // sort the compatibles
    std::sort( compatibles.begin(), compatibles.end(), WCombinerTypes::compatiblesSort );

    return compatibles;
}

WCombinerTypes::WCompatiblesList WModuleFactory::getAllPrototypes()
{
    WCombinerTypes::WCompatiblesList compatibles;

    // for this a read lock is sufficient, gets unlocked if it looses scope
    PrototypeSharedContainerType::ReadTicket l = m_prototypes.getReadTicket();

    // Add all modules.
    for( PrototypeContainerConstIteratorType listIter = l->get().begin(); listIter != l->get().end();
            ++listIter )
    {
        // the modules which match every time need their own groups
        WCombinerTypes::WOneToOneCombiners lComp;

        // NOTE: it is OK here to use the variable module even if it is NULL as the combiner in this case only adds the specified module
        lComp.push_back( boost::shared_ptr< WApplyCombiner >( new WApplyCombiner( *listIter ) ) );

        // add this list
        compatibles.push_back( WCombinerTypes::WCompatiblesGroup( ( *listIter ), lComp ) );
    }

    // unlock. No locking needed for further steps.
    l.reset();

    // sort the compatibles
    std::sort( compatibles.begin(), compatibles.end(), WCombinerTypes::compatiblesSort );

    return compatibles;
}

