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

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Material>
#include <osg/StateSet>
#include <osg/StateAttribute>
#include <osg/PolygonMode>
#include <osg/LightModel>
#include <osgDB/WriteFile>

#include "../../kernel/WKernel.h"

#include "../../common/math/WMatrix.h"
#include "../../common/math/WVector3D.h"

#include "WSphereCreator.h"
#include "WMGeometryGlyphs.h"

WMGeometryGlyphs::WMGeometryGlyphs():
    WModule(),
    m_moduleNode( new WGEGroupNode() )
{
}

WMGeometryGlyphs::~WMGeometryGlyphs()
{
    // Cleanup!
    removeConnectors();
}

boost::shared_ptr< WModule > WMGeometryGlyphs::factory() const
{
    // See "src/modules/template/" for an extensively documented example.
    return boost::shared_ptr< WModule >( new WMGeometryGlyphs() );
}

const std::string WMGeometryGlyphs::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "GeometryGlyphs";
}

const std::string WMGeometryGlyphs::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    // See "src/modules/template/" for an extensively documented example.
    return "Someone should add some documentation here.";
}

void WMGeometryGlyphs::connectors()
{
    // Put the code for your connectors here. See "src/modules/template/" for an extensively documented example.
//    m_input = boost::shared_ptr< WModuleInputData < WDataSetVector > >(
//            new WModuleInputData< WDataSetVector >( shared_from_this(), "in", "Dataset" ) );

    m_input = boost::shared_ptr< WModuleInputData< WDataSetSingle > >( new WModuleInputData< WDataSetSingle >( shared_from_this(),
                    "tensor glpyhes input tensor field", "An input set of 2nd-order tensors on a regular 3d-grid." )
                );

    addConnector( m_input );
    // call WModules initialization
    WModule::connectors();
}

void WMGeometryGlyphs::properties()
{
    m_xPos           = m_properties->addProperty( "X Pos of the slice", "Description.", 80 );
    m_yPos           = m_properties->addProperty( "Y Pos of the slice", "Description.", 100 );
    m_zPos           = m_properties->addProperty( "Z Pos of the slice", "Description.", 80 );

    m_xPos->setHidden( true );
    m_yPos->setHidden( true );
    m_zPos->setHidden( true );

    m_showonX        = m_properties->addProperty( "Show Sagittal", "Show vectors on sagittal slice.", true );
    m_showonY        = m_properties->addProperty( "Show Coronal", "Show vectors on coronal slice.", true );
    m_showonZ        = m_properties->addProperty( "Show Axial", "Show vectors on axial slice.", true );

    m_xPos->setMin( 0 );
    m_xPos->setMax( 160 );
    m_yPos->setMin( 0 );
    m_yPos->setMax( 200 );
    m_zPos->setMin( 0 );
    m_zPos->setMax( 160 );
}

void WMGeometryGlyphs::moduleMain()
{
    // use the m_input "data changed" flag
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );

    // signal ready state
    ready();

    // now, to watch changing/new textures use WSubject's change condition
    //boost::signals2::connection con = WDataHandler::getDefaultSubject()->getChangeCondition()->subscribeSignal(
    //boost::bind( &WMMarchingCubes::notifyTextureChange, this ) );

    // loop until the module container requests the module to quit
    while ( !m_shutdownFlag() )
    {
        // acquire data from the input connector
        m_dataSet = m_input->getData();
        if ( !m_dataSet.get() )
        {
            // OK, the output has not yet sent data
            // NOTE: see comment at the end of this while loop for m_moduleState
            debugLog() << "Waiting for data ...";
            m_moduleState.wait();
            continue;
        }

        // do something here
        prepareGlyphes();
        //renderMesh();

        // this waits for m_moduleState to fire. By default, this is only the m_shutdownFlag condition.
        // NOTE: you can add your own conditions to m_moduleState using m_moduleState.add( ... )
        m_moduleState.wait();
    }
}

void WMGeometryGlyphs::prepareGlyphes()
{
    WSphereCreator* sc = new WSphereCreator();
    boost::shared_ptr< WTriangleMesh2 > triMesh( new WTriangleMesh2( 0, 0 ) );

    debugLog() << "Calculating Glyphes";

//    boost::shared_ptr<WTriangleMesh2> sphere1 = sc->createSphere( 0, 15, 50, 20, 50 );
//    boost::shared_ptr<WTriangleMesh2> sphere2 = sc->createSphere( 1, 15, 50, 60, 50 );
//    boost::shared_ptr<WTriangleMesh2> sphere3 = sc->createSphere( 2, 15, 50, 100, 50 );
//    boost::shared_ptr<WTriangleMesh2> sphere4 = sc->createSphere( 3, 15, 50, 140, 50 );
//    triMesh->addMesh( sphere1 );
//    triMesh->addMesh( sphere2 );
//    triMesh->addMesh( sphere3 );
//    triMesh->addMesh( sphere4 );

    wmath::WPosition current = WKernel::getRunningKernel()->getSelectionManager()->getCrosshair()->getPosition();

    int xPos = current[0];
    int yPos = current[1];
    int zPos = current[2];

    m_xPos->set( xPos );
    m_yPos->set( yPos );
    m_zPos->set( zPos );

    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( m_dataSet->getGrid() );
    boost::shared_ptr< WValueSet< float > > vals = boost::shared_dynamic_cast< WValueSet<float> >( m_dataSet->getValueSet() );

    m_xPos->setMax( grid->getNbCoordsX() );
    m_yPos->setMax( grid->getNbCoordsY() );
    m_zPos->setMax( grid->getNbCoordsZ() );


    int maxX = m_xPos->getMax()->getMax();
    int maxY = m_yPos->getMax()->getMax();
    //int maxZ = m_zPos->getMax()->getMax();

    wmath::WMatrix< float > m( 3, 3 );

    boost::shared_ptr<WTriangleMesh2> sphere = sc->createSphere( 1 );
    if ( m_showonZ->get() )
    {
        boost::shared_ptr< WProgress > progress = boost::shared_ptr< WProgress >( new WProgress( "GeometryGlyphs", maxX ) );
        m_progress->addSubProgress( progress );

        for ( int x = 0; x < maxX; ++x )
        {
            ++*progress;
            for ( int y = 0; y < maxY; ++y )
            {
                size_t id = ( x + y * maxX + zPos * maxX * maxY ) * 6;

                m( 0, 0 ) = vals->getScalar( id );

                if ( m( 0, 0 ) > 0.000000001 )
                {
                    m( 0, 1 ) = vals->getScalar( id + 1 );
                    m( 0, 2 ) = vals->getScalar( id + 2 );

                    m( 1, 0 ) = vals->getScalar( id + 1 );
                    m( 1, 1 ) = vals->getScalar( id + 3 );
                    m( 1, 2 ) = vals->getScalar( id + 4 );

                    m( 2, 0 ) = vals->getScalar( id + 2 );
                    m( 2, 1 ) = vals->getScalar( id + 4 );
                    m( 2, 2 ) = vals->getScalar( id + 5 );


                    boost::shared_ptr<WTriangleMesh2> sphere1 = sc->createSphere( 1, 400 );
                    for ( size_t i = 0; i < sphere1->vertSize(); ++i )
                    {
                        osg::Vec3 v = sphere1->getVertex( i );

                        float v0 = v[0];
                        float v1 = v[1];
                        float v2 = v[2];

                        v[0] = m( 0, 0 ) * v0 + m( 0, 1 ) * v1 + m( 0, 2 ) * v2;
                        v[1] = m( 1, 0 ) * v0 + m( 1, 1 ) * v1 + m( 1, 2 ) * v2;
                        v[2] = m( 2, 0 ) * v0 + m( 2, 1 ) * v1 + m( 2, 2 ) * v2;

                        sphere1->setVertex( i, v );
                    }
                    triMesh->addMesh( sphere1, x + .5, y + .5, zPos + .5 );
                }
            }
        }
        progress->finish();
    }
    std::cout << triMesh->vertSize() << ":" << triMesh->triangleSize() << std::endl;

    renderMesh( triMesh );
    m_triMesh = triMesh;
}

void WMGeometryGlyphs::renderMesh( boost::shared_ptr< WTriangleMesh2 > mesh )
{
    m_moduleNode->remove( m_outputGeode );
    osg::Geometry* surfaceGeometry = new osg::Geometry();
    m_outputGeode = osg::ref_ptr< osg::Geode >( new osg::Geode );

    m_outputGeode->setName( "glyphes" );

    surfaceGeometry->setVertexArray( mesh->getVertexArray() );

    // ------------------------------------------------
    // normals
    surfaceGeometry->setNormalArray( mesh->getVertexNormalArray() );
    surfaceGeometry->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );

    // ------------------------------------------------
    // colors
    //osg::Vec4Array* colors = new osg::Vec4Array;
    //colors->push_back( osg::Vec4( .9f, .9f, 0.9f, 1.0f ) );
    surfaceGeometry->setColorArray( mesh->getVertexColorArray() );
    surfaceGeometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );

    osg::DrawElementsUInt* surfaceElement = new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES, 0 );

    std::vector< size_t >tris = mesh->getTriangles();
    surfaceElement->reserve( tris.size() );

    for( unsigned int vertId = 0; vertId < tris.size(); ++vertId )
    {
        surfaceElement->push_back( tris[vertId] );
    }
    surfaceGeometry->addPrimitiveSet( surfaceElement );
    m_outputGeode->addDrawable( surfaceGeometry );

    osg::StateSet* state = m_outputGeode->getOrCreateStateSet();
    osg::ref_ptr<osg::LightModel> lightModel = new osg::LightModel();
    lightModel->setTwoSided( true );
    state->setAttributeAndModes( lightModel.get(), osg::StateAttribute::ON );
    state->setMode(  GL_BLEND, osg::StateAttribute::ON );

//    {
//        osg::ref_ptr< osg::Material > material = new osg::Material();
//        material->setDiffuse(   osg::Material::FRONT, osg::Vec4( 1.0, 1.0, 1.0, 1.0 ) );
//        material->setSpecular(  osg::Material::FRONT, osg::Vec4( 0.0, 0.0, 0.0, 1.0 ) );
//        material->setAmbient(   osg::Material::FRONT, osg::Vec4( 0.1, 0.1, 0.1, 1.0 ) );
//        material->setEmission(  osg::Material::FRONT, osg::Vec4( 0.0, 0.0, 0.0, 1.0 ) );
//        material->setShininess( osg::Material::FRONT, 25.0 );
//        state->setAttribute( material );
//    }

    m_moduleNode->insert( m_outputGeode );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_moduleNode );
}
