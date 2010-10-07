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

#include "../../common/WSharedSequenceContainer.h"
#include "../../dataHandler/exceptions/WDHNoSuchSubject.h"
#include "../../dataHandler/WDataHandler.h"
#include "../../dataHandler/WSubject.h"
#include "../../graphicsEngine/WGEUtils.h"
#include "../../kernel/WKernel.h"
#include "fibernavigator/SurfaceLIC.h"
#include "WMLIC.h"
#include "WMLIC.xpm"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMLIC )

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

const char** WMLIC::getXPMIcon() const
{
    return lic_xpm;
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
    m_meshIC = boost::shared_ptr< WModuleInputData < WTriangleMesh > >(
            new WModuleInputData< WTriangleMesh >( shared_from_this(),
                "inMesh", "The triangle mesh used for painting the LIC" )
            );
// TODO(math): reenable the connector when selecting of two input in GUI is possible.
// The reason for disabling is just that you can choose the lic on a surface without project files due to its two inputs
// Alternatively the vector dataset is choosen from first subject dataset containing evec in its file name, see moduleMain
// for more informations.
//
//    m_vectorIC = boost::shared_ptr< WModuleInputData < WDataSetVector > >(
//            new WModuleInputData< WDataSetVector >( shared_from_this(),
//                "inVectorDS", "The vectors used for computing the Streamlines used for the LIC" )
//            );

// TODO(math): ATM we are unsure about providing an output. Caution the input mesh is modified!
//    m_meshOC = boost::shared_ptr< WModuleOutputData < WTriangleMesh > >(
//            new WModuleOutputData< WTriangleMesh >( shared_from_this(),
//                "outMesh", "The LIC" )
//            );

    addConnector( m_meshIC );
//    addConnector( m_vectorIC );
//    addConnector( m_meshOC );
    WModule::connectors();
}

void WMLIC::properties()
{
    WModule::properties();
}

void WMLIC::renderMesh( boost::shared_ptr< WTriangleMesh > mesh )
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
    m_shader = osg::ref_ptr< WShader > ( new WShader( "WMLIC", m_localPath ) );
    m_shader->apply( m_surfaceGeode );

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_moduleNode );
}

void WMLIC::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_meshIC->getDataChangedCondition() );
//    m_moduleState.add( m_vectorIC->getDataChangedCondition() );

    ready();

    while ( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        if ( m_shutdownFlag() )
        {
            break;
        }

        boost::shared_ptr< WTriangleMesh > newMesh = m_meshIC->getData();
// TODO(math): if the vector input is available again please enable this again too
//        boost::shared_ptr< WDataSetVector > newVector = m_vectorIC->getData();
//        bool dataChanged = ( m_inMesh != newMesh ) || ( m_inVector != newVector );
//        bool dataValid   = ( newMesh.get() && newVector.get() );

        bool dataChanged = ( m_inMesh != newMesh );
        bool dataValid   = newMesh.get();

        if ( dataChanged && dataValid )
        {
            boost::shared_ptr< WProgress > progress = boost::shared_ptr< WProgress >( new WProgress( "LIC", 5 ) );
            m_progress->addSubProgress( progress );

            debugLog() << "Received Data.";
            m_inMesh = newMesh;
//            m_inVector = newVector;
            WAssert( m_inVector = searchVectorDS(), "There was no vector dataset loaded, please load it first!" );
            ++*progress;
            m_inMesh->doLoopSubD();
            ++*progress;
            m_inMesh->doLoopSubD();
            ++*progress;
            SurfaceLIC lic( m_inVector, m_inMesh );
            lic.execute();
            lic.updateMeshColor( m_inMesh );
//            m_meshOC.updateData( m_inMesh );
            debugLog() << "Start rendering LIC";
            renderMesh( m_inMesh );
            debugLog() << "Rendering done";
            ++*progress;
            progress->finish();
        }
    }
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_moduleNode );
}

boost::shared_ptr< WDataSetVector > WMLIC::searchVectorDS() const
{
    boost::shared_ptr< WDataSetVector > result;
    boost::shared_ptr< WSubject > subject;
    try
    {
        subject =  WDataHandler::getDefaultSubject();
    }
    catch( WDHNoSuchSubject )
    {
        return result;
    }
    if( !subject )
    {
        return result;
    }

    // This lock gets unlocked if it is destroyed ( looses scope )
    WSubject::DatasetSharedContainerType::ReadTicket da = subject->getDatasets();

    for( WSubject::DatasetConstIterator it = da->get().begin(); it != da->get().end(); ++it )
    {
        if( ( *it )->isVectorDataSet() )
        {
            return ( *it )->isVectorDataSet();
        }
    }

    return result;
}

void WMLIC::activate()
{
    if( m_moduleNode )
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

    WModule::activate();
}
