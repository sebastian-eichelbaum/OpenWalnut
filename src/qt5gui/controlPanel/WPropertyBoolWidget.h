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

#ifndef WPROPERTYBOOLWIDGET_H
#define WPROPERTYBOOLWIDGET_H

#include <string>

#include <QtGui/QCheckBox>
#include <QtGui/QAction>
#include <QtGui/QHBoxLayout>

#include "../guiElements/WScaleLabel.h"

#include "WPropertyWidget.h"

/**
 * Implements a property widget for WPropBool.
 */
class WPropertyBoolWidget: public WPropertyWidget
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
    WPropertyBoolWidget( WPropBool property, QGridLayout* propertyGrid, QWidget* parent = 0 );

    /**
     * Destructor.
     */
    virtual ~WPropertyBoolWidget();

protected:
    /**
     * Called whenever the widget should update.
     */
    virtual void update();

    /**
     * The boolean property represented by this widget.
     */
    WPropBool m_boolProperty;

    /**
     * The checkbox - represents the boolean value.
     */
    QCheckBox m_checkbox;

    /**
     * Layout used to position the label and the checkbox
     */
    QHBoxLayout m_layout;

    /**
     * Used to show the property as text.
     */
    WScaleLabel m_asText;

    /**
     * The layout used for the pure output (information properties)
     */
    QHBoxLayout m_infoLayout;

private:
public slots:

    /**
     * called whenever the user modifies the checkbox
     */
    void checkboxChanged();
};

#endif  // WPROPERTYBOOLWIDGET_H

