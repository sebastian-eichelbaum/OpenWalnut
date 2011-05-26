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

#include <QtGui/QKeyEvent>

#include "WQtGLWidget.h"
#include "WQtGLWidget.moc"

#include "core/common/WConditionOneShot.h"
#include "core/common/WFlag.h"
#include "core/common/WLogger.h"
#include "core/graphicsEngine/WGE2DManipulator.h"
#include "core/graphicsEngine/WGEViewer.h"
#include "core/graphicsEngine/WGEZoomTrackballManipulator.h"
#include "core/graphicsEngine/WGraphicsEngine.h"
#include "core/kernel/WKernel.h"

#ifndef __APPLE__
    #ifndef _WIN32
        #include <osgViewer/api/X11/GraphicsWindowX11>
        typedef osgViewer::GraphicsWindowX11::WindowData WindowData;
    #else
        #include <osgViewer/api/Win32/GraphicsWindowWin32>
        typedef osgViewer::GraphicsWindowWin32::WindowData WindowData;
    #endif
#endif

#ifndef _WIN32
WQtGLWidget::WQtGLWidget( std::string nameOfViewer, QWidget* parent, WGECamera::ProjectionMode projectionMode, const QWidget* shareWidget ):
    QGLWidget( getDefaultFormat(), parent, dynamic_cast< const QGLWidget* >( shareWidget ) ),
#else
WQtGLWidget::WQtGLWidget( std::string nameOfViewer, QWidget* parent, WGECamera::ProjectionMode projectionMode, const QWidget* ):
    QWidget( parent ),
#endif
      m_nameOfViewer( nameOfViewer ),
      m_recommendedSize(),
      m_firstPaint( true )
{
    m_recommendedSize.setWidth( 200 );
    m_recommendedSize.setHeight( 200 );

    m_initialProjectionMode = projectionMode;

    // required
    setAttribute( Qt::WA_PaintOnScreen );
    setAttribute( Qt::WA_NoSystemBackground );
    setFocusPolicy( Qt::ClickFocus );

    // initialize OpenGL context and OpenSceneGraph
#ifndef __APPLE__
    osg::ref_ptr<osg::Referenced> wdata = new WindowData( winId() );

    // create viewer
    m_Viewer = WKernel::getRunningKernel()->getGraphicsEngine()->createViewer(
        m_nameOfViewer, wdata, x(), y(), width(), height(), m_initialProjectionMode );
#else
    osg::ref_ptr<osg::Referenced> wdata; // NULL pointer since not used on Mac

    // create viewer
    m_Viewer = WKernel::getRunningKernel()->getGraphicsEngine()->createViewer(
        m_nameOfViewer, wdata, x(), y(), width(), height(), m_initialProjectionMode );

    connect( &m_Timer, SIGNAL( timeout() ), this, SLOT( updateGL() ) );
    m_Timer.start( 10 );
#endif
}

WQtGLWidget::~WQtGLWidget()
{
    WKernel::getRunningKernel()->getGraphicsEngine()->closeViewer( m_nameOfViewer );
    m_Viewer.reset();
}

QSize WQtGLWidget::sizeHint() const
{
    return m_recommendedSize;
}

void WQtGLWidget::setCameraManipulator( WQtGLWidget::CameraManipulators manipulator )
{
    m_CurrentManipulator = manipulator;
    switch ( manipulator )
    {
        case TWO_D:
            WLogger::getLogger()->addLogMessage( "Switched to OSG manipulator \"WGE2D\".",
                                                 "WQtGLWidget(" + m_Viewer->getName() + ")",
                                                 LL_DEBUG );

            m_Viewer->setCameraManipulator( new( WGE2DManipulator ) );
            break;
        case TRACKBALL:
        default:
            WLogger::getLogger()->addLogMessage( "Switched to OSG manipulator \"WGETrackball\".",
                                                 "WQtGLWidget(" + m_Viewer->getName() + ")",
                                                 LL_DEBUG );

            m_Viewer->setCameraManipulator( new( WGEZoomTrackballManipulator ) );
            break;
    }
}

void WQtGLWidget::setBgColor( const WColor& bgColor )
{
    m_Viewer->setBgColor( bgColor );
}

WQtGLWidget::CameraManipulators WQtGLWidget::getCameraManipulators()
{
    return m_CurrentManipulator;
}

boost::shared_ptr< WGEViewer > WQtGLWidget::getViewer() const
{
    return m_Viewer;
}

void WQtGLWidget::paintEvent( QPaintEvent* /*event*/ )
{
    if( m_firstPaint )
    {
        // it is important to let the GE know that there now is an completely initialized widget -> allowing GE startup to complete
        // This is needed as on some machines, the OSG crashes if the GL widget is not fully initialized.
        m_firstPaint = false;
        WKernel::getRunningKernel()->getGraphicsEngine()->finalizeStartup();
    }
}

#ifdef __APPLE__

void WQtGLWidget::paintGL()
{
    m_Viewer->paint();
}

void WQtGLWidget::resizeEvent( QResizeEvent* /* event */ )
{
    // m_Viewer->resize( event->size().width(), event->size().height() );
}

void WQtGLWidget::resizeGL( int width, int height )
{
    m_Viewer->resize( width, height );
}

#else

void WQtGLWidget::paintGL()
{
    // m_Viewer->paint();
}

void WQtGLWidget::resizeEvent( QResizeEvent* event )
{
    m_Viewer->resize( event->size().width(), event->size().height() );
}

void WQtGLWidget::resizeGL( int /* width */, int /* height */ )
{
    // m_Viewer->resize( width, height );
}
#endif

int WQtGLWidget::translateButton( QMouseEvent* event )
{
    switch( event->button() )
    {
        case( Qt::LeftButton ):
            return 1;
        case( Qt::MidButton ):
            return 2;
        case( Qt::RightButton ):
            return 3;
        default:
            return 0;
    }
}

void WQtGLWidget::keyPressEvent( QKeyEvent* event )
{
    if(  event->text() != "" )
    {
        m_Viewer->keyEvent( WGEViewer::KEYPRESS, *event->text().toAscii().data() );
    }
    else
    {
        switch( event->modifiers() )
        {
            case Qt::ShiftModifier :
                m_Viewer->keyEvent( WGEViewer::KEYPRESS, osgGA::GUIEventAdapter::KEY_Shift_L );
                break;
            case Qt::ControlModifier :
                m_Viewer->keyEvent( WGEViewer::KEYPRESS, osgGA::GUIEventAdapter::KEY_Control_L );
                break;
            default :
                break;
        }
    }
}

void WQtGLWidget::keyReleaseEvent( QKeyEvent* event )
{
    switch( event->key() )
    {
        case Qt::Key_Period:
            WGraphicsEngine::getGraphicsEngine()->requestShaderReload();
            break;
        case Qt::Key_1:
            setCameraManipulator( TRACKBALL );
            break;
        case Qt::Key_2:
            setCameraManipulator( TWO_D );
            break;
    }

    switch( event->modifiers() )
    {
        case Qt::ShiftModifier :
            m_Viewer->keyEvent( WGEViewer::KEYRELEASE, osgGA::GUIEventAdapter::KEY_Shift_L );
            break;
        case Qt::ControlModifier :
            m_Viewer->keyEvent( WGEViewer::KEYRELEASE, osgGA::GUIEventAdapter::KEY_Control_L );
            break;
    }

    m_Viewer->keyEvent( WGEViewer::KEYRELEASE, *event->text().toAscii().data() );
}


void WQtGLWidget::mousePressEvent( QMouseEvent* event )
{
    m_Viewer->mouseEvent( WGEViewer::MOUSEPRESS, event->x(), event->y(), translateButton( event ) );
}

void WQtGLWidget::mouseDoubleClickEvent( QMouseEvent* event )
{
    m_Viewer->mouseEvent( WGEViewer::MOUSEDOUBLECLICK, event->x(), event->y(), translateButton( event ) );
}

void WQtGLWidget::mouseReleaseEvent( QMouseEvent* event )
{
    m_Viewer->mouseEvent( WGEViewer::MOUSERELEASE, event->x(), event->y(), translateButton( event ) );
}

void WQtGLWidget::mouseMoveEvent( QMouseEvent* event )
{
    m_Viewer->mouseEvent( WGEViewer::MOUSEMOVE, event->x(), event->y(), 0 );
}

void WQtGLWidget::wheelEvent( QWheelEvent* event )
{
    int x, y;
    if( event->orientation() == Qt::Vertical )
    {
        x = 0;
        y = event->delta();
    }
    else
    {
        x = event->delta();
        y = 0;
    }
    m_Viewer->mouseEvent( WGEViewer::MOUSESCROLL, x, y, 0 );
}

void WQtGLWidget::reset()
{
    m_Viewer->reset();
}

const QGLFormat WQtGLWidget::getDefaultFormat()
{
    QGLFormat format;
    format.setSwapInterval( 1 );    // according to Qt Doc, this should enable VSync. But it doesn't.
    return format;
}

