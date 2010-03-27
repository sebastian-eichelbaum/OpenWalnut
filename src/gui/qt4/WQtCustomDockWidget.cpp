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

#include <QtGui/QCloseEvent>

#include "WQtCustomDockWidget.h"
#include "../../graphicsEngine/WGEViewer.h"

WQtCustomDockWidget::WQtCustomDockWidget( std::string title, QWidget* parent, WGECamera::ProjectionMode projectionMode )
    : QDockWidget( QString::fromStdString( title ), parent ),
      m_useCount( 1 )
{
    // setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
    setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
    setFloating( true );
    resize( 1024, 768 );

    m_glWidget = boost::shared_ptr< WQtGLWidget >( new WQtGLWidget( title, this, projectionMode ) );
    m_glWidget->initialize();
    m_scene = new WGEGroupNode();
    m_scene->setDataVariance( osg::Object::DYNAMIC );
    m_glWidget->getViewer()->setScene( m_scene );

    setWidget( m_glWidget.get() );
}

osg::ref_ptr< WGEGroupNode > WQtCustomDockWidget::getScene() const
{
    return m_scene;
}

boost::shared_ptr< WGEViewer > WQtCustomDockWidget::getViewer() const
{
    return m_glWidget->getViewer();
}

void WQtCustomDockWidget::increaseUseCount()
{
    ++m_useCount;
}

bool WQtCustomDockWidget::decreaseUseCount()
{
    --m_useCount;
    bool shouldClose = ( m_useCount == 0 );
    if( shouldClose )
    {
        close();
    }
    return shouldClose;
}

void WQtCustomDockWidget::closeEvent( QCloseEvent* event )
{
    // forward events
    m_glWidget->close();

    event->accept();
}
