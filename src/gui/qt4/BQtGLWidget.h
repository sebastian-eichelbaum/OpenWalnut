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

#ifndef BQTGLWIDGET_H
#define BQTGLWIDGET_H

#include <QtOpenGL/QGLWidget>


class BGLScenePainter;

/**
 * \ingroup gui
 * A widget containing an open gl display area.
 */
class BQtGLWidget: public QGLWidget
{
    public:
        explicit BQtGLWidget( QWidget *parent = 0 );
        BQtGLWidget( BGLScenePainter* scenePainter, QWidget *parent = 0 );
        virtual ~BQtGLWidget();

        /**
         * returns the recommended size for the widget to allow
         * parent widgets to give it a proper initial layout
         */
        QSize sizeHint() const;

        /**
         * This virtual function is called whenever the widget has been
         * resized. The new size is passed in width and height.
         *
         * \note It seem that the function is not called if declared const, so it
         * is not const.
         */
        void resizeGL( int width, int height );

    protected:
        /**
         * This function is called once before the first
         * call to paintGL() or resizeGL(), and then once whenever
         * the widget has been assigned a new QGLContext.
         *
         * \note It seem that the function is not called if declared const, so it
         * is not const.
         */
        void initializeGL();

    private:
        /**
         * This function is called whenever the widget needs to be painted.
         *
         * \note It seem that the function is not called if declared const, so it
         * is not const.
         */
        void paintGL();

        /**
         * Holds the GL scene painter of the widget
         */
        BGLScenePainter* m_scenePainter;

        /**
         * Holds the recommended size for the widget
         */
        QSize m_recommendedSize;
};

#endif  // BQTGLWIDGET_H
