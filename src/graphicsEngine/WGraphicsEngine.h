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
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

#include <osg/Camera>
#include <osg/Texture3D>
#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <osgViewer/View>

#include "WGEScene.h"
#include "WGEGraphicsWindow.h"
#include "WGEViewer.h"
#include "../common/WThreadedRunner.h"


/**
 * Base class for initializing the graphics engine. This Class also serves as adaptor to access the graphics
 * engine.
 * \ingroup ge
 */
class WGraphicsEngine: public WThreadedRunner
{
public:

    /**
     * Default constructor.
     */
    explicit WGraphicsEngine();

    /**
     * Destructor.
     */
    virtual ~WGraphicsEngine();

    /**
     * Returns the root node of the WGraphicsEngine ( this is not the root node of the OSG).
     *
     * \return the root node.
     */
    osg::ref_ptr<WGEScene> getScene();

    /**
     * Return the path where the shaders are expected to be located.
     *
     * \return shader path
     */
    std::string getShaderPath() const;

    /**
     * Sets the shader path.
     * 
     * \param path path to shaders.
     */
    void setShaderPath( std::string path );

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
     * \return the new instance, ready to be used.
     * \exception WGEInitFailed thrown if initialization of graphics context or graphics window has failed.
     */
    boost::shared_ptr<WGEViewer> createViewer( std::string name, osg::ref_ptr<WindowData> wdata, int x, int y,
        int width, int height, WGECamera::ProjectionMode projectionMode = WGECamera::ORTHOGRAPHIC );

    /**
     * Searchs for a viewer with a given name and returns it, if found.
     */
    boost::shared_ptr<WGEViewer> getViewerByName( std::string name );

protected:

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
    std::map< std::string, boost::shared_ptr< WGEViewer > > m_Viewers;

    /**
     * Mutex used to lock the map of viewers.
     */
    boost::mutex m_ViewersLock;

    /**
     * Path to the shaders.
     */
    std::string m_shaderPath;

    /**
     * OpenSceneGraph composite viewer. Contains all created osg::Viewer.
     */
    osg::ref_ptr<osgViewer::CompositeViewer> m_Viewer;

private:
};

/**
 * \defgroup ge GraphicsEngine
 *
 * \brief
 * This module implements the graphics engine for OpenWalnut.
 */

#endif  // WGRAPHICSENGINE_H

