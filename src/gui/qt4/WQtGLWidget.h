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

#ifndef WQTGLWIDGET_H
#define WQTGLWIDGET_H


#include <QtCore/QTimer>
#include <QtOpenGL/QGLWidget>
#include <QtGui/QWidget>

#include <boost/shared_ptr.hpp>

#include "../../graphicsEngine/WGEGraphicsWindow.h"
#include "../../graphicsEngine/WGEViewer.h"

/**
 * \ingroup gui
 * A widget containing an open gl display area. This initializes OpenGL context and adds a view to the
 * engine.
 */
class WQtGLWidget: public QWidget
{
public:
    /**
     * \par Description
     * Default constructor.
     *
     * \param parent Parent widget.
     *
     * \return
     */
    explicit WQtGLWidget( QWidget* parent = 0 );

    /**
     * \par Description
     * Destructor.
     */
    virtual ~WQtGLWidget();

    /**
     * returns the recommended size for the widget to allow
     * parent widgets to give it a proper initial layout
     */
    QSize sizeHint() const;

protected:
    /**
     * \par Description
     * The viewer to the scene.
     */
    boost::shared_ptr<WGEViewer> m_Viewer;

    //  The GraphincsWindowWin32 implementation already takes care of message handling.
    //  We don't want to relay these on Windows, it will just cause duplicate messages
    //  with further problems downstream (i.e. not being able to throw the trackball
#ifndef WIN32
    /** 
     * \par Description
     * Event handler for double clicks.
     * 
     * \param event the event description.
     */
    virtual void mouseDoubleClickEvent( QMouseEvent* event );

    /** 
     * \par Description
     * Event handler for close events.
     * 
     * \param event the event description.
     */
    virtual void closeEvent( QCloseEvent* event );

    /** 
     * \par Description
     * Event handler for destroy events (called after close).
     * 
     * \param event the event description.
     */
    virtual void destroyEvent( bool destroyWindow = true, bool destroySubWindows = true );

    /** 
     * \par Description
     * Event handler for  resize events.
     * 
     * \param event the event description.
     */
    virtual void resizeEvent( QResizeEvent* event );

    /** 
     * \par Description
     * Event handler for key press.
     * 
     * \param event the event description.
     */
    virtual void keyPressEvent( QKeyEvent* event );

    /** 
     * \par Description
     * Event handler for key release.
     * 
     * \param event the event description.
     */
    virtual void keyReleaseEvent( QKeyEvent* event );

    /** 
     * \par Description
     * Event handler for mouse button press.
     * 
     * \param event the event description.
     */
    virtual void mousePressEvent( QMouseEvent* event );

    /** 
     * \par Description
     * Event handler for mouse button release.
     * 
     * \param event the event description.
     */
    virtual void mouseReleaseEvent( QMouseEvent* event );

    /** 
     * \par Description
     * Event handler for mouse moves.
     * 
     * \param event the event description.
     */
    virtual void mouseMoveEvent( QMouseEvent* event );
#endif

    /** 
     * \par Description
     * QT Callback for handling repaints.
     * 
     * \param event event descriptor.
     */
    virtual void paintEvent( QPaintEvent* event );

    /** 
     * \par Description
     * Simply translate the mouse button from an event to an int.
     * 
     * \param event the QT Event.
     * 
     * \return the translated button number.
     */
    int translateButton( QMouseEvent* event );

private:
    /**
     * Holds the recommended size for the widget
     */
    QSize m_recommendedSize;

    /** 
     * \par Description
     * Window Data for this widget needed by OpenSceneGraph.
     */
    boost::shared_ptr<WindowData> wdata;

    /** 
     * \par Description
     * Timer used for permanent redraw of all views. This is just a hack and will be improved
     * so that redraws are done by separate threads.
     */
    QTimer m_Timer;
};

#endif  // WQTGLWIDGET_H
