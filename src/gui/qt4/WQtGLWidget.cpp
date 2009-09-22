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

#include <iostream>

#include <QtGui/QKeyEvent>

#include "WQtGLWidget.h"

#include "../../graphicsEngine/WGEViewer.h"
#include "../../kernel/WKernel.h"


WQtGLWidget::WQtGLWidget( QWidget* parent )
    : QWidget( parent ),
      m_recommendedSize()
{
    m_recommendedSize.setWidth( 200 );
    m_recommendedSize.setHeight( 200 );

    // initialize OpenGL context and OpenSceneGraph

#if defined(__APPLE__)
    // Extract a WindowPtr from the HIViewRef that QWidget::winId() returns.
    // Without this change, the peer tries to call GetWindowPort on the HIViewRef
    // which returns 0 and we only render white.
    osg::ref_ptr<WindowData> wdata = osg::ref_ptr<WindowData>(
            new WindowData( HIViewGetWindow( static_cast<HIViewRef>winId() ) )
    );
#else  // all others
    osg::ref_ptr<WindowData> wdata = osg::ref_ptr<WindowData>( new WindowData( winId() ) );
#endif

    // create viewer
    m_Viewer = WKernel::getRunningKernel()->getGraphicsEngine()->createViewer( wdata, x(), y(), width(), height() );

    // required
    setAttribute( Qt::WA_PaintOnScreen );
    setAttribute( Qt::WA_NoSystemBackground );
    setFocusPolicy( Qt::ClickFocus );
}

WQtGLWidget::~WQtGLWidget()
{
    // cleanup
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
        case DRIVE:
            std::cout << "selected drive manipulator" << std::endl;

            m_Viewer->setCameraManipulator( new( osgGA::DriveManipulator ) );
            break;
        case FLIGHT:
            std::cout << "selected flight manipulator" << std::endl;

            m_Viewer->setCameraManipulator( new( osgGA::FlightManipulator ) );
            break;
        case TERRAIN:
            std::cout << "selected terrain manipulator" << std::endl;

            m_Viewer->setCameraManipulator( new( osgGA::TerrainManipulator ) );
            break;
        case UFO:
            std::cout << "selected ufo manipulator" << std::endl;

            m_Viewer->setCameraManipulator( new( osgGA::UFOManipulator ) );
            break;
        case TRACKBALL:
        default:
            std::cout << "selected trackball manipulator" << std::endl;

            m_Viewer->setCameraManipulator( new( osgGA::TrackballManipulator ) );
            break;
    }
}

WQtGLWidget::CameraManipulators WQtGLWidget::getCameraManipulators()
{
    return m_CurrentManipulator;
}

void WQtGLWidget::paintEvent( QPaintEvent* /*event*/ )
{
    // m_Viewer->paint();
}

#ifndef WIN32
void WQtGLWidget::destroyEvent( bool /*destroyWindow*/, bool /*destroySubWindows*/ )
{
    // forward events
    m_Viewer->close();
}


void WQtGLWidget::closeEvent( QCloseEvent* event )
{
    event->accept();

    // forward events
    m_Viewer->close();
}


void WQtGLWidget::resizeEvent( QResizeEvent* event )
{
    m_Viewer->resize( event->size().width(), event->size().height() );
}

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
        case( Qt::NoButton ):
            return 0;
        default:
            return 0;
    }
}

void WQtGLWidget::keyPressEvent( QKeyEvent* event )
{
    m_Viewer->keyEvent( WGEViewer::KEYPRESS, *event->text().toAscii().data() );
}

void WQtGLWidget::keyReleaseEvent( QKeyEvent* event )
{
    switch( event->key() )
    {
        case Qt::Key_1:
            setCameraManipulator( TRACKBALL );
            break;
        case Qt::Key_2:
            setCameraManipulator( FLIGHT );
            break;
        case Qt::Key_3:
            setCameraManipulator( DRIVE );
            break;
        case Qt::Key_4:
            setCameraManipulator( TERRAIN );
            break;
        case Qt::Key_5:
            setCameraManipulator( UFO );
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
#endif

