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

#ifndef WGECAMERA_H
#define WGECAMERA_H

#include <osg/Camera>



/**
 * Class for wrapping around the OSG Camera class. It adds some utility
 * functions for simply setting some camera defaults.
 * \ingroup ge
 */
class WGECamera: public osg::Camera
{
public:
    /**
     * List of possible camera modes. The TWO_D modes use a standard two dimensional orthogonal projection. TWO_D_UNOT is somewhat special. It
     * creates a view-cube with an edge-length of 1, centered at 0 for X and Y. For Z, it is from 0 to 1. This relates to the standard glOrtho
     * command.
     */
    enum ProjectionMode
    {
        ORTHOGRAPHIC,
        PERSPECTIVE,
        TWO_D,  // two dimensional ortographic projection, dimension is viewport
        TWO_D_UNIT // like TWO_D but size is unit-cube with proper scaling and aspect ratio. This is useful for widgets where viewport size is
                   // unimportant.
    };

    /**
     * Constructor which sets defaults
     *
     * \param width width of the viewport.
     * \param height height of the viewport.
     * \param projectionMode projection mode of the viewer.
     */
    WGECamera( int width, int height, ProjectionMode projectionMode );

    /**
     * Sets the default projection mode used for cameras getting reset.
     *
     * \param mode the mode to set.
     */
    void setDefaultProjectionMode( ProjectionMode mode );

    /**
     * Returns the current default projection mode.
     *
     * \return the currently set mode.
     */
    ProjectionMode getDefaultProjectionMode();

    /**
     * Resets the camera and activates the prior set defaults.
     */
    void reset();

    /**
     * Change camera parameters which should be changed on a resize.
     */
    void resize();

protected:
    /**
     * Destructor. This desctructor is protected to avoid accidentally deleting
     * a instance of WGECamera.
     * This follows the philosophy of OSG to avoid problems in multithreaded
     * environments, since these camera pointers are used deep in the OSG where
     * an deletion could cause a segfault.
     */
    virtual ~WGECamera();

    /**
     * The projection mode used as default.
     */
    ProjectionMode m_DefProjMode;

private:
};

#endif  // WGECAMERA_H
