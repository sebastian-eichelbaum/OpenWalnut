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
#include <vector>

#include <osg/LightModel>

#include "../../graphicsEngine/WGEUtils.h"
#include "../../kernel/WKernel.h"
#include "fibernavigator/SurfaceLIC.h"
#include "WMLIC.h"

WMLIC::WMLIC()
    : WModule(),
      m_moduleNode( new WGEGroupNode() ),
      m_surfaceGeode( 0 )
{
}

WMLIC::~WMLIC()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMLIC::factory() const
{
    return boost::shared_ptr< WModule >( new WMLIC() );
}

const std::string WMLIC::getName() const
{
    return "LIC";
}

const std::string WMLIC::getDescription() const
{
    return "Line integrate convolution on TriangleMesh with a WDataSetVector";
}

void WMLIC::connectors()
{
    m_meshIC = boost::shared_ptr< WModuleInputData < WTriangleMesh2 > >(
            new WModuleInputData< WTriangleMesh2 >( shared_from_this(),
                "inMesh", "The triangle mesh used for painting the LIC" )
            );
    m_vectorIC = boost::shared_ptr< WModuleInputData < WDataSetVector > >(
            new WModuleInputData< WDataSetVector >( shared_from_this(),
                "inVectorDS", "The vectors used for computing the Streamlines used for the LIC" )
            );
//    m_meshOC = boost::shared_ptr< WModuleOutputData < WTriangleMesh2 > >(
//            new WModuleOutputData< WTriangleMesh2 >( shared_from_this(),
//                "outMesh", "The LIC" )
//            );

    addConnector( m_meshIC );
    addConnector( m_vectorIC );
//    addConnector( m_meshOC );
    WModule::connectors();
}

void WMLIC::properties()
{
}

void WMLIC::renderMesh( boost::shared_ptr< WTriangleMesh2 > mesh )
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
    surfaceGeometry->setColorArray( mesh->getTriangleColors() );
    surfaceGeometry->setColorBinding( osg::Geometry::BIND_PER_PRIMITIVE );

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
    m_shader = osg::ref_ptr< WShader > ( new WShader( "licMeshRenderer" ) );
    m_shader->apply( m_surfaceGeode );

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_moduleNode );
}

void WMLIC::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_meshIC->getDataChangedCondition() );
    m_moduleState.add( m_vectorIC->getDataChangedCondition() );

    ready();

    while ( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        if ( m_shutdownFlag() )
        {
            break;
        }

        boost::shared_ptr< WTriangleMesh2 > newMesh = m_meshIC->getData();
        boost::shared_ptr< WDataSetVector > newVector = m_vectorIC->getData();
        bool dataChanged = ( m_inMesh != newMesh ) || ( m_inVector != newVector );
        bool dataValid   = ( newMesh.get() && newVector.get() );

        if ( dataChanged && dataValid )
        {
            debugLog() << "Received Data.";
            m_inMesh = newMesh;
            m_inVector = newVector;
            m_inMesh->doLoopSubD();
            m_inMesh->doLoopSubD();
            SurfaceLIC lic( m_inVector, m_inMesh );
            lic.execute();
            lic.updateMeshColor( m_inMesh );
//            m_meshOC.updateData( m_inMesh );
            renderMesh( m_inMesh );
        }
    }
}
