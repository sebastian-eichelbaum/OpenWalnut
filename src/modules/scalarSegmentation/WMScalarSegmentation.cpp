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

#include "scalarSegmentation.xpm"
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

    m_algoType = m_properties->addProperty( "Seg Algo", "Choose a segmentation method.", m_algos->getSelectorFirst(), m_propCondition );

    m_propGroups.resize( 1 );
    m_propGroups[ 0 ] = m_properties->addPropertyGroup( "Simple Threshold", "" );

    m_threshold = m_propGroups[ 0 ]->addProperty( "Threshold", "Threshold value for segmentation.", 0.8, m_propCondition );
    m_threshold->setMax( 255.0 );
    m_threshold->setMin( 0.0 );
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

        if( dataChanged )
        {
            m_dataSet = newDataSet;
        }

        //bool propChanged = false;
        //if( !m_algoType->changed() )
        //{
            //WPVGroup::PropertyConstIterator it;
            //for( it = m_propGroups[ m_algoIndex ]->begin(); it != m_propGroups[ m_algoIndex ]->end(); ++it )
            //{
                //propChanged = propChanged || it->changed();
            //}
        //}
        //else
        //{
            //WItemSelector w = m_algoType.get( true );
            //m_algoIndex = w.getItemIndexOfSelected( 0 );
            //propChanged = true;
        //}

        bool propChanged = m_threshold->changed();
        if ( ( dataChanged && dataValid ) || ( propChanged && dataValid ) )
        {
            // redo calculation
            m_result = doSegmentation( m_dataSet );

            m_output->updateData( m_result );
        }
    }
}

void WMScalarSegmentation::activate()
{
    WModule::activate();
}

boost::shared_ptr< WDataSetScalar > WMScalarSegmentation::doSegmentation( boost::shared_ptr< WDataSetScalar > dataSet )
{
    switch( m_algoIndex )
    {
    case 0:
        return segmentationSimple( dataSet, m_threshold->get( true ) );
    default:
        return boost::shared_ptr< WDataSetScalar >();
    }
}

boost::shared_ptr< WDataSetScalar > WMScalarSegmentation::segmentationSimple( boost::shared_ptr< WDataSetScalar > dataSet, double threshold )
{
    debugLog() << "Computing!";

    WAssert( m_dataSet, "" );
    WAssert( m_dataSet->getValueSet(), "" );
    WAssert( m_dataSet->getGrid(), "" );
    WAssert( m_dataSet->getValueSet()->dimension() == 1, "" );
    WAssert( m_dataSet->getValueSet()->order() == 0, "" );

    boost::shared_ptr< WValueSetBase > values = boost::shared_dynamic_cast< WValueSetBase >( dataSet->getValueSet() );
    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( dataSet->getGrid() );
    WAssert( values, "" );
    WAssert( grid, "" );

    std::vector< float > v( grid->size() );

    for( std::size_t k = 0; k < grid->size(); ++k )
    {
        v[ k ] = ( values->getScalarDouble( k ) < threshold ? 0.0f : 255.0f );
    }

    boost::shared_ptr< WGridRegular3D > resGrid = boost::shared_ptr< WGridRegular3D >(
                            new WGridRegular3D( *( grid.get() ) ) );
    boost::shared_ptr< WValueSet< float > > resValues = boost::shared_ptr< WValueSet< float > >(
                            new WValueSet< float >( values->order(), values->dimension(), v, W_DT_FLOAT ) );
    boost::shared_ptr< WDataSetScalar > result = boost::shared_ptr< WDataSetScalar >(
                            new WDataSetScalar( resValues, resGrid ) );
    return result;
}
