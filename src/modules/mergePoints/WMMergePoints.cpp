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

#include "WMMergePoints.h"

// This line is needed by the module loader to actually find your module. You need to add this to your module too. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMMergePoints )

WMMergePoints::WMMergePoints():
    WModule()
{
}

WMMergePoints::~WMMergePoints()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMMergePoints::factory() const
{
    return boost::shared_ptr< WModule >( new WMMergePoints() );
}

const char** WMMergePoints::getXPMIcon() const
{
    return NULL;
}

const std::string WMMergePoints::getName() const
{
    return "Merge Points";
}

const std::string WMMergePoints::getDescription() const
{
    return "This module merges several given point datasets into one new dataset";
}

void WMMergePoints::connectors()
{
    m_pointsInput1 = WModuleInputData< WDataSetPoints >::createAndAdd( shared_from_this(), "in1", "The dataset" );
    m_pointsInput2 = WModuleInputData< WDataSetPoints >::createAndAdd( shared_from_this(), "in2", "The dataset" );
    m_pointsOutput = WModuleOutputData< WDataSetPoints >::createAndAdd( shared_from_this(), "out", "The merged dataset" );

    // call WModule's initialization
    WModule::connectors();
}

void WMMergePoints::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    // call WModule's initialization
    WModule::properties();
}

/**
 * Copy colors from a source field to a target field as RGBA.
 *
 * \param src source field
 * \param srcType source color type
 * \param dest destination field
 */
void copyColors( WDataSetPoints::ColorArray src, WDataSetPoints::ColorType srcType, WDataSetPoints::ColorArray dest )
{
    for( size_t i = 0; i < src->size() / srcType; ++i )
    {
        switch( srcType )
        {
            case WDataSetPoints::GRAY:
                dest->push_back( src->operator[]( i ) );
                dest->push_back( src->operator[]( i ) );
                dest->push_back( src->operator[]( i ) );
                dest->push_back( 1.0 );
                break;
            case WDataSetPoints::RGB:
                dest->push_back( src->operator[]( srcType * i + 0 ) );
                dest->push_back( src->operator[]( srcType * i + 1 ) );
                dest->push_back( src->operator[]( srcType * i + 2 ) );
                dest->push_back( 1.0 );
                break;
            case WDataSetPoints::RGBA:
                dest->push_back( src->operator[]( srcType * i + 0 ) );
                dest->push_back( src->operator[]( srcType * i + 1 ) );
                dest->push_back( src->operator[]( srcType * i + 2 ) );
                dest->push_back( src->operator[]( srcType * i + 3 ) );
                break;
        }
    }
}

void WMMergePoints::moduleMain()
{
    // get notified about data changes
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_pointsInput1->getDataChangedCondition() );
    m_moduleState.add( m_pointsInput2->getDataChangedCondition() );
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
        bool dataUpdated = m_pointsInput1->handledUpdate() || m_pointsInput2->handledUpdate();
        boost::shared_ptr< WDataSetPoints > dataSet1 = m_pointsInput1->getData();
        boost::shared_ptr< WDataSetPoints > dataSet2 = m_pointsInput2->getData();
        bool dataValid = ( dataSet1 && dataSet2 );

        // reset everything if input was disconnected/invalid
        if( !dataValid )
        {
            debugLog() << "Resetting output.";
            m_pointsOutput->reset();
            continue;
        }

        if( dataValid && dataUpdated )
        {
            WProgress::SPtr progress = WProgress::SPtr( new WProgress( "Creating Points from Fibers." ) );
            m_progress->addSubProgress( progress );

            debugLog() << "Merging " << dataSet1->size() << " with " << dataSet2->size() << " points.";

            WDataSetPoints::VertexArray verts( new WDataSetPoints::VertexArray::element_type( *dataSet1->getVertices() ) );
            WDataSetPoints::ColorArray colors;

            verts->resize( verts->size() + dataSet2->getVertices()->size() );
            std::copy( dataSet2->getVertices()->begin(), dataSet2->getVertices()->end(), verts->begin() + dataSet1->getVertices()->size() );

            // we need to handle the case where both datasets have different color types
            if( dataSet1->getColorType() != dataSet2->getColorType() )
            {
                colors = WDataSetPoints::ColorArray( new WDataSetPoints::ColorArray::element_type() );

                debugLog() << "Color types are different. Handling them properly.";

                // create mem and use the "larger" color type
                colors->reserve( dataSet1->size() * dataSet1->size() * 4 ); // NOTE: we always create RGBA results

                // copy values
                copyColors( dataSet1->getColors(), dataSet1->getColorType(), colors );
                copyColors( dataSet2->getColors(), dataSet2->getColorType(), colors );
            }
            else
            {
                debugLog() << "Color types are equal.";
                colors = WDataSetPoints::ColorArray( new WDataSetPoints::ColorArray::element_type( *dataSet1->getColors() ) );

                // merging colors of the same type is as easy as for the vertex data
                colors->resize( colors->size() +  dataSet2->getColors()->size() );
                std::copy( dataSet2->getColors()->begin(), dataSet2->getColors()->end(), colors->begin() + dataSet1->getColors()->size() );
            }

            // directly merge bounding boxes
            WBoundingBox bb = dataSet1->getBoundingBox();
            bb.expandBy( dataSet2->getBoundingBox() );

            debugLog() << "Done merging. Result are " << verts->size() << " points.";
            // done
            WDataSetPoints::SPtr result( new WDataSetPoints( verts, colors, bb ) );
            m_pointsOutput->updateData( result );
            progress->finish();
            m_progress->removeSubProgress( progress );
        }
    }
}

