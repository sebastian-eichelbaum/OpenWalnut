//---------------------------------------------------------------------------
//
// Project: BrainCognize
//
//---------------------------------------------------------------------------

#include <cassert>
#include <GL/gl.h>
#include <GL/glu.h>

#include "BGLScenePainter.h"

BGLScenePainter::BGLScenePainter()
{
    // TODO(wiebel): Auto-generated constructor stub
}

BGLScenePainter::~BGLScenePainter()
{
    // TODO(wiebel): Auto-generated destructor stub
}

void BGLScenePainter::initGL()
{
    GLenum error = glGetError();
    assert( error == GL_NO_ERROR );

    // Set clear color
    glClearColor( 0, 0, 0, 0 );
}

void BGLScenePainter::paintGL()
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

void BGLScenePainter::resizeGL( int width, int height )
{
    glViewport( 0, 0, width, height );
}
