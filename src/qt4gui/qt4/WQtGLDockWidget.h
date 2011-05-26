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

#include "core/graphicsEngine/WGECamera.h"

#include "WQtGLWidget.h"

/**
 * Dock widget containing only a GLWidget.
 */
class WQtGLDockWidget : public QDockWidget
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
    boost::shared_ptr< WQtGLWidget > getGLWidget() const;

protected:

    /**
     * Layout of this widget. Use this to extend the dock width other widgets (i.e. PropWidgets).
     */
    QVBoxLayout* m_layout;

    /**
     * The panel widget using m_layout.
     */
    QWidget* m_panel;

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
};

#endif  // WQTGLDOCKWIDGET_H
