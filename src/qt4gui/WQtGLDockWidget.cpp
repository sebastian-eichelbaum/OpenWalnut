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
#include <QtGui/QHBoxLayout>
#include <QtGui/QToolButton>

#include "WQt4Gui.h"
#include "WMainWindow.h"

#include "WSettingAction.h"

#include "WQtGLDockWidget.h"
#include "WQtGLDockWidget.moc"


WQtGLDockWidget::WQtGLDockWidget( QString viewTitle, QString dockTitle, QWidget* parent, WGECamera::ProjectionMode projectionMode,
                                  const QWidget* shareWidget )
    : QDockWidget( dockTitle, parent ),
    m_dockTitle( dockTitle )
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

    // all view docks have a screen capture object
    m_screenCapture = new WQtGLScreenCapture( this );
    // hide the screen capture object by default
    m_screenCapture->setHidden( true );

    // set custom title
    setTitleBarWidget( new WQtGLDockWidgetTitle( this, dockTitle ) );
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

/**
 * Apply default settings for dock widget title buttons
 *
 * \param btn the button to setup
 */
void setupButton( QToolButton* btn )
{
    btn->setToolButtonStyle( Qt::ToolButtonIconOnly );
    btn->setContentsMargins( 0, 0, 0, 0 );
    btn->setFixedWidth( 24 );
    btn->setFixedHeight( 24 );
}

WQtGLDockWidgetTitle::WQtGLDockWidgetTitle( WQtGLDockWidget* parent, const QString& dockTitle ):
    QWidget( parent )
{
    // screen capture trigger
    QToolButton* screenShotBtn = new QToolButton( this );
    screenShotBtn->setDefaultAction( parent->getScreenCapture()->getScreenshotTrigger() );
    setupButton( screenShotBtn );

    // camera presets
    QAction* camPresets = new QAction( WQt4Gui::getIconManager()->getIcon( "view" ), "Camera Presets", this );
    camPresets->setMenu( parent->getGLWidget()->getCameraPresetsAndResetMenu() );
    QToolButton* presetBtn = new QToolButton( this );
    presetBtn->setDefaultAction( camPresets );
    presetBtn->setPopupMode( QToolButton::InstantPopup );
    setupButton( presetBtn );

    QToolButton* settingsBtn = new QToolButton( this );
    settingsBtn->setPopupMode( QToolButton::InstantPopup );
    settingsBtn->setIcon(  WQt4Gui::getMainWindow()->getIconManager()->getIcon( "configure" ) );
    setupButton( settingsBtn );
    settingsBtn->setToolTip( "Settings" );
    QMenu* settingsMenu = new QMenu( this );
    settingsBtn->setMenu( settingsMenu );

    // throwing
    settingsMenu->addAction( parent->getGLWidget()->getThrowingSetting() );

    // change background color
    settingsMenu->addAction( parent->getGLWidget()->getBackgroundColorAction() );

    // close Btn
    QToolButton* closeBtn = new QToolButton( this );
    QAction* act = new QAction( WQt4Gui::getMainWindow()->getIconManager()->getIcon( "popup_close" ), "Close", this );
    connect( act, SIGNAL( triggered( bool ) ), parent, SLOT( close() ) );
    closeBtn->setDefaultAction( act );
    setupButton( closeBtn );

    // title
    QLabel* title = new QLabel( " " + dockTitle, this );

    // build layout
    QHBoxLayout* layout = new QHBoxLayout( this );
    layout->setMargin( 0 );
    layout->setSpacing( 0 );

    layout->addWidget( title );
    layout->addStretch( 100000 );
    layout->addWidget( screenShotBtn );
    layout->addWidget( presetBtn );
    layout->addWidget( settingsBtn );
    layout->addWidget( closeBtn );

    setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed ) );

    setStyleSheet(
            "QToolButton{"
            "border-style: none;"
            "}"
            "QPushButton{"
            "border-style: none;"
            "}"
    );
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
    m_glWidget->setMinimumHeight( 250 );
    m_glWidget->setMaximumHeight( QWIDGETSIZE_MAX );
    m_glWidget->setMinimumWidth( 250 );
    m_glWidget->setMaximumWidth( QWIDGETSIZE_MAX );
}

const QString& WQtGLDockWidget::getDockTitle() const
{
    return m_dockTitle;
}
