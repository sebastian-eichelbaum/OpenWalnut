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
#include <osg/Camera>

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

#include "exceptions/WGEInitFailed.h"
#include "WGE2DManipulator.h"
#include "WGEGroupNode.h"
#include "WGENoOpManipulator.h"
#include "WGEZoomTrackballManipulator.h"
#include "WPickHandler.h"
#include "WGEGeodeUtils.h"
#include "shaders/WGEShader.h"


#include "../common/WConditionOneShot.h"

#include "../common/WThreadedRunner.h"

#include "WGEViewer.h"

WGEViewer::WGEViewer( std::string name, osg::ref_ptr<osg::Referenced> wdata, int x, int y,
    int width, int height, WGECamera::ProjectionMode projectionMode )
    : WGEGraphicsWindow( wdata, x, y, width, height ),
      boost::enable_shared_from_this< WGEViewer >(),
      m_name( name ),
      m_rendered( WBoolFlag::SPtr( new WBoolFlag( new WConditionOneShot(), false ) ) ),
      m_screenCapture( new WGEScreenCapture() ),
      m_inAnimationMode( false )
{
    try
    {
#ifdef WGEMODE_MULTITHREADED
        m_View = osg::ref_ptr<osgViewer::View>( new osgViewer::View );
#else
        // on mac, this is a viewer!
        m_View = osg::ref_ptr<osgViewer::Viewer>( new osgViewer::Viewer );
#endif

        m_View->setCamera( new WGECamera( width, height, projectionMode ) );
        m_queryCallback = new QueryCallback( m_View->getCamera(), m_rendered );
        m_View->getCamera()->setInitialDrawCallback( m_queryCallback );
        m_View->getCamera()->setFinalDrawCallback( m_screenCapture );

#ifdef WGEMODE_MULTITHREADED
        m_View->getCamera()->setGraphicsContext( m_GraphicsContext.get() );
#else
        m_View->getCamera()->setGraphicsContext( m_GraphicsWindow.get() );
#endif

        switch( projectionMode )
        {
            case( WGECamera::ORTHOGRAPHIC ):
                m_pickHandler = new WPickHandler( name );
                m_View->addEventHandler( m_pickHandler );
                if( name != std::string( "Main View" ) )
                    break;
            case( WGECamera::PERSPECTIVE ):
                // camera manipulator
                m_View->setCameraManipulator( new WGEZoomTrackballManipulator() );

                m_View->setLightingMode( osg::View::HEADLIGHT ); // this is the default anyway

                break;
            case( WGECamera::TWO_D ):
                // no manipulators nor gui handlers
                break;
            case( WGECamera::TWO_D_UNIT ):
                // use no-op handler by default
                m_View->setCameraManipulator( new WGENoOpManipulator() );
                break;
            default:
                throw WGEInitFailed( std::string( "Unknown projection mode" ) );
        }

        // add the stats handler
        m_View->addEventHandler( new osgViewer::StatsHandler );
    }
    catch( ... )
    {
        throw WGEInitFailed( std::string( "Initialization of WGEViewer failed" ) );
    }
}

WGEViewer::~WGEViewer()
{
    // cleanup
    close();
}

#ifdef WGEMODE_SINGLETHREADED
osg::ref_ptr<osgViewer::Viewer>
#else
osg::ref_ptr<osgViewer::View>
#endif
WGEViewer::getView()
{
    return m_View;
}

void WGEViewer::setCameraManipulator( osg::ref_ptr<osgGA::MatrixManipulator> manipulator )
{
    if( !m_inAnimationMode )
    {
        m_View->setCameraManipulator( manipulator );
    }
}

osg::ref_ptr<osgGA::MatrixManipulator> WGEViewer::getCameraManipulator()
{
    return m_View->getCameraManipulator();
}

void WGEViewer::setCamera( osg::ref_ptr<osg::Camera> camera )
{
    m_View->setCamera( camera );
    // redraw request?? No since it redraws permanently and uses the new settings
}

osg::ref_ptr<osg::Camera> WGEViewer::getCamera()
{
    return m_View->getCamera();
}

void WGEViewer::setScene( osg::ref_ptr< WGEGroupNode > node )
{
    m_View->setSceneData( node );
    m_scene = node;


    osg::ref_ptr< osg::Camera > bkCam = new osg::Camera();
    bkCam->setClearMask( GL_DEPTH_BUFFER_BIT );
    bkCam->setRenderOrder( osg::Camera::POST_RENDER );
    bkCam->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
    bkCam->setProjectionMatrixAsOrtho2D( 0.0, 1.0, 0.0, 1.0 );
    bkCam->setViewMatrix( osg::Matrixd::identity() );
    osg::StateSet* state = bkCam->getOrCreateStateSet();
    state->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF );
    state->setMode( GL_LIGHTING, osg::StateAttribute::PROTECTED );
    state->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    state->setMode( GL_BLEND, osg::StateAttribute::PROTECTED );
    state->setMode( GL_BLEND, osg::StateAttribute::ON );

    osg::ref_ptr< osg::Geode > geode = wge::genFinitePlane( osg::Vec3( 0.0, 0.0, 0.0 ),
                                                            osg::Vec3( 1.0, 0.0, 0.0 ),
                                                            osg::Vec3( 0.0, 1.0, 0.0 ) );

    osg::ref_ptr< WGEShader > vignetteShader = new WGEShader( "WGECameraVignette" );
    vignetteShader->apply( geode );


    // add the slice to the geode
    bkCam->addChild( geode );
    node->insert( bkCam );
}

osg::ref_ptr< WGEGroupNode > WGEViewer::getScene()
{
    return m_scene;
}

void WGEViewer::setBgColor( const WColor& bgColor )
{
    m_View->getCamera()->setClearColor( bgColor );
}

void WGEViewer::paint()
{
#ifdef WGEMODE_SINGLETHREADED
    m_View->frame();
#endif
}

void WGEViewer::resize( int width, int height )
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

void WGEViewer::close()
{
    // forward close event
    WGEGraphicsWindow::close();
}

std::string WGEViewer::getName() const
{
    return m_name;
}

osg::ref_ptr< WPickHandler > WGEViewer::getPickHandler()
{
    return m_pickHandler;
}

void WGEViewer::reset()
{
    m_View->home();
}

WGEScreenCapture::RefPtr WGEViewer::getScreenCapture() const
{
    return m_screenCapture;
}

std::string WGEViewer::getOpenGLVendor() const
{
    return m_queryCallback->getVendor();
}

WBoolFlag::SPtr WGEViewer::isFrameRendered() const
{
    return m_rendered;
}

WGEViewer::QueryCallback::QueryCallback( osg::ref_ptr<osg::Camera> camera, WBoolFlag::SPtr run ):
    m_vendor( "" ),
    m_run( run ),
    m_camera( camera )
{
    // init
}

WGEViewer::QueryCallback::~QueryCallback()
{
    // cleanup
}

void WGEViewer::QueryCallback::operator()( osg::RenderInfo& /* renderInfo */ ) const
{
    const GLubyte* vendor = glGetString( GL_VENDOR );
    m_vendor = reinterpret_cast< const char* >( vendor );

    // job done. De-register.
    m_camera->setInitialDrawCallback( NULL );
    m_run->set( true );
}

std::string WGEViewer::QueryCallback::getVendor() const
{
    return m_vendor;
}

WGEAnimationManipulator::RefPtr WGEViewer::animationMode( bool on )
{
    if( m_inAnimationMode && !on ) // turn off mode
    {
        m_inAnimationMode = false;

        // restore old manipulator
        m_View->setCameraManipulator( m_animationModeManipulatorBackup );
        return NULL;
    }
    else if( !m_inAnimationMode && on ) // turn on
    {
        m_inAnimationMode = true;

        // backup
        m_animationModeManipulatorBackup = getCameraManipulator();

        // create animation manipulator
        WGEAnimationManipulator::RefPtr anim = new WGEAnimationManipulator();
        m_View->setCameraManipulator( anim );
        return anim;
    }
    else if( m_inAnimationMode ) // already on
    {
        return dynamic_cast< WGEAnimationManipulator* >( getCameraManipulator().get() );
    }

    // else: do nothing
    return NULL;
}

bool WGEViewer::isAnimationMode() const
{
    return m_inAnimationMode;
}

