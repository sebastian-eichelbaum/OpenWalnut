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

#ifndef WQTGLDOCKWIDGET_H
#define WQTGLDOCKWIDGET_H

#include <QtGui/QDockWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QToolButton>
#include <QtGui/QLabel>
#include <QtGui/QResizeEvent>
#include <QtGui/QWidgetAction>

#include "core/graphicsEngine/WGECamera.h"

#include "guiElements/WScaleLabel.h"
#include "guiElements/WQtDockWidget.h"

#include "WQtGLWidget.h"

/**
 * Dock widget containing only a GLWidget.
 */
class WQtGLDockWidget: public WQtDockWidget
{
    Q_OBJECT
public:
    /**
     * default constructor
     *
     * \param viewTitle Title will be used as view title
     * \param dockTitle The title of the dock widget.
     * \param parent The widget that manages this widget
     * \param projectionMode The projection mode used in the GL widget.
     * \param shareWidget this widget will share OpenGL display lists and texture objects with shareWidget
     */
    WQtGLDockWidget( QString viewTitle, QString dockTitle, QWidget* parent, WGECamera::ProjectionMode projectionMode = WGECamera::ORTHOGRAPHIC,
                                                                            const QWidget* shareWidget = 0 );

    /**
     * destructor.
     */
    virtual ~WQtGLDockWidget();

    /**
     * Gets the contained GL widget instance.
     *
     * \return pointer to GL widget
     */
    WQtGLWidget* getGLWidget() const;

    /**
     * Adds a screen capture dock using this view's screen capture callback.
     *
     * \return the capture dock instance.
     */
    WQtGLScreenCapture* getScreenCapture();

    /**
     * Forces the GL widget to have the desired size. This is mainly useful for screenshots and similar.
     *
     * \param w width
     * \param h height
     */
    void forceGLWidgetSize( size_t w, size_t h );

    /**
     * Restores the GL widget size if it was fixed with forceMainGLWidgetSize() previously.
     */
    void restoreGLWidgetSize();

    /**
     * Return the title of the view/dock.
     *
     * \return the title text
     */
    const QString& getDockTitle() const;

    /**
     * Save settings.
     */
    virtual void saveSettings();

    /**
     * Restore settings
     */
    virtual void restoreSettings();

    /**
     * Allow turning the automatic save and restore of viewer-settings.
     *
     * \param enable true to enable
     */
    void setSaveViewerSettings( bool enable = true );

    /**
     * Check whether the automatic saving of \ref WGEViewer properties is enabled.
     *
     * \return true if enabled.
     */
    bool getSaveViewerSettings() const;
public slots:
    /**
     * Open screen capture config options
     */
    void openScreenCaptureConfig();

protected:
    /**
     * Layout of this widget. Use this to extend the dock width other widgets (i.e. PropWidgets).
     */
    QVBoxLayout* m_layout;

    /**
     * The panel widget using m_layout.
     */
    QWidget* m_panel;

    /**
     * Called whenever a close event is received.
     *
     * \param event the event.
     */
    virtual void closeEvent( QCloseEvent *event );

    /**
     * Called whenever the widget gets opened
     *
     * \param event the event
     */
    virtual void showEvent( QShowEvent* event );

private slots:
    /**
     * If the dock widget changes its visibility.
     *
     * \param visible if true, the dock is visible
     */
    void handleVisibilityChange( bool visible );

private:
    /**
     * My GL widget.
     */
    WQtGLWidget* m_glWidget;

    /**
     * The view name and dock title.
     */
    QString m_dockTitle;

    /**
     * Manager for screen capturing of this view
     */
    WQtGLScreenCapture* m_screenCapture;

    /**
     * If true, the saveSettings method also saves the WGEViewer properties.
     */
    bool m_saveViewerSettings;
};

#endif  // WQTGLDOCKWIDGET_H
