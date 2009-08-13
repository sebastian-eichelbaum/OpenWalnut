//---------------------------------------------------------------------------
//
// Project: OpenWalnut
//
// Copyright 2009 SomeCopyrightowner
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

#ifndef WGEGRAPHICSWINDOW_H
#define WGEGRAPHICSWINDOW_H

#if defined( WIN32 ) && !defined( __CYGWIN__ )
#include <osgViewer/api/Win32/GraphicsWindowWin32>
typedef HWND WindowHandle;
typedef osgViewer::GraphicsWindowWin32::WindowData WindowData;
#elif defined( __APPLE__ )  // Assume using Carbon on Mac.
#include <osgViewer/api/Carbon/GraphicsWindowCarbon>
typedef WindowRef WindowHandle;
typedef osgViewer::GraphicsWindowCarbon::WindowData WindowData;
#else  // all other unix
#include <osgViewer/api/X11/GraphicsWindowX11>
typedef Window WindowHandle;
typedef osgViewer::GraphicsWindowX11::WindowData WindowData;
#endif

#include <boost/shared_ptr.hpp>

/**
 * \par Description:
 * Class managing a single graphics context and OSG GraphicsWindow.
 */
class WGEGraphicsWindow
{
public:

    /**
     * \par Description
     * Default constructor.
     *
     * \param wdata the WindowData instance for the widget to use as render widget
     * \param x X coordinate of widget where to create the context.
     * \param y Y coordinate of widget where to create the context.
     * \param width Width of the widget.
     * \param height Height of the Widget.
     * \exception WGEInitFailed thrown if initialization of graphics context or graphics window has failed.
     */
    WGEGraphicsWindow( boost::shared_ptr<WindowData> wdata, int x, int y, int width, int height );

    /**
     * \par Description
     * Destructor.
     */
    virtual ~WGEGraphicsWindow();

    /**
     * \par Description
     * Getter for m_GraphicsWindow.
     *
     * \return the OSG GraphicsWindow instance.
     */
    boost::shared_ptr<osgViewer::GraphicsWindow> getGraphicsWindow();

    /** 
     * \par Description
     * Event types for the keyEvent() handler.
     */
    enum KeyEvents
    {
        KEYPRESS, KEYRELEASE
    };

    /** 
     * \par Description
     * Mouse event types for the mouseEvent() handler.
     */
    enum MouseEvents
    {
        MOUSEPRESS, MOUSERELEASE, MOUSEDOUBLECLICK, MOUSEMOVE
    };

    /**
     * \par Description
     * Updates size information.
     *
     * \param width new width.
     * \param height new height.
     */
    virtual void resize( int width, int height );

    /** 
     * \par Description
     * Initiates a close event for this viewer. It destroys the graphics context and invalidates the viewer.
     * This should be called whenever a QT Widget closes to also free its OSG Viewer resources.
     */
    virtual void close();

    /** 
     * \par Description
     * Handles key events (if forwarded to this Viewer instance).
     * 
     * \param key the key code.
     * \param eventType the type of event.
     */
    virtual void keyEvent( KeyEvents eventType, int key );

    /** 
     * \par Description
     * Handles mouse events forwarded from widget.
     * 
     * \param eventType the event type.
     * \param x x coordinate of event.
     * \param y y coordinate of event.
     * \param button mouse button.
     */
    virtual void mouseEvent( MouseEvents eventType, int x, int y, int button );

protected:

    /**
     * \par Description
     * Creates a new OpenGL context in the calling thread. Needs to be called before any other OpenGL operation.
     *
     * \param x X coordinate of widget where to create the context.
     * \param y Y coordinate of widget where to create the context.
     * \param width Width of the widget.
     * \param height Height of the Widget.
     */
    void createContext( int x, int y, int width, int height );

    /**
     * \par Description
     * OpenSceneGraph render window.
     */
    boost::shared_ptr<osgViewer::GraphicsWindow> m_GraphicsWindow;

    /**
     * \par Description
     * OpenSceneGraph render context. This is required to be a "normal" pointer since using shared_ptr causes
     * the code not to work (unknown reason for now). But since we do not offer a getter for it -> no prob.
     */
    osg::GraphicsContext* m_GraphicsContext;

    /**
     * \par Description
     * Widget window data.
     */
    boost::shared_ptr<WindowData> m_WindowData;
private:
};

#endif  // WGEGRAPHICSWINDOW_H

