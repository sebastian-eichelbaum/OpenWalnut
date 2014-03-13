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

#include <algorithm>
#include <list>
#include <limits>
#include <map>
#include <string>
#include <vector>

#include <osg/Geode>
#include <osg/LightModel>

#include "core/common/math/WMath.h"
#include "core/common/WLimits.h"
#include "core/graphicsEngine/shaders/WGEPropertyUniform.h"
#include "core/graphicsEngine/shaders/WGEShader.h"
#include "core/graphicsEngine/shaders/WGEShaderPropertyDefineOptions.h"
#include "core/graphicsEngine/WGEColormapping.h"
#include "core/graphicsEngine/WGEGeodeUtils.h"
#include "core/graphicsEngine/WGEManagedGroupNode.h"
#include "core/graphicsEngine/WGEUtils.h"
#include "core/graphicsEngine/WTriangleMesh.h"
#include "core/kernel/WKernel.h"

#include "WMSurfaceIllustrator.h"
#include "WMSurfaceIllustrator.xpm"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMSurfaceIllustrator )

WMSurfaceIllustrator::WMSurfaceIllustrator():
    WModule()
{
}

WMSurfaceIllustrator::~WMSurfaceIllustrator()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMSurfaceIllustrator::factory() const
{
    // See "src/modules/template/" for an extensively documented example.
    return boost::shared_ptr< WModule >( new WMSurfaceIllustrator() );
}

const char** WMSurfaceIllustrator::getXPMIcon() const
{
    return WMSurfaceIllustrator_xpm;
}

const std::string WMSurfaceIllustrator::getName() const
{
    return "Surface Illustrator";
}

const std::string WMSurfaceIllustrator::getDescription() const
{
    return "Takes a (parameterized) triangle mesh as input and renders it as a shaded surface with serverall illustration options.";
}

void WMSurfaceIllustrator::updateMinMax( double& minX, double& maxX, // NOLINT
                                           double& minY, double& maxY, // NOLINT
                                           double& minZ, double& maxZ, const osg::Vec3d& vector ) const // NOLINT
{
    minX = std::min( minX, vector.x() );
    minY = std::min( minY, vector.y() );
    minZ = std::min( minZ, vector.z() );

    maxX = std::max( maxX, vector.x() );
    maxY = std::max( maxY, vector.y() );
    maxZ = std::max( maxZ, vector.z() );
}

double WMSurfaceIllustrator::getMedian( double x, double y, double z ) const
{
    if( ( y < x && z > x ) || ( z < x && y > x) )
    {
        return x;
    }

    if( ( x < y && z > y ) || ( x < y && z > y) )
    {
        return y;
    }

    if( ( y < z && x > z ) || ( x < z && y > z) )
    {
        return z;
    }

    if( x == y )
    {
        return x;
    }
    if( x == z )
    {
        return x;
    }
    if( y == z )
    {
        return z;
    }
    if( x == y && y == z && x == z)
    {
        return x;
    }

    return 0;
}

void WMSurfaceIllustrator::connectors()
{
    // this input contains the triangle data
    m_meshInput = WModuleInputData< WTriangleMesh >::createAndAdd( shared_from_this(), "mesh", "The mesh to display" );

    // this input provides an additional map from vertex ID to color. This is especially useful for using the trimesh renderer in conjunction
    // with  clustering mechanisms and so on
    m_colorMapInput = WModuleInputData< WColoredVertices >::createAndAdd( shared_from_this(), "colorMap", "The special colors" );

    // call WModule's initialization
    WModule::connectors();
}

void WMSurfaceIllustrator::properties()
{
    m_nbTriangles = m_infoProperties->addProperty( "Triangles", "The number of triangles in the mesh.", 0 );
    m_nbTriangles->setMax( std::numeric_limits< int >::max() );

    m_nbVertices = m_infoProperties->addProperty( "Vertices", "The number of vertices in the mesh.", 0 );
    m_nbVertices->setMax( std::numeric_limits< int >::max() );

    // some properties need to trigger an update
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    // setup all the properties. See header file for their meaning and purpose.
    m_showOutline = m_properties->addProperty( "Outline", "Show all edges of the trinagulation as lines.", false, m_propCondition );

    m_coloringGroup = m_properties->addPropertyGroup( "Coloring", "Coloring options and colormap options." );

    m_opacity = m_coloringGroup->addProperty( "Opacity %", "Opaqueness of surface.", 100.0 );
    m_opacity->setMin( 0.0 );
    m_opacity->setMax( 100.0 );

    // Allow the user to select different color modes
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
    m_parameterCenter = m_coloringGroup->addProperty( "Parameter center", "Parameter center selection", 0.5 );
    m_parameterCenter->setMin( -2.0 );
    m_parameterCenter->setMax( 2.0 );
    m_parameterWidth = m_coloringGroup->addProperty( "Parameter width", "Parameter width selection", 0.1 );
    m_parameterWidth->setMin( 0.0 );
    m_parameterWidth->setMax( 2.0 );

    // Allow the user to select different rendering modes
    boost::shared_ptr< WItemSelection > illustrationModes( boost::shared_ptr< WItemSelection >( new WItemSelection() ) );
    illustrationModes->addItem( "None", "Standard rendering." );
    illustrationModes->addItem( "X-slab", "Slab normal to x-axis." );
    illustrationModes->addItem( "Stream ribbon", "A range of streamlines (if red color represents s-parameter of surface)." );
    illustrationModes->addItem( "Time ribbon", "A range timelines (if green color represents t-parameter of surface)." );
    illustrationModes->addItem( "View-dependent opacity", "Less opaque where surface normal parallel to view direction." );
    illustrationModes->addItem( "View-dependent opacity (inverted)", "More opqaue where surface normal parallel to view direction." );
    m_illustrationMode = m_coloringGroup->addProperty( "Illustration mode",
                                                "Choose one of the types of illustrating the surface.",
                                                illustrationModes->getSelectorFirst(),
                                                m_propCondition );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_illustrationMode );


    // call WModule's initialization
    WModule::properties();
}

void WMSurfaceIllustrator::moduleMain()
{
    // let the main loop awake if the data changes.
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_meshInput->getDataChangedCondition() );
    m_moduleState.add( m_colorMapInput->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    // signal ready state. The module is now ready to be used.
    ready();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    // setup the main graphics-node:
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    // create a OSG node, which will contain the triangle data and allows easy transformations:
    m_moduleNode = new WGEManagedGroupNode( m_active );

    osg::ref_ptr<osg::LightModel> lightModel( new osg::LightModel() );
    osg::ref_ptr<osg::StateSet> moduleNodeState = m_moduleNode->getOrCreateStateSet();
    lightModel->setTwoSided( true );
    moduleNodeState->setAttributeAndModes( lightModel.get(), osg::StateAttribute::ON );

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_moduleNode );

    // load the GLSL shader:
    m_shader = new WGEShader( "WMSurfaceIllustrator", m_localPath );
    m_colorMapTransformation = new osg::Uniform( "u_colorMapTransformation", osg::Matrixd::identity() );
    m_shader->addPreprocessor( WGEShaderPreprocessor::SPtr(
        new WGEShaderPropertyDefineOptions< WPropBool >( m_colormap, "COLORMAPPING_DISABLED", "COLORMAPPING_ENABLED" ) )
    );


    // set the opacity and material color property as GLSL uniforms:
    moduleNodeState->addUniform( new WGEPropertyUniform< WPropDouble >( "u_opacity", m_opacity ) );

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
        boost::shared_ptr< WTriangleMesh > mesh = m_meshInput->getData();
        if( !mesh )
        {
            debugLog() << "Invalid Data. Disabling.";
            continue;
        }

        renderMesh( mesh );
    }

    // it is important to always remove the modules again
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_moduleNode );
}



void WMSurfaceIllustrator::renderMesh( boost::shared_ptr< WTriangleMesh > mesh )
{
    boost::shared_ptr< WColoredVertices > colorMap = m_colorMapInput->getData();

    m_nbTriangles->set( mesh->triangleSize() );
    m_nbVertices->set( mesh->vertSize() );

    // prepare the geometry node
    debugLog() << "Start rendering Mesh";
    osg::ref_ptr< osg::Geometry > geometry;
    osg::ref_ptr< osg::Geode > geode( new osg::Geode );
    geode->getOrCreateStateSet()->addUniform( m_colorMapTransformation );
    geode->getOrCreateStateSet()->addUniform( new WGEPropertyUniform< WPropDouble >( "u_parameterCenter", m_parameterCenter ) );
    geode->getOrCreateStateSet()->addUniform( new WGEPropertyUniform< WPropDouble >( "u_parameterWidth", m_parameterWidth ) );
    WItemSelector illustrationModeSelector =  m_illustrationMode->get( true );
    int illustrationTypeId = static_cast<int>( illustrationModeSelector.getItemIndexOfSelected( 0 ) );
    geode->getOrCreateStateSet()->addUniform( new osg::Uniform( "u_renderingType", illustrationTypeId ) );

    // apply shader only to mesh
    m_shader->apply( geode );

    // get the middle point of the mesh
    std::vector< size_t >triangles = mesh->getTriangles();
    std::vector< size_t >::const_iterator trianglesIterator;
    double minX = wlimits::MAX_DOUBLE;
    double minY = wlimits::MAX_DOUBLE;
    double minZ = wlimits::MAX_DOUBLE;


    double maxX = wlimits::MIN_DOUBLE;
    double maxY = wlimits::MIN_DOUBLE;
    double maxZ = wlimits::MIN_DOUBLE;

    for( trianglesIterator = triangles.begin();
         trianglesIterator != triangles.end();
         trianglesIterator++ )
    {
        osg::Vec3d vectorX = mesh->getVertex( *trianglesIterator );
        updateMinMax( minX, maxX, minY, maxY, minZ, maxZ, vectorX );
    }

    // start rendering
    boost::shared_ptr< WProgress > progress( new WProgress( "Rendering", 3 ) );
    m_progress->addSubProgress( progress );

    ++*progress;

    // now create the mesh but handle the color mode properly
    if( m_showOutline->get( true ) )
    {
        geometry = wge::convertToOsgGeometryLines( mesh, m_color->get(), false );
    }
    else
    {
        WItemSelector colorModeSelector = m_colorMode->get( true );
        if( colorModeSelector.getItemIndexOfSelected( 0 ) == 0 )
        {
            // use single color
            geometry = wge::convertToOsgGeometry( mesh, m_color->get(), true, true, false );
        }
        else if( colorModeSelector.getItemIndexOfSelected( 0 ) == 1 )
        {
            // take color from mesh
            geometry = wge::convertToOsgGeometry( mesh,  m_color->get(), true, true, true );
        }
        else
        {
            // take color from map
            if( colorMap )
            {
                geometry = wge::convertToOsgGeometry( mesh, *colorMap, m_color->get(), true, true );
            }
            else
            {
                warnLog() << "External colormap not connected. Using default color.";
                geometry = wge::convertToOsgGeometry( mesh, m_color->get(), true, true, false );
            }
        }
    }

    WGEColormapping::apply( geode, m_shader );

    // done. Set the new drawable
    geode->addDrawable( geometry );
    m_moduleNode->clear();
    m_moduleNode->insert( geode );
    debugLog() << "Rendering Mesh done";
    progress->finish();
}
