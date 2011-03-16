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

#include <osgDB/WriteFile>

#include "WQtGLWidgetAll.h"

#include "../../../common/WConditionOneShot.h"
#include "../../../common/WFlag.h"
#include "../../../common/WLogger.h"
#include "../../../graphicsEngine/WGE2DManipulator.h"
#include "../../../graphicsEngine/WGEViewer.h"
#include "../../../graphicsEngine/WGEZoomTrackballManipulator.h"
#include "../WQtGLScreenCapture.h"
#include "../WMainWindow.h"
#include "../../../kernel/WKernel.h"

#ifndef _MSC_VER
#include <osgViewer/api/X11/GraphicsWindowX11>
typedef osgViewer::GraphicsWindowX11::WindowData WindowData;
#else
#include <osgViewer/api/Win32/GraphicsWindowWin32>
typedef osgViewer::GraphicsWindowWin32::WindowData WindowData;
#endif


WQtGLWidgetAll::WQtGLWidgetAll( std::string nameOfViewer, QWidget* parent, WGECamera::ProjectionMode projectionMode, const QGLWidget * shareWidget )
    : QGLWidget( getDefaultFormat(), parent, shareWidget ),
      m_nameOfViewer( nameOfViewer ),
      m_recommendedSize()
{
    m_recommendedSize.setWidth( 200 );
    m_recommendedSize.setHeight( 200 );

    m_initialProjectionMode = projectionMode;

    // required
    setAttribute( Qt::WA_PaintOnScreen );
    setAttribute( Qt::WA_NoSystemBackground );
    setFocusPolicy( Qt::ClickFocus );

    // initialize OpenGL context and OpenSceneGraph
    osg::ref_ptr<osg::Referenced> wdata = new WindowData( winId() );

    // create viewer
    m_Viewer = WKernel::getRunningKernel()->getGraphicsEngine()->createViewer(
        m_nameOfViewer, wdata, x(), y(), width(), height(), m_initialProjectionMode );
}

WQtGLWidgetAll::~WQtGLWidgetAll()
{
    WKernel::getRunningKernel()->getGraphicsEngine()->closeViewer( m_nameOfViewer );
    m_Viewer.reset();
}

QSize WQtGLWidgetAll::sizeHint() const
{
    return m_recommendedSize;
}

void WQtGLWidgetAll::setCameraManipulator( WQtGLWidgetAll::CameraManipulators manipulator )
{
    m_CurrentManipulator = manipulator;
    switch ( manipulator )
    {
        case TWO_D:
            WLogger::getLogger()->addLogMessage( "Switched to OSG manipulator \"WGE2D\".",
                                                 "WQtGLWidgetAll(" + m_Viewer->getName() + ")",
                                                 LL_DEBUG );

            m_Viewer->setCameraManipulator( new( WGE2DManipulator ) );
            break;
        case TRACKBALL:
        default:
            WLogger::getLogger()->addLogMessage( "Switched to OSG manipulator \"WGETrackball\".",
                                                 "WQtGLWidgetAll(" + m_Viewer->getName() + ")",
                                                 LL_DEBUG );

            m_Viewer->setCameraManipulator( new( WGEZoomTrackballManipulator ) );
            break;
    }
}

void WQtGLWidgetAll::setBgColor( const WColor& bgColor )
{
    m_Viewer->setBgColor( bgColor );
}

WQtGLWidgetAll::CameraManipulators WQtGLWidgetAll::getCameraManipulators()
{
    return m_CurrentManipulator;
}

boost::shared_ptr< WGEViewer > WQtGLWidgetAll::getViewer() const
{
    return m_Viewer;
}

void WQtGLWidgetAll::paintEvent( QPaintEvent* /*event*/ )
{
    // TODO(math): Remove this if there are now startup segfaults
    //   ^--- Or maybe not but producing some :), since its not initialized :D haha
    // m_Viewer->paint();
}

void WQtGLWidgetAll::resizeEvent(  QResizeEvent* event )
{
    m_Viewer->resize( event->size().width(), event->size().height() );
}

int WQtGLWidgetAll::translateButton( QMouseEvent* event )
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

void WQtGLWidgetAll::keyPressEvent( QKeyEvent* event )
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

void WQtGLWidgetAll::keyReleaseEvent( QKeyEvent* event )
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


void WQtGLWidgetAll::mousePressEvent( QMouseEvent* event )
{
    m_Viewer->mouseEvent( WGEViewer::MOUSEPRESS, event->x(), event->y(), translateButton( event ) );
}

void WQtGLWidgetAll::mouseDoubleClickEvent( QMouseEvent* event )
{
    m_Viewer->mouseEvent( WGEViewer::MOUSEDOUBLECLICK, event->x(), event->y(), translateButton( event ) );
}

void WQtGLWidgetAll::mouseReleaseEvent( QMouseEvent* event )
{
    m_Viewer->mouseEvent( WGEViewer::MOUSERELEASE, event->x(), event->y(), translateButton( event ) );
}

void WQtGLWidgetAll::mouseMoveEvent( QMouseEvent* event )
{
    m_Viewer->mouseEvent( WGEViewer::MOUSEMOVE, event->x(), event->y(), 0 );
}

void WQtGLWidgetAll::wheelEvent( QWheelEvent* event )
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

void WQtGLWidgetAll::reset()
{
    m_Viewer->reset();
}

const QGLFormat WQtGLWidgetAll::getDefaultFormat()
{
    QGLFormat format;
    format.setSwapInterval( 1 );    // according to Qt Doc, this should enable VSync. But it doesn't.
    return format;
}

WQtGLScreenCapture* WQtGLWidgetAll::getScreenCapture( WMainWindow* parent )
{
    WQtGLScreenCapture* sc = new WQtGLScreenCapture( getViewer(), parent );
    return sc;
}

