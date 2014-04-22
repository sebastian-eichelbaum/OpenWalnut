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

#ifndef WUIQTVIEWWIDGET_H
#define WUIQTVIEWWIDGET_H

#include <string>

#include <boost/shared_ptr.hpp>

#include "core/graphicsEngine/WGEGroupNode.h"
#include "core/graphicsEngine/WGECamera.h"
#include "core/graphicsEngine/WGEViewer.h"
#include "core/common/WCondition.h"

#include "core/ui/WUIViewWidget.h"
#include "../WMainWindow.h"
#include "../WQtGLDockWidget.h"
#include "../WQtGLWidget.h"

#include "WUIQtWidgetBase.h"

/**
 * Implementation of \ref WUIViewWidget.
 */
class WUIQtViewWidget: public WUIViewWidget,
                       public WUIQtWidgetBase
{
public:
    /**
     * Convenience typedef for a boost::shared_ptr< WUIQtViewWidget >.
     */
    typedef boost::shared_ptr< WUIQtViewWidget > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WUIQtViewWidget >.
     */
    typedef boost::shared_ptr< const WUIQtViewWidget > ConstSPtr;

    /**
     * Default constructor.
     *
     * \param mainWindow the main window instance
     * \param parent the Qt parent. Can be NULL.
     * \param title the title of the widget
     * \param projectionMode the kind of projection which should be used
     */
    WUIQtViewWidget(
            std::string title,
            WGECamera::ProjectionMode projectionMode,
            WMainWindow* mainWindow,
            WUIQtWidgetBase::SPtr parent = WUIQtWidgetBase::SPtr() );

    /**
     * Destructor.
     */
    virtual ~WUIQtViewWidget();

    /**
     * Title as QString.
     *
     * \return the title
     */
    virtual QString getTitleQString() const;

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
     * Handle shutdown. This includes notification of the creating module and closing the widget. Can be called from any thread.
     * Implement in your implementation.
     */
    virtual void close();

    /**
     * Implement \ref WUIWidgetBase::addAction.
     *
     * \param group the property to use.
     * \param icon the icon to use. Consider a minimum size of 32x32.
     */
    virtual void addAction( WPropGroup group, WGEImage::SPtr icon = WGEImage::SPtr() )
    {
        WUIQtWidgetBase::addAction( group, icon );
    }

    /**
     * Implement \ref WUIWidgetBase::addAction.
     *
     * \param trigger the property to use.
     * \param icon the icon to use. Consider a minimum size of 32x32.
     */
    virtual void addAction( WPropTrigger trigger, WGEImage::SPtr icon = WGEImage::SPtr() )
    {
        WUIQtWidgetBase::addAction( trigger, icon );
    }

    /**
     * Implement \ref WUIWidgetBase::addAction.
     *
     * \param toggle the property to use.
     * \param icon the icon to use. Consider a minimum size of 32x32.
     */
    virtual void addAction( WPropBool toggle, WGEImage::SPtr icon = WGEImage::SPtr() )
    {
        WUIQtWidgetBase::addAction( toggle, icon );
    }

    /**
     * Remove any pre-existing camera preset. Implements \ref WUIViewWidget::clearCameraPresets.
     */
    virtual void clearCameraPresets();

    /**
     * Add a custom camera preset. Implements \ref WUIViewWidget::addCameraPreset
     *
     * \param name the name of the preset.
     * \param preset the trigger to set the preset.
     * \param icon optional icon.
     */
    virtual void addCameraPreset( std::string name, WPropTrigger preset, WGEImage::SPtr icon = WGEImage::SPtr() );
protected:
    /**
     * Realize the widget. This method blocks until the GUI thread created the widget. Called from within the GUI thread! So you can safely do Qt
     * stuff.
     */
    virtual void realizeImpl();

    /**
     * Close the widget. When done, the widget can be safely deleted.
     */
    virtual void closeImpl();

    /**
     * Cleanup the GUI. Do not delete m_widget, or your content widget. This is done by WUIQtWidgetBase. This method allows you to free resources
     * that are not automatically freed by the Qt delete mechanism.
     */
    virtual void cleanUpGT();

    /**
     * Remove any pre-existing camera preset. Implements \ref WUIViewWidget::clearCameraPresets.
     */
    virtual void clearCameraPresetsGT();

    /**
     * Add a custom camera preset. Implements \ref WUIViewWidget::addCameraPreset
     *
     * \param name the name of the preset.
     * \param preset the trigger to set the preset.
     * \param icon optional icon.
     */
    virtual void addCameraPresetGT( std::string name, WPropTrigger preset, WGEImage::SPtr icon = WGEImage::SPtr() );
private:
    /**
     * Projection mode of the cam
     */
    WGECamera::ProjectionMode m_projectionMode;

    /**
     * Scene in this view
     */
    osg::ref_ptr< WGEGroupNode > m_scene;

    /**
     * The Qt widget representing this abstract widget.
     * \note this is the same pointer as WUIQtWidgetBase::m_widget as WQtGLDockWidget.
     */
    WQtGLDockWidget* m_widgetDock;
};

#endif  // WUIQTVIEWWIDGET_H

