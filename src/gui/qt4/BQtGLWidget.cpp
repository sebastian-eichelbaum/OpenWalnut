//---------------------------------------------------------------------------
//
// Project: BrainCognize
//
//---------------------------------------------------------------------------

#include "BQtGLWidget.h"

BQtGLWidget::BQtGLWidget( QWidget *parent ) :
    QGLWidget( parent ), m_recommendedSize()
{
    m_recommendedSize.setWidth( 200 );
    m_recommendedSize.setHeight( 200 );

}

BQtGLWidget::~BQtGLWidget()
{
    // TODO Auto-generated destructor stub
}

void BQtGLWidget::paintGL()
{
    // TODO we have to have something automatic in here in the future
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glLoadIdentity();

    glBegin( GL_QUADS );
    glColor3f( 0.5, 0.5, 0.9 );
    glVertex3d( 0, 0, -0.05 );
    glVertex3d( .5, 0, -0.05 );
    glVertex3d( .5, .5, -0.05 );
    glVertex3d( 0, .5, -0.05 );

    glEnd();
}

QSize BQtGLWidget::sizeHint() const
{
    return m_recommendedSize;
}

