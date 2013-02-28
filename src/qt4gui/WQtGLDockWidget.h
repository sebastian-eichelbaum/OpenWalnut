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

#include "WQtGLWidget.h"

/**
 * Dock widget containing only a GLWidget.
 */
class WQtGLDockWidget: public QDockWidget
{
    Q_OBJECT

    friend class WQtGLDockWidgetTitle;
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
    boost::shared_ptr< WQtGLWidget > getGLWidget() const;

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
    void closeEvent( QCloseEvent *event );

    /**
     * Called whenever the widget gets opened
     *
     * \param event the event
     */
    void showEvent( QShowEvent* event );

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
    boost::shared_ptr<WQtGLWidget> m_glWidget;

    /**
     * The view name and dock title.
     */
    QString m_dockTitle;

    /**
     * Manager for screen capturing of this view
     */
    WQtGLScreenCapture* m_screenCapture;
};

/**
 * Class for managing dock widget title bars.
 */
class WQtGLDockWidgetTitle: public QWidget
{
    Q_OBJECT
public:
    /**
     * Constructor
     *
     * \param parent the parent
     * \param dockTitle The title text
     */
    WQtGLDockWidgetTitle( WQtGLDockWidget* parent, const QString& dockTitle );
protected:
    /**
     * Called upon resize. Used to switch between the more menu and the tools widget
     *
     * \param event the event
     */
    virtual void resizeEvent( QResizeEvent* event );
private:
    /**
     * The tools buttons
     */
    QWidget* m_tools;

    /**
     * The tool inside the menu
     */
    QWidget* m_toolsMenu;

    /**
     * LAyout containing the tools
     */
    QHBoxLayout* m_toolsLayout;

    /**
     * The tool button used when shrinking the title bar too much
     */
    QToolButton* m_moreBtn;

    /**
     * Title label
     */
    WScaleLabel* m_title;

    /**
     * Close button
     */
    QToolButton* m_closeBtn;

    /**
     * Fill the layout with all the tool buttons
     *
     * \param parent the parent widget for the buttons
     * \param layout the layout to fill
     * \param screenShotConfigMenu the menu to add as screen shot config
     */
    void fillToolLayout( QWidget* parent, QBoxLayout* layout, QMenu* screenShotConfigMenu );

    /**
     * The dock
     */
    WQtGLDockWidget* m_dock;

    /**
     * Used to contain the screen capture dialog
     */
    QWidgetAction* m_screenCaptureWidgetAction;

    /**
     * Menu 1, used to alternate the m_screenCaptureWidgetAction in the tools bar and the tools menu
     */
    QMenu* m_screenCaptureMenu1;

    /**
     * Menu 2, used to alternate the m_screenCaptureWidgetAction in the tools bar and the tools menu
     */
    QMenu* m_screenCaptureMenu2;
};

#endif  // WQTGLDOCKWIDGET_H
