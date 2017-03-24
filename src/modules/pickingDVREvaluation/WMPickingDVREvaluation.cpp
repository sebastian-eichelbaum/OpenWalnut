//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2017 OpenWalnut Community
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

#include "core/dataHandler/WDataSetScalar.h"
#include "core/graphicsEngine/WGERequirement.h"
#include "core/kernel/WKernel.h"
#include "core/ui/WUIRequirement.h"

#include "WMPickingDVREvaluation.h"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMPickingDVREvaluation )

WMPickingDVREvaluation::WMPickingDVREvaluation():
    WModule()
{
}

WMPickingDVREvaluation::~WMPickingDVREvaluation()
{
}

boost::shared_ptr< WModule > WMPickingDVREvaluation::factory() const
{
    return boost::shared_ptr< WModule >( new WMPickingDVREvaluation() );
}

const std::string WMPickingDVREvaluation::getName() const
{
    return "Picking in DVR Evaluation";
}

const std::string WMPickingDVREvaluation::getDescription() const
{
    return "Evaluate different picking techniques with regard to certain metrics.";
}

void WMPickingDVREvaluation::connectors()
{
    // The transfer function for our DVR
    m_transferFunction = WModuleInputData< WDataSetSingle >::createAndAdd( shared_from_this(), "transfer function", "The 1D transfer function." );

    // Scalar field
    m_scalarData = WModuleInputData< WDataSetScalar >::createAndAdd( shared_from_this(), "scalar data", "Scalar data." );

    WModule::connectors();
}

void WMPickingDVREvaluation::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );
    m_viewDirection =  m_properties->addProperty( "Viewing Direction",
                                                  "Viewing and thus projection direction for DVR.",
                                                  WVector3d( 0.0, 0.0, -1.0 ),
                                                  m_propCondition );
    m_sampleSteps = m_properties->addProperty( "Samples - steps",
                      "Number of samples. Choose this appropriately for the settings used for the DVR itself.",
                       256,
                       m_propCondition );

    WModule::properties();
}

void WMPickingDVREvaluation::requirements()
{
    m_requirements.push_back( new WGERequirement() );
    m_requirements.push_back( new WUIRequirement() );
}

void WMPickingDVREvaluation::moduleMain()
{
    // get notified about data changes
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_scalarData->getDataChangedCondition() );
    m_moduleState.add( m_transferFunction->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    ready();

    // main loop
    while( !m_shutdownFlag() )
    {
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        WDataSet::SPtr dataSet = m_scalarData->getData();
        // is this a DS with a regular grid?
        WDataSetSingle::SPtr dsSingle = boost::dynamic_pointer_cast< WDataSetSingle >( dataSet );
        if( dsSingle )
        {
            WGridRegular3D::SPtr regGrid;
            regGrid = boost::dynamic_pointer_cast< WGridRegular3D >( dsSingle->getGrid() );
            WBoundingBox bbox = regGrid->getBoundingBox();
            debugLog() << bbox.getMin()[0] << " " << bbox.getMin()[1] << " " << bbox.getMin()[1];
        }
    }
}
