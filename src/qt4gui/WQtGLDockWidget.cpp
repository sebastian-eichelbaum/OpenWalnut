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

#include <QtGui/QAction>
#include <QtGui/QWidgetAction>
#include <QtGui/QDockWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QToolButton>
#include <QtGui/QToolBar>

#include "WQt4Gui.h"
#include "WMainWindow.h"

#include "WSettingAction.h"

#include "guiElements/WQtDockWidget.h"

#include "WQtGLDockWidget.h"
#include "WQtGLDockWidget.moc"


WQtGLDockWidget::WQtGLDockWidget( QString viewTitle, QString dockTitle, QWidget* parent, WGECamera::ProjectionMode projectionMode,
                                  const QWidget* shareWidget ):
    WQtDockWidget( viewTitle, parent ),
    m_dockTitle( dockTitle )
{
    setObjectName( QString( "GL - " ) + dockTitle );

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

    // all view docks have a screen capture object
    m_screenCapture = new WQtGLScreenCapture( this );

    // screen capture trigger
    QWidgetAction* screenCaptureWidgetAction = new QWidgetAction( this );
    screenCaptureWidgetAction->setDefaultWidget( m_screenCapture );
    QMenu* screenCaptureMenu = new QMenu();
    screenCaptureMenu->addAction( screenCaptureWidgetAction );
    QToolButton* screenShotBtn = new QToolButton( parent );
    screenShotBtn->setDefaultAction( m_screenCapture->getScreenshotTrigger() );
    screenShotBtn->setPopupMode( QToolButton::MenuButtonPopup );
    screenShotBtn->setMenu( screenCaptureMenu );

    // camera presets
    QToolButton* presetBtn = new QToolButton( parent );
    presetBtn->setDefaultAction( getGLWidget()->getCameraResetAction() );
    presetBtn->setMenu(  getGLWidget()->getCameraPresetsMenu() );
    presetBtn->setPopupMode( QToolButton::MenuButtonPopup );

    QToolButton* settingsBtn = new QToolButton( parent );
    settingsBtn->setPopupMode( QToolButton::InstantPopup );
    settingsBtn->setIcon(  WQt4Gui::getMainWindow()->getIconManager()->getIcon( "configure" ) );
    settingsBtn->setToolTip( "Settings" );
    QMenu* settingsMenu = new QMenu( parent );
    settingsBtn->setMenu( settingsMenu );

    // throwing
    settingsMenu->addAction( getGLWidget()->getThrowingSetting() );

    // change background color
    settingsMenu->addAction( getGLWidget()->getBackgroundColorAction() );

    // add them to the title
    addTitleButton( screenShotBtn );
    addTitleButton( presetBtn );
    addTitleButton( settingsBtn );
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

void WQtGLDockWidget::closeEvent( QCloseEvent *event )
{
    getGLWidget()->getViewer()->setClosed( true );
    QDockWidget::closeEvent( event );
}

void WQtGLDockWidget::showEvent( QShowEvent* event )
{
    getGLWidget()->getViewer()->setClosed( false );
    QDockWidget::showEvent( event );
}

WQtGLScreenCapture* WQtGLDockWidget::getScreenCapture()
{
    return m_screenCapture;
}

void WQtGLDockWidget::forceGLWidgetSize( size_t w, size_t h )
{
    m_glWidget->setFixedSize( w, h );
}

void WQtGLDockWidget::restoreGLWidgetSize()
{
    m_glWidget->setMinimumSize( 0, 0 );
    m_glWidget->setMaximumSize( QWIDGETSIZE_MAX, QWIDGETSIZE_MAX );
    m_panel->setMinimumSize( 0, 0 );
    m_panel->setMaximumSize( QWIDGETSIZE_MAX, QWIDGETSIZE_MAX );
    this->setMinimumSize( 0, 0 );
    this->setMaximumSize( QWIDGETSIZE_MAX, QWIDGETSIZE_MAX );
}

const QString& WQtGLDockWidget::getDockTitle() const
{
    return m_dockTitle;
}

void WQtGLDockWidget::openScreenCaptureConfig()
{
    m_screenCapture->setWindowFlags( Qt::Tool );
    m_screenCapture->show();
}
