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

#ifndef WGRAPHICSENGINE_H
#define WGRAPHICSENGINE_H

#include <map>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/thread/mutex.hpp>

#include <osg/Camera>
#include <osg/Texture3D>
#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/ref_ptr>
#include <osgViewer/CompositeViewer>

#include "../common/WThreadedRunner.h"
#include "../common/WConditionOneShot.h"
#include "../common/WColor.h"
#include "WGEGraphicsWindow.h"
#include "WGEScene.h"
#include "WGEViewer.h"
#include "WGESignals.h"

/**
 * Base class for initializing the graphics engine. This Class also serves as adaptor to access the graphics
 * engine.
 * \ingroup ge
 */
class WGraphicsEngine: public WThreadedRunner
{
public:
    /**
     * Destructor.
     */
    virtual ~WGraphicsEngine();

    /**
     * Returns the root node of the WGraphicsEngine (this is not the root node of the OSG).
     *
     * \return the root node.
     */
    osg::ref_ptr<WGEScene> getScene();

    /**
     * Creates a new viewer. Does basic initialization and sets the default scene.
     *
     * \param name the name of the viewer
     * \param wdata the WindowData instance for the widget to use as render widget
     * \param x X coordinate of widget where to create the context.
     * \param y Y coordinate of widget where to create the context.
     * \param width Width of the widget.
     * \param height Height of the Widget.
     * \param projectionMode Projection mode of the viewer.
     * \param bgColor background color shown in the viewer.
     * \return the new instance, ready to be used.
     * \exception WGEInitFailed thrown if initialization of graphics context or graphics window has failed.
     */
    boost::shared_ptr< WGEViewer > createViewer( std::string name, osg::ref_ptr<osg::Referenced> wdata, int x, int y,
                                               int width, int height, WGECamera::ProjectionMode projectionMode = WGECamera::ORTHOGRAPHIC,
                                               WColor bgColor = WColor( 0.9, 0.9, 0.9, 1.0 ) );

    /**
     * Closes a viewer and deletes it from the list of viewers.
     *
     * \param name the name of the viewer
     */
    void closeViewer( const std::string name );

    /**
     * Searches for a viewer with a given name and returns it, if found.
     *
     * \param name the name of the viewer
     * \returns a shared pointer to the viewer or NULL if not found
     */
    boost::shared_ptr< WGEViewer > getViewerByName( std::string name );

    /**
     * Returns the unnamed view, which is the view for the default scene which can be acquired using getScene().
     *
     * \return the viewer for the default scene.
     */
    boost::shared_ptr< WGEViewer > getViewer();

    /**
     * Returns instance of the graphics engine. If it does not exists, it will be created.
     *
     * \return the running graphics engine instance.
     */
    static boost::shared_ptr< WGraphicsEngine > getGraphicsEngine();

    /**
     * This requests all shaders to reload during the next update cycle.
     */
    void requestShaderReload();

    /**
     * Subscribe a specified handler to the specified signal emited by the GE.
     *
     * \param signal    the signal to connect to
     * \param notifier the signal handler
     *
     * \return  connection object.
     */
    boost::signals2::connection subscribeSignal( GE_SIGNAL signal, t_GEGenericSignalHandlerType notifier );

    /**
     * Checks whether the graphics engine is currently running or not.
     *
     * \return true if running
     */
    static bool isRunning();

    /**
     * Waits for the GE to come up. Fails if engine is not started.
     *
     * \return true if engine now running
     */
    static bool waitForStartupComplete();

    /**
     * Function notifies the viewer threads (if any) to start. This should only be called AFTER the OpenGL widgets/windows have been initialized.
     */
    void finalizeStartup();

    /**
     * Wait until someone called \ref finalizeStartup().
     */
    void waitForFinalize();

    /**
     * Enables multithreaded view. This MUST be called before run(). On Mac, this has no function.
     *
     * \param enable true if multithreaded
     */
    void setMultiThreadedViews( bool enable = true );

    /**
     * Checks whether the viewers work multithreaded.
     *
     * \return true if multithreaded
     */
    bool isMultiThreadedViews() const;

protected:
    /**
     * Constructors are protected because this is a Singleton.
     */
    explicit WGraphicsEngine();

    /**
     * Handler for repainting and event handling. Gets executed in separate thread.
     */
    virtual void threadMain();

    /**
     * Gets called when the thread should be stopped.
     */
    virtual void notifyStop();

    /**
     * OpenSceneGraph root node.
     */
    osg::ref_ptr<WGEScene> m_rootNode;

    /**
     * All registered viewers.
     */
    std::map< std::string, boost::shared_ptr< WGEViewer > > m_viewers;

    /**
     * Mutex used to lock the map of viewers.
     */
    boost::mutex m_viewersLock;

    /**
     * OpenSceneGraph composite viewer. Contains all created osgViewer::Views.
     */
    osg::ref_ptr<osgViewer::CompositeViewer> m_viewer;

    /**
     * Signal getting emitted whenever a reload shader request is waiting.
     */
    t_GEGenericSignalType m_reloadShadersSignal;

private:
    /**
     * Singleton instance of WGraphicsEngine.
     */
    static boost::shared_ptr< WGraphicsEngine > m_instance;

    /**
     * True if graphics engine is running.
     */
    bool m_running;

    /**
     * This condition is fired externally if all the GUI startup is done to ensure all OGL stuff is initialized prior to OSG threading startup.
     */
    WConditionOneShot m_startThreadingCondition;
};

/**
 * \defgroup ge GraphicsEngine
 *
 * \brief
 * This library implements the graphics engine for OpenWalnut.
 */

/**
 * Convinient functions for use with the graphics engine of OpenWalnut. ATM the
 * namespace is filled by several files: WGEGeodeUtils, WGEGeometryUtils and
 * WGEUtils.
 */
namespace wge
{
} // end of namespace

#endif  // WGRAPHICSENGINE_H
