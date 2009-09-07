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

#include <osg/Node>

#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/TerrainManipulator>
#include <osgGA/UFOManipulator>

#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <osgViewer/View>

#include <boost/shared_ptr.hpp>


#include "WGEGraphicsWindow.h"

/**
 * \par Description:
 * Class for managing one viewer to the scene. This includes viewport, camera and graphics context.
 * It is, besides WGraphicsEngine, the ONLY entry point for each widget for accessing the graphics engine.
 * \ingroup ge
 */
class WGEViewer: public WGEGraphicsWindow
{
public:

    /**
     * \par Description
     * Default constructor.
     *
     * \param wdata the WindowData instance for the widget to use as render widget
     * \param x X coordinate of widget where to create the context.
     * \param y Y coordinate of widget where to create the context.
     * \param width Width of the widget.
     * \param height Height of the Widget.
     * \exception WGEInitFailed thrown if initialization of graphics context or graphics window has failed.
     */
    WGEViewer( boost::shared_ptr<WindowData> wdata, int x, int y, int width, int height );

    /**
     * \par Description
     * Destructor.
     */
    virtual ~WGEViewer();

    /**
     * \par Description
     * Paints this view to the window specified in constructor.
     */
    void paint();

    /**
     * \par Description
     * Updates size information. Also updates camera.
     *
     * \param width new width.
     * \param height new height.
     */
    virtual void resize( int width, int height );

    /**
     * \par Description
     * Getter for OpenSceneGraph Viewer instance.
     *
     * \return the OSG Viewer instance.
     */
    boost::shared_ptr<osgViewer::CompositeViewer> getViewer();

    /**
     * \par Description
     * Sets the camera manipulator to use.
     *
     * \param manipulator the manipulator to use.
     */
    void setCameraManipulator( osgGA::MatrixManipulator* manipulator );

    /**
     * \par Description
     * Returns current active camera manipulator
     *
     * \return the active camera manipulator.
     */
    osgGA::MatrixManipulator* getCameraManipulator();

    /** 
     * \par Description
     * Sets the current camera.
     * 
     * \param camera the OSG camera instance.
     */
    void setCamera( osg::Camera* camera );

    /** 
     * \par Description
     * Returns the camera currently in use.
     * 
     * \return the camera currently in use.
     */
    osg::Camera* getCamera();

    /** 
     * \par Description
     * Sets the scene graph node to be used for rendering.
     * 
     * \param node part of the scene graph
     */
    void setScene( osg::Node* node );

    /** 
     * \par Description
     * Returns the currently set OSG node.
     * 
     * \return the node.
     */
    osg::Node* getNode();

protected:

    /**
     * \par Description
     * OpenSceneGraph viewer.
     */
    boost::shared_ptr<osgViewer::CompositeViewer> m_Viewer;

    /**
     * \par Description
     * The OpenSceneGraph view used in this (Composite)Viewer.
     */
    boost::shared_ptr<osgViewer::View> m_View;

private:
};

#endif  // WGEVIEWER_H

