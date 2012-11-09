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

#include <QtGui/QAction>
#include <QtGui/QDockWidget>
#include <QtGui/QVBoxLayout>

#include "WQt4Gui.h"

#include "WSettingAction.h"

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
    m_layout->setContentsMargins( 1, 1, 1, 1 );

    m_glWidget = boost::shared_ptr<WQtGLWidget>( new WQtGLWidget( viewTitle.toStdString(), m_panel, projectionMode, shareWidget ) );

    // NOTE: do not remove this. When creating custom widgets using the OSG manipulators, a too small size here (or even no min size) causes the
    // cull visitor to do crap ... unknown reason ...
    setMinimumSize( 50, 50 );

    // add panel to layout.
    m_layout->addWidget( m_glWidget.get() );
    m_panel->setLayout( m_layout );
    setWidget( m_panel );

    // we need to know whether the dock is visible or not
    connect( this, SIGNAL( visibilityChanged( bool ) ), this, SLOT( handleVisibilityChange( bool ) ) );

    // create the dock widget context menu

    // important: do not use this menu for the m_panel widget but ensure the right click event to be sent to the widget
    m_panel->setContextMenuPolicy( Qt::PreventContextMenu );
    setContextMenuPolicy( Qt::ActionsContextMenu );

    // reset the scene
    QAction* resetButton = new QAction( WQt4Gui::getIconManager()->getIcon( "view" ), "Reset", this );
    connect( resetButton, SIGNAL(  triggered( bool ) ), m_glWidget.get(), SLOT( reset() ) );
    addAction( resetButton );

    // camera presets
    QAction* camPresets = new QAction( WQt4Gui::getIconManager()->getIcon( "view" ), "Camera Presets", this );
    camPresets->setMenu( getGLWidget()->getCameraPresetsMenu() );
    addAction( camPresets );

    // throwing
    addAction( getGLWidget()->getThrowingSetting() );

    // change background color
    addAction( getGLWidget()->getBackgroundColorAction() );
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

