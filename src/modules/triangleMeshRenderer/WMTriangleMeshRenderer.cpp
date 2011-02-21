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

#include <osg/LightModel>

#include "../../graphicsEngine/WGEUtils.h"
#include "../../graphicsEngine/WTriangleMesh.h"
#include "../../kernel/WKernel.h"
#include "WMTriangleMeshRenderer.xpm"
#include "WMTriangleMeshRenderer.h"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMTriangleMeshRenderer )

WMTriangleMeshRenderer::WMTriangleMeshRenderer():
    WModule(),
    m_moduleNode( new WGEGroupNode() ),
    m_surfaceGeode( 0 )
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
    return trianglemeshrenderer_xpm;
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
    m_meshInput = boost::shared_ptr< WModuleInputData < WTriangleMesh > >(
        new WModuleInputData< WTriangleMesh >( shared_from_this(), "mesh", "The mesh to display" )
        );

    addConnector( m_meshInput );

    m_colorMapInput = boost::shared_ptr< WModuleInputData < WColoredVertices > >(
        new WModuleInputData< WColoredVertices >( shared_from_this(), "colorMap", "The special colors" )
        );

    addConnector( m_colorMapInput );

    // call WModules initialization
    WModule::connectors();
}

void WMTriangleMeshRenderer::properties()
{
    m_meshColor   = m_properties->addProperty( "Mesh Color", "Color of the mesh.", WColor( .9f, .9f, 0.9f, 1.0f ) );
    m_mainComponentOnly = m_properties->addProperty( "Main Component", "Main component only", false );
    m_opacityProp = m_properties->addProperty( "Opacity %", "Opaqueness of surface.", 100 );
    m_opacityProp->setMin( 0 );
    m_opacityProp->setMax( 100 );

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
    // let the main loop awake if the data changes or the properties changed.
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_meshInput->getDataChangedCondition() );
    m_moduleState.add( m_colorMapInput->getDataChangedCondition() );
    m_moduleState.add( m_meshColor->getCondition() );
    m_moduleState.add( m_mainComponentOnly->getCondition() );

    // signal ready state
    ready();


    // loop until the module container requests the module to quit
    while( !m_shutdownFlag() )
    {
        // Now, the moduleState variable comes into play. The module can wait for the condition, which gets fired whenever the input receives data
        // or an property changes. The main loop now waits until something happens.
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        // woke up since the module is requested to finish
        if ( m_shutdownFlag() )
        {
            break;
        }
        // invalid data
        boost::shared_ptr< WTriangleMesh > mesh = m_meshInput->getData();
        if ( !mesh )
        {
            debugLog() << "Invalid Data. Disabling.";
            continue;
        }
        debugLog() << "Start rendering Mesh";
        if( m_mainComponentOnly->get( true ) )
        {
            debugLog() << "Start mesh decomposition";
            boost::shared_ptr< std::list< boost::shared_ptr< WTriangleMesh > > > m_components = tm_utils::componentDecomposition( *mesh );
            debugLog() << "Decomposing mesh done";
            renderMesh( *std::max_element( m_components->begin(), m_components->end(), WMeshSizeComp() ) );
        }
        else
        {
            renderMesh( mesh );
        }
        debugLog() << "Rendering Mesh done";
    }
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_moduleNode );
}

void WMTriangleMeshRenderer::renderMesh( boost::shared_ptr< WTriangleMesh > mesh )
{
    m_moduleNode->remove( m_surfaceGeode );
    osg::Geometry* surfaceGeometry = new osg::Geometry();
    m_surfaceGeode = osg::ref_ptr< osg::Geode >( new osg::Geode );

    surfaceGeometry->setVertexArray( mesh->getVertexArray() );

    osg::DrawElementsUInt* surfaceElement;

    surfaceElement = new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES, 0 );

    std::vector< size_t > tris = mesh->getTriangles();
    surfaceElement->reserve( tris.size() );

    for( unsigned int vertId = 0; vertId < tris.size(); ++vertId )
    {
        surfaceElement->push_back( tris[vertId] );
    }
    surfaceGeometry->addPrimitiveSet( surfaceElement );

    // ------------------------------------------------
    // normals
    surfaceGeometry->setNormalArray( mesh->getVertexNormalArray() );
    surfaceGeometry->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );

    m_surfaceGeode->addDrawable( surfaceGeometry );
    osg::StateSet* state = m_surfaceGeode->getOrCreateStateSet();


    // ------------------------------------------------
    // colors
    osg::ref_ptr< osg::Vec4Array > colors   = osg::ref_ptr< osg::Vec4Array >( new osg::Vec4Array );
    boost::shared_ptr< WColoredVertices > colorMap = m_colorMapInput->getData();

    if( !colorMap.get() || m_meshColor->changed() )
    {
        debugLog() << "No Color Map found, using a single color";
        colors->push_back( m_meshColor->get( true ) );
        surfaceGeometry->setColorArray( colors );
        surfaceGeometry->setColorBinding( osg::Geometry::BIND_OVERALL );
    }
    else
    {
        debugLog() << "Color Map found... using it";
        for( size_t i = 0; i < mesh->vertSize(); ++i )
        {
            colors->push_back( m_meshColor->get() );
        }
        for( std::map< size_t, WColor >::const_iterator vc = colorMap->getData().begin(); vc != colorMap->getData().end(); ++vc )
        {
            // ATTENTION: the colormap might not be available and hence an old one, but the new mesh might have triggered the update
            if( vc->first < colors->size() )
            {
                colors->at( vc->first ) = vc->second;
            }
        }

        surfaceGeometry->setColorArray( colors );
        surfaceGeometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );
    }

    osg::ref_ptr<osg::LightModel> lightModel = new osg::LightModel();
    lightModel->setTwoSided( true );
    state->setAttributeAndModes( lightModel.get(), osg::StateAttribute::ON );
    state->setMode(  GL_BLEND, osg::StateAttribute::ON  );

    {
        osg::ref_ptr< osg::Material > material = new osg::Material();
        material->setDiffuse(   osg::Material::FRONT, osg::Vec4( 1.0, 1.0, 1.0, 1.0 ) );
        material->setSpecular(  osg::Material::FRONT, osg::Vec4( 0.0, 0.0, 0.0, 1.0 ) );
        material->setAmbient(   osg::Material::FRONT, osg::Vec4( 0.1, 0.1, 0.1, 1.0 ) );
        material->setEmission(  osg::Material::FRONT, osg::Vec4( 0.0, 0.0, 0.0, 1.0 ) );
        material->setShininess( osg::Material::FRONT, 25.0 );
        state->setAttribute( material );
    }

    state->addUniform( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "opacity", m_opacityProp->get( true ) ) ) );

    m_moduleNode->insert( m_surfaceGeode );
    m_shader = osg::ref_ptr< WGEShader > ( new WGEShader( "WMTriangleMeshRenderer", m_localPath ) );
    m_shader->apply( m_surfaceGeode );


    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_moduleNode );

    m_moduleNode->addUpdateCallback( new TriangleMeshRendererCallback( boost::shared_dynamic_cast< WMTriangleMeshRenderer >( shared_from_this() ) ) );
}

void WMTriangleMeshRenderer::activate()
{
    // Draw/hide the surface
    if ( m_moduleNode )
    {
        if ( m_active->get() )
        {
            m_moduleNode->setNodeMask( 0xFFFFFFFF );
        }
        else
        {
            m_moduleNode->setNodeMask( 0x0 );
        }
    }

    // Always call WModule's activate!
    WModule::activate();
}


void WMTriangleMeshRenderer::update()
{
    if( m_opacityProp->changed() )
    {
        osg::StateSet* rootState = m_surfaceGeode->getOrCreateStateSet();
        rootState->addUniform( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "opacity", m_opacityProp->get( true ) ) ) );
    }
}
