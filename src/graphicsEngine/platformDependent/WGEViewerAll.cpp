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

#include "WGEViewerAll.h"

WGEViewerAll::WGEViewerAll( std::string name, osg::ref_ptr<osg::Referenced> wdata, int x, int y,
    int width, int height, WGECamera::ProjectionMode projectionMode )
    : WGEGraphicsWindow( wdata, x, y, width, height ),
      boost::enable_shared_from_this< WGEViewerAll >(),
      m_name( name ),
      m_screenCapture( new WGEScreenCapture() ),
      m_inAnimationMode( false )
{
    try
    {
        m_View = osg::ref_ptr<osgViewer::View>( new osgViewer::View );
        //m_View = osg::ref_ptr<osgViewer::Viewer>( new osgViewer::Viewer() );
        m_View->setCamera( new WGECamera( width, height, projectionMode ) );
        m_View->getCamera()->setGraphicsContext( m_GraphicsContext );
        m_View->getCamera()->setFinalDrawCallback( m_screenCapture );

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
        throw WGEInitFailed( std::string( "Initialization of WGEViewerAll failed" ) );
    }
}

WGEViewerAll::~WGEViewerAll()
{
    // cleanup
    close();
}

osg::ref_ptr<osgViewer::View> WGEViewerAll::getView()
{
    return m_View;
}

void WGEViewerAll::setCameraManipulator( osg::ref_ptr<osgGA::MatrixManipulator> manipulator )
{
    if ( !m_inAnimationMode )
    {
        m_View->setCameraManipulator( manipulator );
    }
}

osg::ref_ptr<osgGA::MatrixManipulator> WGEViewerAll::getCameraManipulator()
{
    return m_View->getCameraManipulator();
}

void WGEViewerAll::setCamera( osg::ref_ptr<osg::Camera> camera )
{
    m_View->setCamera( camera );
    // redraw request?? No since it redraws permanently and uses the new settings
}

osg::ref_ptr<osg::Camera> WGEViewerAll::getCamera()
{
    return m_View->getCamera();
}

void WGEViewerAll::setScene( osg::ref_ptr< WGEGroupNode > node )
{
    m_View->setSceneData( node );
    m_scene = node;
}

osg::ref_ptr< WGEGroupNode > WGEViewerAll::getScene()
{
    return m_scene;
}

void WGEViewerAll::setBgColor( const WColor& bgColor )
{
    m_View->getCamera()->setClearColor( bgColor );
}

void WGEViewerAll::resize( int width, int height )
{
    m_View->getEventQueue()->windowResize( 0, 0, width, height );

    WGEGraphicsWindow::resize( width, height );

    // also update the camera
    m_View->getCamera()->setViewport( 0, 0, width, height );
    WGECamera* camera = dynamic_cast< WGECamera* >( m_View->getCamera() );
    if( camera )
    {
        camera->resize();
    }
}

void WGEViewerAll::close()
{
    // forward close event
    WGEGraphicsWindow::close();
}

std::string WGEViewerAll::getName() const
{
    return m_name;
}

osg::ref_ptr< WPickHandler > WGEViewerAll::getPickHandler()
{
    return m_pickHandler;
}

void WGEViewerAll::reset()
{
    m_View->home();
}

WGEScreenCapture::RefPtr WGEViewerAll::getScreenCapture() const
{
    return m_screenCapture;
}


WGEAnimationManipulator::RefPtr WGEViewerAll::animationMode( bool on )
{
    if ( m_inAnimationMode && !on ) // turn off mode
    {
        m_inAnimationMode = false;

        // restore old manipulator
        m_View->setCameraManipulator( m_animationModeManipulatorBackup );
        return NULL;
    }
    else if ( !m_inAnimationMode && on ) // turn on
    {
        m_inAnimationMode = true;

        // backup
        m_animationModeManipulatorBackup = getCameraManipulator();

        // create animation manipulator
        WGEAnimationManipulator::RefPtr anim = new WGEAnimationManipulator();
        m_View->setCameraManipulator( anim );
        return anim;
    }
    else if ( m_inAnimationMode ) // already on
    {
        return dynamic_cast< WGEAnimationManipulator* >( getCameraManipulator().get() );
    }

    // else: do nothing
    return NULL;
}

bool WGEViewerAll::isAnimationMode() const
{
    return m_inAnimationMode;
}

