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

#ifndef WGEGRAPHICSWINDOW_H
#define WGEGRAPHICSWINDOW_H

#include <boost/shared_ptr.hpp>
#include <osgViewer/GraphicsWindow>

/**
 * Class managing a single graphics context and OSG GraphicsWindow.
 * \ingroup ge
 */
class WGEGraphicsWindow
{
public:

    /**
     * Default constructor.
     *
     * \param wdata the WindowData instance for the widget to use as render widget. NULL on Mac!
     * \param x X coordinate of widget where to create the context.
     * \param y Y coordinate of widget where to create the context.
     * \param width Width of the widget.
     * \param height Height of the Widget.
     * \exception WGEInitFailed thrown if initialization of graphics context or graphics window has failed.
     */
    WGEGraphicsWindow( osg::ref_ptr<osg::Referenced> wdata, int x, int y, int width, int height );

    /**
     * Destructor.
     */
    virtual ~WGEGraphicsWindow();

    /**
     * Getter for m_GraphicsWindow.
     *
     * \return the OSG GraphicsWindow instance.
     */
    osg::ref_ptr<osgViewer::GraphicsWindow> getGraphicsWindow();

    /**
     * Event types for the keyEvent() handler.
     */
    enum KeyEvents
    {
        KEYPRESS, KEYRELEASE
    };

    /**
     * Mouse event types for the mouseEvent() handler.
     */
    enum MouseEvents
    {
        MOUSEPRESS, MOUSERELEASE, MOUSEDOUBLECLICK, MOUSEMOVE, MOUSESCROLL
    };

    /**
     * Updates size information.
     *
     * \param width new width.
     * \param height new height.
     */
    virtual void resize( int width, int height );

    /**
     * Initiates a close event for this viewer. It destroys the graphics context and invalidates the viewer.
     * This should be called whenever a QT Widget closes to also free its OSG Viewer resources.
     */
    virtual void close();

    /**
     * Handles key events (if forwarded to this Viewer instance).
     *
     * \param key the key code.
     * \param eventType the type of event.
     */
    virtual void keyEvent( KeyEvents eventType, int key );

    /**
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
     * OpenSceneGraph render window.
     */
    osg::ref_ptr<osgViewer::GraphicsWindow> m_GraphicsWindow;

#ifndef __APPLE__
     /**
     * Creates a new OpenGL context in the calling thread. Needs to be called before any other OpenGL operation.
     *
     * \param x X coordinate of widget where to create the context.
     * \param y Y coordinate of widget where to create the context.
     * \param width Width of the widget.
     * \param height Height of the Widget.
     */
    void createContext( int x, int y, int width, int height );

    /**
     * OpenSceneGraph render context.
     */
    osg::ref_ptr<osg::GraphicsContext> m_GraphicsContext;

    /**
     * Widget window data.
     */
    osg::ref_ptr<osg::Referenced> m_WindowData;
#endif
private:
};

#endif  // WGEGRAPHICSWINDOW_H

