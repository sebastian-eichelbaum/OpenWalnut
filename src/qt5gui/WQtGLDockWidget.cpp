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
#include <string>

#include <boost/function.hpp>
#include <boost/lambda/lambda.hpp>

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


WQtGLDockWidget::WQtGLDockWidget( QString viewTitle, QString dockTitle, QWidget* parent, WGECamera::ProjectionMode projectionMode,
                                  const QWidget* shareWidget ):
    WQtDockWidget( dockTitle, parent ),
    m_dockTitle( dockTitle ),
    m_saveViewerSettings( true )
{
    setObjectName( QString( "GL - " ) + dockTitle );

    // the panel contains all other widgets, including the gl widget
    // This allows adding other widgets to certain docks
    m_panel = new QWidget( this );
    m_layout = new QVBoxLayout( m_panel );
    m_layout->setContentsMargins( 1, 1, 1, 1 );

    m_glWidget = new WQtGLWidget( viewTitle.toStdString(), m_panel, projectionMode, shareWidget );

    // NOTE: do not remove this. When creating custom widgets using the OSG manipulators, a too small size here (or even no min size) causes the
    // cull visitor to do crap ... unknown reason ...
    setMinimumSize( 50, 50 );

    // add panel to layout.
    m_layout->addWidget( m_glWidget );
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
    m_presetBtn = new QToolButton( parent );
    m_presetBtn->setDefaultAction( m_glWidget->getCameraResetAction() );
    m_presetBtn->setMenu( m_glWidget->getCameraPresetsMenu() );
    m_presetBtn->setPopupMode( QToolButton::MenuButtonPopup );

    // add them to the title
    addTitleButton( screenShotBtn );
    addTitleButton( m_presetBtn );

    addTitleProperty( m_glWidget->getViewer()->getProperties() );
}

WQtGLDockWidget::~WQtGLDockWidget()
{
    // cleanup
    delete m_screenCapture;
    m_screenCapture = NULL;
}

void WQtGLDockWidget::saveSettings()
{
    if( m_screenCapture )
    {
        m_screenCapture->saveSettings();
    }
    WQtDockWidget::saveSettings();

    // optional:
    if( m_saveViewerSettings )
    {
        // visit the properties and save in QSettings. You cannot bind QSettings::setValue directly as the parameters need to be cast to QString and
        // QVariant, which does not happen implicitly
        m_glWidget->getViewer()->getProperties()->visitAsString( &WMainWindow::setSetting, m_dockTitle.toStdString() );
    }
}

void WQtGLDockWidget::restoreSettings()
{
    m_screenCapture->restoreSettings();
    WQtDockWidget::restoreSettings();

    if( m_saveViewerSettings )
    {
        // do not forget to load the config properties of the viewer
        WMainWindow::getSettings().beginGroup( m_dockTitle );
        QStringList keys = WMainWindow::getSettings().allKeys();
        // iterate all the keys in the group of this viewer. QSettings does not implement a visitor mechanism, thus we iterate manually.
        for( QStringList::const_iterator it = keys.constBegin(); it != keys.constEnd(); ++it )
        {
            std::string value = WMainWindow::getSettings().value( *it ).toString().toStdString();
            std::string key = ( *it ).toStdString();

            // NOTE: findProperty does not throw an exception, but setAsString.
            WPropertyBase::SPtr prop = m_glWidget->getViewer()->getProperties()->findProperty( key );
            if( prop )
            {
                // just in case something is going wrong (faulty setting): cannot cast string to property type. Be kind and ignore it.
                try
                {
                    prop->setAsString( value );
                }
                catch( ... )
                {
                    // ignore faulty/old settings
                }
            }
        }
        WMainWindow::getSettings().endGroup();
    }
}

void WQtGLDockWidget::setSaveViewerSettings( bool enable )
{
    m_saveViewerSettings = enable;
}

bool WQtGLDockWidget::getSaveViewerSettings() const
{
    return m_saveViewerSettings;
}

WQtGLWidget* WQtGLDockWidget::getGLWidget() const
{
    return m_glWidget;
}

void WQtGLDockWidget::handleVisibilityChange( bool visible )
{
    // this can help to reduce CPU load. Especially if multithreading viewers are used with cull thread per context.
    if( m_glWidget->getViewer() )
    {
        m_glWidget->getViewer()->getView()->getScene()->getSceneData()->setNodeMask( visible * 0xFFFFFFFF );
    }
}

void WQtGLDockWidget::closeEvent( QCloseEvent *event )
{
    event->accept();
    m_glWidget->setPaused( true );
    WQtDockWidget::closeEvent( event );
}

void WQtGLDockWidget::showEvent( QShowEvent* event )
{
    m_glWidget->setPaused( false );
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
    if( m_screenCapture )
    {
        m_screenCapture->setWindowFlags( Qt::Tool );
        m_screenCapture->show();
    }
}

void WQtGLDockWidget::updateCameraPresetButton()
{
    if( getCameraPresetMenu()->isEmpty() )
    {
        m_presetBtn->setMenu( NULL );
    }
    else
    {
        m_presetBtn->setMenu( getCameraPresetMenu() );
    }
}

QMenu* WQtGLDockWidget::getCameraPresetMenu() const
{
    return m_glWidget->getCameraPresetsMenu();
}
