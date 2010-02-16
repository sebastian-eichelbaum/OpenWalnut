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
#include <utility>

#include <osg/ShapeDrawable>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Material>
#include <osg/StateAttribute>

#include "../../kernel/WKernel.h"
#include "../../common/WColor.h"
#include "../../graphicsEngine/WGEUtils.h"
#include "../../graphicsEngine/WGEGeodeUtils.h"
#include "../../graphicsEngine/WShader.h"

#include "WMDirectVolumeRendering.h"

WMDirectVolumeRendering::WMDirectVolumeRendering():
    WModule()
{
    // Initialize members
}

WMDirectVolumeRendering::~WMDirectVolumeRendering()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMDirectVolumeRendering::factory() const
{
    return boost::shared_ptr< WModule >( new WMDirectVolumeRendering() );
}

const std::string WMDirectVolumeRendering::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "Direct Volume Rendering";
}

const std::string WMDirectVolumeRendering::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    return "This module shows a direct volume rendering of the specified scalar dataset.";
}

void WMDirectVolumeRendering::connectors()
{
    // DVR needs one input: the scalar dataset
    m_input = boost::shared_ptr< WModuleInputData < WDataSetSingle  > >(
        new WModuleInputData< WDataSetSingle >( shared_from_this(), "in", "The scalar dataset shown using DVR." )
    );

    // As properties, every connector needs to be added to the list of connectors.
    addConnector( m_input );

    // call WModules initialization
    WModule::connectors();
}

void WMDirectVolumeRendering::properties()
{
    // Initialize the properties
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );
}

void WMDirectVolumeRendering::moduleMain()
{
    m_shader = osg::ref_ptr< WShader > ( new WShader( "DVRRaycast" ) );

    // let the main loop awake if the data changes or the properties changed.
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    // Signal ready state.
    ready();
    debugLog() << "Module is now ready.";

    // Normally, you will have a loop which runs as long as the module should not shutdown. In this loop you can react on changing data on input
    // connectors or on changed in your properties.
    debugLog() << "Entering main loop";
    while ( !m_shutdownFlag() )
    {
        // Now, the moduleState variable comes into play. The module can wait for the condition, which gets fired whenever the input receives data
        // or an property changes. The main loop now waits until something happens.
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        // has the data changed?
        boost::shared_ptr< WDataSetSingle > newDataSet = m_input->getData();
        bool dataChanged = ( m_dataSet != newDataSet );
        if ( dataChanged || !m_dataSet )
        // this condition will become true whenever the new data is different from the current one or our actual data is NULL. This handles all
        // cases.
        {
            // The data is different. Copy it to our internal data variable:
            debugLog() << "Received Data.";
            m_dataSet = newDataSet;

            // invalid data
            if ( !m_dataSet )
            {
                debugLog() << "Invalid Data. Disabling.";
                continue;
            }
        }

        // As the data has changed, we need to recreate the texture.
        if ( dataChanged )
        {
            debugLog() << "Data changed. Uploading new data as texture.";

            // First, grab the grid
            boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( m_dataSet->getGrid() );
            if ( !grid )
            {
                errorLog() << "The dataset does not provide a regular grid. Ignoring dataset.";
                continue;
            }

            // get the BBox
            std::pair< wmath::WPosition, wmath::WPosition > bb = grid->getBoundingBox();

            // use the OSG Shapes, create unit cube
            osg::ref_ptr< osg::Node > cube = wge::generateSolidBoundingBoxNode( bb.first, bb.second, WColor( 0.0, 0.0, 0.0, 1.0 ) );
            m_shader->apply( cube );

            // update node
            WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );
            m_rootNode = cube;
            WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootNode );
        }
    }

    // At this point, the container managing this module signalled to shutdown. The main loop has ended and you should clean up. Always remove
    // allocated memory and remove all OSG nodes.
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );
}

void WMDirectVolumeRendering::SafeUpdateCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    traverse( node, nv );
}

void WMDirectVolumeRendering::activate()
{
    // Activate/Deactivate the DVR
    if ( m_active->get() )
    {
        m_rootNode->setNodeMask( 0xFFFFFFFF );
    }
    else
    {
        m_rootNode->setNodeMask( 0x0 );
    }

    // Always call WModule's activate!
    WModule::activate();
}

