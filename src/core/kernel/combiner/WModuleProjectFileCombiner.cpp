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

#include <iostream>
#include <map>
#include <set>
#include <list>
#include <string>
#include <utility>

#include <boost/regex.hpp>

#include "../WKernel.h"
#include "../WModuleCombiner.h"
#include "../WModuleFactory.h"
#include "../WModuleConnector.h"
#include "../WModule.h"
#include "../WDataModule.h"
#include "../WModuleInputConnector.h"
#include "../WModuleOutputConnector.h"
#include "../exceptions/WModuleConnectorNotFound.h"

#include "../../common/exceptions/WFileNotFound.h"
#include "../../common/WStringUtils.h"
#include "../../common/WProperties.h"
#include "../../common/WPropertyBase.h"
#include "../../common/WPropertyVariable.h"
#include "../../common/WPropertyTypes.h"
#include "../../common/WLogger.h"

#include "../../graphicsEngine/WGEColormapping.h"

#include "WModuleProjectFileCombiner.h"

WModuleProjectFileCombiner::WModuleProjectFileCombiner( boost::shared_ptr< WModuleContainer > target ):
    WModuleCombiner( target ),
    WProjectFileIO()
{
}

WModuleProjectFileCombiner::WModuleProjectFileCombiner():
    WModuleCombiner( WKernel::getRunningKernel()->getRootContainer() ),
    WProjectFileIO()
{
}

WModuleProjectFileCombiner::~WModuleProjectFileCombiner()
{
    // cleanup
}

bool WModuleProjectFileCombiner::parse( std::string line, unsigned int lineNumber )
{
    // this is the proper regular expression for modules
    static const boost::regex modRe( "^ *MODULE:([0-9]*):(.*)$" );
    static const boost::regex dataRe( "^ *DATA:([0-9]*):\"?([^\"]*)\"?$" );
    static const boost::regex conRe( "^ *CONNECTION:\\(([0-9]*),(.*)\\)->\\(([0-9]*),(.*)\\)$" );
    static const boost::regex propRe( "^ *PROPERTY:\\(([0-9]*),(.*)\\)=(.*)$" );

    boost::smatch matches;  // the list of matches
    if( boost::regex_match( line, matches, modRe ) )
    {
        // it is a module line
        // matches[1] is the ID
        // matches[2] is the name of the module

        wlog::debug( "Project Loader [Parser]" ) << "Line " << lineNumber << ": Module \"" << matches[2] << "\" with ID " << matches[1];

        // create a module instance
        boost::shared_ptr< WModule > proto = WModuleFactory::getModuleFactory()-> isPrototypeAvailable( matches[2] );

        // data modules are not allowed here
        if( !proto )
        {
            addError( "There is no prototype available for module \"" + matches[2] + "\". Skipping." );
        }
        else if( proto->getType() == MODULE_DATA )
        {
            addError( "Data modules are not allowed to be specified in a \"MODULE\" Statement. Use the \"DATA\" statement instead. Skipping." );
        }
        else
        {
            boost::shared_ptr< WModule > module = WModuleFactory::getModuleFactory()->create( proto );
            // set restore mode
            module->setRestoreNeeded();

            m_modules.insert( ModuleID( string_utils::fromString< unsigned int >( matches[1] ), module ) );
        }
    }
    else if( boost::regex_match( line, matches, dataRe ) )
    {
        // it is a dataset line
        // matches[1] is the ID
        // matches[2] is the filename
        wlog::debug( "Project Loader [Parser]" ) << "Line " << lineNumber << ": Data \"" << matches[2] << "\" with ID " << matches[1];

        // create a module instance
        boost::shared_ptr< WModule > proto = WModuleFactory::getModuleFactory()-> isPrototypeAvailable( "Data Module" );
        if( !proto )
        {
            addError( "There is no prototype available for module \"Data Module\". This should not happen!. Skipping." );
        }
        else
        {
            std::string parameter = std::string( matches[2] );
            boost::shared_ptr< WModule > module = WModuleFactory::getModuleFactory()->create( proto );

            // set restore mode
            module->setRestoreNeeded();
            if( parameter.empty() )
            {
                addError( "Data modules need an additional filename parameter. Skipping." );
            }
            else
            {
                boost::static_pointer_cast< WDataModule >( module )->setFilename( parameter );
                m_modules.insert( ModuleID( string_utils::fromString< unsigned int >( matches[1] ), module ) );
            }
        }
    }
    else if( boost::regex_match( line, matches, conRe ) )
    {
        // it is a connector line
        // matches[1] and [2] are the module ID and connector name of the output connector
        // matches[3] and [4] are the module ID and connector name of the target input connector

        wlog::debug( "Project Loader [Parser]" ) << "Line " << lineNumber << ": Connection between \"" << matches[2] << "\" of module "
                                                 << matches[1] << " and \"" << matches[4] << "\" of module " << matches[3] << ".";

        // now we search in modules[ matches[1] ] for an output connector named matches[2]
        m_connections.push_back( Connection( Connector( string_utils::fromString< unsigned int >( matches[1] ), matches[2] ),
                                           Connector( string_utils::fromString< unsigned int >( matches[3] ), matches[4] ) ) );
    }
    else if( boost::regex_match( line, matches, propRe ) )
    {
        // it is a property line
        // matches[1] is the module ID
        // matches[2] is the property name
        // matches[3] is the property value

        wlog::debug( "Project Loader [Parser]" ) << "Line " << lineNumber << ": Property \"" << matches[2] << "\" of module " << matches[1]
                                                 << " set to " << matches[3];

        m_properties.push_back( PropertyValue( Property( string_utils::fromString< unsigned int >( matches[1] ), matches[2] ), matches[3] ) );
    }
    else
    {
        return false;
    }

    return true;
}

void WModuleProjectFileCombiner::apply()
{
    // reset sort indices in colormapper as we load new ones.
    WGEColormapping::instance()->resetSortIndices();

    // now add each module to the target container
    for( std::map< unsigned int, boost::shared_ptr< WModule > >::const_iterator iter = m_modules.begin(); iter != m_modules.end(); ++iter )
    {
        m_container->add( ( *iter ).second );
    }

    // now wait for the modules to get ready. We could have waited for this in the previous loop, but a long loading module would block others.
    // -> so we wait after adding and starting them
    for( std::map< unsigned int, boost::shared_ptr< WModule > >::iterator iter = m_modules.begin(); iter != m_modules.end(); ++iter )
    {
        ( *iter ).second->isReadyOrCrashed().wait();

        // if isReady now is false, the module has crashed before it got ready -> remove the module from the list
        if( ( *iter ).second->isCrashed()() )
        {
            addError( "In the module with ID " + ( *iter ).first +
                      std::string( " a problem occurred. Connections and properties relating to this module will fail." ) );
            m_modules.erase( iter );
        }
    }

    // now, as we have created the modules, we need to set the properties for each of it.
    for( std::list< PropertyValue >::const_iterator iter = m_properties.begin(); iter != m_properties.end(); ++iter )
    {
        // grab corresponding module
        if( !m_modules.count( ( *iter ).first.first ) )
        {
            addError( "There is no module with ID \"" + string_utils::toString( ( *iter ).first.first ) + "\" to set the property \"" +
                                                        ( *iter ).first.second + std::string( "\" for. Skipping." ) );
            continue;
        }
        boost::shared_ptr< WModule > m = m_modules[ ( *iter ).first.first ];

        // has this module the specified property?
        boost::shared_ptr< WPropertyBase > prop = m->getProperties()->findProperty( ( *iter ).first.second );
        if( !prop )
        {
            addWarning( "The module \"" + m->getName() + std::string( "\" has no property named \"" ) + ( *iter ).first.second +
                        std::string( "\". Skipping." ) );
            continue;
        }
        else
        {
            if( prop->getPurpose() != PV_PURPOSE_INFORMATION )
            {
                // set the property here
                bool result = prop->setAsString( ( *iter ).second );
                if( !result )
                {
                    addError( "Failed to set property " + ( *iter ).first.second + " in module \"" + m->getName() + "\"." );
                }
            }
            else
            {
                addWarning( "The module \"" + m->getName() + "\" has a property named \"" +
                            ( *iter ).first.second + "\" which is an INFORMATION property. Skipping." );
            }
        }
    }

    // and finally, connect them all together
    for( std::list< Connection >::const_iterator iter = m_connections.begin(); iter != m_connections.end(); ++iter )
    {
        // each connection contains two connectors
        Connector c1 = ( *iter ).first;
        Connector c2 = ( *iter ).second;

        // each of these connectors contains the module ID and the connector name
        // grab corresponding module 1
        boost::shared_ptr< WModule > m1;
        if( !m_modules.count( c1.first ) )
        {
            addError( "There is no module with ID \"" + string_utils::toString( c1.first ) + "\" for the connection "
                      + "(" + string_utils::toString( c1.first ) + "," + c1.second + ")->(" +  string_utils::toString( c2.first ) + "," +
                      c2.second + "). Skipping." );
            continue;
        }
        m1 = m_modules[ c1.first ];

        boost::shared_ptr< WModule > m2;
        if( !m_modules.count( c2.first ) )
        {
            addError( "There is no module with ID \"" + string_utils::toString( c2.first ) +  "\" for the connection "
                      + "(" + string_utils::toString( c1.first ) + "," + c1.second + ")->(" + string_utils::toString( c2.first ) +
                      "," + c2.second + "). Skipping." );

            continue;
        }
        m2 = m_modules[ c2.first ];

        // now we have the modules referenced by the ID
        // -> query the connectors
        // NOTE: we assume the first connector to be an output connector!
        boost::shared_ptr< WModuleOutputConnector > con1;
        try
        {
            con1 = m1->getOutputConnector( c1.second );
        }
        catch( const WModuleConnectorNotFound& e )
        {
            addError( "There is no output connector \"" + c1.second + "\" in module \"" + m1->getName() + "\"" );
            continue;
        }
        boost::shared_ptr< WModuleInputConnector > con2;
        try
        {
            con2 = m2->getInputConnector( c2.second );
        }
        catch( const WModuleConnectorNotFound& e )
        {
            addError( "There is no input connector \"" + c2.second + "\" in module \"" + m2->getName() + "\"" );
            continue;
        }

        // finally, connect them
        try
        {
            con1->connect( con2 );
        }
        catch( const WException& e )
        {
            addError( "Connection (" + string_utils::toString( c1.first ) + "," + c1.second + ")->(" +
                      string_utils::toString( c2.first ) + "," + c2.second +
                      ") could not be created. Incompatible connectors?. Skipping." );
            continue;
        }
    }

    // the colornapper should now sort the textures according to the loaded indices
    WGEColormapping::instance()->sortByIndex();

    // notify modules about the loaded set properties
    for( std::map< unsigned int, boost::shared_ptr< WModule > >::iterator iter = m_modules.begin(); iter != m_modules.end(); ++iter )
    {
        ( *iter ).second->reportRestoreComplete();
    }

    // clear all our lists (deref all contained pointers)
    m_modules.clear();
    m_connections.clear();
    m_properties.clear();
}

void WModuleProjectFileCombiner::done()
{
    apply();
}

void WModuleProjectFileCombiner::save( std::ostream& output )   // NOLINT
{
    // we need to save the colormapper's texture order. To do this, we need to update the textures sort indices
    WGEColormapping::instance()->setSortIndices();

    // grab access object of root container
    WModuleContainer::ModuleSharedContainerType::ReadTicket container = WKernel::getRunningKernel()->getRootContainer()->getModules();

    std::map< boost::shared_ptr< WModule >, unsigned int > moduleToIDMap;

    output << "//////////////////////////////////////////////////////////////////////////////////////////////////////////////////" << std::endl <<
              "// Modules and Properties" << std::endl <<
              "//////////////////////////////////////////////////////////////////////////////////////////////////////////////////" << std::endl <<
              std::endl;

    // iterate all modules:
    unsigned int i = 0;
    for( WModuleContainer::ModuleConstIterator iter = container->get().begin(); iter != container->get().end(); ++iter )
    {
        // store the mapping of ptr to ID
        moduleToIDMap[ ( *iter ) ] = i;

        // handle data modules separately
        if( ( *iter )->getType() == MODULE_DATA )
        {
            output << "DATA:" << i << ":" <<  boost::static_pointer_cast< WDataModule >( ( *iter ) )->getFilename().string() << std::endl;
        }
        else
        {
            output << "MODULE:" << i << ":" <<  ( *iter )->getName() << std::endl;
        }

        // the properties:
        printProperties( output, ( *iter )->getProperties(), "", "", i );

        // some readability:
        output << std::endl;
        ++i;
    }

    // finally, process all connections for each module
    output << "//////////////////////////////////////////////////////////////////////////////////////////////////////////////////" << std::endl <<
              "// Connections" << std::endl <<
              "//////////////////////////////////////////////////////////////////////////////////////////////////////////////////" << std::endl <<
              std::endl;


    // iterate over all modules
    for( WModuleContainer::ModuleConstIterator iter = container->get().begin(); iter != container->get().end(); ++iter )
    {
        // iterate over all outputs
        const WModule::OutputConnectorList& outs = ( *iter )->getOutputConnectors();
        for( WModule::OutputConnectorList::const_iterator citer = outs.begin(); citer != outs.end(); ++citer )
        {
            // iterate over all connections:
            boost::unique_lock<boost::shared_mutex> lock( ( *citer )->m_connectionListLock );
            for( std::set<boost::shared_ptr<WModuleConnector> >::const_iterator iciter = ( *citer )->m_connected.begin();
                  iciter != ( *citer )->m_connected.end(); ++iciter )
            {
                // as the module is a weak_ptr -> lock and get access to it
                boost::shared_ptr< WModule > theOtherModule = ( *iciter )->m_module.lock();
                output << "CONNECTION:(" << moduleToIDMap[ ( *iter ) ] << "," << ( *citer )->getName() << ")->(" <<
                                            moduleToIDMap[ theOtherModule ] << "," << ( *iciter )->getName() << ")" << std::endl;
            }
            lock.unlock();
        }
    }
}

