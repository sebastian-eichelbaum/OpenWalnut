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

#ifndef WQTGLWIDGET_H
#define WQTGLWIDGET_H

#include <string>

#include <QtOpenGL/QGLWidget>
#include <QtGui/QWidget>

#include <boost/shared_ptr.hpp>
#include <boost/signals2/signal.hpp>

#include "../../graphicsEngine/WGECamera.h"
#include "../../common/WFlag.h"

class WGEViewer;
class WColor;

/**
 * A widget containing an open gl display area. This initializes OpenGL context and adds a view to the
 * engine.
 * \ingroup gui
 */
class WQtGLWidget
#ifdef _WIN32
    : public QWidget
#else
    : public QGLWidget
#endif
{
public:
    /**
     * Default constructor.
     *
     * \param nameOfViewer Name of the Viewer
     * \param parent Parent widget.
     * \param projectionMode decides whether the widget uses perspective or othographic projection
     *
     * \return
     */
    explicit WQtGLWidget( std::string nameOfViewer, QWidget* parent = 0,
        WGECamera::ProjectionMode projectionMode = WGECamera::ORTHOGRAPHIC );

    /**
     * Destructor.
     */
    virtual ~WQtGLWidget();

    /**
     * Since the widget is not visible during construction, the OSG thread may cause errors, so we use a post constructor.
     */
    virtual void initialize();

    /**
     * returns the recommended size for the widget to allow
     * parent widgets to give it a proper initial layout
     */
    QSize sizeHint() const;

    /**
     * List of currently possible camera manipulators.
     */
    enum CameraManipulators
    {
        TRACKBALL, TERRAIN, UFO, DRIVE, FLIGHT, TWO_D
    };

    /**
     * Sets the camera manipulator to use.
     *
     * \param manipulator the manipulator.
     */
    void setCameraManipulator( CameraManipulators manipulator );

    /**
     * Sets the background color of the widget.
     * \param bgColor the new backgorund color
     */
    void setBgColor( WColor bgColor );

    /**
     * Returns the actually set camera manipulator.
     *
     * \return the manipulator.
     */
    CameraManipulators getCameraManipulators();

    /**
     * Get the included viewer.
     *
     * \return a shared pointer to the viewer
     */
    boost::shared_ptr< WGEViewer > getViewer() const;

    /**
     * Determines whether the widget is properly initialized.
     *
     * \return flag - initialized.
     */
    const WBoolFlag& isInitialized() const;

protected:
    /**
     * The viewer to the scene.
     */
    boost::shared_ptr<WGEViewer> m_Viewer;

    /**
     * The name of the viewer.
     */
    std::string m_nameOfViewer;


    //  The GraphincsWindowWin32 implementation already takes care of message handling.
    //  We don't want to relay these on Windows, it will just cause duplicate messages
    //  with further problems downstream (i.e. not being able to throw the trackball
#ifndef WIN32
    /**
     * Event handler for double clicks.
     *
     * \param event the event description.
     */
    virtual void mouseDoubleClickEvent( QMouseEvent* event );

    /**
     * Event handler for close events.
     *
     * \param event the event description.
     */
    virtual void closeEvent( QCloseEvent* event );

    /**
     * Event handler for destroy events (called after close).
     *
     * \param destroyWindow flag indicatinng whether to destroy window
     * \param destroySubWindows flag indicatinng whether to destroy sub windows
     */
    virtual void destroyEvent( bool destroyWindow = true, bool destroySubWindows = true );

    /**
     * Event handler for  resize events.
     *
     * \param event the event description.
     */
    virtual void resizeEvent( QResizeEvent* event );

    /**
     * Event handler for key press.
     *
     * \param event the event description.
     */
    virtual void keyPressEvent( QKeyEvent* event );

    /**
     * Event handler for key release.
     *
     * \param event the event description.
     */
    virtual void keyReleaseEvent( QKeyEvent* event );

    /**
     * Event handler for mouse button press.
     *
     * \param event the event description.
     */
    virtual void mousePressEvent( QMouseEvent* event );

    /**
     * Event handler for mouse button release.
     *
     * \param event the event description.
     */
    virtual void mouseReleaseEvent( QMouseEvent* event );

    /**
     * Event handler for mouse moves.
     *
     * \param event the event description.
     */
    virtual void mouseMoveEvent( QMouseEvent* event );

    /**
     * Event handler for the mouse wheel.
     *
     * \param event the event description.
     */
    virtual void wheelEvent( QWheelEvent* event );
#endif

    /**
     * QT Callback for handling repaints.
     *
     * \param event event descriptor.
     */
    virtual void paintEvent( QPaintEvent* event );

    /**
     * Simply translate the mouse button from an event to an int.
     *
     * \param event the QT Event.
     *
     * \return the translated button number.
     */
    int translateButton( QMouseEvent* event );

    /**
     * Stores the current manipulator.
     */
    CameraManipulators m_CurrentManipulator;

    /**
     * Camera projection mode used to initialize widget. May not be the actual one!
     */
    WGECamera::ProjectionMode m_initialProjectionMode;

private:
    /**
     * Holds the recommended size for the widget
     */
    QSize m_recommendedSize;

    /**
     * True when initialized.
     */
    WBoolFlag m_isInitialized;

    /**
     * True when the widget got drawn the very first time.
     */
    bool m_firstPaint;
};

#endif  // WQTGLWIDGET_H
