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

#include <list>
#include <map>
#include <string>
#include <vector>

#include <osg/Geode>

#include "core/graphicsEngine/WGEManagedGroupNode.h"
#include "core/graphicsEngine/WGEUtils.h"
#include "core/graphicsEngine/WTriangleMesh.h"
#include "core/graphicsEngine/WGEGeodeUtils.h"
#include "core/graphicsEngine/shaders/WGEShader.h"
#include "core/kernel/WKernel.h"
#include "core/graphicsEngine/WGEColormapping.h"
#include "core/common/math/WMath.h"
#include "core/common/WLimits.h"
#include "core/graphicsEngine/shaders/WGEShaderPropertyDefineOptions.h"
#include "core/graphicsEngine/shaders/WGEPropertyUniform.h"


#include "WMTriangleMeshRenderer.xpm"
#include "WMTriangleMeshRenderer.h"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMTriangleMeshRenderer )

WMTriangleMeshRenderer::WMTriangleMeshRenderer():
    WModule()
{
}

WMTriangleMeshRenderer::~WMTriangleMeshRenderer()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMTriangleMeshRenderer::factory() const
{
    // See "src/modules/template/" for an extensively documented example.
    return boost::shared_ptr< WModule >( new WMTriangleMeshRenderer() );
}

const char** WMTriangleMeshRenderer::getXPMIcon() const
{
    // just return the icon
    return WMTriangleMeshRenderer_xpm;
}

const std::string WMTriangleMeshRenderer::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "Triangle Mesh Renderer";
}

const std::string WMTriangleMeshRenderer::getDescription() const
{
    return "Takes a triangle mesh as input and renders it as a shaded surface.";
}

void WMTriangleMeshRenderer::updateMinMax( double& minX, double& maxX, // NOLINT
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

double WMTriangleMeshRenderer::getMedian( double x, double y, double z ) const
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

double WMTriangleMeshRenderer::getIntervallCenterMiddle( double min, double max ) const
{
    return min + ( max - min) / 2;
}

void WMTriangleMeshRenderer::connectors()
{
    // this input contains the triangle data
    m_meshInput = WModuleInputData< WTriangleMesh >::createAndAdd( shared_from_this(), "mesh", "The mesh to display" );

    // this input provides an additional map from vertex ID to color. This is especially useful for using the trimesh renderer in conjunction
    // with  clustering mechanisms and so on
    m_colorMapInput = WModuleInputData< WColoredVertices >::createAndAdd( shared_from_this(), "colorMap", "The special colors" );

    // call WModule's initialization
    WModule::connectors();
}

void WMTriangleMeshRenderer::properties()
{
    // some properties need to trigger an update
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    // setup all the properties. See header file for their meaning and purpose.
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

    m_groupTransformation = m_properties->addPropertyGroup( "Transformation",  "A group which contains transformation tools." );

    //Scaling
    m_scale = m_groupTransformation->addProperty( "Scale whole surface?", "The whole surface will be scaled.", false );

    m_scaleX = m_groupTransformation->addProperty( "Scale X", "Scaling X of surface.", 1.0 );
    m_scaleX->setMin( -10.0 );
    m_scaleX->setMax( 10.0 );

    m_scaleY = m_groupTransformation->addProperty( "Scale Y", "Scaling Y of surface.", 1.0 );
    m_scaleY->setMin( -10.0 );
    m_scaleY->setMax( 10.0 );

    m_scaleZ = m_groupTransformation->addProperty( "Scale Z", "Scaling Z of surface.", 1.0 );
    m_scaleZ->setMin( -10.0 );
    m_scaleZ->setMax( 10.0 );

    //Rotating
    m_rotateX = m_groupTransformation->addProperty( "Rotate X", "Rotate X in °", 0.0 );
    m_rotateX->setMin( -360.0 );
    m_rotateX->setMax( 360.0 );

    m_rotateY = m_groupTransformation->addProperty( "Rotate Y", "Rotate Y in °", 0.0 );
    m_rotateY->setMin( -360.0 );
    m_rotateY->setMax( 360.0 );

    m_rotateZ = m_groupTransformation->addProperty( "Rotate Z", "Rotate Z in °", 0.0 );
    m_rotateZ->setMin( -360.0 );
    m_rotateZ->setMax( 360.0 );

    //Translating
    m_translateX = m_groupTransformation->addProperty( "Translate X", "Translate the surface to X", 0.0 );
    m_translateX->setMin( -100.0 );
    m_translateX->setMax( 100.0 );

    m_translateY = m_groupTransformation->addProperty( "Translate Y", "Translate the surface to Y", 0.0 );
    m_translateY->setMin( -100.0 );
    m_translateY->setMax( 100.0 );

    m_translateZ = m_groupTransformation->addProperty( "Translate Z", "Translate the surface to Z", 0.0 );
    m_translateZ->setMin( -100.0 );
    m_translateZ->setMax( 100.0 );

    m_setDefault = m_groupTransformation->addProperty( "Reset to default", "Set!",
                                                        WPVBaseTypes::PV_TRIGGER_READY,
                                                        boost::bind( &WMTriangleMeshRenderer::setToDefault, this ) );

    // call WModule's initialization
    WModule::properties();
}

/**
 * Compares two WTrianglesMeshes on their size of vertices. This is private here since it makes sense only to this module ATM.
 */
struct WMeshSizeComp
{
    /**
     * Comparator on num vertex of two WTriangleMeshes
     *
     * \param m First Mesh
     * \param n Second Mesh
     *
     * \return True if and only if the first Mesh has less vertices as the second mesh.
     */
    bool operator()( const boost::shared_ptr< WTriangleMesh >& m, const boost::shared_ptr< WTriangleMesh >& n ) const
    {
        return m->vertSize() < n->vertSize();
    }
};

void WMTriangleMeshRenderer::moduleMain()
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
    osg::StateSet* moduleNodeState = m_moduleNode->getOrCreateStateSet();
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_moduleNode );

    // set the member function "updateTransformation" as callback
    WGEFunctorCallback< osg::Node >::SPtr transformationCallback(
        new WGEFunctorCallback< osg::Node >( boost::bind( &WMTriangleMeshRenderer::updateTransformation, this ) )
    );
    m_moduleNode->addUpdateCallback( transformationCallback );

    // load the GLSL shader:
    osg::ref_ptr< WGEShader > shader( new WGEShader( "WMTriangleMeshRenderer", m_localPath ) );
    m_colorMapTransformation = new osg::Uniform( "u_colorMapTransformation", osg::Matrixd::identity() );
    shader->addPreprocessor( WGEShaderPreprocessor::SPtr(
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
        boost::shared_ptr< WColoredVertices > colorMap = m_colorMapInput->getData();
        if( !mesh )
        {
            debugLog() << "Invalid Data. Disabling.";
            continue;
        }

          // prepare the geometry node
        debugLog() << "Start rendering Mesh";
        osg::ref_ptr< osg::Geometry > geometry;
        osg::ref_ptr< osg::Geode > geode( new osg::Geode );
        geode->getOrCreateStateSet()->addUniform( m_colorMapTransformation );
        geode->getOrCreateStateSet()->addUniform( new WGEPropertyUniform< WPropDouble >( "u_colormapRatio", m_colormapRatio ) );

        // apply shader only to mesh
        shader->apply( geode );

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

        m_meshCenter = WVector3d( getIntervallCenterMiddle( minX, maxX ),
                                  getIntervallCenterMiddle( minY, maxY ),
                                  getIntervallCenterMiddle( minZ, maxZ ) );

        // start rendering
        boost::shared_ptr< WProgress > progress = boost::shared_ptr< WProgress >( new WProgress( "Rendering", 3 ) );
        m_progress->addSubProgress( progress );

        if( m_mainComponentOnly->get( true ) )
        {
            // component decomposition
            debugLog() << "Start mesh decomposition";
            boost::shared_ptr< std::list< boost::shared_ptr< WTriangleMesh > > > m_components = tm_utils::componentDecomposition( *mesh );
            mesh = *std::max_element( m_components->begin(), m_components->end(), WMeshSizeComp() );
            debugLog() << "Decomposing mesh done";
        }
        ++*progress;

        // now create the mesh but handle the color mode properly
        WItemSelector s = m_colorMode->get( true );
        if( s.getItemIndexOfSelected( 0 ) == 0 )
        {
            // use single color
            geometry = wge::convertToOsgGeometry( mesh, m_color->get(), true, true, false );
        }
        else if( s.getItemIndexOfSelected( 0 ) == 1 )
        {
            // take color from mesh
            geometry = wge::convertToOsgGeometry( mesh, m_color->get(), true, true, true );
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
        ++*progress;

        WGEColormapping::apply( geode, shader );

        // done. Set the new drawable
        geode->addDrawable( geometry );
        m_moduleNode->clear();
        m_moduleNode->insert( geode );
        if( m_showCoordinateSystem->get() )
        {
            m_moduleNode->insert(
                wge::creatCoordinateSystem(
                    m_meshCenter,
                    maxX-minX,
                    maxY-minY,
                    maxZ-minZ
                )
             );
        }
        debugLog() << "Rendering Mesh done";
        ++*progress;
        progress->finish();
    }

    // it is important to always remove the modules again
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_moduleNode );
}

void WMTriangleMeshRenderer::updateTransformation()
{
    if( m_moduleNode )
    {
        if( m_scale->changed() && m_scale->get( true ) )
        {
            m_scaleX->set( getMedian( m_scaleX->get(), m_scaleY->get(), m_scaleZ->get() ) );
            m_scaleY->set( getMedian( m_scaleX->get(), m_scaleY->get(), m_scaleZ->get() ) );
            m_scaleZ->set( getMedian( m_scaleX->get(), m_scaleY->get(), m_scaleZ->get() ) );
        }
        if( m_scale->get() )
        {
            if( m_scaleX->changed() && m_scaleX->get( true ) )
            {
                m_scaleY->set( m_scaleX->get() );
                m_scaleZ->set( m_scaleX->get() );
            }
            if( m_scaleY->changed() && m_scaleY->get( true ) )
            {
                m_scaleX->set( m_scaleY->get() );
                m_scaleZ->set( m_scaleZ->get() );
            }
            if( m_scaleZ->changed() && m_scaleZ->get( true ) )
            {
                m_scaleX->set( m_scaleZ->get() );
                m_scaleY->set( m_scaleZ->get() );
            }
        }

        osg::Matrixd matrixTranslateTo0 = osg::Matrixd::translate( static_cast< osg::Vec3f >( m_meshCenter ) * -1.0 );
        osg::Matrixd matrixTranslateFrom0 = osg::Matrixd::translate( static_cast< osg::Vec3f >( m_meshCenter ) );
        osg::Matrixd matrixScale = osg::Matrixd::scale( m_scaleX->get(), m_scaleY->get(), m_scaleZ->get() );
        osg::Matrixd matrixRotateX = osg::Matrixd::rotate( m_rotateX->get() * piDouble / 180, osg::Vec3f( 1, 0, 0 ) );
        osg::Matrixd matrixRotateY = osg::Matrixd::rotate( m_rotateY->get() * piDouble / 180, osg::Vec3f( 0, 1, 0 ) );
        osg::Matrixd matrixRotateZ = osg::Matrixd::rotate( m_rotateZ->get() * piDouble / 180, osg::Vec3f( 0, 0, 1 ) );
        osg::Matrixd matrixRotate = matrixRotateX * matrixRotateY * matrixRotateZ;
        osg::Matrixd matrixTranslate = osg::Matrixd::translate( m_translateX->get(), m_translateY->get(), m_translateZ->get() );
        osg::Matrixd matrixComplete =
                                matrixTranslateTo0 *
                                matrixScale *
                                matrixTranslateFrom0 *
                                matrixTranslateTo0 *
                                matrixRotate *
                                matrixTranslateFrom0 *
                                matrixTranslate;

        m_moduleNode->setMatrix( matrixComplete );
        m_colorMapTransformation->set( matrixComplete );
    }
}

void WMTriangleMeshRenderer::setToDefault()
{
    if( m_setDefault->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
    {
        WMTriangleMeshRenderer::m_opacity->set( 100 );

        WMTriangleMeshRenderer::m_scale->set( false );
        WMTriangleMeshRenderer::m_scaleX->set( 1 );
        WMTriangleMeshRenderer::m_scaleY->set( 1 );
        WMTriangleMeshRenderer::m_scaleZ->set( 1 );

        WMTriangleMeshRenderer::m_rotateX->set( 0 );
        WMTriangleMeshRenderer::m_rotateY->set( 0 );
        WMTriangleMeshRenderer::m_rotateZ->set( 0 );

        WMTriangleMeshRenderer::m_translateX->set( 0 );
        WMTriangleMeshRenderer::m_translateY->set( 0 );
        WMTriangleMeshRenderer::m_translateZ->set( 0 );

        m_setDefault->set( WPVBaseTypes::PV_TRIGGER_READY );
    }
}

