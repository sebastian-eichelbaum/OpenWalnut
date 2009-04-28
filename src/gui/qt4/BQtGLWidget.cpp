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

#include "BQtGLWidget.h"
#include "../../graphicsEngine/BGLScenePainter.h"

BQtGLWidget::BQtGLWidget( BGLScenePainter* scenePainter, QWidget *parent )
    : QGLWidget( parent ),
      m_scenePainter( scenePainter ),
      m_recommendedSize()
{
    m_recommendedSize.setWidth( 200 );
    m_recommendedSize.setHeight( 200 );
}

BQtGLWidget::BQtGLWidget( QWidget *parent )
    : QGLWidget( parent ),
      m_recommendedSize()
{
    m_recommendedSize.setWidth( 200 );
    m_recommendedSize.setHeight( 200 );
    m_scenePainter = new BGLScenePainter();
}

BQtGLWidget::~BQtGLWidget()
{
    // TODO(wiebel): Auto-generated destructor stub
}

void BQtGLWidget::paintGL()
{
    // TODO(wiebel): we have to have something automatic in here in the future
    m_scenePainter->paintGL();
}

QSize BQtGLWidget::sizeHint() const
{
    return m_recommendedSize;
}

void BQtGLWidget::resizeGL( int width, int height )
{
    m_scenePainter->resizeGL( width, height );
}

void BQtGLWidget:: initializeGL()
{
    m_scenePainter->initGL();
}
