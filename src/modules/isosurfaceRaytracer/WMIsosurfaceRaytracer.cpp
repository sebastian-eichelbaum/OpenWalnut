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

#include <osg/Geode>
#include <osg/Group>
#include <osg/Material>
#include <osg/ShapeDrawable>
#include <osg/StateAttribute>

#include "../../common/WColor.h"
#include "../../common/WPropertyHelper.h"
#include "../../dataHandler/WDataSetScalar.h"
#include "../../dataHandler/WDataTexture3D_2.h"
#include "../../graphicsEngine/WGEColormapping.h"
#include "../../graphicsEngine/WGEGeodeUtils.h"
#include "../../graphicsEngine/WGEManagedGroupNode.h"
#include "../../graphicsEngine/WGEUtils.h"
#include "../../graphicsEngine/WGEPropertyUniform.h"
#include "../../graphicsEngine/WShader.h"
#include "../../kernel/WKernel.h"
#include "WMIsosurfaceRaytracer.xpm"
#include "WMIsosurfaceRaytracer.h"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMIsosurfaceRaytracer )

WMIsosurfaceRaytracer::WMIsosurfaceRaytracer():
    WModule()
{
    // Initialize members
}

WMIsosurfaceRaytracer::~WMIsosurfaceRaytracer()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMIsosurfaceRaytracer::factory() const
{
    return boost::shared_ptr< WModule >( new WMIsosurfaceRaytracer() );
}

const char** WMIsosurfaceRaytracer::getXPMIcon() const
{
    return isosurfaceraytracer_xpm;
}

const std::string WMIsosurfaceRaytracer::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "Isosurface Raytracer";
}

const std::string WMIsosurfaceRaytracer::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    return "This module shows a fast raytraced isosurface of the specified scalar dataset.";
}

void WMIsosurfaceRaytracer::connectors()
{
    // DVR needs one input: the scalar dataset
    m_input = boost::shared_ptr< WModuleInputData < WDataSetScalar  > >(
        new WModuleInputData< WDataSetScalar >( shared_from_this(), "in", "The scalar dataset shown using isosurface." )
    );

    // As properties, every connector needs to be added to the list of connectors.
    addConnector( m_input );

    // call WModules initialization
    WModule::connectors();
}

void WMIsosurfaceRaytracer::properties()
{
    // Initialize the properties
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_isoValue      = m_properties->addProperty( "Isovalue",         "The isovalue used whenever the isosurface Mode is turned on.",
                                                                      128.0 );

    m_isoColor      = m_properties->addProperty( "Iso color",        "The color to blend the isosurface with.", WColor( 1.0, 1.0, 1.0, 1.0 ),
                      m_propCondition );

    m_stepCount     = m_properties->addProperty( "Step count",       "The number of steps to walk along the ray during raycasting. A low value "
                                                                      "may cause artifacts whilst a high value slows down rendering.", 250 );
    m_stepCount->setMin( 1 );
    m_stepCount->setMax( 1000 );

    m_alpha         = m_properties->addProperty( "Opacity %",        "The opacity in %. Transparency = 1 - Opacity.", 1.0 );
    m_alpha->setMin( 0.0 );
    m_alpha->setMax( 1.0 );

    m_colormapRatio = m_properties->addProperty( "Colormap Ratio",   "The intensity of the colormap in contrast to surface shading.", 0.5 );
    m_colormapRatio->setMin( 0.0 );
    m_colormapRatio->setMax( 1.0 );

    // Lighting
    m_shadingSelections = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_shadingSelections->addItem( "Emphasize cortex", "Emphasize the cortex. Inner parts are not that well lighten." );
    m_shadingSelections->addItem( "Depth only",       "Only show the depth of the surface along the ray." );
    m_shadingSelections->addItem( "Phong",            "Phong lighting. Slower but more realistic lighting" );
    m_shadingSelections->addItem( "Phong + depth",    "Phong lighting in combination with depth cueing." );
    m_shadingAlgo   = m_properties->addProperty( "Shading", "The shading algorithm.", m_shadingSelections->getSelectorLast(), m_propCondition );

    WPropertyHelper::PC_SELECTONLYONE::addTo( m_shadingAlgo );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_shadingAlgo );

    WModule::properties();
}

void WMIsosurfaceRaytracer::moduleMain()
{
    m_shader = osg::ref_ptr< WShader > ( new WShader( "WMIsosurfaceRaytracer", m_localPath ) );

    // let the main loop awake if the data changes or the properties changed.
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    // Signal ready state.
    ready();
    debugLog() << "Module is now ready.";

    osg::ref_ptr< WGEManagedGroupNode > rootNode = new WGEManagedGroupNode( m_active );
    bool rootInserted = false;

    // Normally, you will have a loop which runs as long as the module should not shutdown. In this loop you can react on changing data on input
    // connectors or on changed in your properties.
    debugLog() << "Entering main loop";
    while ( !m_shutdownFlag() )
    {
        // Now, the moduleState variable comes into play. The module can wait for the condition, which gets fired whenever the input receives data
        // or an property changes. The main loop now waits until something happens.
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        // quit if requested
        if ( m_shutdownFlag() )
        {
            break;
        }

        // was there an update?
        bool dataUpdated = m_input->updated();
        boost::shared_ptr< WDataSetScalar > dataSet = m_input->getData();
        bool dataValid   = ( dataSet );

        // m_isoColor or shading changed
        if ( m_isoColor->changed() || m_shadingAlgo->changed() )
        {
            // a new color requires the proxy geometry to be rebuild as we store it as color in this geometry
            dataUpdated = true;
        }

        // As the data has changed, we need to recreate the texture.
        if ( dataUpdated && dataValid )
        {
            debugLog() << "Data changed. Uploading new data as texture.";

            m_isoValue->setMin( dataSet->getTexture2()->minimum()->get() );
            m_isoValue->setMax( dataSet->getTexture2()->scale()->get() + dataSet->getTexture2()->minimum()->get() );
            m_isoValue->set( dataSet->getTexture2()->minimum()->get() + ( 0.5 * dataSet->getTexture2()->scale()->get() ) );

            // First, grab the grid
            boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( dataSet->getGrid() );
            if ( !grid )
            {
                errorLog() << "The dataset does not provide a regular grid. Ignoring dataset.";
                continue;
            }

            // use the OSG Shapes, create unit cube
            osg::ref_ptr< osg::Node > cube = wge::generateSolidBoundingBoxNode(
                wmath::WPosition( 0.0, 0.0, 0.0 ),
                wmath::WPosition( grid->getNbCoordsX() - 1, grid->getNbCoordsY() - 1, grid->getNbCoordsZ() - 1 ),
                m_isoColor->get( true )
            );
            cube->asTransform()->getChild( 0 )->setName( "_DVR Proxy Cube" ); // Be aware that this name is used in the pick handler.
                                                                              // because of the underscore in front it won't be picked
            // we also set the grid's transformation here
            rootNode->setMatrix( wge::toOSGMatrix( grid->getTransformationMatrix() ) );

            m_shader->apply( cube );

            // bind the texture to the node
            osg::StateSet* rootState = cube->getOrCreateStateSet();
            osg::ref_ptr< WGETexture3D > texture3D = dataSet->getTexture2();
            texture3D->bind( cube );
            WGEColormapping::apply( cube, false, 1 );

            // enable transparency
            rootState->setMode( GL_BLEND, osg::StateAttribute::ON );

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            // setup defines (lighting)
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            size_t shadingAlgo = m_shadingAlgo->get( true ).getItemIndexOfSelected( 0 );
            m_shader->eraseAllDefines();
            switch ( shadingAlgo )
            {
                case Cortex:
                    m_shader->setDefine( "CORTEX" );
                    break;
                case Depth:
                    m_shader->setDefine( "DEPTHONLY" );
                    break;
                case Phong:
                    m_shader->setDefine( "PHONG" );
                    break;
                case PhongDepth:
                    m_shader->setDefine( "PHONGWITHDEPTH" );
                    break;
                default:
                    m_shader->setDefine( "CORTEX" );
                    break;
            }

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            // setup all those uniforms
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            rootState->addUniform( new WGEPropertyUniform< WPropDouble >( "u_isovalue", m_isoValue ) );
            rootState->addUniform( new WGEPropertyUniform< WPropInt >( "u_steps", m_stepCount ) );
            rootState->addUniform( new WGEPropertyUniform< WPropDouble >( "u_alpha", m_alpha ) );
            rootState->addUniform( new WGEPropertyUniform< WPropDouble >( "u_colormapRatio", m_colormapRatio ) );

            // update node
            debugLog() << "Adding new rendering.";
            rootNode->clear();
            rootNode->insert( cube );
            // insert root node if needed. This way, we ensure that the root node gets added only if the proxy cube has been added AND the bbox
            // can be calculated properly by the OSG to ensure the proxy cube is centered in the scene if no other item has been added earlier.
            if ( !rootInserted )
            {
                rootInserted = true;
                WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( rootNode );
            }
        }
    }

    // At this point, the container managing this module signalled to shutdown. The main loop has ended and you should clean up. Always remove
    // allocated memory and remove all OSG nodes.
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( rootNode );
}

