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

#include "WMTriangleMeshRenderer.h"

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
    m_input = boost::shared_ptr< WModuleInputData < WTriangleMesh  > >(
        new WModuleInputData< WTriangleMesh >( shared_from_this(), "in", "The mesh to display" )
        );

    addConnector( m_input );

    // call WModules initialization
    WModule::connectors();
}

void WMTriangleMeshRenderer::properties()
{
    // Put the code for your properties here. See "src/modules/template/" for an extensively documented example.
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
        boost::shared_ptr< WTriangleMesh > mesh = m_input->getData();
        if ( !mesh )
        {
            debugLog() << "Invalid Data. Disabling.";
            continue;
        }

        renderMesh( mesh );
    }
}

void WMTriangleMeshRenderer::renderMesh( boost::shared_ptr< WTriangleMesh > mesh )
{
    m_moduleNode->remove( m_surfaceGeode );
    osg::Geometry* surfaceGeometry = new osg::Geometry();
    m_surfaceGeode = osg::ref_ptr< osg::Geode >( new osg::Geode );

    osg::Vec3Array* vertices = new osg::Vec3Array;
    for( size_t i = 0; i < mesh->getNumVertices(); ++i )
    {
        wmath::WPosition vertPos;
        vertPos = mesh->getVertex( i );
        vertices->push_back( osg::Vec3( vertPos[0], vertPos[1], vertPos[2] ) );
    }
    surfaceGeometry->setVertexArray( vertices );

    osg::DrawElementsUInt* surfaceElement;

    surfaceElement = new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES, 0 );
    for( unsigned int triId = 0; triId < mesh->getNumTriangles(); ++triId )
    {
        for( unsigned int vertId = 0; vertId < 3; ++vertId )
        {
            surfaceElement->push_back( mesh->getTriangleVertexId( triId, vertId ) );
        }
    }
    surfaceGeometry->addPrimitiveSet( surfaceElement );

    // ------------------------------------------------
    // normals
    osg::ref_ptr< osg::Vec3Array> normals( new osg::Vec3Array() );

    mesh->computeVertNormals(); // time consuming
    for( unsigned int vertId = 0; vertId < mesh->getNumVertices(); ++vertId )
    {
        wmath::WVector3D tmpNormal = mesh->getVertexNormal( vertId );
        normals->push_back( osg::Vec3( tmpNormal[0], tmpNormal[1], tmpNormal[2] ) );
    }
    surfaceGeometry->setNormalArray( normals.get() );
    surfaceGeometry->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );

    m_surfaceGeode->addDrawable( surfaceGeometry );
    osg::StateSet* state = m_surfaceGeode->getOrCreateStateSet();

    // ------------------------------------------------
    // colors
    osg::Vec4Array* colors = new osg::Vec4Array;

    colors->push_back( osg::Vec4( .9f, .9f, 0.9f, 1.0f ) );
    surfaceGeometry->setColorArray( colors );
    surfaceGeometry->setColorBinding( osg::Geometry::BIND_OVERALL );

    osg::ref_ptr<osg::LightModel> lightModel = new osg::LightModel();
    lightModel->setTwoSided( true );
    state->setAttributeAndModes( lightModel.get(), osg::StateAttribute::ON );
    state->setMode(  GL_BLEND, osg::StateAttribute::ON  );


    m_moduleNode->insert( m_surfaceGeode );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_moduleNode );
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

