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

#include <list>
#include <string>


#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

#include <osg/Camera>
#include <osgViewer/Viewer>
#include <osg/Texture3D>

#include "WGEScene.h"
#include "WGEGraphicsWindow.h"
#include "WGEViewer.h"
#include "../common/WThreadedRunner.h"


/**
 * Base class for initializing the graphics engine. This Class also serves as adaptor to access the graphics
 * engine.
 * \ingroup ge
 */
class WGraphicsEngine
{
public:

    /**
     * Default constructor.
     */
    explicit WGraphicsEngine( std::string shaderPath );

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
    std::string getShaderPath();

    /**
     * Creates a new viewer. Does basic initialization and sets the default scene.
     *
     * \param wdata the WindowData instance for the widget to use as render widget
     * \param x X coordinate of widget where to create the context.
     * \param y Y coordinate of widget where to create the context.
     * \param width Width of the widget.
     * \param height Height of the Widget.
     * \param projectionMode Projection mode of the viewer.
     * \return the new instance, ready to be used.
     * \exception WGEInitFailed thrown if initialization of graphics context or graphics window has failed.
     */
    boost::shared_ptr<WGEViewer> createViewer( osg::ref_ptr<WindowData> wdata, int x, int y, int width, int height,
        WGECamera::ProjectionMode projectionMode = WGECamera::ORTHOGRAPHIC );

protected:

    /**
     * OpenSceneGraph root node.
     */
    osg::ref_ptr<WGEScene> m_rootNode;

    /**
     * All registered viewer.
     */
    std::list<boost::shared_ptr<WGEViewer> > m_Viewer;

    /**
     * Mutex used to lock the list of viewers.
     */
    boost::mutex m_ViewerLock;

private:
    std::string m_shaderPath;
};

/**
 * \defgroup ge GraphicsEngine
 *
 * \brief
 * This module implements the graphics engine for OpenWalnut.
 */

#endif  // WGRAPHICSENGINE_H

