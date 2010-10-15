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

#include "WQtGLWidgetMac.h"

#include "../../../graphicsEngine/WGE2DManipulator.h"
#include "../../../graphicsEngine/platformDependent/WGEViewerMac.h"
#include "../../../graphicsEngine/platformDependent/WGraphicsEngineMac.h"
#include "../../../graphicsEngine/WGEZoomTrackballManipulator.h"
#include "../../../common/WFlag.h"
#include "../../../common/WLogger.h"
#include "../../../common/WConditionOneShot.h"
#include "../../../kernel/WKernel.h"


WQtGLWidgetMac::WQtGLWidgetMac( std::string nameOfViewer, QWidget* parent, WGECamera::ProjectionMode projectionMode, const QGLWidget * shareWidget )
    : QGLWidget( parent, shareWidget ),
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

    // create viewer
    m_Viewer = WKernel::getRunningKernel()->getGraphicsEngine()->createViewer(
        m_nameOfViewer, x(), y(), width(), height(), m_initialProjectionMode );

    connect( &m_Timer, SIGNAL( timeout() ), this, SLOT( updateGL() ) );
    m_Timer.start( 10 );
}

WQtGLWidgetMac::~WQtGLWidgetMac()
{
    WKernel::getRunningKernel()->getGraphicsEngine()->closeViewer( m_nameOfViewer );
    m_Viewer.reset();
}

QSize WQtGLWidgetMac::sizeHint() const
{
    return m_recommendedSize;
}

void WQtGLWidgetMac::setCameraManipulator( WQtGLWidgetMac::CameraManipulators manipulator )
{
    m_CurrentManipulator = manipulator;
    switch ( manipulator )
    {
        case DRIVE:
            WLogger::getLogger()->addLogMessage( "Switched to OSG manipulator \"drive\".",
                                                 "WQtGLWidgetMac(" + m_Viewer->getName() + ")",
                                                 LL_DEBUG );

            m_Viewer->setCameraManipulator( new( osgGA::DriveManipulator ) );
            break;
        case FLIGHT:
            WLogger::getLogger()->addLogMessage( "Switched to OSG manipulator \"flight\".",
                                                 "WQtGLWidgetMac(" + m_Viewer->getName() + ")",
                                                 LL_DEBUG );

            m_Viewer->setCameraManipulator( new( osgGA::FlightManipulator ) );
            break;
        case TERRAIN:
            WLogger::getLogger()->addLogMessage( "Switched to OSG manipulator \"terrain\".",
                                                 "WQtGLWidgetMac(" + m_Viewer->getName() + ")",
                                                 LL_DEBUG );

            m_Viewer->setCameraManipulator( new( osgGA::TerrainManipulator ) );
            break;
        case UFO:
            WLogger::getLogger()->addLogMessage( "Switched to OSG manipulator \"ufo\".",
                                                 "WQtGLWidgetMac(" + m_Viewer->getName() + ")",
                                                 LL_DEBUG );

            m_Viewer->setCameraManipulator( new( osgGA::UFOManipulator ) );
            break;
        case TWO_D:
            WLogger::getLogger()->addLogMessage( "Switched to OSG manipulator \"WGE2D\".",
                                                 "WQtGLWidgetMac(" + m_Viewer->getName() + ")",
                                                 LL_DEBUG );

            m_Viewer->setCameraManipulator( new( WGE2DManipulator ) );
            break;
        case TRACKBALL:
        default:
            WLogger::getLogger()->addLogMessage( "Switched to OSG manipulator \"WGETrackball\".",
                                                 "WQtGLWidgetMac(" + m_Viewer->getName() + ")",
                                                 LL_DEBUG );

            m_Viewer->setCameraManipulator( new( WGEZoomTrackballManipulator ) );
            break;
    }
}

void WQtGLWidgetMac::setBgColor( WColor bgColor )
{
    m_Viewer->setBgColor( bgColor );
}

WQtGLWidgetMac::CameraManipulators WQtGLWidgetMac::getCameraManipulators()
{
    return m_CurrentManipulator;
}

boost::shared_ptr< WGEViewerMac > WQtGLWidgetMac::getViewer() const
{
    return m_Viewer;
}

void WQtGLWidgetMac::paintGL()
{
    m_Viewer->paint();
}

void WQtGLWidgetMac::resizeGL( int width, int height )
{
    m_Viewer->resize( width, height );
}

int WQtGLWidgetMac::translateButton( QMouseEvent* event )
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

void WQtGLWidgetMac::keyPressEvent( QKeyEvent* event )
{
    if(  event->text() != "" )
    {
        m_Viewer->keyEvent( WGEViewerMac::KEYPRESS, *event->text().toAscii().data() );
    }
    else
    {
        switch( event->modifiers() )
        {
            case Qt::ShiftModifier :
                m_Viewer->keyEvent( WGEViewerMac::KEYPRESS, osgGA::GUIEventAdapter::KEY_Shift_L );
                break;
            case Qt::ControlModifier :
                m_Viewer->keyEvent( WGEViewerMac::KEYPRESS, osgGA::GUIEventAdapter::KEY_Control_L );
                break;
            default :
                break;
        }
    }
}

void WQtGLWidgetMac::keyReleaseEvent( QKeyEvent* event )
{
    switch( event->key() )
    {
        case Qt::Key_Period:
            WGraphicsEngineMac::getGraphicsEngine()->requestShaderReload();
            break;
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
        case Qt::Key_6:
            setCameraManipulator( TWO_D );
            break;
    }

    switch( event->modifiers() )
    {
        case Qt::ShiftModifier :
            m_Viewer->keyEvent( WGEViewerMac::KEYRELEASE, osgGA::GUIEventAdapter::KEY_Shift_L );
            break;
        case Qt::ControlModifier :
            m_Viewer->keyEvent( WGEViewerMac::KEYRELEASE, osgGA::GUIEventAdapter::KEY_Control_L );
            break;
    }

    m_Viewer->keyEvent( WGEViewerMac::KEYRELEASE, *event->text().toAscii().data() );
}


void WQtGLWidgetMac::mousePressEvent( QMouseEvent* event )
{
    m_Viewer->mouseEvent( WGEViewerMac::MOUSEPRESS, event->x(), event->y(), translateButton( event ) );
}

void WQtGLWidgetMac::mouseDoubleClickEvent( QMouseEvent* event )
{
    m_Viewer->mouseEvent( WGEViewerMac::MOUSEDOUBLECLICK, event->x(), event->y(), translateButton( event ) );
}

void WQtGLWidgetMac::mouseReleaseEvent( QMouseEvent* event )
{
    m_Viewer->mouseEvent( WGEViewerMac::MOUSERELEASE, event->x(), event->y(), translateButton( event ) );
}

void WQtGLWidgetMac::mouseMoveEvent( QMouseEvent* event )
{
    m_Viewer->mouseEvent( WGEViewerMac::MOUSEMOVE, event->x(), event->y(), 0 );
}

void WQtGLWidgetMac::wheelEvent( QWheelEvent* event )
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
    m_Viewer->mouseEvent( WGEViewerMac::MOUSESCROLL, x, y, 0 );
}
