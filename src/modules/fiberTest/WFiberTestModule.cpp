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

#include <osg/Geode>

#include "WFiberTestModule.h"
#include "../../common/WLogger.h"
#include "../../dataHandler/WDataHandler.h"
#include "../../dataHandler/WLoaderManager.h"

WFiberTestModule::WFiberTestModule()
    : WModule()
{
}

WFiberTestModule::~WFiberTestModule()
{
}

const std::string WFiberTestModule::getName() const
{
    return std::string( "FiberTestModule" );
}

const std::string WFiberTestModule::getDescription() const
{
    return std::string( "Draws fibers out of a WDataSetFibers" );
}

void WFiberTestModule::threadMain()
{
    using boost::shared_ptr;
    std::string fname = "dataHandler/io/fixtures/Fibers/valid_small_example.fib";
    WLogger::getLogger()->addLogMessage( "Test loading of file: " + fname, "Kernel", LL_DEBUG );
    shared_ptr< WDataHandler > dataHandler = shared_ptr< WDataHandler >( new WDataHandler() );
    WLoaderManager testLoaderManager;
    testLoaderManager.load( fname, dataHandler );
}
