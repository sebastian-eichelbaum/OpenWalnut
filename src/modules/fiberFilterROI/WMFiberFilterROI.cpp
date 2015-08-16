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
#include <vector>

#include "core/kernel/WKernel.h"

#include "WMFiberFilterROI.h"

// This line is needed by the module loader to actually find your module. You need to add this to your module too. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMFiberFilterROI )

WMFiberFilterROI::WMFiberFilterROI():
    WModule()
{
    // Init
}

WMFiberFilterROI::~WMFiberFilterROI()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMFiberFilterROI::factory() const
{
    return boost::shared_ptr< WModule >( new WMFiberFilterROI() );
}

const std::string WMFiberFilterROI::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "Fiber Filter ROI";
}

const std::string WMFiberFilterROI::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    return "This module provides fiber data filtered by the current ROI configuration.";
}

void WMFiberFilterROI::connectors()
{
    m_input = WModuleInputData< WDataSetFibers >::createAndAdd( shared_from_this(), "in", "The dataset to filter" );
    m_output = WModuleOutputData < WDataSetFibers  >::createAndAdd( shared_from_this(), "out", "The filtered dataset" );

    // call WModule's initialization
    WModule::connectors();
}

void WMFiberFilterROI::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );
    m_aTrigger = m_properties->addProperty( "Update", "Trigger an update of your result data.", WPVBaseTypes::PV_TRIGGER_READY,
                                            m_propCondition );

    WModule::properties();
}

void WMFiberFilterROI::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    // Signal ready state. Now your module can be connected by the container, which owns the module.
    ready();
    waitRestored();

    // main loop
    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        // woke up since the module is requested to finish
        if( m_shutdownFlag() )
        {
            break;
        }

        // Remember the above criteria. We now need to check if the data is valid. After a connect-update, it might be NULL.
        boost::shared_ptr< WDataSetFibers > dataSet = m_input->getData();
        bool dataValid = ( dataSet != NULL );
        bool dataChanged = dataSet != m_fibers;

        // do something with the data
        if( dataChanged )
        {
            m_fibers = dataSet;

            if( dataValid )
            {
                // The data is valid and we received an update. The data is not NULL but may be the same as in previous loops.
                debugLog() << "Received new data. Creating new selector.";
                m_fiberSelector = WFiberSelector::SPtr( new WFiberSelector( m_fibers ) );

                debugLog() << "Data changed. Recalculating output.";
                updateOutput();
            }
            else
            {
                m_fiberSelector.reset();
            }
        }

        if( m_aTrigger->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            updateOutput();
            m_aTrigger->set( WPVBaseTypes::PV_TRIGGER_READY, false );
        }
    }
}

void WMFiberFilterROI::updateOutput()
{
    // target memory
    boost::shared_ptr< std::vector< bool > >    active = m_fiberSelector->getBitfield();
    boost::shared_ptr< std::vector< float > >   vertices( new std::vector< float >() );
    boost::shared_ptr< std::vector< size_t > >  lineStartIndexes( new std::vector< size_t >() );
    boost::shared_ptr< std::vector< size_t > >  lineLengths( new std::vector< size_t >() );
    boost::shared_ptr< std::vector< size_t > >  verticesReverse( new std::vector< size_t >() );

    boost::shared_ptr< WProgress > progress1( new WProgress( "Filtering", active->size() ) );
    m_progress->addSubProgress( progress1 );

    size_t countLines = 0;
    for( size_t l = 0; l < active->size(); ++l )
    {
        if( ( *active )[l] )
        {
            size_t pc = m_fibers->getLineStartIndexes()->at( l ) * 3;

            lineStartIndexes->push_back( vertices->size() / 3 );
            lineLengths->push_back( m_fibers->getLineLengths()->at( l ) );

            for( size_t j = 0; j < m_fibers->getLineLengths()->at( l ); ++j )
            {
                vertices->push_back( m_fibers->getVertices()->at( pc ) );
                ++pc;
                vertices->push_back( m_fibers->getVertices()->at( pc ) );
                ++pc;
                vertices->push_back( m_fibers->getVertices()->at( pc ) );
                ++pc;
                verticesReverse->push_back( countLines );
            }
            ++countLines;
        }
    }

    boost::shared_ptr< WDataSetFibers> newOutput( new WDataSetFibers( vertices, lineStartIndexes, lineLengths, verticesReverse,
                                                                      m_fibers->getBoundingBox() ) );
    m_output->updateData( newOutput );
    progress1->finish();
}
