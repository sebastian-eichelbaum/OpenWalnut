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

#include "../WModuleFactory.h"

#include "WApplyCombiner.h"

WApplyCombiner::WApplyCombiner( boost::shared_ptr< WModuleContainer > target,
                                boost::shared_ptr< WModule > srcModule, std::string srcConnector,
                                boost::shared_ptr< WModule > targetModule, std::string targetConnector ):
    WModuleOneToOneCombiner( target, srcModule, srcConnector, targetModule, targetConnector )
{
}

WApplyCombiner::WApplyCombiner( boost::shared_ptr< WModule > srcModule, std::string srcConnector,
                                boost::shared_ptr< WModule > targetModule, std::string targetConnector ):
    WModuleOneToOneCombiner( srcModule, srcConnector, targetModule, targetConnector )
{
}

WApplyCombiner::~WApplyCombiner()
{
    // cleanup
}

void WApplyCombiner::apply()
{
    // create the modules from the prototypes if needed
    boost::shared_ptr< WModule > srcModule = m_srcModule;
    boost::shared_ptr< WModule > targetModule = m_targetModule;

    // create module instance if src is a prototype
    if( srcModule && WModuleFactory::isPrototype( srcModule ) )
    {
        srcModule = WModuleFactory::getModuleFactory()->create( m_srcModule );
    }

    // create module instance if target is a prototype
    if( targetModule && WModuleFactory::isPrototype( targetModule ) )
    {
        targetModule = WModuleFactory::getModuleFactory()->create( m_targetModule );
    }

    // add the src and target module to the container
    // NOTE: the container does nothing if a NULL pointer has been specified and it also does nothing if the module already is associated with
    // the container
    m_container->add( srcModule );
    m_container->add( targetModule );

    // wait for the source module if there is any
    if( srcModule )
    {
        srcModule->isReadyOrCrashed().wait();
        if( srcModule->isCrashed()() )
        {
            // NOTE: throwing an exception here should not be needed as the module container already has forwarded the exception
            wlog::error( "Prototype Combiner" ) << "The source module \"" << srcModule->getName() << "\" has crashed. Abort.";
            return;
        }
    }

    // wait for the source module if there is any
    if( targetModule )
    {
        targetModule->isReadyOrCrashed().wait();
        if( targetModule->isCrashed()() )
        {
            // NOTE: throwing an exception here should not be needed as the module container already has forwarded the exception
            wlog::error( "Prototype Combiner" ) << "The target module \"" << targetModule->getName() << "\" has crashed. Abort.";
            return;
        }
    }

    // if the connector is an empty string -> do not connect, just add
    if( ( m_srcConnector.empty() ) || ( m_targetConnector.empty() ) )
    {
        return;
    }

    // and connect them finally:
    if( srcModule && targetModule )
    {
        targetModule->getInputConnector( m_targetConnector )->disconnectAll(); // before connecting, remove existing connection on input
        targetModule->getInputConnector( m_targetConnector )->connect( srcModule->getOutputConnector( m_srcConnector ) );
    }
}

