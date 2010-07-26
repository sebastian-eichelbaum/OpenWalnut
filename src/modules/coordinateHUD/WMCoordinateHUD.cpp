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
#include <osg/Material>
#include <osg/StateAttribute>

#include <osg/Vec3>
#include <osg/Geometry>

#include "../../kernel/WKernel.h"
#include "../../common/WColor.h"
#include "../../common/WPropertyHelper.h"
#include "../../graphicsEngine/WGEUtils.h"

#include "coordinateHUD.xpm"
#include "WMCoordinateHUD.h"

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
}

void WMCoordinateHUD::moduleMain()
{
    debugLog() << "Entering moduleMain()";
    m_moduleState.setResetable( true, true );

    ready();
    debugLog() << "Module is now ready.";

    m_shader = osg::ref_ptr< WShader > ( new WShader( "WMCoordinateHUD" , m_localPath ) );

    //m_rootNode =  new WGEManagedGroupNode( m_active );
    m_rootNode = new osg::Geode;
    m_rootNode->setName( "coordHUDNode" );

    osg::ref_ptr< osg::Geode > coordGeode = new osg::Geode();
    osg::ref_ptr< osg::Geometry>  coordGeom = new osg::Geometry;

    //Eckpunkte
    float size = 50;
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

    //colors
    osg::Vec4 x_color( 1.0f, 0.0f, 0.0f, 1.0f );     //red
    osg::Vec4 y_color( 0.0f, 1.0f, 0.0f, 1.0f );     //green
    osg::Vec4 z_color( 0.0f, 0.0f, 1.0f, 1.0f );     //blue
    osg::Vec4 neg_color( 1.0f, 1.0f, 1.0f, 1.0f );   //white

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

    coordGeom->setColorArray( color );
    coordGeom->setColorBinding( osg::Geometry::BIND_PER_VERTEX );

    osg::DrawArrays *da = new osg::DrawArrays( osg::PrimitiveSet::LINES, 0, vertices->size( ) );

    coordGeom->setVertexArray( vertices );
    coordGeom->addPrimitiveSet( da );
    coordGeode->addDrawable( coordGeom );
    coordGeode->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

    //m_rootNode->remove( coordGeode );
    //m_rootNode->insert( coordGeode );

    m_rootNode = coordGeode;

    //m_rootNode->setNodeMask( m_active->get() ? 0xFFFFFFFF : 0x0 );
    //m_rootNode->addUpdateCallback( new SafeUpdateCallback( this ) );

    m_shader->apply( m_rootNode );

    //WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( coordGeode );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootNode );

    if ( m_active->get() )
    {
        m_rootNode->setNodeMask( 0xFFFFFFFF );
    }
    else
    {
        m_rootNode->setNodeMask( 0x0 );
    }

    //WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( coordGeode );
}

void WMCoordinateHUD::activate()
{
    if ( m_active->get() )
    {
        debugLog() << "Activate.";
    }
    else
    {
        debugLog() << "Deactivate.";
    }

    WModule::activate();
}

