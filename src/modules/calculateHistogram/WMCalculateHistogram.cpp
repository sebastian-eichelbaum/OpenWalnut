//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2014 OpenWalnut Community, Nemtics, BSV-Leipzig
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

#include "../../core/kernel/WKernel.h"

#include "WMCalculateHistogram.h"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMCalculateHistogram )

WMCalculateHistogram::WMCalculateHistogram():
    WModule()
{
}

WMCalculateHistogram::~WMCalculateHistogram()
{
}

boost::shared_ptr< WModule > WMCalculateHistogram::factory() const
{
    return boost::shared_ptr< WModule >( new WMCalculateHistogram() );
}

const char** WMCalculateHistogram::getXPMIcon() const
{
    return NULL;
}
const std::string WMCalculateHistogram::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "CalculateHistogram";
}

const std::string WMCalculateHistogram::getDescription() const
{
    return "Calculates the histogram of the values found in the input dataset.";
}

void WMCalculateHistogram::connectors()
{
    m_dataInput = boost::shared_ptr< WModuleInputData< WDataSetSingle > >( new WModuleInputData< WDataSetSingle >(
                    shared_from_this(), "Input data", "A dataset to calculate the histogram from." ) );
    addConnector( m_dataInput );

    m_histogramOutput = boost::shared_ptr< WModuleOutputData< WDataSetHistogram1D > >( new WModuleOutputData< WDataSetHistogram1D >(
                    shared_from_this(), "Histogram", "The calculated histogram." ) );
    addConnector( m_histogramOutput );

    WModule::connectors();
}

void WMCalculateHistogram::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition );

    m_histogramBins = m_properties->addProperty( "Histogram bins", "The number of bins in the calculated histogram.", 100, m_propCondition );
    m_histogramBins->setMin( 1 );
    m_histogramBins->setMax( 1000 );

    WModule::properties();
}

void WMCalculateHistogram::requirements()
{
}

void WMCalculateHistogram::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_propCondition );
    m_moduleState.add( m_dataInput->getDataChangedCondition() );

    ready();

    while( !m_shutdownFlag() )
    {
        debugLog() << "Waiting ...";

        m_moduleState.wait();

        if( m_shutdownFlag() )
        {
            break;
        }

        if( m_dataInput->getData() && m_data != m_dataInput->getData() )
        {
            m_data = m_dataInput->getData();

            updateOutput();
        }

        if( m_data && m_histogramBins->changed() )
        {
            updateOutput();
        }
    }
}

void WMCalculateHistogram::updateOutput()
{
    boost::shared_ptr< WHistogramBasic > histo( new WHistogramBasic( m_data->getValueSet()->getMinimumValue(),
                                                                     m_data->getValueSet()->getMaximumValue(),
                                                                     m_histogramBins->get( true ) ) );
    for( std::size_t j = 0; j < m_data->getValueSet()->size(); ++j )
    {
        histo->insert( m_data->getValueSet()->getScalarDouble( j ) );
    }

    m_histogramOutput->updateData( boost::shared_ptr< WDataSetHistogram1D >( new WDataSetHistogram1D( histo ) ) );
}

