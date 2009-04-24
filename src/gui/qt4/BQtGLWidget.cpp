//---------------------------------------------------------------------------
//
// Project: BrainCognize
//
//---------------------------------------------------------------------------

#include "BQtGLWidget.h"
#include "../../graphicsEngine/BGLScenePainter.h"

BQtGLWidget::BQtGLWidget( BGLScenePainter* scenePainter, QWidget *parent ) :
    m_scenePainter( scenePainter ), QGLWidget( parent ), m_recommendedSize()
{
    m_recommendedSize.setWidth( 200 );
    m_recommendedSize.setHeight( 200 );

}

BQtGLWidget::BQtGLWidget( QWidget *parent ) :
    QGLWidget( parent ), m_recommendedSize()
{
    m_recommendedSize.setWidth( 200 );
    m_recommendedSize.setHeight( 200 );
    m_scenePainter = new BGLScenePainter();

}

BQtGLWidget::~BQtGLWidget()
{
    // TODO Auto-generated destructor stub
}

void BQtGLWidget::paintGL()
{
    // TODO we have to have something automatic in here in the future
    m_scenePainter->paintGL();
}

QSize BQtGLWidget::sizeHint() const
{
    return m_recommendedSize;
}

