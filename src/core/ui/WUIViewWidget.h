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

#ifndef WUIVIEWWIDGET_H
#define WUIVIEWWIDGET_H

#include <string>

#include <boost/shared_ptr.hpp>

#include <osg/ref_ptr>

#include "../graphicsEngine/WGEViewer.h"

#include "WUIWidgetBase.h"

class WGEGroupNode;

/**
 * Is just a short hand to the long name "osgGA::GUIEventAdapter".
 */
class GUIEvents : public osgGA::GUIEventAdapter
{
public:
    using osgGA::GUIEventAdapter::EventType;
    using osgGA::GUIEventAdapter::MouseButtonMask;
    using osgGA::GUIEventAdapter::KeySymbol;
    using osgGA::GUIEventAdapter::ModKeyMask;
    using osgGA::GUIEventAdapter::MouseYOrientation;
    using osgGA::GUIEventAdapter::ScrollingMotion;
    using osgGA::GUIEventAdapter::TabletPointerType;

private:
    /**
     * The constructor is private to forbid instance generation.
     */
    GUIEvents()
    {
    }
};

/**
 * Custom widget which is created by a module to display custom information.
 *
 * \note Please read documentation of \ref WUIWidgetFactory for limitations, requirements and creation of these widgets.
 */
class WUIViewWidget: public WUIWidgetBase
{
    friend class WUI;
public:
    /**
     * Abbreviation for a shared pointer on a instance of this class.
     */
    typedef boost::shared_ptr< WUIViewWidget > SPtr;

    /**
     * Abbreviation for a const shared pointer on a instance of this class.
     */
    typedef boost::shared_ptr< const WUIViewWidget > ConstSPtr;

    /**
     * Destructor
     */
    virtual ~WUIViewWidget();

    /**
     * Get the scene which is displayed
     *
     * \return the scene as osg::ref_ptr
     */
    virtual osg::ref_ptr< WGEGroupNode > getScene() const = 0;

    /**
     * Get the viewer which is used
     *
     * \return the viewer as boost::shard_ptr
     */
    virtual boost::shared_ptr< WGEViewer > getViewer() const = 0;

    /**
     * Returns the height of the viewport of the camera.
     *
     * \return Height in pixels.
     */
    virtual size_t height() const = 0;

    /**
     * Returns the width of the viewport of the camera.
     *
     * \return Width in pixels.
     */
    virtual size_t width() const = 0;

    /**
     * Adds an event handler to the widget's view.
     *
     * \param handler Pointer to the handler.
     */
    virtual void addEventHandler( osgGA::GUIEventHandler* handler ) = 0;

    /**
     * Remove any pre-existing camera preset. Very useful when adding custom presets or presets do not make sense at all.
     */
    virtual void clearCameraPresets() = 0;

    /**
     * Add a custom camera preset. Appends at the end of any pre-existing presets. The presets are not directly specified via a matrix or
     * quaternion. A trigger property allows implementing own callbacks to actually set the preset. This is very useful when using your own
     * osg::CameraManupulator implementation for your view.
     *
     * \param name the name of the preset.
     * \param preset the trigger to set the preset.
     * \param icon optional icon.
     */
    virtual void addCameraPreset( std::string name, WPropTrigger preset, WGEImage::SPtr icon = WGEImage::SPtr() ) = 0;
protected:
    /**
     * Constructor. Create a custom widget instance.
     *
     * \param title the title of the widget
     */
    explicit WUIViewWidget( std::string title );
private:
};

#endif  // WUIVIEWWIDGET_H
