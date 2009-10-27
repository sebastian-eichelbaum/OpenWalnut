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

#ifndef WQTSLIDERWITHEDIT_H
#define WQTSLIDERWITHEDIT_H

#include <string>

#include "WQtNumberEdit.h"
#include <QtGui/QSlider>
#include <QtGui/QHBoxLayout>

/**
 * container widget for a slider with a connected number edit
 */
class WQtSliderWithEdit : public QWidget
{
    Q_OBJECT

public:
    /**
     * default constructor
     */
    explicit WQtSliderWithEdit( QString name, QWidget* parent = 0 );

    /**
     * destructor
     */
    virtual ~WQtSliderWithEdit();

    /**
     * setter for name
     */
    void setName( QString name );

    /**
     * setter for min value
     */
    void setMin( int min );

    /**
     * setter for max value
     */
    void setMax( int max );

    /**
     * setter for current value
     */
    void setValue( int value );

public slots:
    /**
     * emits the slider value to the outside world
     */
    void emitValue();


protected:
private:
    QSlider m_slider;
    WQtNumberEdit m_edit;
    QHBoxLayout m_layout;

    QString m_name;

signals:
    void signalNumberWithName( QString name, int number );
};

#endif  // WQTSLIDERWITHEDIT_H
