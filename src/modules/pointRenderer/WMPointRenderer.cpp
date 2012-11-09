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

#include <osg/Geode>

#include "core/common/math/WMath.h"
#include "core/common/WLimits.h"
#include "core/graphicsEngine/shaders/WGEShader.h"
#include "core/graphicsEngine/WGEColormapping.h"
#include "core/graphicsEngine/WGEGeodeUtils.h"
#include "core/graphicsEngine/WGEManagedGroupNode.h"
#include "core/graphicsEngine/postprocessing/WGEPostprocessingNode.h"
#include "core/graphicsEngine/WGEUtils.h"
#include "core/kernel/WKernel.h"

#include "core/dataHandler/WDataSetPoints.h"

#include "WMPointRenderer.h"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMPointRenderer )

WMPointRenderer::WMPointRenderer():
    WModule()
{
}

WMPointRenderer::~WMPointRenderer()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMPointRenderer::factory() const
{
    // See "src/modules/template/" for an extensively documented example.
    return boost::shared_ptr< WModule >( new WMPointRenderer() );
}

const char** WMPointRenderer::getXPMIcon() const
{
    // Thank you Alex for providing some nice icon here ;-)
    return NULL;
}

const std::string WMPointRenderer::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "Point Renderer";
}

const std::string WMPointRenderer::getDescription() const
{
    return "Render a given point dataset.";
}

void WMPointRenderer::connectors()
{
    // this input contains the triangle data
    m_pointInput = WModuleInputData< WDataSetPoints >::createAndAdd( shared_from_this(), "points", "The data to display" );

    // call WModule's initialization
    WModule::connectors();
}

void WMPointRenderer::properties()
{
    // some properties need to trigger an update
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

/*    m_nbTriangles = m_infoProperties->addProperty( "Triangles", "The number of triangles in the mesh.", 0 );
    m_nbTriangles->setMax( std::numeric_limits< int >::max() );

    m_nbVertices = m_infoProperties->addProperty( "Vertices", "The number of vertices in the mesh.", 0 );
    m_nbVertices->setMax( std::numeric_limits< int >::max() );

    // setup all the properties. See header file for their meaning and purpose.
    m_showOutline = m_properties->addProperty( "Outline", "Show all edges of the trinagulation as lines.", false, m_propCondition );
    m_mainComponentOnly = m_properties->addProperty( "Main component", "Main component only", false, m_propCondition );
    m_showCoordinateSystem = m_properties->addProperty( "Coordinate system", "If enabled, the coordinate system of the mesh will be shown.",
                                                        false, m_propCondition );

    m_coloringGroup = m_properties->addPropertyGroup( "Coloring", "Coloring options and colormap options." );

    m_opacity = m_coloringGroup->addProperty( "Opacity %", "Opaqueness of surface.", 100.0 );
    m_opacity->setMin( 0.0 );
    m_opacity->setMax( 100.0 );

    // Allow the user to select different colormodes
    boost::shared_ptr< WItemSelection > colorModes( boost::shared_ptr< WItemSelection >( new WItemSelection() ) );
    colorModes->addItem( "Single Color", "The whole surface is colored using the default color." );
    colorModes->addItem( "From Mesh", "The surface is colored according to the mesh." );
    colorModes->addItem( "From colormap connector", "The surface is colored using the colormap on colorMap connector." );
    m_colorMode = m_coloringGroup->addProperty( "Color mode", "Choose one of the available colorings.", colorModes->getSelectorFirst(),
                                             m_propCondition );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_colorMode );

    // this is the color used if single color is selected
    m_color = m_coloringGroup->addProperty( "Default color", "The color of of the surface.",
                                         WColor( .9f, .9f, 0.9f, 1.0f ), m_propCondition );

    m_colormap = m_coloringGroup->addProperty( "Enable colormapping", "Turn colormapping on", false );
    m_colormapRatio = m_coloringGroup->addProperty( "Colormap ratio", "Set the colormap Ratio", 0.5 );
    m_colormapRatio->setMin( 0.0 );
    m_colormapRatio->setMax( 1.0 );
*/
    // call WModule's initialization
    WModule::properties();
}

void WMPointRenderer::moduleMain()
{
    // let the main loop awake if the data changes.
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_pointInput->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    // signal ready state. The module is now ready to be used.
    ready();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    // setup the main graphics-node:
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    // create the post-processing node which actually does the nice stuff to the rendered image
    osg::ref_ptr< WGEPostprocessingNode > postNode = new WGEPostprocessingNode(
        WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getCamera()
    );
    postNode->addUpdateCallback( new WGENodeMaskCallback( m_active ) ); // disable the postNode with m_active
    // provide the properties of the post-processor to the user
    m_properties->addProperty( postNode->getProperties() );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( postNode );

    // load the GLSL shader:
    m_shader = new WGEShader( "WMPointRenderer", m_localPath );
    // set geometry shader options
    m_shader->setParameter( GL_GEOMETRY_VERTICES_OUT_EXT, 4 );
    m_shader->setParameter( GL_GEOMETRY_INPUT_TYPE_EXT, GL_POINTS );
    m_shader->setParameter( GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_TRIANGLE_STRIP );


    // loop until the module container requests the module to quit
    while( !m_shutdownFlag() )
    {
        // Now, the moduleState variable comes into play. The module can wait for the condition, which gets fired whenever the input receives data
        // or an property changes. The main loop now waits until something happens.
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        // woke up since the module is requested to finish
        if( m_shutdownFlag() )
        {
            break;
        }

        // Get data and check for invalid data.
        WDataSetPoints::SPtr points = m_pointInput->getData();
        if( !points )
        {
            debugLog() << "Invalid Data. Disabling.";
            postNode->clear();
            continue;
        }

        // we have valid data. Put this into a geode
        osg::ref_ptr< osg::Geometry > geometry = osg::ref_ptr< osg::Geometry >( new osg::Geometry );
        osg::ref_ptr< osg::Geode >  geode( new osg::Geode() );

        osg::StateSet* state = geode->getOrCreateStateSet();
        state->setMode( GL_BLEND, osg::StateAttribute::ON );

        // convert point arrays to osg vec3 arrays
        osg::ref_ptr< osg::Vec3Array > vertices = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
        osg::ref_ptr< osg::Vec4Array > colors = osg::ref_ptr< osg::Vec4Array >( new osg::Vec4Array );

        WDataSetPoints::VertexArray pointVertices = points->getVertices();
        WDataSetPoints::ColorArray pointColors = points->getColors();
        for( size_t pointIdx = 0; pointIdx < points->size(); ++pointIdx )
        {

            osg::Vec3 vert = points->operator[]( pointIdx );
            osg::Vec4 color = points->getColor( pointIdx );

            vertices->push_back( vert );
            colors->push_back( color );
        }

        // combine to geometry
        geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::POINTS, 0, vertices->size() ) );
        geometry->setVertexArray( vertices );
        geometry->setColorArray( colors );
        geometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );

        // add geometry to geode
        geode->addDrawable( geometry );

        // shader and colormapping
        m_shader->apply( geode );

        // add geode to group
        postNode->clear();
        postNode->insert( geode, m_shader );
    }

    // it is important to always remove the modules again
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( postNode );
}

