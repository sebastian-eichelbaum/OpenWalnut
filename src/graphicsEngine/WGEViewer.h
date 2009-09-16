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

#ifndef WGEVIEWER_H
#define WGEVIEWER_H

#include <boost/shared_ptr.hpp>

#include <osg/Node>

#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/TerrainManipulator>
#include <osgGA/UFOManipulator>

#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <osgViewer/View>

#include "../common/WThreadedRunner.h"

#include "WGEGraphicsWindow.h"

/**
 * Class for managing one viewer to the scene. This includes viewport, camera and graphics context.
 * It is, besides WGraphicsEngine, the ONLY entry point for each widget for accessing the graphics engine.
 * \ingroup ge
 */
class WGEViewer: public WGEGraphicsWindow,
                 public WThreadedRunner
{
public:

    /**
     * Default constructor.
     *
     * \param wdata the WindowData instance for the widget to use as render widget
     * \param x X coordinate of widget where to create the context.
     * \param y Y coordinate of widget where to create the context.
     * \param width Width of the widget.
     * \param height Height of the Widget.
     * \exception WGEInitFailed thrown if initialization of graphics context or graphics window has failed.
     */
    WGEViewer( osg::ref_ptr<WindowData> wdata, int x, int y, int width, int height );

    /**
     * Destructor.
     */
    virtual ~WGEViewer();

    /**
     * Paints this view to the window specified in constructor.
     */
    void paint();

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
     * Getter for OpenSceneGraph Viewer instance.
     *
     * \return the OSG Viewer instance.
     */
    osg::ref_ptr<osgViewer::CompositeViewer> getViewer();

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
    void setScene( osg::ref_ptr<osg::Node> node );

    /**
     * Returns the currently set OSG node.
     *
     * \return the node.
     */
    osg::ref_ptr<osg::Node> getNode();

protected:
    /**
     * Handler for repainting and event handling. Gets executed in separate thread.
     */
    virtual void threadMain();

    /**
     * OpenSceneGraph viewer.
     */
    osg::ref_ptr<osgViewer::CompositeViewer> m_Viewer;

    /**
     * The OpenSceneGraph view used in this (Composite)Viewer.
     */
    osg::ref_ptr<osgViewer::View> m_View;

private:
};

#endif  // WGEVIEWER_H

