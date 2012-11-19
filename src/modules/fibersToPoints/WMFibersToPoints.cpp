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

#include "core/kernel/WKernel.h"
#include "core/common/WPropertyHelper.h"
#include "core/common/math/WMath.h"
#include "core/dataHandler/WDataHandler.h"

#include "WMFibersToPoints.h"

// This line is needed by the module loader to actually find your module. You need to add this to your module too. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMFibersToPoints )

WMFibersToPoints::WMFibersToPoints():
    WModule()
{
}

WMFibersToPoints::~WMFibersToPoints()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMFibersToPoints::factory() const
{
    return boost::shared_ptr< WModule >( new WMFibersToPoints() );
}

const char** WMFibersToPoints::getXPMIcon() const
{
    return NULL;
}

const std::string WMFibersToPoints::getName() const
{
    return "Fibers To Points";
}

const std::string WMFibersToPoints::getDescription() const
{
    return "This module converts fiber data to points by using the fiber vertex points and colors..";
}

void WMFibersToPoints::connectors()
{
    // The input fiber dataset
    m_fiberInput = boost::shared_ptr< WModuleInputData < WDataSetFibers > >(
        new WModuleInputData< WDataSetFibers >( shared_from_this(), "fibers", "The fiber dataset" )
    );

    // As properties, every connector needs to be added to the list of connectors.
    addConnector( m_fiberInput );

    // the points
    m_pointsOutput = boost::shared_ptr< WModuleOutputData < WDataSetPoints > >(
        new WModuleOutputData< WDataSetPoints >( shared_from_this(), "out", "The point data." )
    );

    // As above: make it known.
    addConnector( m_pointsOutput );

    // call WModule's initialization
    WModule::connectors();
}

void WMFibersToPoints::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    // call WModule's initialization
    WModule::properties();
}


void WMFibersToPoints::moduleMain()
{
    // get notified about data changes
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_fiberInput->getDataChangedCondition() );
    // Remember the condition provided to some properties in properties()? The condition can now be used with this condition set.
    m_moduleState.add( m_propCondition );

    ready();

    // main loop
    while( !m_shutdownFlag() )
    {
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        // woke up since the module is requested to finish?
        if( m_shutdownFlag() )
        {
            break;
        }

        // To query whether an input was updated, simply ask the input:
        bool dataUpdated = m_fiberInput->handledUpdate();
        boost::shared_ptr< WDataSetFibers > dataSet = m_fiberInput->getData();
        bool dataValid = ( dataSet );

        // reset everything if input was disconnected/invalid
        if( !dataValid )
        {
            debugLog() << "Resetting output.";
            m_pointsOutput->reset();
            continue;
        }

        if( dataValid && !dataUpdated )
        {
            continue;
        }

        // progress indication
        WProgress::SPtr progress = WProgress::SPtr( new WProgress( "Creating Points from Fibers." ) );
        m_progress->addSubProgress( progress );

        debugLog() << "Creating point data. Num Points = " << dataSet->getVertices()->size() << ".";
        WDataSetFibers::VertexArray fibVerts = dataSet->getVertices();
        WDataSetFibers::ColorArray fibColors = dataSet->getColorScheme()->getColor();
        WDataSetPoints::SPtr result( new WDataSetPoints( fibVerts, fibColors, dataSet->getBoundingBox() ) );
        m_pointsOutput->updateData( result );

        progress->finish();
        m_progress->removeSubProgress( progress );
    }
}

