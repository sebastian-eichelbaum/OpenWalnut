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

#ifndef WPROPERTYDOUBLEWIDGET_H
#define WPROPERTYDOUBLEWIDGET_H

#include <string>

#include <QtGui/QLineEdit>
#include <QtGui/QSlider>
#include <QtGui/QHBoxLayout>
#include <QtGui/QToolButton>

#include "../guiElements/WQtIntervalEdit.h"
#include "WPropertyWidget.h"

/**
 * Implements a property widget for WPropDouble.
 */
class WPropertyDoubleWidget: public WPropertyWidget
{
    Q_OBJECT
public:
    /**
     * Constructor. Creates a new widget appropriate for the specified property.
     *
     * \param property the property to handle
     * \param parent the parent widget.
     * \param propertyGrid the grid used to layout the labels and property widgets
     */
    WPropertyDoubleWidget( WPropDouble property, QGridLayout* propertyGrid, QWidget* parent = 0 );

    /**
     * Destructor.
     */
    virtual ~WPropertyDoubleWidget();

protected:
    /**
     * Called whenever the widget should update.
     */
    virtual void update();

    /**
     * The property represented by this widget.
     */
    WPropDouble m_integralProperty;

    /**
     * The slider allowing modification of the integer value
     */
    QSlider m_slider;

    /**
     * The edit field showing the value of the slider
     */
    QLineEdit m_edit;

    /**
     * Layout used to position the label and the checkbox
     */
    QHBoxLayout m_layout;

    /**
     * Layout used to combine the property widgets with the WQtIntervalEdit.
     */
    QVBoxLayout m_vLayout;

    /**
     * Converts a given value to a slider value between m_min and m_max.
     *
     * \param value the value.
     *
     * \return the percentage.
     */
    int toSliderValue( double value );

    /**
     * Converts the given slider value to the real double value using m_min and m_max.
     *
     * \param perc the percentage.
     *
     * \return the real double value.
     */
    double fromSliderValue( int perc );

    /**
     * The current minimum value.
     */
    double m_min;

    /**
     * The current maximum value.
     */
    double m_max;

    /**
     * Used to show the property as text.
     */
    QLabel m_asText;

    /**
     * The layout used for the pure output (information properties)
     */
    QHBoxLayout m_infoLayout;

    /**
     * The edit for the interval.
     */
    WQtIntervalEdit< double, double > m_intervalEdit;

    /**
     * Resolution of the slider.
     */
    const static int SliderResolution;

private:
public slots:

    /**
     * Called whenever the slider changes
     *
     * \param value the new value
     */
    void sliderChanged( int value );

    /**
     * Called whenever the edit field changes
     */
    void editChanged();

    /**
     * Called when the text in m_edit changes.
     *
     * \param text
     */
    void textEdited( const QString& text );

    /**
     * Called whenever the interval edit changes.
     */
    void minMaxUpdated();
};

#endif  // WPROPERTYDOUBLEWIDGET_H

