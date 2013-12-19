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

#include "core/graphicsEngine/WGEViewerEffect.h"

#include "WQt4Gui.h"
#include "WMainWindow.h"

#include "WSettingAction.h"

#include "guiElements/WQtDockWidget.h"
#include "controlPanel/WQtPropertyGroupWidget.h"

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
    m_layout = new QVBoxLayout( m_panel );
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

    // get the viewer background effect
    WGEViewerEffect::SPtr bkEffect = m_glWidget->getViewer()->getBackground();
    WGEViewerEffect::SPtr vignetteEffect = m_glWidget->getViewer()->getVignette();
    WGEViewerEffect::SPtr overlayEffect = m_glWidget->getViewer()->getImageOverlay();

    // create property widgets for each effect
    QWidget* bkWidget = WQtPropertyGroupWidget::createPropertyGroupBox( bkEffect->getProperties() );
    QWidget* vignetteWidget = WQtPropertyGroupWidget::createPropertyGroupBox( vignetteEffect->getProperties() );
    QWidget* overlayWidget = WQtPropertyGroupWidget::createPropertyGroupBox( overlayEffect->getProperties() );

    // create container for all the config widgets
    QWidget* viewConfigWidget = new QWidget();
    QVBoxLayout* viewConfigLayout = new QVBoxLayout();
    viewConfigLayout->setAlignment( Qt::AlignTop );
    viewConfigWidget->setLayout( viewConfigLayout );

    // force the widget to shrink when the content shrinks.
    QSizePolicy sizePolicy( QSizePolicy::Preferred, QSizePolicy::Maximum );
    sizePolicy.setHorizontalStretch( 0 );
    sizePolicy.setVerticalStretch( 0 );
    viewConfigWidget->setSizePolicy( sizePolicy );

    // add the property widgets to container
    viewConfigLayout->addWidget( bkWidget );
    viewConfigLayout->addWidget( vignetteWidget );
    viewConfigLayout->addWidget( overlayWidget );

    // Create the toolbutton and the menu containing the config widgets
    QWidgetAction* viewerConfigWidgetAction = new QWidgetAction( this );
    viewerConfigWidgetAction->setDefaultWidget( viewConfigWidget );
    QMenu* viewerConfigMenu = new QMenu();
    viewerConfigMenu->addAction( viewerConfigWidgetAction );
    QToolButton* viewerConfigBtn = new QToolButton( parent );
    viewerConfigBtn->setPopupMode( QToolButton::InstantPopup );
    viewerConfigBtn->setIcon(  WQt4Gui::getMainWindow()->getIconManager()->getIcon( "configure" ) );
    viewerConfigBtn->setToolTip( "Configure View" );
    viewerConfigBtn->setMenu( viewerConfigMenu );

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
    addTitleButton( viewerConfigBtn );
    addTitleButton( settingsBtn );
}

WQtGLDockWidget::~WQtGLDockWidget()
{
    // cleanup
}

void WQtGLDockWidget::saveSettings()
{
    m_screenCapture->saveSettings();
    WQtDockWidget::saveSettings();
}

void WQtGLDockWidget::restoreSettings()
{
    m_screenCapture->restoreSettings();
    WQtDockWidget::restoreSettings();
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
    WQtDockWidget::closeEvent( event );
}

void WQtGLDockWidget::showEvent( QShowEvent* event )
{
    getGLWidget()->getViewer()->setClosed( false );
    WQtDockWidget::showEvent( event );
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
