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

#include <algorithm>
#include <string>

#include "core/kernel/WKernel.h"
#include "core/common/WPropertyHelper.h"
#include "core/common/math/WMath.h"
#include "core/dataHandler/WDataHandler.h"

#include "WMFibersToPoints.h"

// This line is needed by the module loader to actually find your module. You need to add this to your module too. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMFibersToPoints )

WMFibersToPoints::WMFibersToPoints():
    WModule()
{
}

WMFibersToPoints::~WMFibersToPoints()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMFibersToPoints::factory() const
{
    return boost::shared_ptr< WModule >( new WMFibersToPoints() );
}

const char** WMFibersToPoints::getXPMIcon() const
{
    return NULL;
}

const std::string WMFibersToPoints::getName() const
{
    return "Fibers To Points";
}

const std::string WMFibersToPoints::getDescription() const
{
    return "This module converts fiber data to points by using the fiber vertex points and colors..";
}

void WMFibersToPoints::connectors()
{
    // The input fiber dataset
    m_fiberInput = boost::shared_ptr< WModuleInputData < WDataSetFibers > >(
        new WModuleInputData< WDataSetFibers >( shared_from_this(), "fibers", "The fiber dataset" )
    );

    // As properties, every connector needs to be added to the list of connectors.
    addConnector( m_fiberInput );

    // the points
    m_pointsOutput = boost::shared_ptr< WModuleOutputData < WDataSetPoints > >(
        new WModuleOutputData< WDataSetPoints >( shared_from_this(), "out", "The point data." )
    );

    // As above: make it known.
    addConnector( m_pointsOutput );

    // call WModule's initialization
    WModule::connectors();
}

void WMFibersToPoints::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_filterGroup = m_properties->addPropertyGroup( "Filtering", "Filtering based on fiber parameters." );
    m_paramHint = m_filterGroup->addProperty( "Hint", "If you see this, your data does not contain fiber parameters.",
                                              std::string( "Your data cannot be filtered due to missing parameters." ) );
    m_paramHint->setPurpose( PV_PURPOSE_INFORMATION );

    m_color = m_filterGroup->addProperty( "Color", "The color of the resulting points", defaultColor::WHITE, m_propCondition );
    m_parametersFilterValue = m_filterGroup->addProperty( "Parameter", "Value", 0.0, m_propCondition );
    m_parametersFilterWidth = m_filterGroup->addProperty( "Filter Width", "Width", 0.1, m_propCondition );

    // call WModule's initialization
    WModule::properties();
}


void WMFibersToPoints::moduleMain()
{
    // get notified about data changes
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_fiberInput->getDataChangedCondition() );
    // Remember the condition provided to some properties in properties()? The condition can now be used with this condition set.
    m_moduleState.add( m_propCondition );

    ready();

    // main loop
    while( !m_shutdownFlag() )
    {
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        // woke up since the module is requested to finish?
        if( m_shutdownFlag() )
        {
            break;
        }

        // To query whether an input was updated, simply ask the input:
        bool dataUpdated = m_fiberInput->handledUpdate();
        boost::shared_ptr< WDataSetFibers > dataSet = m_fiberInput->getData();
        bool dataValid = ( dataSet != NULL );
        bool propsChanged = m_parametersFilterValue->changed() ||
                            m_parametersFilterWidth->changed() ||
                            m_color->changed();

        // reset everything if input was disconnected/invalid
        if( !dataValid )
        {
            debugLog() << "Resetting output.";
            m_pointsOutput->reset();
            continue;
        }

        if( dataValid && dataUpdated )
        {
            if( dataSet->getVertexParameters() )
            {
                m_parameterMin = dataSet->getVertexParameters()->operator[]( 0 );
                m_parameterMax = dataSet->getVertexParameters()->operator[]( 0 );
                for( WDataSetFibers::VertexParemeterArray::element_type::const_iterator it = dataSet->getVertexParameters()->begin();
                     it != dataSet->getVertexParameters()->end(); ++it )
                {
                    m_parameterMax = std::max( *it, m_parameterMax );
                    m_parameterMin = std::min( *it, m_parameterMin );
                }
            }
            else
            {
                m_parameterMin = 0.0;
                m_parameterMax = 1.0;
            }

            m_parametersFilterValue->setRecommendedValue( m_parameterMin );
            m_parametersFilterValue->setMin( m_parameterMin );
            m_parametersFilterValue->setMax( m_parameterMax );
            m_parametersFilterWidth->setRecommendedValue( m_parameterMax - m_parameterMin );
            m_parametersFilterWidth->setMin( 0.0 );
            m_parametersFilterWidth->setMax( m_parameterMax - m_parameterMin );
        }

        if( dataValid && !dataUpdated && !propsChanged )
        {
            continue;
        }

        // progress indication
        WProgress::SPtr progress = WProgress::SPtr( new WProgress( "Creating Points from Fibers." ) );
        m_progress->addSubProgress( progress );

        debugLog() << "Creating point data. Num Points = " << dataSet->getVertices()->size() / 3 << ".";
        WDataSetFibers::VertexArray fibVerts = dataSet->getVertices();
        WDataSetFibers::ColorArray fibColors = dataSet->getColorScheme()->getColor();
        WDataSetFibers::VertexParemeterArray fibParams = dataSet->getVertexParameters();

        WDataSetFibers::VertexArray filteredVerts( new WDataSetFibers::VertexArray::element_type() );
        WDataSetFibers::ColorArray filteredColors( new WDataSetFibers::ColorArray::element_type() );
        WBoundingBox filteredBB;

        if( fibParams )
        {
            m_paramHint->setHidden( true );

            WIntervalDouble interval( std::max( m_parametersFilterValue->get( true ) - m_parametersFilterWidth->get( true ), m_parameterMin ),
                                      std::min( m_parametersFilterValue->get( true ) + m_parametersFilterWidth->get( true ), m_parameterMax ) );

            // filter
            for( size_t idx = 0; idx < fibVerts->size() / 3; ++idx )
            {
                if( isInClosed( interval, fibParams->operator[]( idx ) ) )
                {
                    filteredBB.expandBy( fibVerts->operator[]( idx * 3 + 0 ),
                                         fibVerts->operator[]( idx * 3 + 1 ),
                                         fibVerts->operator[]( idx * 3 + 2 ) );

                    filteredVerts->push_back( fibVerts->operator[]( idx * 3 + 0 ) );
                    filteredVerts->push_back( fibVerts->operator[]( idx * 3 + 1 ) );
                    filteredVerts->push_back( fibVerts->operator[]( idx * 3 + 2 ) );

                    filteredColors->push_back( m_color->get().x() );
                    filteredColors->push_back( m_color->get().y() );
                    filteredColors->push_back( m_color->get().z() );
                    filteredColors->push_back( m_color->get().w() );
                }
            }
        }
        else
        {
            m_paramHint->setHidden( false );

            filteredVerts = fibVerts;
            filteredColors = fibColors;
            filteredBB = dataSet->getBoundingBox();
        }


        debugLog() << "Done filtering. Result are " << filteredVerts->size() / 3 << " points.";
        WDataSetPoints::SPtr result( new WDataSetPoints( filteredVerts, filteredColors, filteredBB ) );
        m_pointsOutput->updateData( result );

        progress->finish();
        m_progress->removeSubProgress( progress );
    }
}

