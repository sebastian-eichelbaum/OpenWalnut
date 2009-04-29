//---------------------------------------------------------------------------
//
// Project: BrainCognize
//
// Copyright 2009 SomeCopyrightowner
//
// This file is part of BrainCognize.
//
// BrainCognize is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// BrainCognize is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with BrainCognize. If not, see <http://www.gnu.org/licenses/>.
//
//---------------------------------------------------------------------------

#include <cassert>
#include <GL/glew.h>

#include "BGLScenePainter.h"

BGLScenePainter::BGLScenePainter()
{
    // TODO(wiebel): Auto-generated constructor stub
}

BGLScenePainter::~BGLScenePainter()
{
    // TODO(wiebel): Auto-generated destructor stub
}

void BGLScenePainter::initGL() const
{
    GLenum glewError = glewInit();
    GLenum error = glGetError();
    assert( error == GL_NO_ERROR );

    // Set clear color
    glClearColor( 1., 1., 1., 0 );
}

void BGLScenePainter::paintGL() const
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

void BGLScenePainter::resizeGL( int width, int height ) const
{
    glViewport( 0, 0, width, height );
}
