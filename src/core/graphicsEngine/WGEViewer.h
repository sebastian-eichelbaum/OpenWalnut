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

#ifndef WGEVIEWER_H
#define WGEVIEWER_H

#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <osg/Node>
#include <osg/Version>
#include <osgViewer/View>
#include <osgViewer/Viewer>

// OSG interface changed in 2.9.7, to make it compile also with those versions we do this:
// OSG_MIN_VERSION_REQUIRED(2, 9, 8) macro is not available in e.g. OSG 2.8.1, hence we use the old way
#if ( ( OPENSCENEGRAPH_MAJOR_VERSION > 2 ) || ( OPENSCENEGRAPH_MAJOR_VERSION == 2 && ( OPENSCENEGRAPH_MINOR_VERSION > 9 || \
                            ( OPENSCENEGRAPH_MINOR_VERSION == 9 && OPENSCENEGRAPH_PATCH_VERSION >= 8 ) ) ) )
    #include <osgGA/CameraManipulator>
    namespace osgGA
    {
        typedef CameraManipulator MatrixManipulator;
    }
#else
    #include <osgGA/MatrixManipulator>
#endif

#include "../common/WColor.h"
#include "../common/WFlag.h"

#include "WGECamera.h"
#include "WGEGraphicsWindow.h"
#include "WGraphicsEngineMode.h"
class WGEGroupNode;
#include "WGEScreenCapture.h"
class WPickHandler;
#include "animation/WGEAnimationManipulator.h"

/**
 * Class for managing one view to the scene. This includes viewport, camera and graphics context.
 * It is, besides WGraphicsEngine, the ONLY entry point for each widget for accessing the graphics engine.
 * \ingroup ge
 */
class WGEViewer: public WGEGraphicsWindow,
                            public boost::enable_shared_from_this< WGEViewer >
{
public:
    /**
     * Convenience typedef
     */
    typedef boost::shared_ptr< WGEViewer > SPtr;

    /**
     * Convenience typedef
     */
    typedef boost::shared_ptr< const WGEViewer > ConstSPtr;

    /**
     * Default constructor.
     *
     * \param name the name of the viewer
     * \param wdata the WindowData instance for the widget to use as render widget
     * \param x X coordinate of widget where to create the context.
     * \param y Y coordinate of widget where to create the context.
     * \param width Width of the widget.
     * \param height Height of the Widget.
     * \param projectionMode Projection mode of the viewer.
     * \exception WGEInitFailed thrown if initialization of graphics context or graphics window has failed.
     */
    WGEViewer( std::string name, osg::ref_ptr<osg::Referenced> wdata, int x, int y, int width, int height,
        WGECamera::ProjectionMode projectionMode = WGECamera::ORTHOGRAPHIC );

    /**
     * Destructor.
     */
    virtual ~WGEViewer();

    /**
     * Repaints the contents. Mac only.
     */
    virtual void paint();

    /**
     * Updates size information. Also updates camera.
     *
     * \param width new width.
     * \param height new height.
     */
    virtual void resize( int width, int height );

    /**
     * Close the viewer, but wait for the rendering thread to finish.
     */
    virtual void close();

    /**
     * Getter for OpenSceneGraph View instance.
     *
     * \return the OSG Viewer instance.
     */
#ifdef WGEMODE_SINGLETHREADED
    osg::ref_ptr<osgViewer::Viewer> getView();
#else
    osg::ref_ptr<osgViewer::View> getView();
#endif

    /**
     * Resets the view using the installed manipulator.
     */
    void reset();

    /**
     * Sets the camera manipulator to use.
     *
     * \param manipulator the manipulator to use.
     */
    void setCameraManipulator( osg::ref_ptr<osgGA::MatrixManipulator> manipulator );

    /**
     * Returns current active camera manipulator
     *
     * \return the active camera manipulator.
     */
    osg::ref_ptr<osgGA::MatrixManipulator> getCameraManipulator();

    /**
     * Sets the current camera.
     *
     * \param camera the OSG camera instance.
     */
    void setCamera( osg::ref_ptr<osg::Camera> camera );

    /**
     * Returns the camera currently in use.
     *
     * \return the camera currently in use.
     */
    osg::ref_ptr<osg::Camera> getCamera();

    /**
     * Sets the scene graph node to be used for rendering.
     *
     * \param node part of the scene graph
     */
    void setScene( osg::ref_ptr< WGEGroupNode > node );

    /**
     * Returns the currently set OSG node.
     *
     * \return the node.
     */
    osg::ref_ptr< WGEGroupNode > getScene();

    /**
     * Returns the name of the viewer.
     *
     * \return the name
     */
    std::string getName() const;

    /**
     * Determine the color of the viewer's background.
     * \param bgColor the new background color
     */
    void setBgColor( const WColor& bgColor );

    /**
     * Getter for the pick handler
     *
     * \return the pick handler
     */
    osg::ref_ptr< WPickHandler > getPickHandler();

    /**
     * Queries the OpenGL vendor info.
     *
     * \return Vendor string.
     */
    std::string getOpenGLVendor() const;

    /**
     * Returns the flag which denotes whether a frame was rendered.
     *
     * \return the flag.
     */
    WBoolFlag::SPtr isFrameRendered() const;

    /**
     * Returns the main cameras screen capture callback.
     *
     * \return the screen capture callback.
     */
    WGEScreenCapture::RefPtr getScreenCapture() const;

    /**
     * The (de-)activates the animation mode. In animation mode, a special camera manipulator is used instead of the currently set. This
     * manipulator can then play some animation path in realtime, frame-rate independent or in frame-per-frame mode which is useful if combined
     * with the getScreenCapture() record function.
     *
     * If animation mode is turned off again, the previously set manipulator / camera setting is restored.
     *
     * \note do not modify camera or camera manipulator manually while in animation mode.
     *
     * \param on true to turn on.
     *
     * \return the animation manipulator. This, and only this should be used to provide the animation.
     */
    WGEAnimationManipulator::RefPtr animationMode( bool on = true );

    /**
     * Checks if the viewer is in animation mode.
     *
     * \return true if in animation mode
     */
    bool isAnimationMode() const;

protected:
    /**
     * The OpenSceneGraph view used in this (Composite)Viewer.
     */
#ifdef WGEMODE_SINGLETHREADED
    osg::ref_ptr< osgViewer::Viewer > m_View;
#else
    osg::ref_ptr< osgViewer::View > m_View;
#endif

    /**
     * The name of the viewer.
     */
    std::string m_name;

    /**
     * Pointer to the pick handler of the viewer.
     */
    osg::ref_ptr<WPickHandler> m_pickHandler;

    /**
     * reference to the scene which is displayed by viewer
     */
    osg::ref_ptr< WGEGroupNode > m_scene;

    /**
     * This flag is true and notifies after the first rendered frame.
     */
    WBoolFlag::SPtr m_rendered;

    /**
     * Small class used for querying glGet info during rendering.
     */
    class QueryCallback: public osg::Camera::DrawCallback
    {
    public:
        /**
         * Constructor. Automatically de-registers from camera after one run.
         *
         * \param camera the cam to which this was registered
         * \param run notifies the flag when run.
         */
        QueryCallback( osg::ref_ptr<osg::Camera> camera, WBoolFlag::SPtr run );

        /**
         * Destructor.
         */
        virtual ~QueryCallback();

        /**
         * Query operator.
         *
         * \param renderInfo render info object
         */
        virtual void operator()( osg::RenderInfo& renderInfo ) const;   // NOLINT - this is OSG API

        /**
         * Returns the queried vendor string.
         *
         * \return the vendor
         */
        std::string getVendor() const;

    protected:
        /**
         * The vendor string.
         */
        mutable std::string m_vendor;

        /**
         * True if callback was run once.
         */
        WBoolFlag::SPtr m_run;

        /**
         * The camera to which this was connected.
         */
        osg::ref_ptr<osg::Camera> m_camera;
    };

    /**
     * The callback used for querying OpenGL features
     */
    osg::ref_ptr< QueryCallback > m_queryCallback;


    /**
     * The screen capture callback.
     */
    WGEScreenCapture::RefPtr m_screenCapture;

    /**
     * True -> animation mode on.
     */
    bool m_inAnimationMode;

    /**
     * The manipulator that was set before entering animation mode. Null if not in animation mode.
     */
    osg::ref_ptr<osgGA::MatrixManipulator> m_animationModeManipulatorBackup;

private:
};

#endif  // WGEVIEWER_H
