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

#ifndef WPROPERTYINTERVALWIDGET_H
#define WPROPERTYINTERVALWIDGET_H

#include <string>

#include <QtGui/QLineEdit>
#include <QtGui/QSlider>
#include <QtGui/QHBoxLayout>
#include <QtGui/QToolButton>

#include "../guiElements/WQtIntervalEdit.h"
#include "WPropertyWidget.h"

/**
 * Implements a property widget for WPropInterval.
 */
class WPropertyIntervalWidget: public WPropertyWidget
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
    WPropertyIntervalWidget( WPropInterval property, QGridLayout* propertyGrid, QWidget* parent = 0 );

    /**
     * Destructor.
     */
    virtual ~WPropertyIntervalWidget();

protected:
    /**
     * Called whenever the widget should update.
     */
    virtual void update();

    /**
     * The property represented by this widget.
     */
    WPropInterval m_intervalProperty;

    /**
     * Layout used to position the label and the checkbox
     */
    QHBoxLayout m_layout;

    /**
     * Layout used to combine the property widgets with the WQtIntervalEdit.
     */
    QVBoxLayout m_vLayout;

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

private:
public slots:

    /**
     * Called whenever the interval edit changes.
     */
    void minMaxUpdated();
};

#endif  // WPROPERTYINTERVALWIDGET_H

