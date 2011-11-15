//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV@Uni-Leipzig and CNCF@MPI-CBS
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

#include <osg/ShapeDrawable>
#include <osg/Group>
#include <osg/Geode>
#include <osg/MatrixTransform>
#include <osg/Material>
#include <osg/StateAttribute>
#include <osg/Texture2D>
#include <osgDB/ReadFile>

#include <osg/Vec3>
#include <osg/Geometry>

#include "core/kernel/WKernel.h"
#include "core/common/WColor.h"
#include "core/common/WPropertyHelper.h"
#include "core/graphicsEngine/WGEUtils.h"

#include "WMCoordinateHUD.h"
#include "WMCoordinateHUD.xpm"
#include "option_1.xpm"
#include "option_2.xpm"
#include "option_3.xpm"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMCoordinateHUD )

WMCoordinateHUD::WMCoordinateHUD():WModule()
{
}

WMCoordinateHUD::~WMCoordinateHUD()
{
}

boost::shared_ptr< WModule > WMCoordinateHUD::factory() const
{
    return boost::shared_ptr< WModule >( new WMCoordinateHUD() );
}

const char** WMCoordinateHUD::getXPMIcon() const
{
    return coordinateHUD_xpm;
}

const std::string WMCoordinateHUD::getName() const
{
    return "Coordinate HUD";
}

const std::string WMCoordinateHUD::getDescription() const
{
    return "This module is to display a coordinatesystem as HUD.";
}

void WMCoordinateHUD::connectors()
{
}

void WMCoordinateHUD::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    // list of alternatives:
    m_possibleSelections = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_possibleSelections->addItem( "colored axis", "colorfull coordinate axis", option_1_xpm );
    m_possibleSelections->addItem( "b/w axis", "black & white coordinate axis", option_2_xpm );
    m_possibleSelections->addItem( "colored cube", "colorfull coordinate cube", option_3_xpm );
    m_possibleSelections->addItem( "colored, labeled cube", "colorfull coordinate cube with labels", option_3_xpm );

    m_aSingleSelection = m_properties->addProperty( "HUD structure",
            "Which look should the coordinateHUD have?", m_possibleSelections->getSelector( 3 ),
            m_propCondition );

    WPropertyHelper::PC_SELECTONLYONE::addTo( m_aSingleSelection );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_aSingleSelection );
    // set the x axis color if in color axis or color cube mode

    WModule::properties();
}

void WMCoordinateHUD::moduleMain()
{
    // added own shader for visualisation as HUD
    m_shader = osg::ref_ptr< WGEShader > ( new WGEShader( "WMCoordinateHUD" , m_localPath ) );

    m_rootNode = new WGEManagedGroupNode( m_active );
    m_rootNode->setName( "coordHUDNode" );
    m_shader->apply( m_rootNode );
    m_enableFaceTexture = WGEShaderDefineOptions::SPtr( new WGEShaderDefineOptions( "DISABLE_FACETEXTURE", "USE_FACETEXTURE" ) );
    m_shader->addPreprocessor( m_enableFaceTexture );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootNode );

    // let the main loop awake if the properties changed.
    debugLog() << "Entering moduleMain()";
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_propCondition );

    // signal ready state
    ready();
    debugLog() << "Module is now ready.";

    while( !m_shutdownFlag() )
    {
        WItemSelector s = m_aSingleSelection->get( true );
        debugLog() << "New mode selected: " << s.at( 0 )->getName();

        m_enableFaceTexture->activateOption( 0, true );
        if( s.at( 0 )->getName() == "colored axis" )
        {
            buildColorAxis();
        }
        else if( s.at( 0 )->getName() == "b/w axis" )
        {
            buildBWAxis();
        }
        else if( s.at( 0 )->getName() == "colored cube" )
        {
            buildColorCube( false );
        }
        else if( s.at( 0 )->getName() == "colored, labeled cube" )
        {
            buildColorCube();
        }

        //update node
        m_rootNode->clear();
        m_geode->setCullingActive( false ); // this disables frustrum culling for the geode to avoid the coordinate system to disappear.
        m_rootNode->setCullingActive( false );
        m_rootNode->insert( m_geode );

        debugLog() << "Waiting ...";
        m_moduleState.wait();

        // quit if requested
        if( m_shutdownFlag() )
        {
            break;
        }
    }

    // remove the node from the graph
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );
}

void WMCoordinateHUD::buildColorAxis()
{
    // build the geometry & geode for the coordinate axis
    osg::ref_ptr< osg::Geode > coordGeode = new osg::Geode();
    osg::ref_ptr< osg::Geometry>  coordGeom = new osg::Geometry;

    // Vertices
    osg::Vec3Array* vertices = new osg::Vec3Array;
    vertices = buildAxisVertices();

    // Colors
    osg::Vec4 x_color( 1.0f, 0.0f, 0.0f, 1.0f );     //red
    osg::Vec4 y_color( 0.0f, 1.0f, 0.0f, 1.0f );     //green
    osg::Vec4 z_color( 0.0f, 0.0f, 1.0f, 1.0f );     //blue
    osg::Vec4 neg_color( 1.0f, 1.0f, 1.0f, 1.0f );   //white

    // x direction transition from red to white
    // y direction transition from green to white
    // z direction transition from blue to white
    osg::Vec4Array* color = new osg::Vec4Array( 12 );
    (*color)[0] = x_color;
    (*color)[1] = x_color;
    (*color)[2] = y_color;
    (*color)[3] = y_color;
    (*color)[4] = z_color;
    (*color)[5] = z_color;
    (*color)[6] = x_color;
    (*color)[7] = neg_color;
    (*color)[8] = y_color;
    (*color)[9] = neg_color;
    (*color)[10] = z_color;
    (*color)[11] = neg_color;

    // add color to geometry
    coordGeom->setColorArray( color );
    coordGeom->setColorBinding( osg::Geometry::BIND_PER_VERTEX );

    // primitives = draw lines between vertices
    osg::DrawArrays *da = new osg::DrawArrays( osg::PrimitiveSet::LINES, 0, vertices->size( ) );

    // add geometry to geode
    coordGeom->setVertexArray( vertices );
    coordGeom->addPrimitiveSet( da );
    coordGeode->addDrawable( coordGeom );
    coordGeode->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

    m_geode = coordGeode;
}

void WMCoordinateHUD::buildBWAxis()
{
    //build the geometry & geode for the coordinate axis
    osg::ref_ptr< osg::Geode > coordGeode = new osg::Geode();
    osg::ref_ptr< osg::Geometry>  coordGeom = new osg::Geometry;

    //Vertices
    osg::Vec3Array* vertices = new osg::Vec3Array;
    vertices = buildAxisVertices();

    //Colors
    osg::Vec4 b_color( 0.0f, 0.0f, 0.0f, 1.0f );     //black
    osg::Vec4 w_color( 1.0f, 1.0f, 1.0f, 1.0f );     //white

    osg::Vec4Array* color = new osg::Vec4Array( 12 );
    (*color)[0] = b_color;
    (*color)[1] = b_color;
    (*color)[2] = b_color;
    (*color)[3] = b_color;
    (*color)[4] = b_color;
    (*color)[5] = b_color;
    (*color)[6] = b_color;
    (*color)[7] = w_color;
    (*color)[8] = b_color;
    (*color)[9] = w_color;
    (*color)[10] = b_color;
    (*color)[11] = w_color;

    //add color to geometry
    coordGeom->setColorArray( color );
    coordGeom->setColorBinding( osg::Geometry::BIND_PER_VERTEX );

    //primitives = draw lines between vertices
    osg::DrawArrays *da = new osg::DrawArrays( osg::PrimitiveSet::LINES, 0, vertices->size( ) );

    //add geometry to geode
    coordGeom->setVertexArray( vertices );
    coordGeom->addPrimitiveSet( da );
    coordGeode->addDrawable( coordGeom );
    coordGeode->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

    m_geode = coordGeode;
}

void WMCoordinateHUD::buildColorCube( bool withFaceLabels )
{
    // build the geometry & geode for the coordinate axis
    osg::ref_ptr< osg::Geode > coordGeode = new osg::Geode();
    osg::ref_ptr< osg::Geometry>  coordGeom = new osg::Geometry;

    // Vertices
    osg::Vec3Array* vertices = new osg::Vec3Array( 36 );
    osg::Vec2Array* texCoord = new osg::Vec2Array( 36 );
    vertices = buildCubeVertices( texCoord );

    // Colors
    osg::Vec4 x_color( 0.9f, 0.0f, 0.0f, 1.0f );        // red
    osg::Vec4 xl_color( 1.0f, 0.7f, 0.7f, 1.0f );       // lightred
    osg::Vec4 y_color( 0.0f, 0.9f, 0.0f, 1.0f );        // green
    osg::Vec4 yl_color( 0.7f, 1.0f, 0.7f, 1.0f );       // lightgreen
    osg::Vec4 z_color( 0.0f, 0.0f, 0.9f, 1.0f );        // blue
    osg::Vec4 zl_color( 0.7f, 0.7f, 1.0f, 1.0f );       // lightblue
    osg::Vec4 neg_color( 1.0f, 1.0f, 1.0f, 1.0f );      // white

    // x direction transition from red to lightred
    // y direction transition from green to lightgreen
    // z direction transition from blue to lightblue
    osg::Vec4Array* color = new osg::Vec4Array( 12 );
    (*color)[0] = (*color)[1] = x_color;
    (*color)[2] = (*color)[3] = z_color;
    (*color)[4] = (*color)[5] = y_color;

    (*color)[6] = (*color)[7] = xl_color;
    (*color)[8] = (*color)[9] = zl_color;
    (*color)[10] = (*color)[11] = yl_color;

    // add color to geometry
    coordGeom->setColorArray( color );
    coordGeom->setColorBinding( osg::Geometry::BIND_PER_PRIMITIVE );

    // primitives = draw lines between vertices
    osg::DrawArrays *da = new osg::DrawArrays( osg::PrimitiveSet::TRIANGLES, 0, vertices->size( ) );

    // add geometry to geode
    coordGeom->setVertexArray( vertices );
    coordGeom->setTexCoordArray( 0, texCoord );
    coordGeom->addPrimitiveSet( da );
    coordGeode->addDrawable( coordGeom );
    coordGeode->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

    if( withFaceLabels )
    {
        // we just created and enable the texture.
        osg::Image* faceLabelsImg = osgDB::readImageFile( ( m_localPath / "labels" / "tiled.png" ).string() );
        if( !faceLabelsImg )
        {
            errorLog() << "Loading face labels from \"" << ( m_localPath / "labels" / "tiled.png" ).string() << "\" failed. Disabled face labels.";
        }
        else
        {
            // gen texture and bind it
            osg::Texture2D* faceLabels = new osg::Texture2D();
            faceLabels->setImage( faceLabelsImg );
            coordGeode->getOrCreateStateSet()->setTextureAttributeAndModes( 0, faceLabels, osg::StateAttribute::ON );
            m_enableFaceTexture->activateOption( 1, true );
        }
    }

    m_geode = coordGeode;
}

osg::Vec3Array* WMCoordinateHUD::buildAxisVertices()
{
    // Vertices
    float size = 1.0;
    osg::Vec3Array* vertices = new osg::Vec3Array;

    vertices->push_back( osg::Vec3( 0, 0, 0 ) );
    vertices->push_back( osg::Vec3( size, 0, 0 ) );
    vertices->push_back( osg::Vec3( 0, 0, 0 ) );
    vertices->push_back( osg::Vec3( 0, size, 0 ) );
    vertices->push_back( osg::Vec3( 0, 0, 0 ) );
    vertices->push_back( osg::Vec3( 0, 0, size ) );
    vertices->push_back( osg::Vec3( 0, 0, 0 ) );
    vertices->push_back( osg::Vec3( -size, 0, 0 ) );
    vertices->push_back( osg::Vec3( 0, 0, 0 ) );
    vertices->push_back( osg::Vec3( 0, -size, 0 ) );
    vertices->push_back( osg::Vec3( 0, 0, 0 ) );
    vertices->push_back( osg::Vec3( 0, 0, -size ) );

    return vertices;
}

osg::Vec3Array* WMCoordinateHUD::buildCubeVertices( osg::Vec2Array* texcoords )
{
    // Vertices
    float s = 0.4;
    osg::Vec3Array* vertices = new osg::Vec3Array( 36 );

    (*vertices)[15] = (*vertices)[12] = (*vertices)[9] =
        (*vertices)[6] = (*vertices)[0] = osg::Vec3( s, s, s );         //1

    (*vertices)[33] = (*vertices)[30] = (*vertices)[8] =
        (*vertices)[3] = (*vertices)[1] = osg::Vec3( s, -s, s );        //2

    (*vertices)[26] = (*vertices)[17] = (*vertices)[5] =
        (*vertices)[2] = osg::Vec3( s, s, -s );                         //3

    (*vertices)[32] = (*vertices)[29] = (*vertices)[25] =
        (*vertices)[4] = osg::Vec3( s, -s, -s );                        //4

    (*vertices)[18] = (*vertices)[13] = (*vertices)[10] =
        osg::Vec3( -s, s, s );                                          //5

    (*vertices)[19] = (*vertices)[34] = (*vertices)[21] =
        (*vertices)[11] = (*vertices)[7] =  osg::Vec3( -s, -s, s );     //6

    (*vertices)[27] = (*vertices)[24] = (*vertices)[23] =
        (*vertices)[20] = (*vertices)[16] =
        (*vertices)[14] = osg::Vec3( -s, s, -s );                       //7

    (*vertices)[31] = (*vertices)[28] = (*vertices)[22] =
        (*vertices)[35] = osg::Vec3( -s, -s, -s );                      //8

    // do the same for the texture coordinates
    if( !texcoords )
    {
        return vertices;
    }

    // how wide is a face label in tex coords?
    float d = 1.0 / 6.0;
    // which label? the order is mX,mY,mZ,pX,pY,pZ
    float o = 3.0 / 6.0;

    // the positive X face
    ( *texcoords )[0] = osg::Vec2( o + d, 1 );
    ( *texcoords )[1] = osg::Vec2( o + 0, 1 );
    ( *texcoords )[2] = osg::Vec2( o + d, 0 );
    ( *texcoords )[3] = osg::Vec2( o + 0, 1 );
    ( *texcoords )[4] = osg::Vec2( o + 0, 0 );
    ( *texcoords )[5] = osg::Vec2( o + d, 0 );

    // the positive Z face
    o = 5.0 / 6.0;
    ( *texcoords )[6 ] = osg::Vec2( o + d, 0 );
    ( *texcoords )[7 ] = osg::Vec2( o + 0, 1 );
    ( *texcoords )[8]  = osg::Vec2( o + 0, 0 );
    ( *texcoords )[9]  = osg::Vec2( o + d, 0 );
    ( *texcoords )[10] = osg::Vec2( o + d, 1 );
    ( *texcoords )[11] = osg::Vec2( o + 0, 1 );

    // the positive Y face
    o = 4.0 / 6.0;
    ( *texcoords )[12] = osg::Vec2( o + 0, 1 );
    ( *texcoords )[13] = osg::Vec2( o + d, 1 );
    ( *texcoords )[14] = osg::Vec2( o + d, 0 );
    ( *texcoords )[15] = osg::Vec2( o + 0, 1 );
    ( *texcoords )[16] = osg::Vec2( o + d, 0 );
    ( *texcoords )[17] = osg::Vec2( o + 0, 0 );

    // the negative X face
    o = 0.0 / 6.0;
    ( *texcoords )[18] = osg::Vec2( o + 0, 1 );
    ( *texcoords )[19] = osg::Vec2( o + d, 1 );
    ( *texcoords )[20] = osg::Vec2( o + 0, 0 );
    ( *texcoords )[21] = osg::Vec2( o + d, 1 );
    ( *texcoords )[22] = osg::Vec2( o + d, 0 );
    ( *texcoords )[23] = osg::Vec2( o + 0, 0 );

    // the negative Z face
    o = 2.0 / 6.0;
    ( *texcoords )[24] = osg::Vec2( o + d, 0 );
    ( *texcoords )[25] = osg::Vec2( o + 0, 1 );
    ( *texcoords )[26] = osg::Vec2( o + d, 1 );
    ( *texcoords )[27] = osg::Vec2( o + d, 0 );
    ( *texcoords )[28] = osg::Vec2( o + 0, 0 );
    ( *texcoords )[29] = osg::Vec2( o + 0, 1 );

    // the negative Y face
    o = 1.0 / 6.0;
    ( *texcoords )[30] = osg::Vec2( o + d, 1 );
    ( *texcoords )[31] = osg::Vec2( o + 0, 0 );
    ( *texcoords )[32] = osg::Vec2( o + d, 0 );
    ( *texcoords )[33] = osg::Vec2( o + d, 1 );
    ( *texcoords )[34] = osg::Vec2( o + 0, 1 );
    ( *texcoords )[35] = osg::Vec2( o + 0, 0 );

    return vertices;
}

