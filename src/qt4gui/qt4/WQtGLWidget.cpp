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

#include <QtGui/QApplication>
#include <QtGui/QColorDialog>
#include <QtGui/QFileDialog>
#include <QtGui/QKeyEvent>
#include <QtGui/QPixmap>

#include "WQtGLWidget.h"
#include "WQtGLWidget.moc"

#include "core/common/WConditionOneShot.h"
#include "core/common/WFlag.h"
#include "core/common/WLogger.h"
#include "core/common/WColor.h"
#include "core/graphicsEngine/WGE2DManipulator.h"
#include "core/graphicsEngine/WGENoOpManipulator.h"
#include "core/graphicsEngine/WGEViewer.h"
#include "core/graphicsEngine/WGEZoomTrackballManipulator.h"
#include "core/graphicsEngine/WGraphicsEngine.h"
#include "core/graphicsEngine/WGraphicsEngineMode.h"
#include "core/kernel/WKernel.h"

#include "WQtGLScreenCapture.h"
#include "events/WRenderedFrameEvent.h"
#include "events/WEventTypes.h"
#include "WSettingAction.h"
#include "WMainWindow.h"

#ifdef WGEMODE_MULTITHREADED
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
#ifdef WGEMODE_MULTITHREADED
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

    m_Viewer->isFrameRendered()->getCondition()->subscribeSignal( boost::bind( &WQtGLWidget::notifyFirstRenderedFrame, this ) );

    // set bg color
    updateViewerBackground();
    // this action manages the above settings
    m_changeBGColorAction = new QAction( QString::fromStdString( nameOfViewer ), parent );
    connect( m_changeBGColorAction, SIGNAL( triggered( bool ) ), this, SLOT( changeBGColor() ) );

    // enable throwing of wanted
    m_allowThrowSetting = new WSettingAction( parent, std::string( "qt4gui/" ) + nameOfViewer + std::string( "/allowThrow" ),
                                                      "Camera Throwing",
                                                      "If enabled, the camera can be thrown. Try it by dragging the camera. "
                                                      "The camera then continues the movement.",
                                                      false );
    connect( m_allowThrowSetting, SIGNAL( change( bool ) ), this, SLOT( updateThrowing() ) );
    updateThrowing();
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
    switch( manipulator )
    {
        case TWO_D:
            WLogger::getLogger()->addLogMessage( "Switched to OSG manipulator \"WGE2D\".",
                                                 "WQtGLWidget(" + m_Viewer->getName() + ")",
                                                 LL_DEBUG );

            m_Viewer->setCameraManipulator( new( WGE2DManipulator ) );
            break;
        case NO_OP:
            WLogger::getLogger()->addLogMessage( "Switched to OSG manipulator \"WGENoOp\".",
                                                 "WQtGLWidget(" + m_Viewer->getName() + ")",
                                                 LL_DEBUG );

            m_Viewer->setCameraManipulator( new( WGENoOpManipulator ) );
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

#ifdef WGEMODE_SINGLETHREADED

void WQtGLWidget::paintGL()
{
    m_Viewer->paint();
}

void WQtGLWidget::resizeEvent( QResizeEvent* event )
{
    // we must hand over the event to our base class because that one takes care
    // that resizeGL is eventually called
    WQtGLWidgetParent::resizeEvent( event );
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

    WQtGLWidgetParent::resizeEvent( event );
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

bool WQtGLWidget::event( QEvent* event )
{
    if( event->type() == WQT_RENDERED_FRAME_EVENT )
    {
        emit renderedFirstFrame();
        return true;
    }

    return WQtGLWidgetParent::event( event );
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

void WQtGLWidget::updateThrowing()
{
    WGEZoomTrackballManipulator* manipulator = dynamic_cast< WGEZoomTrackballManipulator* >( m_Viewer->getCameraManipulator().get() );
    if( manipulator )
    {
        manipulator->setThrow( m_allowThrowSetting->get() );
    }
}

WSettingAction* WQtGLWidget::getThrowingSetting() const
{
    return m_allowThrowSetting;
}

QAction* WQtGLWidget::getBackgroundColorAction() const
{
    return m_changeBGColorAction;
}

void WQtGLWidget::updateViewerBackground()
{
    QColor bg = WMainWindow::getSettings().value( QString( "qt4gui/" ) + QString::fromStdString( m_nameOfViewer ) + QString( "/BGColor" ),
                                                  QColor( 255, 255, 255, 255 ) ).value< QColor >();
    m_Viewer->setBgColor( WColor( bg.redF(), bg.greenF(), bg.blueF(), 1.0 ) );
}

void WQtGLWidget::changeBGColor()
{
    QColor bgOld = WMainWindow::getSettings().value( QString( "qt4gui/" ) + QString::fromStdString( m_nameOfViewer ) + QString( "/BGColor" ),
                                                  QColor( 255, 255, 255, 255 ) ).value< QColor >();
    QColor bg = QColorDialog::getColor( bgOld, this );
    WMainWindow::getSettings().setValue( QString( "qt4gui/" ) + QString::fromStdString( m_nameOfViewer ) + QString( "/BGColor" ), bg );

    updateViewerBackground();
}

void WQtGLWidget::notifyFirstRenderedFrame()
{
    QCoreApplication::postEvent( this, new WRenderedFrameEvent() );
}

WQtGLScreenCapture* WQtGLWidget::getScreenCapture( WMainWindow* parent ) const
{
    WQtGLScreenCapture* sc = new WQtGLScreenCapture( getViewer(), parent );
    return sc;
}

