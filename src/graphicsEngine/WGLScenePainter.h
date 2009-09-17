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

#ifndef WGLSCENEPAINTER_H
#define WGLSCENEPAINTER_H

/**
 * This is the base class for rendering the scene graph.
 * \ingroup ge
 */
class WGLScenePainter
{
public:
    WGLScenePainter();
    virtual ~WGLScenePainter();

    /**
     * Initialize the OpenGL context, e.g., set main transformation matrices.
     */
    virtual void initGL() const;

    /**
     * Do the OpenGL painting in every frame.
     */
    virtual void paintGL() const;

    /**
     * Called, when the window is resized and re-initialization of the
     * window parameters and matrices may need to be done.
     */
    virtual void resizeGL( int width, int height ) const;
};

#endif  // WGLSCENEPAINTER_H
