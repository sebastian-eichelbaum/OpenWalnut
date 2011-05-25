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

#include "core/common/math/WMath.h"
#include "core/common/WPropertyHelper.h"
#include "core/dataHandler/WDataHandler.h"
#include "core/dataHandler/WDataSetFiberVector.h"
#include "core/kernel/WKernel.h"
#include "WMFiberResampling.h"
#include "WMFiberResampling.xpm"
#include "WSimpleResampler.h"

W_LOADABLE_MODULE( WMFiberResampling )

WMFiberResampling::WMFiberResampling():
    WModule()
{
}

WMFiberResampling::~WMFiberResampling()
{
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

    addConnector( m_fiberOutput );

    WModule::connectors();
}

void WMFiberResampling::properties()
{
    m_newSamples = m_properties->addProperty( "#Sample Points", "The is the new number of which the tracts are resampled to", 20 );
    m_newSamples->setMin( 2 );
    m_newSamples->setMax( 200 );
    WModule::properties();
}

void WMFiberResampling::moduleMain()
{
    // get notified about data changes
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_fiberInput->getDataChangedCondition() );
    m_moduleState.add( m_newSamples->getCondition() );

    ready();

    while( !m_shutdownFlag() )
    {
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        if( m_shutdownFlag() )
        {
            break;
        }

        // To query whether an input was updated, simply ask the input:
        bool dataUpdated = m_fiberInput->handledUpdate();
        boost::shared_ptr< WDataSetFibers > dataSet = m_fiberInput->getData();
        bool dataValid = ( dataSet );
        if( m_newSamples->get() <= 1 )
        {
            warnLog() << "Resampling lines to have only 1 or less points is not supported, skipping this action";
        }
        if( !dataValid || ( m_newSamples->get() <= 1 ) ) // resampling with just one vertex will lead to points not to lines
        {
            continue;
        }

        WAssert( m_newSamples->changed() || ( dataValid && dataUpdated ), "Bug: Think! I though this could never happen!" );

        m_fiberOutput->updateData( resample( dataSet ) );

        debugLog() << "Done";
    }
}

boost::shared_ptr< WDataSetFibers > WMFiberResampling::resample( boost::shared_ptr< const WDataSetFibers > dataSet ) const
{
    debugLog() << "Start resampling: " << dataSet->getLineStartIndexes()->size() << " tracts";

    boost::shared_ptr< WProgress > progress1 = boost::shared_ptr< WProgress >( new WProgress( "Resampling tracts.", dataSet->getLineStartIndexes()->size() ) ); // NOLINT line length
    m_progress->addSubProgress( progress1 );

    boost::shared_ptr< WDataSetFiberVector > newDS( new WDataSetFiberVector() );

    for( size_t fidx = 0; fidx < dataSet->getLineStartIndexes()->size() ; ++fidx )
    {
        WFiber tract( ( *dataSet )[ fidx ] );
        tract.resampleByNumberOfPoints( m_newSamples->get( true ) );
        newDS->push_back( tract );
        ++*progress1;
    }

    progress1->finish();
    m_progress->removeSubProgress( progress1 );

    return newDS->toWDataSetFibers();
}

