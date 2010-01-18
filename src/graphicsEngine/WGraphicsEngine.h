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
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>

#include <osg/Camera>
#include <osg/Texture3D>
#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/ref_ptr>
#include <osgViewer/CompositeViewer>
#include <osgViewer/View>
#include <osgViewer/Viewer>

#include "../common/WThreadedRunner.h"
#include "../math/WPosition.h"
#include "WGEGraphicsWindow.h"
#include "WGEScene.h"
#include "WGEViewer.h"

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
     * \param bgColor background color shown in the viewer.
     * \return the new instance, ready to be used.
     * \exception WGEInitFailed thrown if initialization of graphics context or graphics window has failed.
     */
    boost::shared_ptr< WGEViewer > createViewer( std::string name, osg::ref_ptr<WindowData> wdata, int x, int y,
                                               int width, int height, WGECamera::ProjectionMode projectionMode = WGECamera::ORTHOGRAPHIC,
                                               WColor bgColor = WColor( .9, .9, .9 ) );

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
     * Returns the currently selected default font usable in osgText nodes.
     *
     * \return Filename of the default font.
     *
     * \note this also contains the absolute path.
     */
    std::string getDefaultFont();

    /**
     * Returns instance of the graphics engine. If it does not exists, it will be created.
     *
     * \return the running graphics engine instance.
     */
    static boost::shared_ptr< WGraphicsEngine > getGraphicsEngine();

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

    /**
     * Singleton instance of WGraphicsEngine.
     */
    static boost::shared_ptr< WGraphicsEngine > m_instance;
};

/**
 * \defgroup ge GraphicsEngine
 *
 * \brief
 * This module implements the graphics engine for OpenWalnut.
 */

/**
 * Convinient functions for use with the graphics engine of OpenWalnut.
 */
namespace wge
{
    /**
     * Converts a WColor to an OSG compatible color
     *
     *\param color The color in WColor format
     *\return A color which may be used inside of OSG
     */
    osg::Vec4 osgColor( const WColor& color );

    /**
     * Converts a given WPosition into an osg::Vec3.
     *
     * \param pos The WPosition which should be converted
     *
     * \return The osg::Vec3 vector of pos
     */
    osg::Vec3 osgVec3( const wmath::WPosition& pos );

    /**
     * Converts a whole vector of WPositions into an osg::Vec3Array.
     *
     * \param posArray The given positions vector
     *
     * \return Refernce to the same vector but as osg::Vec3Array.
     */
    osg::ref_ptr< osg::Vec3Array > osgVec3Array( const std::vector< wmath::WPosition >& posArray );

    /**
     * Creates out of eight corner vertices QUAD vertices.
     *
     * \param corners The eight corner vertices which must be in the following order:
     *
     * \verbatim
        z-axis  y-axis
        |      /
        | h___/_g
        |/:    /|
        d_:___c |
        | :...|.|
        |.e   | f
        |_____|/ ____x-axis
       a      b
       \endverbatim
     *
     * \return OSG vertex array where every four vertices describing a QUAD.
     */
    osg::ref_ptr< osg::Vec3Array > generateCuboidQuads( const std::vector< wmath::WPosition >& corners );
} // end of namespace

#endif  // WGRAPHICSENGINE_H

