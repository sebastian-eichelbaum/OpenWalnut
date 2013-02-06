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

#ifndef WPROPERTYTRIGGERWIDGET_H
#define WPROPERTYTRIGGERWIDGET_H

#include <string>

#include <QtGui/QCheckBox>
#include <QtGui/QPushButton>
#include <QtGui/QHBoxLayout>

#include "../guiElements/WScaleLabel.h"

#include "WPropertyWidget.h"

/**
 * Implements a property widget for WPropTrigger.
 */
class WPropertyTriggerWidget: public WPropertyWidget
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
    WPropertyTriggerWidget( WPropTrigger property, QGridLayout* propertyGrid, QWidget* parent = 0 );

    /**
     * Destructor.
     */
    virtual ~WPropertyTriggerWidget();

    /**
     * Returns the QT PushButton widget used. It always returns a valid pointer.
     *
     * \return the button
     */
    virtual QPushButton* getButton();

protected:
    /**
     * Called whenever the widget should update.
     */
    virtual void update();

    /**
     * The boolean property represented by this widget.
     */
    WPropTrigger m_triggerProperty;

    /**
     * If asButton is set to true: use this button instead of the m_checkbox
     */
    QPushButton m_button;

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
     * called whenever the user modifies the widget
     */
    void changed();
};

#endif  // WPROPERTYTRIGGERWIDGET_H

