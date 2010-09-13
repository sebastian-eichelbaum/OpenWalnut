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

#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/TerrainManipulator>
#include <osgGA/UFOManipulator>

#include <osgViewer/View>

#include "../common/WThreadedRunner.h"
#include "../common/WColor.h"

#include "WGEGraphicsWindow.h"
#include "WGECamera.h"
#include "WPickHandler.h"
#include "WGEGroupNode.h"

#include "WExportWGE.h"


/**
 * Class for managing one view to the scene. This includes viewport, camera and graphics context.
 * It is, besides WGraphicsEngine, the ONLY entry point for each widget for accessing the graphics engine.
 * \ingroup ge
 */
class WGE_EXPORT WGEViewer: public WGEGraphicsWindow,
                                 public boost::enable_shared_from_this< WGEViewer >
{
public:
    /**
     * Default constructor.
     *
     * \param name the name of the viewer
     * \param x X coordinate of widget where to create the context.
     * \param y Y coordinate of widget where to create the context.
     * \param width Width of the widget.
     * \param height Height of the Widget.
     * \param projectionMode Projection mode of the viewer.
     * \exception WGEInitFailed thrown if initialization of graphics context or graphics window has failed.
     */
    WGEViewer( std::string name, int x, int y, int width, int height,
        WGECamera::ProjectionMode projectionMode = WGECamera::ORTHOGRAPHIC );

    /**
     * Destructor.
     */
    virtual ~WGEViewer();

    /**
     * Repaints the contents.
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
    osg::ref_ptr<osgViewer::Viewer> getView();

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
    void setBgColor( WColor bgColor );

    /**
     * Getter for the pick handler
     *
     * \return the pick handler
     */
    osg::ref_ptr< WPickHandler > getPickHandler();

protected:
    /**
     * The OpenSceneGraph view used in this (Composite)Viewer.
     */
    osg::ref_ptr< osgViewer::Viewer > m_View;

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

private:
};

#endif  // WGEVIEWER_H
