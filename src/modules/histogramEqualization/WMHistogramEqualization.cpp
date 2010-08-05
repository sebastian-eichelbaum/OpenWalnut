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

#include "../../kernel/WKernel.h"
#include "../../common/WPropertyHelper.h"

#include "WMHistogramEqualization.h"
#include "WMHistogramEqualization.xpm"

// This line is needed by the module loader to actually find your module. You need to add this to your module too. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMHistogramEqualization )

WMHistogramEqualization::WMHistogramEqualization():
    WModule()
{
}

WMHistogramEqualization::~WMHistogramEqualization()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMHistogramEqualization::factory() const
{
    return boost::shared_ptr< WModule >( new WMHistogramEqualization() );
}

const char** WMHistogramEqualization::getXPMIcon() const
{
    return WMHistogramEqualization_xpm;
}

const std::string WMHistogramEqualization::getName() const
{
    return "Histogram Equalization";
}

const std::string WMHistogramEqualization::getDescription() const
{
    return "This module takes an arbitrary data set and equalizes its histogram. This increases contrast in several regions of the data.";
}

void WMHistogramEqualization::connectors()
{
    // the dataset to process. Only accept scalar data.
    m_input = boost::shared_ptr< WModuleInputData < WDataSetScalar  > >(
        new WModuleInputData< WDataSetScalar >( shared_from_this(),
                                                               "in", "The dataset whose histogram gets equalized." )
        );
    addConnector( m_input );

    // the output containing the equalized data.
    m_output = boost::shared_ptr< WModuleOutputData < WDataSetScalar  > >(
        new WModuleOutputData< WDataSetScalar >( shared_from_this(),
                                                               "out", "The dataset which has a linear cumulative histogram." )
        );
    addConnector( m_output );

    // call WModule's initialization
    WModule::connectors();
}

void WMHistogramEqualization::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    // enable histogram equalization?
    m_equalize = m_properties->addProperty( "Equalize Histogram", "If true, the dataset's cumulative histogram gets linearized.",
                                            true, m_propCondition );

    // call WModule's initialization
    WModule::properties();
}

void WMHistogramEqualization::moduleMain()
{
    // get notified about data changes
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
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
        bool dataUpdated = m_input->updated();

        // Remember the above criteria. We now need to check if the data is valid. After a connect-update, it might be NULL.
        boost::shared_ptr< WDataSetScalar > dataSet = m_input->getData();
        dataUpdated = dataUpdated && dataSet;

        // this ensures the below code only gets executed upon data or property change
        if ( !dataUpdated && !m_equalize->changed() )
        {
            continue;
        }

        // The data is valid and we received an update. The data is not NULL but may be the same as in previous loops.
        debugLog() << "Recalculating";

        // Grab the histogram
        boost::shared_ptr< const WValueSetHistogram > hist = dataSet->getHistogram();
        debugLog() << *hist;
    }
}

