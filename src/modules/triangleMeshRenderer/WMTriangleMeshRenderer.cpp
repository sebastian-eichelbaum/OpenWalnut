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
    m_mainComponentOnly = m_properties->addProperty( "Main Component", "Main component only", false, m_propCondition );
    m_opacity = m_properties->addProperty( "Opacity %", "Opaqueness of surface.", 100.0 );
    m_opacity->setMin( 0.0 );
    m_opacity->setMax( 100.0 );

    m_externalColormapGroup = m_properties->addPropertyGroup( "External ColorMap.", "All the properties for the external colormap." );

    m_externalDefaultColor = m_externalColormapGroup->addProperty( "Default Color", "The color where no mapping is defined.",
                                                        WColor( .9f, .9f, 0.9f, 1.0f ), m_propCondition );

    m_useExternalColormap = m_externalColormapGroup->addProperty( "External Colormap", "If enabled, the renderer uses the external colormap "
                                    "specified on the input connector \"colorMap\". If not, it uses the mesh colors.", false, m_propCondition );

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
    WGEManagedGroupNode::SPtr moduleNode( new WGEManagedGroupNode( m_active ) );
    osg::StateSet* moduleNodeState = moduleNode->getOrCreateStateSet();
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( moduleNode );

    // load the GLSL shader:
    m_shader = osg::ref_ptr< WGEShader > ( new WGEShader( "WMTriangleMeshRenderer", m_localPath ) );

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

        boost::shared_ptr< WProgress > progress = boost::shared_ptr< WProgress >( new WProgress( "Rendering", 3 ) );
        m_progress->addSubProgress( progress );

        // prepare the geometry node
        debugLog() << "Start rendering Mesh";
        osg::ref_ptr< osg::Geometry > geometry;
        osg::ref_ptr< osg::Geode > geode( new osg::Geode );
        if( m_mainComponentOnly->get( true ) )
        {
            // component decomposition
            debugLog() << "Start mesh decomposition";
            boost::shared_ptr< std::list< boost::shared_ptr< WTriangleMesh > > > m_components = tm_utils::componentDecomposition( *mesh );
            mesh = *std::max_element( m_components->begin(), m_components->end(), WMeshSizeComp() );
            debugLog() << "Decomposing mesh done";
        }
        ++*progress;

        // Should the colorMap be enabled?
        if( m_useExternalColormap->get() && colorMap )
        {
            // this simply converts our triangle mesh to an OSG geometry.
            geometry = wge::convertToOsgGeometry( mesh, *colorMap, m_externalDefaultColor->get(), true, true );
        }
        else if( m_useExternalColormap->get() )
        {
            warnLog() << "External colormap not connected.";
            geometry = wge::convertToOsgGeometry( mesh, true, true );
        }
        else
        {
            geometry = wge::convertToOsgGeometry( mesh, true, true );
        }
        ++*progress;

        // done. Set the new drawable
        geode->addDrawable( geometry );
        moduleNode->clear();
        moduleNode->insert( geode );
        debugLog() << "Rendering Mesh done";
        ++*progress;
        progress->finish();
    }

    // it is important to always remove the modules again
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( moduleNode );
}

