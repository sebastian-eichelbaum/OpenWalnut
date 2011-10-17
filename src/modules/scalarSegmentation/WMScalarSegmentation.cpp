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

#include "core/common/WColor.h"
#include "core/common/WPropertyHelper.h"

#include "WMScalarSegmentation.xpm"
#include "WMScalarSegmentation.h"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMScalarSegmentation )

WMScalarSegmentation::WMScalarSegmentation():
    WModule()
{
    m_algoIndex = 0;
    m_algos.push_back( boost::shared_ptr< WSegmentationAlgo >( new WSegmentationAlgoThreshold() ) );
#ifdef OW_USE_ITK
    m_algos.push_back( boost::shared_ptr< WSegmentationAlgo >( new WSegmentationAlgoWatershed() ) );
    m_algos.push_back( boost::shared_ptr< WSegmentationAlgo >( new WSegmentationAlgoOtsu() ) );
    m_algos.push_back( boost::shared_ptr< WSegmentationAlgo >( new WSegmentationAlgoRegionGrowingConfidenceConnected() ) );
    m_algos.push_back( boost::shared_ptr< WSegmentationAlgo >( new WSegmentationAlgoLevelSetCanny() ) );
#endif  // OW_USE_ITK
}

WMScalarSegmentation::~WMScalarSegmentation()
{
    m_algos.clear();
}

boost::shared_ptr< WModule > WMScalarSegmentation::factory() const
{
    return boost::shared_ptr< WModule >( new WMScalarSegmentation() );
}

const char** WMScalarSegmentation::getXPMIcon() const
{
    return scalarSegmentation_xpm;
}

const std::string WMScalarSegmentation::getName() const
{
    return "Scalar Segmentation";
}

const std::string WMScalarSegmentation::getDescription() const
{
    return "This module segments scalar datasets.";
}

void WMScalarSegmentation::connectors()
{
    m_input = boost::shared_ptr< WModuleInputData < WDataSetScalar  > >(
        new WModuleInputData< WDataSetScalar >( shared_from_this(), "inputSet", "The dataset to segment." )
        );

    addConnector( m_input );

    m_output = boost::shared_ptr< WModuleOutputData < WDataSetScalar  > >(
        new WModuleOutputData< WDataSetScalar >( shared_from_this(), "outputSet", "The calculated dataset." )
        );

    addConnector( m_output );

    WModule::connectors();
}

void WMScalarSegmentation::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_algoSelection = boost::shared_ptr< WItemSelection >( new WItemSelection );
    for( AlgoList::iterator it = m_algos.begin(); it != m_algos.end(); ++it )
    {
        m_algoSelection->addItem( ( *it )->getName(), ( *it )->getDescription() );
    }
    m_algoType = m_properties->addProperty( "Segmentation algorithm", "Choose a segmentation method.",
                                            m_algoSelection->getSelectorFirst(), m_propCondition );

    for( AlgoList::iterator it = m_algos.begin(); it != m_algos.end(); ++it )
    {
        ( *it )->initProperties( m_properties->addPropertyGroup( ( *it )->getName(), "The properties for this segmentation algorithm.", true ) );
    }

    m_algos.at( 0 )->hideProperties( false );

    WPropertyHelper::PC_SELECTONLYONE::addTo( m_algoType );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_algoType );

    WModule::properties();
}

void WMScalarSegmentation::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );
    for( AlgoList::iterator it = m_algos.begin(); it != m_algos.end(); ++it )
    {
        m_moduleState.add( ( *it )->getCondition() );
    }

    ready();

    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        if( m_shutdownFlag() )
        {
            break;
        }

        boost::shared_ptr< WDataSetScalar > newDataSet = m_input->getData();
        bool dataChanged = ( m_dataSet != newDataSet );
        bool dataValid   = ( newDataSet );

        if( dataChanged && dataValid )
        {
            m_dataSet = newDataSet;
            if( !m_dataSet->getValueSet() || !m_dataSet->getGrid()
                || m_dataSet->getValueSet()->dimension() != 1 || m_dataSet->getValueSet()->order() != 0 )
            {
                m_dataSet = boost::shared_ptr< WDataSetScalar >();
            }
        }

        bool algoChanged = m_algoType->changed();

        if( algoChanged )
        {
            WItemSelector w = m_algoType->get( true );
            m_algos.at( m_algoIndex )->hideProperties( true );
            m_algoIndex = w.getItemIndexOfSelected( 0 );
            m_algos.at( m_algoIndex )->hideProperties( false );
        }

        bool propChanged = m_algos.at( m_algoIndex )->propChanged();
        if( m_dataSet && ( dataChanged || propChanged || algoChanged ) )
        {
            // redo calculation
            doSegmentation();
            m_output->updateData( m_result );
        }
    }

    for( AlgoList::iterator it = m_algos.begin(); it != m_algos.end(); ++it )
    {
        m_moduleState.remove( ( *it )->getCondition() );
    }
}

void WMScalarSegmentation::activate()
{
    WModule::activate();
}

void WMScalarSegmentation::doSegmentation()
{
    debugLog() << "Starting segmentation.";
    m_result = m_algos.at( m_algoIndex )->segment( m_dataSet );
    debugLog() << "Segmentation finished.";
}
