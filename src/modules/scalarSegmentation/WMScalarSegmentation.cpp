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

#include "../../kernel/WKernel.h"

#include "../../common/WColor.h"
#include "../../common/WPropertyHelper.h"

#include "WMScalarSegmentation.xpm"
#include "WMScalarSegmentation.h"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMScalarSegmentation )

WMScalarSegmentation::WMScalarSegmentation():
    WModule()
{
    m_algoIndex = 0;
}

WMScalarSegmentation::~WMScalarSegmentation()
{
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
    return "This module segments scalar datasets by threshold.";
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

    m_algos = boost::shared_ptr< WItemSelection >( new WItemSelection );
    m_algos->addItem( "Simple Threshold", "" );
    m_algos->addItem( "Watershed", "" );
    m_algos->addItem( "Otsu Threshold", "" );
    m_algos->addItem( "Region Growing", "" );

    m_algoType = m_properties->addProperty( "Seg Algo", "Choose a segmentation method.", m_algos->getSelectorFirst(), m_propCondition );

    m_threshold = m_properties->addProperty( "Threshold", "Threshold value for segmentation.", 10.0, m_propCondition );
    m_threshold->setMax( 100.0 );
    m_threshold->setMin( 0.0 );

    m_level = m_properties->addProperty( "Level", "Watershed level value for segmentation.", 10.0, m_propCondition );
    m_level->setMax( 100.0 );
    m_level->setMin( 0.0 );

    WModule::properties();
}

void WMScalarSegmentation::moduleMain()
{
    debugLog() << "Entering moduleMain()";

    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    ready();

    while ( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        if ( m_shutdownFlag() )
        {
            break;
        }

        boost::shared_ptr< WDataSetScalar > newDataSet = m_input->getData();
        bool dataChanged = ( m_dataSet != newDataSet );
        bool dataValid   = ( newDataSet );

        if( dataChanged && dataValid )
        {
            m_dataSet = newDataSet;
            WAssert( m_dataSet, "" );
            WAssert( m_dataSet->getValueSet(), "" );
            WAssert( m_dataSet->getGrid(), "" );
            WAssert( m_dataSet->getValueSet()->dimension() == 1, "" );
            WAssert( m_dataSet->getValueSet()->order() == 0, "" );
        }

        bool algoChanged = m_algoType->changed();
        WItemSelector w = m_algoType->get( true );
        m_algoIndex = w.getItemIndexOfSelected( 0 );

        bool propChanged = m_threshold->changed() || m_level->changed();
        if( m_dataSet && ( dataChanged || propChanged || algoChanged ) )
        {
            // redo calculation
            doSegmentation();
            m_output->updateData( m_result );
        }
    }
}

void WMScalarSegmentation::activate()
{
    WModule::activate();
}

void WMScalarSegmentation::doSegmentation()
{
    boost::shared_ptr< WValueSetBase > vs;

    debugLog() << m_algoIndex;

    switch( m_algoIndex )
    {
    case 0:
        vs = m_dataSet->getValueSet()->applyFunction( SimpleSegmentation( m_threshold->get( true ) ) );
        break;
#ifdef OW_USE_ITK
    case 1:
        vs = m_dataSet->getValueSet()->applyFunction( WatershedSegmentation( m_level->get( true ), m_threshold->get( true ), m_dataSet ) );
        break;
    case 2:
        vs = m_dataSet->getValueSet()->applyFunction( OtsuSegmentation( m_dataSet ) );
        break;
    case 3:
        vs = m_dataSet->getValueSet()->applyFunction( RegionGrowingSegmentation( m_dataSet ) );
        break;
#endif  // OW_USE_ITK
    default:
        errorLog() << "Invalid algorithm selected." << std::endl;
    }
    m_result = boost::shared_ptr< WDataSetScalar >( new WDataSetScalar( vs, m_dataSet->getGrid() ) );
}
