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

#include <osg/LightModel>

#include "../../kernel/WKernel.h"
#include "../../common/datastructures/WTriangleMesh.h"
#include "../../graphicsEngine/WTriangleMesh2.h"

#include "WMTriangleMeshRenderer.h"
#include "trianglemeshrenderer.xpm"

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
    m_input = boost::shared_ptr< WModuleInputData < WTriangleMesh2  > >(
        new WModuleInputData< WTriangleMesh2 >( shared_from_this(), "in", "The mesh to display" )
        );

    addConnector( m_input );

    // call WModules initialization
    WModule::connectors();
}

void WMTriangleMeshRenderer::properties()
{
    m_opacityProp = m_properties->addProperty( "Opacity %", "Opaqueness of surface.", 100 );
    m_opacityProp->setMin( 0 );
    m_opacityProp->setMax( 100 );
}

void WMTriangleMeshRenderer::moduleMain()
{
    // let the main loop awake if the data changes or the properties changed.
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );

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
        boost::shared_ptr< WTriangleMesh2 > mesh = m_input->getData();
        if ( !mesh )
        {
            debugLog() << "Invalid Data. Disabling.";
            continue;
        }

        renderMesh( mesh );
    }
}

void WMTriangleMeshRenderer::renderMesh( boost::shared_ptr< WTriangleMesh2 > mesh )
{
    m_moduleNode->remove( m_surfaceGeode );
    osg::Geometry* surfaceGeometry = new osg::Geometry();
    m_surfaceGeode = osg::ref_ptr< osg::Geode >( new osg::Geode );

    osg::Vec3Array* vertices = new osg::Vec3Array;
    for( size_t i = 0; i < mesh->vertSize(); ++i )
    {
        // wmath::WPosition vertPos;
        // vertPos = mesh->getVertex( i );
        vertices->push_back( mesh->getVertex( i ) );
    }
    surfaceGeometry->setVertexArray( vertices );

    osg::DrawElementsUInt* surfaceElement;

    surfaceElement = new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES, 0 );
    for( unsigned int triId = 0; triId < mesh->triangleSize(); ++triId )
    {
            surfaceElement->push_back( mesh->getTriVertId0( triId ) );
            surfaceElement->push_back( mesh->getTriVertId1( triId ) );
            surfaceElement->push_back( mesh->getTriVertId2( triId ) );
    }
    surfaceGeometry->addPrimitiveSet( surfaceElement );

    // ------------------------------------------------
    // normals
    // osg::ref_ptr< osg::Vec3Array> normals( new osg::Vec3Array() );

    // mesh->computeVertNormals(); // time consuming
    // for( unsigned int vertId = 0; vertId < mesh->getNumVertices(); ++vertId )
    // {
    //     wmath::WVector3D tmpNormal = mesh->getVertexNormal( vertId );
    //     normals->push_back( osg::Vec3( tmpNormal[0], tmpNormal[1], tmpNormal[2] ) );
    // }
    surfaceGeometry->setNormalArray( mesh->getVertexNormalArray() );
    surfaceGeometry->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );

    m_surfaceGeode->addDrawable( surfaceGeometry );
    osg::StateSet* state = m_surfaceGeode->getOrCreateStateSet();

    // ------------------------------------------------
    // colors
    osg::Vec4Array* colors = new osg::Vec4Array;

    colors->push_back( osg::Vec4( .3f, .3f, 0.3f, 1.0f ) );
    surfaceGeometry->setColorArray( colors );
    surfaceGeometry->setColorBinding( osg::Geometry::BIND_OVERALL );

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

    m_moduleNode->insert( m_surfaceGeode );
    m_shader = osg::ref_ptr< WShader > ( new WShader( "WMTriangleMeshRenderer", m_localPath ) );
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
