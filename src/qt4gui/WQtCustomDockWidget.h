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

#ifndef WQTCUSTOMDOCKWIDGET_H
#define WQTCUSTOMDOCKWIDGET_H

#include <string>

#include "core/graphicsEngine/WGEGroupNode.h"

#include <QtGui/QDockWidget>

#include "WQtGLDockWidget.h"

#include "core/gui/WCustomWidget.h"

/**
 * Dock Widget which is created by a module to display custom information.
 */
class WQtCustomDockWidget : public WQtGLDockWidget,
                            public WCustomWidget
{
public:
    /**
     * constructor
     *
     * \param title the title of the widget
     * \param parent the parent of the widget
     * \param projectionMode The kind of projection which should be used
     */
    WQtCustomDockWidget( std::string title, QWidget* parent, WGECamera::ProjectionMode projectionMode );

    /**
     * Get the scene which is displayed by the GL widget
     *
     * \return the scene as osg::ref_ptr
     */
    virtual osg::ref_ptr< WGEGroupNode > getScene() const;

    /**
     * Get the viewer which is used by the GL widget
     *
     * \return the viewer as boost::shard_ptr
     */
    virtual boost::shared_ptr< WGEViewer > getViewer() const;

    /**
     * Notify the widget that another module needs it.
     */
    void increaseUseCount();

    /**
     * Notify the widget that it is not needed by one module anymore.
     *
     * \returns whether the widget is not needed at all and should be deleted
     */
    bool decreaseUseCount();

    /**
     * Returns the height of the viewport of the camera.
     *
     * \return Height in pixels.
     */
    virtual size_t height() const;

    /**
     * Returns the width of the viewport of the camera.
     *
     * \return Width in pixels.
     */
    virtual size_t width() const;

protected:
private:
    /**
     * the scene which is displayed by the GL widget
     */
    osg::ref_ptr< WGEGroupNode > m_scene;

    /**
     * How many modules currently need this widget?
     * Widget should be closed if this count reaches 0.
     *
     * \note It is not thread safe. But concurrent access is prevented by
     *       WMainWindow's m_customDockWidgetsLock
     */
    unsigned int m_useCount;
};

#endif  // WQTCUSTOMDOCKWIDGET_H
