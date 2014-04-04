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

#ifndef WQTVIEWWIDGET_H
#define WQTVIEWWIDGET_H

#include <boost/shared_ptr.hpp>

#include "core/graphicsEngine/WGEGroupNode.h"
#include "core/graphicsEngine/WGECamera.h"
#include "core/graphicsEngine/WGEViewer.h"
#include "core/common/WCondition.h"

#include "core/ui/WUIViewWidget.h"
#include "../WMainWindow.h"

#include "WQtWidgetBase.h"

/**
 * Implementation of \ref WUIViewWidget.
 */
class WQtViewWidget: public WUIViewWidget,
                     public WQtWidgetBase
{
public:

    /**
     * Convenience typedef for a boost::shared_ptr< WQtViewWidget >.
     */
    typedef boost::shared_ptr< WQtViewWidget > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WQtViewWidget >.
     */
    typedef boost::shared_ptr< const WQtViewWidget > ConstSPtr;

    /**
     * Default constructor.
     *
     * \param mainWindow the main window instance
     *
     * \param title the title of the widget
     * \param projectionMode the kind of projection which should be used
     */
    WQtViewWidget(
            std::string title,
            WGECamera::ProjectionMode projectionMode,
            WMainWindow* mainWindow );

    /**
     * Destructor.
     */
    virtual ~WQtViewWidget();

    /**
     * Get the scene which is displayed
     *
     * \return the scene as osg::ref_ptr
     */
    virtual osg::ref_ptr< WGEGroupNode > getScene() const;

    /**
     * Get the viewer which is used
     *
     * \return the viewer as boost::shard_ptr
     */
    virtual boost::shared_ptr< WGEViewer > getViewer() const;

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

    /**
     * Adds an event handler to the widget's view.
     *
     * \param handler Pointer to the handler.
     */
    virtual void addEventHandler( osgGA::GUIEventHandler* handler );


    /**
     * Show this widget if not yet visible.
     */
    virtual void show();

    /**
     * Hide/show this widget. Unlike close(), you can show the widget again using show().
     *
     * \param visible false to hide widget
     */
    virtual void setVisible( bool visible = true );

    /**
     * Check if the widget is hidden or not.
     *
     * \return true if visible.
     */
    virtual bool isVisible() const;

    /**
     * Close the widget. When done, the widget can be safely deleted.
     */
    virtual void close();

protected:
    /**
     * Realize the widget. This method blocks until the GUI thread created the widget. Called from within the GUI thread! So you can safely do Qt
     * stuff.
     */
    virtual void realizeImpl();
private:
};

#endif  // WQTVIEWWIDGET_H

