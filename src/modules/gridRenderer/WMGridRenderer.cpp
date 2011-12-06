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

#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osgText/Text>

#include "core/common/math/linearAlgebra/WLinearAlgebra.h"
#include "core/common/WAssert.h"
#include "core/common/WBoundingBox.h"
#include "core/common/WStringUtils.h"
#include "core/dataHandler/WGridRegular3D.h"
#include "core/graphicsEngine/WGEGeodeUtils.h"
#include "core/graphicsEngine/callbacks/WGENodeMaskCallback.h"
#include "core/kernel/WKernel.h"
#include "WMGridRenderer.h"
#include "WMGridRenderer.xpm"
#include "WMGridRenderer_boundary.xpm"
#include "WMGridRenderer_grid.xpm"
#include "WMGridRenderer_label.xpm"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMGridRenderer )

WMGridRenderer::WMGridRenderer():
    WModule(),
    m_recompute( boost::shared_ptr< WCondition >( new WCondition() ) )
{
    // WARNING: initializing connectors inside the constructor will lead to an exception.
    // Implement WModule::initializeConnectors instead.
}

WMGridRenderer::~WMGridRenderer()
{
    // cleanup
    removeConnectors();
}

boost::shared_ptr< WModule > WMGridRenderer::factory() const
{
    return boost::shared_ptr< WModule >( new WMGridRenderer() );
}

const char** WMGridRenderer::getXPMIcon() const
{
    return WMGridRenderer_xpm;
}

const std::string WMGridRenderer::getName() const
{
    return "Grid Renderer";
}

const std::string WMGridRenderer::getDescription() const
{
    return "Shows the bounding box and grid of a data set.";
}

void WMGridRenderer::moduleMain()
{
    // use the m_input "data changed" flag
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_recompute );

    // signal ready state
    ready();

    // loop until the module container requests the module to quit
    while( !m_shutdownFlag() )
    {
        debugLog() << "Waiting for data ...";
        m_moduleState.wait();

        boost::shared_ptr< WDataSetSingle > dataSet = m_input->getData();
        bool dataValid = ( dataSet );

        if( !dataValid )
        {
            // OK, the output has not yet sent data
            // NOTE: see comment at the end of this while loop for m_moduleState
            WGraphicsEngine::getGraphicsEngine()->getScene()->remove( m_gridNode );
            continue;
        }

        WGridRegular3D::SPtr regGrid = boost::shared_dynamic_cast< WGridRegular3D >( dataSet->getGrid() );
        if( !regGrid )
        {
            // the data has no regular 3d grid.
            errorLog() << "Dataset does not contain a regular 3D grid.";
            WGraphicsEngine::getGraphicsEngine()->getScene()->remove( m_gridNode );
            continue;
        }

        // create the new grid node if it not exists
        if( !m_gridNode )
        {
            m_gridNode = new WGEGridNode( regGrid );
            m_gridNode->addUpdateCallback( new WGENodeMaskCallback( m_active ) );
            WGraphicsEngine::getGraphicsEngine()->getScene()->insert( m_gridNode );
        }

        m_gridNode->setBBoxColor( *m_bboxColor );
        m_gridNode->setGridColor( *m_gridColor );
        updateNode( m_mode );
        m_gridNode->setGrid( regGrid );
    }

    WGraphicsEngine::getGraphicsEngine()->getScene()->remove( m_gridNode );
}

void WMGridRenderer::connectors()
{
    // initialize connectors
    m_input = boost::shared_ptr< WModuleInputData < WDataSetSingle  > >(
        new WModuleInputData< WDataSetSingle >( shared_from_this(),
                                                               "in", "The dataset to filter" )
        );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_input );

    // call WModules initialization
    WModule::connectors();
}

void WMGridRenderer::properties()
{
    WPropertyBase::PropertyChangeNotifierType  notifier = boost::bind( &WMGridRenderer::updateNode, this, _1 );

    m_bboxColor = m_properties->addProperty( "Bounding box color", "The color of the bounding box.", WColor( 0.3, 0.3, 0.3, 1.0 ), notifier );

    m_gridColor = m_properties->addProperty( "Grid color", "The color of the grid.", WColor( 0.1, 0.1, 0.1, 1.0 ), notifier );

    m_possibleModes = WItemSelection::SPtr( new WItemSelection() );
    m_possibleModes->addItem( "Labels", "Show the boundary labels.", WMGridRenderer_label_xpm );          // NOTE: you can add XPM images here.
    m_possibleModes->addItem( "Bounding box", "Show the bounding box.", WMGridRenderer_boundary_xpm );
    m_possibleModes->addItem( "Grid", "Show the inner grid.",  WMGridRenderer_grid_xpm );

    // selecting all at once might be a bad idea since the grid rendering can be very very slow. So, by default, only show bbox and labels.
    WItemSelector sel = m_possibleModes->getSelectorFirst();
    m_mode = m_properties->addProperty( "Mode", "What should be rendered.",  sel.newSelector( 1 ), notifier );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_mode );

    WModule::properties();
}

void WMGridRenderer::updateNode( WPropertyBase::SPtr property )
{
    // only update if there is a grid node
    if( !m_gridNode )
    {
        return;
    }

    // color of bbox changed
    if( property == m_bboxColor )
    {
        m_gridNode->setBBoxColor( *m_bboxColor );
    }

    // color of grid changed
    if( property == m_gridColor )
    {
        m_gridNode->setGridColor( *m_gridColor );
    }

    // mode changed
    if( property == m_mode )
    {
        WItemSelector s = m_mode->get( true );

        bool labels = false;
        bool bbox = false;
        bool grid = false;

        // The multi property allows the selection of several items. So, iteration needs to be done here:
        for( size_t i = 0; i < s.size(); ++i )
        {
            size_t idx = s.getItemIndexOfSelected( i );

            // check what was selected
            labels = labels || ( idx == 0 );
            bbox = bbox || ( idx == 1 );
            grid = grid || ( idx == 2 );
        }

        m_gridNode->setEnableLabels( labels );
        m_gridNode->setEnableGrid( grid );
        m_gridNode->setEnableBBox( bbox );
    }
}
