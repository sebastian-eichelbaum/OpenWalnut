//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV-Leipzig and CNCF-CBS
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
#include <sstream>

#include "core/kernel/WKernel.h"
#include "core/dataHandler/WDataHandler.h"
#include "core/graphicsEngine/WGEColormapping.h"

#include "WMFunctionalMRIViewer.h"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMFunctionalMRIViewer )

WMFunctionalMRIViewer::WMFunctionalMRIViewer():
    WModule()
{
}

WMFunctionalMRIViewer::~WMFunctionalMRIViewer()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMFunctionalMRIViewer::factory() const
{
    // See "src/modules/template/" for an extensively documented example.
    return boost::shared_ptr< WModule >( new WMFunctionalMRIViewer() );
}

const char** WMFunctionalMRIViewer::getXPMIcon() const
{
    return NULL;
}
const std::string WMFunctionalMRIViewer::getName() const
{
    return "Functional MRI Viewer";
}

const std::string WMFunctionalMRIViewer::getDescription() const
{
    return "Allows to select a point of time and to show the dataset at that point of time.";
}

void WMFunctionalMRIViewer::connectors()
{
    m_input = boost::shared_ptr< WModuleInputData< WDataSetTimeSeries > >(
                            new WModuleInputData< WDataSetTimeSeries >( shared_from_this(),
                                "in", "A time series." )
            );

    m_output = boost::shared_ptr< WModuleOutputData< WDataSetScalar > >(
                            new WModuleOutputData< WDataSetScalar >( shared_from_this(),
                                "out", "The selected time slice." ) );

    addConnector( m_input );
    addConnector( m_output );

    WModule::connectors();
}

void WMFunctionalMRIViewer::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_time = m_properties->addProperty( "Time", "The current time.", 0.0, m_propCondition );
    m_time->setMax( 1.0 );
    m_time->setMin( 0.0 );

    m_texScaleNormalized = m_properties->addProperty( "Norm. Tex Scale", "Use the same texture scaling for all textures.", true, m_propCondition );

    WModule::properties();
}

void WMFunctionalMRIViewer::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_propCondition );

    ready();

    while( !m_shutdownFlag() )
    {
        debugLog() << "Waiting.";
        m_moduleState.wait();

        boost::shared_ptr< WDataSetTimeSeries > inData = m_input->getData();
        bool dataChanged = ( m_dataSet != inData );
        if( ( dataChanged && inData ) || ( m_dataSet && m_time->changed() ) )
        {
            // remove old texture
            if( m_dataSetAtTime )
            {
                m_properties->removeProperty( m_dataSetAtTime->getTexture()->getProperties() );
                m_infoProperties->removeProperty( m_dataSetAtTime->getTexture()->getInformationProperties() );
                WGEColormapping::deregisterTexture( m_dataSetAtTime->getTexture() );
                m_dataSetAtTime.reset();
            }
            m_dataSet = inData;
            m_time->setMin( m_dataSet->getMinTime() );
            m_time->setMax( m_dataSet->getMaxTime() );
            m_time->ensureValidity( m_dataSet->getMinTime() );
            float time = m_time->get( true );

            std::stringstream s;
            s << m_dataSet->getFilename() << "_time" << time;
            boost::shared_ptr< WDataSetScalar const > ds = m_dataSet->calcDataSetAtTime( time, s.str() );
            // get rid of the const
            m_dataSetAtTime = boost::shared_ptr< WDataSetScalar >( new WDataSetScalar( ds->getValueSet(), ds->getGrid() ) );

            if( m_dataSetAtTime )
            {
                WGEColormapping::registerTexture( m_dataSetAtTime->getTexture(), s.str() );
                m_properties->addProperty( m_dataSetAtTime->getTexture()->getProperties() );
                m_infoProperties->addProperty( m_dataSetAtTime->getTexture()->getInformationProperties() );
                if( m_texScaleNormalized->get( true ) )
                {
                    m_dataSetAtTime->getTexture()->minimum()->set( static_cast< float >( m_dataSet->getMinValue() ) );
                    m_dataSetAtTime->getTexture()->scale()->set( static_cast< float >( m_dataSet->getMaxValue() - m_dataSet->getMinValue() ) );
                }
            }
            m_output->updateData( m_dataSetAtTime );
        }
    }

    if( m_dataSetAtTime )
    {
        m_properties->removeProperty( m_dataSetAtTime->getTexture()->getProperties() );
        m_infoProperties->removeProperty( m_dataSetAtTime->getTexture()->getInformationProperties() );
        WGEColormapping::deregisterTexture( m_dataSetAtTime->getTexture() );
        m_dataSetAtTime.reset();
    }
}
