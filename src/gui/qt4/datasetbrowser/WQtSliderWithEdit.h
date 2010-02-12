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

#include "WPropertyWidget.h"

/**
 * container widget for a slider with a connected number edit
 */
class WQtSliderWithEdit : public QWidget
{
    Q_OBJECT

public:
    /**
     * default constructor
     * \param property the property to handle
     * \param parent the widget that is the parent of this widget and thus manages it.
     */
    explicit WQtSliderWithEdit( boost::shared_ptr< WPropertyBase > property, QWidget* parent = 0 );

    /**
     * destructor
     */
    virtual ~WQtSliderWithEdit();

    /**
     * Setter for name.
     *
     * \param name the new name for the GUI element.
     */
    void setName( QString name );

    /**
     * Setter for min value.
     *
     * \param min the new minimum value.
     */
    void setMin( int min );

    /**
     * Setter for max value.
     *
     * \param max the new maximum value.
     */
    void setMax( int max );

    /**
     * Setter for current value.
     *
     * \param value the new current value.
     */
    void setValue( int value );

public slots:

    /**
     * emits the slider value to the outside world
     */
    void emitValue();

protected:
private:
    QSlider m_slider; //!< The slider of this GUI element;
    WQtNumberEdit m_edit; //!< The number field of the this GUI element.
    QHBoxLayout m_layout; //!< The layout organizing the positioning of the edit and the slider.

    QString m_name; //!< The name of this gui element.

signals:
    /**
     * Signal to propagate the value of the slider.
     *
     * \param name name of the slider
     * \param number actual value
     */
    void signalNumberWithName( QString name, int number );
};

#endif  // WQTSLIDERWITHEDIT_H
