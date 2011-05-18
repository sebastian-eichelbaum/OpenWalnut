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

#include "../common/WLogger.h"

#include "WKernel.h"
#include "WModuleContainer.h"

#include "WModuleCombiner.h"

WModuleCombiner::WModuleCombiner( boost::shared_ptr< WModuleContainer > target ):
    WThreadedRunner(),
    m_container( target )
{
    // initialize members
}

WModuleCombiner::WModuleCombiner():
    m_container( WKernel::getRunningKernel()->getRootContainer() )
{
    // initialize members
}

WModuleCombiner::~WModuleCombiner()
{
    // cleanup
}

void WModuleCombiner::run()
{
    // the module needs to be added here, as it else could be freed before the thread finishes ( remember: it is a shared_ptr).
    m_container->addPendingThread( shared_from_this() );

    // actually run
    WThreadedRunner::run();
}

void WModuleCombiner::threadMain()
{
    try
    {
        apply();
    }
    catch( ... )
    {
        wlog::error( "ModuleCombiner" ) << "Exception thrown. Aborting module combiner.";
    }

    // remove from list of threads
    m_container->finishedPendingThread( shared_from_this() );
}

