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

#ifndef WQTNAVGLWIDGET_H
#define WQTNAVGLWIDGET_H

#include <string>

#include "WQtGLWidget.h"

#include <QtGui/QDockWidget>
/**
 * container widget to hold as GL widget and a slider
 */
class WQtNavGLWidget : public QDockWidget
{
    Q_OBJECT

public:
    /**
     * default constructor
     *
     * \param parent the parent widget.
     */
    explicit WQtNavGLWidget( QString title, QWidget* parent, int maxValue = 100, std::string sliderTitle="pos" );

    /**
     * destructor
     */
    virtual ~WQtNavGLWidget();

    /**
     * \return pointer to GL widget
     */
    boost::shared_ptr<WQtGLWidget>getGLWidget();

    /**
     *
     */
    void setSliderTitle( std::string title );

protected:

    /**
     * Event handler for close events.
     *
     * \param event the event description.
     */
    virtual void closeEvent( QCloseEvent* event );

private:
    QString m_sliderTitle;

    /**
     * My GL widget.
     */
    boost::shared_ptr<WQtGLWidget> m_glWidget;

private slots:
    void sliderValueChanged( int value );

signals:
    void navSliderValueChanged( QString name, int value );
};

#endif  // WQTNAVGLWIDGET_H
