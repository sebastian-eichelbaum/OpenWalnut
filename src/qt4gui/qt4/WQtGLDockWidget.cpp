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

#include <QtGui/QDockWidget>
#include <QtGui/QVBoxLayout>

#include "WQtGLDockWidget.h"
#include "WQtGLDockWidget.moc"

WQtGLDockWidget::WQtGLDockWidget( QString viewTitle, QString dockTitle, QWidget* parent, WGECamera::ProjectionMode projectionMode,
                                  const QWidget* shareWidget )
    : QDockWidget( dockTitle, parent )
{
    setObjectName( QString( "GL - " ) + dockTitle );

    setAllowedAreas( Qt::AllDockWidgetAreas );
    setFeatures( QDockWidget::AllDockWidgetFeatures );

    // the panel contains all other widgets, including the gl widget
    // This allows adding other widgets to certain docks
    m_panel = new QWidget( this );
    m_layout = new QVBoxLayout();

    m_glWidget = boost::shared_ptr<WQtGLWidget>( new WQtGLWidget( viewTitle.toStdString(), m_panel, projectionMode, shareWidget ) );

    setMinimumSize( 240, 240 );
    //setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Maximum );

    // add panel to layout.
    m_layout->addWidget( m_glWidget.get() );
    m_panel->setLayout( m_layout );
    setWidget( m_panel );

    // we need to know whether the dock is visible or not
    connect( this, SIGNAL( visibilityChanged( bool ) ), this, SLOT( handleVisibilityChange( bool ) ) );
}

WQtGLDockWidget::~WQtGLDockWidget()
{
    // cleanup
}

boost::shared_ptr<WQtGLWidget>WQtGLDockWidget::getGLWidget() const
{
    return m_glWidget;
}

void WQtGLDockWidget::handleVisibilityChange( bool visible )
{
    // this can help to reduce CPU load. Especially if multithreading viewers are used with cull thread per context.
    m_glWidget->getViewer()->getView()->getScene()->getSceneData()->setNodeMask( visible * 0xFFFFFFFF );
}

