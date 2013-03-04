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

#include "WMSampleOnFibers.h"

// This line is needed by the module loader to actually find your module. You need to add this to your module too. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMSampleOnFibers )

WMSampleOnFibers::WMSampleOnFibers():
    WModule()
{
}

WMSampleOnFibers::~WMSampleOnFibers()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMSampleOnFibers::factory() const
{
    return boost::shared_ptr< WModule >( new WMSampleOnFibers() );
}

const char** WMSampleOnFibers::getXPMIcon() const
{
    return NULL;
}

const std::string WMSampleOnFibers::getName() const
{
    return "Sample On Fibers";
}

const std::string WMSampleOnFibers::getDescription() const
{
    return "This module allows you to sample points on fibers using the parameters on ea fiber.";
}

void WMSampleOnFibers::connectors()
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

void WMSampleOnFibers::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_paramHint = m_properties->addProperty( "Hint", "If you see this, your data does not contain fiber parameters.",
                                              std::string( "Your data cannot be filtered due to missing parameters." ) );
    m_paramHint->setPurpose( PV_PURPOSE_INFORMATION );

    m_color = m_properties->addProperty( "Color", "The color of the resulting points", defaultColor::WHITE, m_propCondition );
    m_parameter = m_properties->addProperty( "Parameter", "Value where to sample", 0.0, m_propCondition );

    // call WModule's initialization
    WModule::properties();
}

void WMSampleOnFibers::moduleMain()
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
        bool dataValid = ( dataSet );
        bool propsChanged = m_parameter ||
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

            m_parameter->setRecommendedValue( m_parameterMin );
            m_parameter->setMin( m_parameterMin );
            m_parameter->setMax( m_parameterMax );
        }

        if( dataValid && !dataUpdated && !propsChanged )
        {
            continue;
        }

        WDataSetFibers::VertexArray fibVerts = dataSet->getVertices();
        WDataSetFibers::ColorArray fibColors = dataSet->getColorScheme()->getColor();
        WDataSetFibers::IndexArray  fibStart = dataSet->getLineStartIndexes();
        WDataSetFibers::LengthArray fibLen   = dataSet->getLineLengths();
        WDataSetFibers::VertexParemeterArray fibParams = dataSet->getVertexParameters();

        debugLog() << "Creating point data.";
        WDataSetFibers::VertexArray filteredVerts( new WDataSetFibers::VertexArray::element_type() );
        WDataSetFibers::ColorArray filteredColors( new WDataSetFibers::ColorArray::element_type() );
        WBoundingBox filteredBB;

        if( fibParams )
        {
            m_paramHint->setHidden( true );

            double v = m_parameter->get( true );

            // progress indication
            WProgress::SPtr progress = WProgress::SPtr( new WProgress( "Creating Points from Fibers.", fibStart->size() ) );
            m_progress->addSubProgress( progress );

            // for each fiber:
            for( size_t fidx = 0; fidx < fibStart->size() ; ++fidx )
            {
                ++*progress;

                // the start vertex index
                size_t sidx = fibStart->at( fidx ) * 3;

                // the length of the fiber
                size_t len = fibLen->at( fidx );

                // a line needs 2 verts at least, but also check first and only vertex
                if( len < 2 )
                {
                    if( fibParams->at( fibStart->at( fidx ) ) == v )
                    {
                        filteredVerts->push_back( fibVerts->at( sidx + 0 ) );
                        filteredVerts->push_back( fibVerts->at( sidx + 1 ) );
                        filteredVerts->push_back( fibVerts->at( sidx + 2 ) );

                        filteredBB.expandBy(
                            osg::Vec3(
                                fibVerts->at( sidx + 0 ),
                                fibVerts->at( sidx + 1 ),
                                fibVerts->at( sidx + 2 ) )
                        );
                    }
                    continue;
                }

                // keep track of the previous vertex
                osg::Vec3 lastVert = osg::Vec3( fibVerts->at( sidx ),
                                                fibVerts->at( sidx + 1 ),
                                                fibVerts->at( sidx + 2 ) );
                // keep track of the previous value
                double lastParam = fibParams->at( fibStart->at( fidx ) );

                // walk along the fiber
                for( size_t k = 0; k < len; ++k )
                {
                    // get current vertex and value
                    osg::Vec3 vert = osg::Vec3( fibVerts->at( ( 3 * k ) + sidx ),
                                                fibVerts->at( ( 3 * k ) + sidx + 1 ),
                                                fibVerts->at( ( 3 * k ) + sidx + 2 ) );
                    double param = fibParams->at( k + fibStart->at( fidx ) );

                    // is there a sample in between the values?
                    WIntervalDouble interval( param, lastParam );
                    if( isInClosed( interval, v ) )
                    {
                        // linearly interpolate
                        double n = ( v - lastParam ) / ( param - lastParam );
                        osg::Vec3 interpolatedVert( lastVert + ( vert - lastVert ) * n );

                        filteredVerts->push_back( interpolatedVert.x() );
                        filteredVerts->push_back( interpolatedVert.y() );
                        filteredVerts->push_back( interpolatedVert.z() );

                        filteredBB.expandBy( interpolatedVert );
                    }

                    // keep track of the last value/vertex
                    lastParam = param;
                    lastVert = vert;
                }
            }

            // add colors
            for( size_t i = 0; i < filteredVerts->size() / 3; ++i )
            {
                filteredColors->push_back( m_color->get( true ).r() );
                filteredColors->push_back( m_color->get( true ).g() );
                filteredColors->push_back( m_color->get( true ).b() );
                filteredColors->push_back( m_color->get( true ).a() );
            }

            WDataSetPoints::SPtr result( new WDataSetPoints( filteredVerts, filteredColors, filteredBB ) );
            m_pointsOutput->updateData( result );
            progress->finish();
            m_progress->removeSubProgress( progress );
            debugLog() << "Done filtering. Result are " << filteredVerts->size() / 3 << " points.";
        }
        else
        {
            warnLog() << "Data does not contain the values to use for sampling.";
            m_paramHint->setHidden( false );
        }
    }
}

