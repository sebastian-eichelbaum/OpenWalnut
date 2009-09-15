//---------------------------------------------------------------------------
//
// Project: OpenWalnut
//
// Copyright 2009 SomeCopyrightowner
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
#include <list>
#include <string>
#include <vector>

#include <boost/thread/xtime.hpp>

#include "WKernel.h"
#include "WModule.h"
#include "WTestModule.h"
#include "WNavigationSliceModule.h"
#include "../common/WException.h"

#include "../graphicsEngine/WGraphicsEngine.h"

/**
 * Used for program wide access to the kernel.
 */
WKernel* kernel = NULL;

WKernel::WKernel( int argc, char* argv[] )
{
    std::cout << "Initializing Kernel" << std::endl;

    kernel = this;

    // initialize members
    m_ArgC = argc;
    m_ArgV = argv;
    m_FinishRequested = false;

    // init GE, DataHandler, ...
    init();

    // load modules
    loadModules();
}

WKernel::~WKernel()
{
    // cleanup
    std::cout << "Shutting down Kernel" << std::endl;
}

WKernel::WKernel( const WKernel& other )
{
    *this = other;
}

WKernel* WKernel::getRunningKernel()
{
    return kernel;
}

boost::shared_ptr<WGraphicsEngine> WKernel::getGraphicsEngine() const
{
    return m_GraphicsEngine;
}

boost::shared_ptr<WDataHandler> WKernel::getDataHandler() const
{
    return m_DataHandler;
}


int WKernel::getArgumentCount() const
{
    return m_ArgC;
}

char** WKernel::getArguments() const
{
    return m_ArgV;
}

int WKernel::run()
{
    std::cout << "Starting Kernel" << std::endl;

    // TODO(ebaum): add separate graphics thread here
    m_GraphicsEngine->run();

    // run Gui
    // TODO(all): clean up this option handler mess
    m_Gui->run();

    // run? data handler stuff?

    // run module execution threads
    // TODO(ebaum): after having modules loaded they should be started here.
    // currently this is just the test module
    std::cout << "Starting modules:" << std::endl;
    for( std::list<WModule*>::iterator list_iter = m_modules.begin(); list_iter != m_modules.end();
            ++list_iter )
    {
        std::cout << "Starting Module: " << ( *list_iter )->getName() << std::endl;
        ( *list_iter )->run();
    }

    // wait
    m_Gui->wait( false );
    m_FinishRequested = true;

    // wait for modules to finish
    for( std::list<WModule*>::iterator list_iter = m_modules.begin(); list_iter != m_modules.end();
            ++list_iter )
    {
        ( *list_iter )->wait( true );
    }

    // finally GE
    m_GraphicsEngine->wait( true );


    // how to get QT return code from its thread?
    return 0;
}

void WKernel::loadModules()
{
    // TODO(ebaum): add dynamic loading here
    std::cout << "Loading modules:" << std::endl;
    m_modules.clear();

    WModule* m = new WNavigationSliceModule();
    std::cout << "Loading Module: " << m->getName() << std::endl;

    m_modules.push_back( m );
}

void WKernel::init()
{
    // initialize

    // initialize graphics engine
    // this also includes initialization of WGEScene and OpenSceneGraph
    m_GraphicsEngine = boost::shared_ptr<WGraphicsEngine>( new WGraphicsEngine );

    // initialize GUI
    // TODO(all): clean up this option handler mess
    m_Gui = boost::shared_ptr<WMainApplication>( new WMainApplication() );

    // initialize Datahandler
    m_DataHandler = boost::shared_ptr<WDataHandler>( new WDataHandler() );
}

bool WKernel::isFinishRequested() const
{
    return m_FinishRequested;
}

void WKernel::doLoadDataSets( std::vector< std::string > fileNames )
{
    m_DataHandler->loadDataSets( fileNames );
}

boost::shared_ptr<WDataHandler> WKernel::getDataHandler()
{
    return m_DataHandler;
}

