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

#include <cassert>
#include <GL/glew.h>

#include "WGLScenePainter.h"

WGLScenePainter::WGLScenePainter()
{
    // TODO(wiebel): Auto-generated constructor stub
}

WGLScenePainter::~WGLScenePainter()
{
    // TODO(wiebel): Auto-generated destructor stub
}

void WGLScenePainter::initGL() const
{
    GLenum glewInitResult = glewInit();
    assert( glewInitResult == GLEW_OK );
    GLenum error = glGetError();
    assert( error == GL_NO_ERROR );

    // Set clear color
    glClearColor( 1., 1., 1., 0 );
}

void WGLScenePainter::paintGL() const
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glLoadIdentity();

    glBegin( GL_QUADS );
    glColor3f( 0.5, 0.5, 0.9 );
    glVertex3d( 0, 0, -0.05 );
    glVertex3d( .5, 0, -0.05 );
    glVertex3d( .5, .5, -0.05 );
    glVertex3d( 0, .5, -0.05 );

    glColor3f( 1, 0.5, 0.9 );
    glVertex3d( .25, 0.3, -0.05 );
    glVertex3d( .25, 0.3, -0.05 );
    glVertex3d( 5, .5, -0.05 );
    glVertex3d( .25, .5, -0.05 );

    glEnd();
}

void WGLScenePainter::resizeGL( int width, int height ) const
{
    glViewport( 0, 0, width, height );
}
