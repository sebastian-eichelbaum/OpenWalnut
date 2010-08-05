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




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// How to create your own module in OpenWalnut? Here are the steps to take:
//   * copy the template module directory
//   * think about a name for your module
//   * rename the files from WMTemplate.cpp and WMTemplate.h to WMYourModuleName.cpp and WMYourModuleName.h
//   * rename the class inside these files to WMYourModuleName
//   * change WMYourModuleName::getName() to a unique name, like "Your Module Name"
//   * add a new prototype of your module to src/kernel/WModuleFactory.cpp -> search for m_prototypes.insert
//     * analogously to the other modules, add yours
//     * Note: this step will be automated in some time
//   * run CMake and compile
//   * read the documentation in this module and modify it to your needs
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




// Some rules to the inclusion of headers:
//  * Ordering:
//    * C Headers
//    * C++ Standard headers
//    * External Lib headers (like OSG or Boost headers)
//    * headers of other classes inside OpenWalnut
//    * your own header file

#include <string>

#include <osg/ShapeDrawable>
#include <osg/Group>
#include <osg/Geode>
#include <osg/MatrixTransform>
#include <osg/Material>
#include <osg/StateAttribute>

#include <osg/Vec3>
#include <osg/Geometry>

#include "../../kernel/WKernel.h"
#include "../../common/WColor.h"
#include "../../common/WPropertyHelper.h"
#include "../../graphicsEngine/WGEUtils.h"

#include "WMCoordinateHUD.h"
#include "coordinateHUD.xpm"
#include "option_1.xpm"
#include "option_2.xpm"
#include "option_3.xpm"
#include "option_4.xpm"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMCoordinateHUD )

WMCoordinateHUD::WMCoordinateHUD():
    WModule()
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
    return "CoordinateHUD";
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

    //list of alternatives:
    m_possibleSelections = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_possibleSelections->addItem( "colorfull axis", "colorfull coordinate axis", option_1_xpm );
    m_possibleSelections->addItem( "b/w axis", "black & white coordinate axis", option_2_xpm );
    m_possibleSelections->addItem( "colorfull cube", "colorfull coordinate cube", option_3_xpm );
    m_possibleSelections->addItem( "b/w cube", "black & white coordinate cube", option_4_xpm );

    m_aSingleSelection = m_properties->addProperty( "HUD structure",
            "Which look should the coordinateHUD have?", m_possibleSelections->getSelectorFirst(),
            m_propCondition );

    WPropertyHelper::PC_SELECTONLYONE::addTo( m_aSingleSelection );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_aSingleSelection );
}

void WMCoordinateHUD::moduleMain()
{
    // added own shader for visualisation as HUD
    m_shader = osg::ref_ptr< WShader > ( new WShader( "WMCoordinateHUD" , m_localPath ) );
    // added own shader for visualisation as HUD
    //m_txtShader = osg::ref_ptr< WShader > ( new WShader( "WMCoordinateHUD-text" , m_localPath ) );

    m_rootNode = new WGEManagedGroupNode( m_active );
    m_shader->apply( m_rootNode );
    m_rootNode->setName( "coordHUDNode" );

    // let the main loop awake if the properties changed.
    debugLog() << "Entering moduleMain()";
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_propCondition );

    // signal ready state
    ready();
    debugLog() << "Module is now ready.";

    //default visualisation
    buildColorAxis();
    m_rootNode->insert( m_geode );
    //buildCaption();
    //m_rootNode->insert( m_txtGeode );
    //m_txtShader->apply(m_txtGeode );
    //WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_txtGeode );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootNode );

    while( !m_shutdownFlag() )
    {
        // Now, the moduleState variable comes into play.
        // The module can wait for the condition, which gets fired whenever the input receives data
        // or an property changes. The main loop now waits until something happens.
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        // quit if requested
        if ( m_shutdownFlag() )
        {
            break;
        }

        if ( m_aSingleSelection->changed() )
        {
            WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );

            WItemSelector s = m_aSingleSelection->get( true );
            infoLog() << "New mode selected: " << s.at( 0 ).name;

            if ( s.at( 0 ).name == "colorfull axis" )
            {
                buildColorAxis();
            }
            else if ( s.at( 0 ).name == "b/w axis" )
            {
                buildBWAxis();
            }
            else if ( s.at( 0 ).name == "colorfull cube" )
            {
                buildColorCube();
            }
            else if ( s.at( 0 ).name == "b/w cube" )
            {
                buildBWCube();
            }

            //update node
            m_rootNode->clear();
            m_rootNode->insert( m_geode );
            //m_rootNode->insert( m_txtGeode );
            WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootNode );
        }
    }

    // Since the modules run in a separate thread: wait
    waitForStop();
    //remove the node from the graph
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );
}

void WMCoordinateHUD::buildColorAxis()
{
        //build the geometry & geode for the coordinate axis
        osg::ref_ptr< osg::Geode > coordGeode = new osg::Geode();
        osg::ref_ptr< osg::Geometry>  coordGeom = new osg::Geometry;

        //Vertices
        osg::Vec3Array* vertices = new osg::Vec3Array;
        vertices = buildAxisVertices();

        //Colors
        osg::Vec4 x_color( 1.0f, 0.0f, 0.0f, 1.0f );     //red
        osg::Vec4 y_color( 0.0f, 1.0f, 0.0f, 1.0f );     //green
        osg::Vec4 z_color( 0.0f, 0.0f, 1.0f, 1.0f );     //blue
        osg::Vec4 neg_color( 1.0f, 1.0f, 1.0f, 1.0f );   //white

        //x direction transition from red to white
        //y direction transition from green to white
        //z direction transition from blue to white
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
        osg::Vec4 w_color( 1.0f, 1.0f, 1.0f, 1.0f );   //white

        //x direction transition from red to white
        //y direction transition from green to white
        //z direction transition from blue to white
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

void WMCoordinateHUD::buildBWCube()
{
        //build the geometry & geode for the coordinate axis
        osg::ref_ptr< osg::Geode > coordGeode = new osg::Geode();
        osg::ref_ptr< osg::Geometry>  coordGeom = new osg::Geometry;

       //Vertices
        osg::Vec3Array* vertices = new osg::Vec3Array( 36 );
        vertices = buildCubeVertices();

        //Colors
        osg::Vec4 b_color( 0.0f, 0.0f, 0.0f, 1.0f );     //black
        osg::Vec4 bl_color( 0.2f, 0.2f, 0.2f, 1.0f );     //black
        osg::Vec4 bll_color( 0.4f, 0.4f, 0.4f, 1.0f );     //black
        osg::Vec4 w_color( 1.0f, 1.0f, 1.0f, 1.0f );   //white
        osg::Vec4 wl_color( 0.97f, 0.97f, 0.97f, 1.0f );   //white
        osg::Vec4 wll_color( 0.93f, 0.93f, 0.93f, 1.0f );   //white

        osg::Vec4Array* color = new osg::Vec4Array( 12 );
        (*color)[0] = (*color)[1] = bl_color;
        (*color)[2] = (*color)[3] = bll_color;
        (*color)[4] = (*color)[5] = b_color;

        (*color)[6] = (*color)[7] = wl_color;
        (*color)[8] = (*color)[9] = wll_color;
        (*color)[10] = (*color)[11] = w_color;

        //add color to geometry
        coordGeom->setColorArray( color );
        coordGeom->setColorBinding( osg::Geometry::BIND_PER_PRIMITIVE );

        //primitives = draw lines between vertices
        osg::DrawArrays *da = new osg::DrawArrays( osg::PrimitiveSet::TRIANGLES, 0, vertices->size( ) );

        //add geometry to geode
        coordGeom->setVertexArray( vertices );
        coordGeom->addPrimitiveSet( da );
        coordGeode->addDrawable( coordGeom );
        coordGeode->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

        m_geode = coordGeode;
}

void WMCoordinateHUD::buildColorCube()
{
        //build the geometry & geode for the coordinate axis
        osg::ref_ptr< osg::Geode > coordGeode = new osg::Geode();
        osg::ref_ptr< osg::Geometry>  coordGeom = new osg::Geometry;

       //Vertices
        osg::Vec3Array* vertices = new osg::Vec3Array( 36 );
        vertices = buildCubeVertices();

        //Colors
        osg::Vec4 x_color( 1.0f, 0.0f, 0.0f, 1.0f );     //red
        osg::Vec4 xl_color( 1.0f, 0.9f, 0.9f, 1.0f );     //lightred
        osg::Vec4 y_color( 0.0f, 1.0f, 0.0f, 1.0f );     //green
        osg::Vec4 yl_color( 0.9f, 1.0f, 0.9f, 1.0f );     //lightgreen
        osg::Vec4 z_color( 0.0f, 0.0f, 1.0f, 1.0f );     //blue
        osg::Vec4 zl_color( 0.9f, 0.9f, 1.0f, 1.0f );     //lightblue
        osg::Vec4 neg_color( 1.0f, 1.0f, 1.0f, 1.0f );   //white

        //x direction transition from red to lightred
        //y direction transition from green to lightgreen
        //z direction transition from blue to lightblue
        osg::Vec4Array* color = new osg::Vec4Array( 12 );
        (*color)[0] = (*color)[1] = y_color;
        (*color)[2] = (*color)[3] = z_color;
        (*color)[4] = (*color)[5] = x_color;

        (*color)[6] = (*color)[7] = yl_color;
        (*color)[8] = (*color)[9] = zl_color;
        (*color)[10] = (*color)[11] = xl_color;

        //add color to geometry
        coordGeom->setColorArray( color );
        coordGeom->setColorBinding( osg::Geometry::BIND_PER_PRIMITIVE );

        //primitives = draw lines between vertices
        osg::DrawArrays *da = new osg::DrawArrays( osg::PrimitiveSet::TRIANGLES, 0, vertices->size( ) );

        //add geometry to geode
        coordGeom->setVertexArray( vertices );
        coordGeom->addPrimitiveSet( da );
        coordGeode->addDrawable( coordGeom );
        coordGeode->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

        m_geode = coordGeode;
}

osg::Vec3Array* WMCoordinateHUD::buildAxisVertices()
{
        //Vertices
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

osg::Vec3Array* WMCoordinateHUD::buildCubeVertices()
{
       //Vertices
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
             osg::Vec3( -s, s, s );                                         //5

        (*vertices)[19] = (*vertices)[34] = (*vertices)[21] =
            (*vertices)[11] = (*vertices)[7] =  osg::Vec3( -s, -s, s );     //6

        (*vertices)[27] = (*vertices)[24] = (*vertices)[23] =
            (*vertices)[20] = (*vertices)[16] =
            (*vertices)[14] = osg::Vec3( -s, s, -s );                       //7

       (*vertices)[31] = (*vertices)[28] = (*vertices)[22] =
           (*vertices)[35] = osg::Vec3( -s, -s, -s );                       //8

       return vertices;
}

void WMCoordinateHUD::buildCaption()
{
    osgText::Text* textOne = new osgText::Text();
    osg::ref_ptr< osg::Geode > txtGeode = new osg::Geode();

    txtGeode->addDrawable( textOne );

    // Set up the parameters for the text we'll add to the HUD:
    textOne->setCharacterSize( 25 );
    //textOne->setFont("C:/WINDOWS/Fonts/impact.ttf");
    textOne->setText( "D" );
    textOne->setAxisAlignment( osgText::Text::USER_DEFINED_ROTATION );
    textOne->setAutoRotateToScreen( false );
    textOne->setCharacterSizeMode( osgText::Text::OBJECT_COORDS );
    textOne->setPosition( osg::Vec3( 0, 0.5, 0.5 ) );
    textOne->setColor( osg::Vec4( 255, 0, 0, 1 ) );
    m_txtGeode = txtGeode;
}
