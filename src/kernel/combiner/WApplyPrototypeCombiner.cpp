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
#include <list>
#include <string>
#include <utility>

#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include "../WKernel.h"
#include "../WModuleCombiner.h"
#include "../WModuleFactory.h"
#include "../WModuleConnector.h"
#include "../WModule.h"
#include "../WModuleInputConnector.h"
#include "../WModuleOutputConnector.h"
#include "../exceptions/WModuleConnectorNotFound.h"

#include "../../common/WLogger.h"

#include "WApplyPrototypeCombiner.h"

WApplyPrototypeCombiner::WApplyPrototypeCombiner( boost::shared_ptr< WModuleContainer > target,
                                                  boost::shared_ptr< WModule > srcModule, std::string srcConnector,
                                                  boost::shared_ptr< WModule > prototype, std::string targetConnector ):
    WModuleCombiner( target ),
    m_srcModule( srcModule ),
    m_srcConnector( srcConnector ),
    m_targetPrototype( prototype ),
    m_targetConnector( targetConnector )
{
}

WApplyPrototypeCombiner::WApplyPrototypeCombiner( boost::shared_ptr< WModule > srcModule, std::string srcConnector,
                                                  boost::shared_ptr< WModule > prototype, std::string targetConnector ):
    WModuleCombiner( WKernel::getRunningKernel()->getRootContainer() ),
    m_srcModule( srcModule ),
    m_srcConnector( srcConnector ),
    m_targetPrototype( prototype ),
    m_targetConnector( targetConnector )
{
}

WApplyPrototypeCombiner::~WApplyPrototypeCombiner()
{
    // cleanup
}

boost::shared_ptr< WModule > WApplyPrototypeCombiner::getSrcModule() const
{
    return m_srcModule;
}

std::string WApplyPrototypeCombiner::getSrcConnector() const
{
    return m_srcConnector;
}

boost::shared_ptr< WModule > WApplyPrototypeCombiner::getTargetPrototype() const
{
    return m_targetPrototype;
}

std::string WApplyPrototypeCombiner::getTargetConnector() const
{
    return m_targetConnector;
}

void WApplyPrototypeCombiner::apply()
{
    // create module instance
    boost::shared_ptr< WModule > targetModule = WModuleFactory::getModuleFactory()->create( m_targetPrototype );

    // add the target module to the container
    m_container->add( targetModule );
    // NOTE: here, we assume the src module already to be in the container. If not, connect will fail with an exception -> so no need for a
    // separate exception here

    // wait for the source module if there is any
    if ( m_srcModule )  // specifying a NULL source module causes the combiner to only add the target
    {
        m_srcModule->isReadyOrCrashed().wait();
        if ( m_srcModule->isCrashed()() )
        {
            wlog::error( "Prototype Combiner" ) << "The source module \"" << m_srcModule->getName() << "\" has crashed. Abort.";
            return;
        }
    }

    targetModule->isReadyOrCrashed().wait();
    if ( targetModule->isCrashed()() )
    {
        wlog::error( "Prototype Combiner" ) << "The target module \"" << targetModule->getName() << "\" has crashed. Abort.";
        return;
    }

    // if the target connector is an empty string -> do not connect, just add
    if ( m_targetConnector.empty() )
    {
        return;
    }

    // and connect them finally:
    if ( m_srcModule )
    {
        targetModule->getInputConnector( m_targetConnector )->connect( m_srcModule->getOutputConnector( m_srcConnector ) );
    }
}

