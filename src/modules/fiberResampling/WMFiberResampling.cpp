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

#include <cmath>
#include <string>
#include <vector>

#include <core/common/WStrategyHelper.h>
#include <core/common/math/WMath.h>
#include <core/common/WPropertyHelper.h>
#include <core/dataHandler/WDataHandler.h>
#include <core/kernel/WKernel.h>

#include "WMFiberResampling.h"
#include "WResampleByMaxPoints.h"
#include "WResampleByNumPoints.h"
#include "WResampleBySegLength.h"

W_LOADABLE_MODULE( WMFiberResampling )

WMFiberResampling::WMFiberResampling():
    WModule(),
    m_strategy( "Fiber Resampling Algorithms", "Select one algorithm to resample fibers", NULL,
                "FiberResampling", "A list of all known resampling strategies" )
{
    m_strategy.addStrategy( WResampleByNumPoints::SPtr( new WResampleByNumPoints() ) );
    m_strategy.addStrategy( WResampleBySegLength::SPtr( new WResampleBySegLength() ) );
    m_strategy.addStrategy( WResampleByMaxPoints::SPtr( new WResampleByMaxPoints() ) );
}

WMFiberResampling::~WMFiberResampling()
{
}

boost::shared_ptr< WModule > WMFiberResampling::factory() const
{
    return boost::shared_ptr< WModule >( new WMFiberResampling() );
}

const std::string WMFiberResampling::getName() const
{
    return "Fiber Re-Sampling";
}

const std::string WMFiberResampling::getDescription() const
{
    return "This module can re-sample a fiber dataset and ensure equal segment lengths.";
}

void WMFiberResampling::connectors()
{
    m_fiberIC = WModuleInputData< WDataSetFibers >::createAndAdd( shared_from_this(), "fibers", "The line data to resample." );
    m_fiberOC = WModuleOutputData< WDataSetFibers >::createAndAdd( shared_from_this(), "resampledFibers", "The resampled fiber dataset." );

    WModule::connectors();
}

void WMFiberResampling::properties()
{
    m_properties->addProperty( m_strategy.getProperties() );

    WModule::properties();
}

void WMFiberResampling::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_fiberIC->getDataChangedCondition() );
    m_moduleState.add( m_strategy.getProperties()->getUpdateCondition() );

    ready();

    while( !m_shutdownFlag() )
    {
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        if( m_shutdownFlag() )
        {
            break;
        }

        WDataSetFibers::SPtr fibers = m_fiberIC->getData();

        if( !fibers )
        {
            continue;
        }

        WProgress::SPtr progress( new WProgress( "Resampling Fibers", fibers->size() ) );
        m_progress->addSubProgress( progress );
        debugLog() << "Start resampling";
        m_fiberOC->updateData( m_strategy()->operator()( progress, m_shutdownFlag, fibers ) );
        progress->finish();
        m_progress->removeSubProgress( progress );
        debugLog() << "Finished resampling";
    }
}
