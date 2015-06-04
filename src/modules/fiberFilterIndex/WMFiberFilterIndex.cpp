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
#include <algorithm>

#include "core/kernel/WKernel.h"

#include "WMFiberFilterIndex.h"

// This line is needed by the module loader to actually find your module. You need to add this to your module too. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMFiberFilterIndex )

WMFiberFilterIndex::WMFiberFilterIndex():
    WModule()
{
    // Init
}

WMFiberFilterIndex::~WMFiberFilterIndex()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMFiberFilterIndex::factory() const
{
    return boost::shared_ptr< WModule >( new WMFiberFilterIndex() );
}

const std::string WMFiberFilterIndex::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "Fiber Filter Index";
}

const std::string WMFiberFilterIndex::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    return "This module provides fiber data filtered by fiber index.";
}

void WMFiberFilterIndex::connectors()
{
    m_input = WModuleInputData< WDataSetFibers >::createAndAdd( shared_from_this(), "in", "The dataset to filter" );
    m_output = WModuleOutputData < WDataSetFibers  >::createAndAdd( shared_from_this(), "out", "The filtered dataset" );

    // call WModule's initialization
    WModule::connectors();
}

void WMFiberFilterIndex::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );
    m_indexProp = m_properties->addProperty( "Index", "Select the fiber with this index.", 0, m_propCondition );
    m_indexProp->setMin( 0 );
    m_indexProp->setMax( 1 );

    WModule::properties();
}

void WMFiberFilterIndex::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    // Signal ready state. Now your module can be connected by the container, which owns the module.
    ready();
    waitRestored();

    // main loop
    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        // woke up since the module is requested to finish
        if( m_shutdownFlag() )
        {
            break;
        }

        // Remember the above criteria. We now need to check if the data is valid. After a connect-update, it might be NULL.
        boost::shared_ptr< WDataSetFibers > dataSet = m_input->getData();
        bool dataValid = ( dataSet );
        bool dataChanged = dataSet != m_fibers;

        // do something with the data
        if( dataChanged )
        {
            m_fibers = dataSet;

            if( dataValid )
            {
                // The data is valid and we received an update. The data is not NULL but may be the same as in previous loops.
                debugLog() << "Data changed. Recalculating output.";
                m_indexProp->setMax( m_fibers->size() - 1 );
                updateOutput();
            }
        }

        if( m_indexProp->changed( true ) )
        {
            updateOutput();
        }
    }
}

void WMFiberFilterIndex::updateOutput()
{
    // target memory
    boost::shared_ptr< std::vector< float > >   vertices( new std::vector< float >() );
    boost::shared_ptr< std::vector< size_t > >  lineStartIndexes( new std::vector< size_t >() );
    boost::shared_ptr< std::vector< size_t > >  lineLengths( new std::vector< size_t >() );
    boost::shared_ptr< std::vector< size_t > >  verticesReverse( new std::vector< size_t >() );
    WDataSetFibers::VertexParemeterArray        vertexParameter;

    boost::shared_ptr< WProgress > progress1( new WProgress( "Filtering" ) );
    m_progress->addSubProgress( progress1 );

    size_t idx = m_indexProp->get( true );

    // Valid index?
    if( idx >= m_fibers->size() )
    {
        errorLog() << "Index invalid: " << idx << " - data has only " << m_fibers->size() << " lines.";
        m_output->reset();
    }

    // Is there an parameter array defined in the source data?
    bool hasAttribs = ( m_fibers->getVertexParameters() != NULL );

    // Get vertex data
    size_t startIdx =m_fibers->getStartIndex( idx );
    size_t startIdx3 = 3 * startIdx;
    size_t len = m_fibers->getLengthOfLine( idx );
    size_t len3 = 3 * len;

    // Copy vertex data
    vertices->resize( len * 3 );
    std::copy( m_fibers->getVertices()->begin() + startIdx3, m_fibers->getVertices()->begin() + startIdx3 + len3, vertices->begin() );

    // Copy attrib data if any
    if( hasAttribs )
    {
        vertexParameter = WDataSetFibers::VertexParemeterArray( new WDataSetFibers::VertexParemeterArray::element_type( len ) );
        std::copy( m_fibers->getVertexParameters()->begin() + startIdx, m_fibers->getVertexParameters()->begin() + startIdx + len,
                   vertexParameter->begin() );
    }

    // As the fiber display has problems with data with less than 3 vertices -> ensure 3
    if( len == 2 )
    {
        len = 3;

        // Use the last vertex again -> visually not visible.
        vertices->push_back( vertices->at( 3 ) );
        vertices->push_back( vertices->at( 4 ) );
        vertices->push_back( vertices->at( 5 ) );

        if( hasAttribs )
        {
            vertexParameter->push_back( vertexParameter->at( 1 ) );
        }
    }

    // the remaining info is trivial
    lineStartIndexes->push_back( 0 );
    lineLengths->push_back( len );
    verticesReverse->resize( len, 0 );

    // Update output and finish
    boost::shared_ptr< WDataSetFibers> newOutput( new WDataSetFibers( vertices, lineStartIndexes, lineLengths, verticesReverse, vertexParameter ) );
    m_output->updateData( newOutput );

    progress1->finish();
}
