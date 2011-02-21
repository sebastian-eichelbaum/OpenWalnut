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
#include <iostream>

#include <osg/ShapeDrawable>
#include <osg/Geode>

#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/UFOManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/AnimationPathManipulator>
#include <osgGA/TerrainManipulator>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/View>

#include <osgDB/ReadFile>

#include "../exceptions/WGEInitFailed.h"
#include "../WGE2DManipulator.h"
#include "../WGEZoomTrackballManipulator.h"
#include "../WPickHandler.h"

#include "WGEViewerMac.h"

WGEViewerMac::WGEViewerMac( std::string name, int x, int y,
    int width, int height, WGECamera::ProjectionMode projectionMode )
    : WGEGraphicsWindowMac( x, y, width, height ),
      boost::enable_shared_from_this< WGEViewerMac >(),
      m_name( name )
{
    try
    {
        m_View = osg::ref_ptr<osgViewer::Viewer>( new osgViewer::Viewer() );
        m_View->setCamera( new WGECamera( width, height, projectionMode ) );
        m_View->getCamera()->setGraphicsContext( m_GraphicsWindow.get() );

        switch( projectionMode )
        {
            case( WGECamera::ORTHOGRAPHIC ):
                m_pickHandler = new WPickHandler( name );
                m_View->addEventHandler( m_pickHandler );
                if ( name != std::string( "main" ) )
                    break;
            case( WGECamera::PERSPECTIVE ):
                // camera manipulator
                m_View->setCameraManipulator( new WGEZoomTrackballManipulator() );

                m_View->setLightingMode( osg::View::HEADLIGHT ); // this is the default anyway

                break;
            case( WGECamera::TWO_D ):
                // no manipulators nor gui handlers
                break;
            default:
                throw WGEInitFailed( std::string( "Unknown projection mode" ) );
        }

        // add the stats handler
        m_View->addEventHandler( new osgViewer::StatsHandler );
    }
    catch( ... )
    {
        throw WGEInitFailed( std::string( "Initialization of WGEViewerMac failed" ) );
    }
}

WGEViewerMac::~WGEViewerMac()
{
    // cleanup
    close();
}

osg::ref_ptr<osgViewer::Viewer> WGEViewerMac::getView()
{
    return m_View;
}

void WGEViewerMac::setCameraManipulator( osg::ref_ptr<osgGA::MatrixManipulator> manipulator )
{
    m_View->setCameraManipulator( manipulator );
    // redraw request?? no since it redraws permanently and uses the new settings
}

osg::ref_ptr<osgGA::MatrixManipulator> WGEViewerMac::getCameraManipulator()
{
    return m_View->getCameraManipulator();
}

void WGEViewerMac::setCamera( osg::ref_ptr<osg::Camera> camera )
{
    m_View->setCamera( camera );
    // redraw request?? No since it redraws permanently and uses the new settings
}

osg::ref_ptr<osg::Camera> WGEViewerMac::getCamera()
{
    return m_View->getCamera();
}

void WGEViewerMac::setScene( osg::ref_ptr< WGEGroupNode > node )
{
    m_View->setSceneData( node );
    m_scene = node;
}

osg::ref_ptr< WGEGroupNode > WGEViewerMac::getScene()
{
    return m_scene;
}

void WGEViewerMac::setBgColor( const WColor& bgColor )
{
    m_View->getCamera()->setClearColor( bgColor );
}

void WGEViewerMac::paint()
{
    m_View->frame();
}

void WGEViewerMac::resize( int width, int height )
{
    m_View->getEventQueue()->windowResize( 0, 0, width, height );

    WGEGraphicsWindowMac::resize( width, height );

    // also update the camera
    m_View->getCamera()->setViewport( 0, 0, width, height );
    WGECamera* camera = dynamic_cast< WGECamera* >( m_View->getCamera() );
    if( camera )
    {
        camera->resize();
    }
}

void WGEViewerMac::close()
{
    // forward close event
    WGEGraphicsWindowMac::close();
}

std::string WGEViewerMac::getName() const
{
    return m_name;
}

osg::ref_ptr< WPickHandler > WGEViewerMac::getPickHandler()
{
    return m_pickHandler;
}

void WGEViewerMac::reset()
{
    m_View->home();
}

