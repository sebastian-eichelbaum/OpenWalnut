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
    btn->setFixedHeight( 24 );
    btn->setAutoRaise( true );
}

void WQtGLDockWidgetTitle::fillToolLayout( QWidget* parent, QBoxLayout* layout, QMenu* screenShotConfigMenu )
{
    QFrame* line = new QFrame();
    line->setFrameShape( QFrame::VLine );
    line->setFrameShadow( QFrame::Sunken );

    // screen capture trigger
    QToolButton* screenShotBtn = new QToolButton( parent );
    screenShotBtn->setDefaultAction( m_dock->getScreenCapture()->getScreenshotTrigger() );
    setupButton( screenShotBtn );
    screenShotBtn->setPopupMode( QToolButton::MenuButtonPopup );
    screenShotBtn->setMenu( screenShotConfigMenu );

    // camera presets
    QToolButton* presetBtn = new QToolButton( parent );
    presetBtn->setDefaultAction( m_dock->getGLWidget()->getCameraResetAction() );
    presetBtn->setMenu(  m_dock->getGLWidget()->getCameraPresetsMenu() );
    presetBtn->setPopupMode( QToolButton::MenuButtonPopup );
    setupButton( presetBtn );

    QToolButton* settingsBtn = new QToolButton( parent );
    settingsBtn->setPopupMode( QToolButton::InstantPopup );
    settingsBtn->setIcon(  WQt4Gui::getMainWindow()->getIconManager()->getIcon( "configure" ) );
    setupButton( settingsBtn );
    settingsBtn->setToolTip( "Settings" );
    QMenu* settingsMenu = new QMenu( parent );
    settingsBtn->setMenu( settingsMenu );

    // throwing
    settingsMenu->addAction( m_dock->getGLWidget()->getThrowingSetting() );

    // change background color
    settingsMenu->addAction( m_dock->getGLWidget()->getBackgroundColorAction() );

    // add all the tool buttons to the tool widget
    layout->addWidget( screenShotBtn );
    layout->addWidget( presetBtn );
    layout->addWidget( line );
    layout->addWidget( settingsBtn );
}

WQtGLDockWidgetTitle::WQtGLDockWidgetTitle( WQtGLDockWidget* parent, const QString& dockTitle ):
    QWidget( parent ),
    m_dock( parent )
{
    m_screenCaptureWidgetAction = new QWidgetAction( this );
    m_screenCaptureWidgetAction->setDefaultWidget( m_dock->getScreenCapture() );

    m_screenCaptureMenu1 = new QMenu();
    m_screenCaptureMenu2 = new QMenu();

    // all tool buttons go into this widget
    m_tools = new QWidget( this );
    m_toolsLayout = new QHBoxLayout( m_tools );
    m_tools->setLayout( m_toolsLayout );
    m_tools->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
    m_toolsLayout->setMargin( 0 );
    m_toolsLayout->setSpacing( 0 );
    m_tools->setContentsMargins( 0, 0, 0, 0 );
    m_tools->setMinimumSize( 1, 24 );

    m_toolsMenu = new QWidget( this );
    QHBoxLayout* toolsMenuLayout = new QHBoxLayout( m_toolsMenu );
    m_toolsMenu->setLayout( toolsMenuLayout );
    toolsMenuLayout->setMargin( 0 );
    toolsMenuLayout->setSpacing( 0 );
    m_toolsMenu->setContentsMargins( 0, 0, 0, 0 );

    m_moreBtn = new QToolButton( this );
    m_moreBtn->setHidden( true );
    m_moreBtn->setFixedWidth( 32 );
    setupButton( m_moreBtn );
    m_moreBtn->setPopupMode( QToolButton::InstantPopup );
    m_moreBtn->setIcon(  WQt4Gui::getMainWindow()->getIconManager()->getIcon( "popup_more" ) );
    QMenu* moreMenu = new QMenu();
    QWidgetAction* moreAction = new QWidgetAction( m_toolsMenu );
    moreAction->setDefaultWidget( m_toolsMenu );
    moreMenu->addAction( moreAction );
    m_moreBtn->setMenu( moreMenu );

    fillToolLayout( m_tools, m_toolsLayout, m_screenCaptureMenu1 );
    fillToolLayout( m_toolsMenu, toolsMenuLayout, m_screenCaptureMenu2 );

    // close Btn
    m_closeBtn = new QToolButton( this );
    QAction* act = new QAction( WQt4Gui::getMainWindow()->getIconManager()->getIcon( "popup_close" ), "Close", this );
    connect( act, SIGNAL( triggered( bool ) ), parent, SLOT( close() ) );
    m_closeBtn->setDefaultAction( act );
    setupButton( m_closeBtn );

    m_closeBtn->setMinimumSize( 12, 12 );
    m_moreBtn->setMinimumSize( 12, 12 );

    // title
    m_title = new WScaleLabel( " " + dockTitle, 3, this );
    m_title->setTextInteractionFlags( Qt::NoTextInteraction );

    // build layout
    QHBoxLayout* layout = new QHBoxLayout( this );
    layout->setMargin( 0 );
    layout->setSpacing( 0 );

    layout->addWidget( m_title );
    layout->addStretch( 100000 );
    layout->addWidget( m_tools );
    layout->addWidget( m_moreBtn );
    layout->addWidget( m_closeBtn );

    setSizePolicy( QSizePolicy( QSizePolicy::Ignored, QSizePolicy::Fixed ) );
}

void WQtGLDockWidgetTitle::resizeEvent( QResizeEvent* event )
{
    int required = m_title->calculateSize( m_title->text().length() ) + m_tools->sizeHint().width() + m_closeBtn->sizeHint().width();
    if( event->size().width() < required )
    {
        m_tools->setHidden( true );
        m_moreBtn->setHidden( false );
        m_screenCaptureMenu1->clear();
        m_screenCaptureMenu2->addAction( m_screenCaptureWidgetAction );
    }
    else
    {
        m_tools->setHidden( false );
        m_moreBtn->setHidden( true );
        m_screenCaptureMenu2->clear();
        m_screenCaptureMenu1->addAction( m_screenCaptureWidgetAction );
    }
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
