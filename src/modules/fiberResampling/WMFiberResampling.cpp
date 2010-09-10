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
#include <vector>
#include <string>

#include "../../kernel/WKernel.h"
#include "../../common/WPropertyHelper.h"
#include "../../common/math/WMath.h"
#include "../../dataHandler/WDataHandler.h"

#include "WMFiberResampling.h"
#include "WMFiberResampling.xpm"

// This line is needed by the module loader to actually find your module. You need to add this to your module too. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMFiberResampling )

WMFiberResampling::WMFiberResampling():
    WModule()
{
}

WMFiberResampling::~WMFiberResampling()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMFiberResampling::factory() const
{
    return boost::shared_ptr< WModule >( new WMFiberResampling() );
}

const char** WMFiberResampling::getXPMIcon() const
{
    return WMFiberResampling_xpm;
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
    // The input fiber dataset
    m_fiberInput = boost::shared_ptr< WModuleInputData < WDataSetFibers > >(
        new WModuleInputData< WDataSetFibers >( shared_from_this(), "fibers", "The fiber dataset to re-sample" )
    );

    // As properties, every connector needs to be added to the list of connectors.
    addConnector( m_fiberInput );

    // the selected fibers go to this output
    m_fiberOutput = boost::shared_ptr< WModuleOutputData < WDataSetFibers > >(
        new WModuleOutputData< WDataSetFibers >( shared_from_this(),
                                        "out", "The fiber dataset re-sampled." )
    );

    // As above: make it known.
    addConnector( m_fiberOutput );

    // call WModule's initialization
    WModule::connectors();
}

void WMFiberResampling::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    // call WModule's initialization
    WModule::properties();
}

void WMFiberResampling::moduleMain()
{
    // get notified about data changes
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_fiberInput->getDataChangedCondition() );
    // Remember the condition provided to some properties in properties()? The condition can now be used with this condition set.
    m_moduleState.add( m_propCondition );

    ready();

    // main loop
    while ( !m_shutdownFlag() )
    {
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        // woke up since the module is requested to finish?
        if ( m_shutdownFlag() )
        {
            break;
        }

        // To query whether an input was updated, simply ask the input:
        bool dataUpdated = m_fiberInput->updated();
        boost::shared_ptr< WDataSetFibers > dataSet = m_fiberInput->getData();
        bool dataValid = ( dataSet );
        if ( !dataValid && !dataUpdated )
        {
            continue;
        }

        // get the fiber definitions
        boost::shared_ptr< std::vector< size_t > > fibStart = dataSet->getLineStartIndexes();
        boost::shared_ptr< std::vector< size_t > > fibLen   = dataSet->getLineLengths();
        boost::shared_ptr< std::vector< float > >  fibVerts = dataSet->getVertices();

        // progress indication
        boost::shared_ptr< WProgress > progress1 = boost::shared_ptr< WProgress >( new WProgress( "Coloring fibers.",
                                                                                                  fibStart->size() ) );
        m_progress->addSubProgress( progress1 );

        // for each fiber:
        debugLog() << "Iterating over all fibers.";
        for( size_t fidx = 0; fidx < fibStart->size() ; ++fidx )
        {
            ++*progress1;

        }

        // forward the data
        m_fiberOutput->updateData( dataSet );

        progress1->finish();
        m_progress->removeSubProgress( progress1 );

        debugLog() << "Done";
    }
}

