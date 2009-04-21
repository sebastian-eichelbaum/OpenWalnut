//---------------------------------------------------------------------------
//
// Project: BrainCognize
//
//---------------------------------------------------------------------------

#ifndef BQTGLWIDGET_H
#define BQTGLWIDGET_H

#include <QtOpenGL/QGLWidget>

/**
 * A widget containing an open gl display area.
 */
class BQtGLWidget: public QGLWidget
{
public:
    BQtGLWidget( QWidget *parent = 0 );
    virtual ~BQtGLWidget();

    /**
     * returns the recommended size for the widget to allow
     * parent widgets to give it a proper initial layout
     */
    QSize sizeHint() const;
private:
    /**
     * This function is called whenever the widget needs to be painted.
     */
    void paintGL();

    /**
     * Holds the recommended size for the widget
     */
    QSize m_recommendedSize;
};

#endif /* BQTGLWIDGET_H */
