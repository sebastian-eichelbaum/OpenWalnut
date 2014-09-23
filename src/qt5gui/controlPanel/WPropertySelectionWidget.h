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

#ifndef WPROPERTYSELECTIONWIDGET_H
#define WPROPERTYSELECTIONWIDGET_H

#include <string>

#include <QtGui/QComboBox>
#include <QtGui/QListWidget>
#include <QtGui/QHBoxLayout>

#include "WPropertyWidget.h"

/**
 * Implements a property widget for WPropSelection.
 */
class WPropertySelectionWidget: public WPropertyWidget
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
    WPropertySelectionWidget( WPropSelection property, QGridLayout* propertyGrid, QWidget* parent = 0 );

    /**
     * Destructor.
     */
    virtual ~WPropertySelectionWidget();

protected:
    /**
     * Called whenever the widget should update.
     */
    virtual void update();

    /**
     * The integer property represented by this widget.
     */
    WPropSelection m_selectionProperty;

    /**
     * The list holding all items
     */
    QListWidget* m_list;

    /**
     * The combobox holding all items.
     */
    QComboBox* m_combo;

    /**
     * Layout used to position the label and the checkbox
     */
    QGridLayout m_layout;

    /**
     * True if a selection update is currently in progress. This is needed as QT does not provide a signal for selection changes which is NOT
     * called when changed programmatically.
     */
    bool m_update;

private:
public slots:

    /**
     * Called whenever the selection in m_list has changed.
     */
    void listSelectionChanged();

    /**
     * Selection of the combobox has changed.
     *
     * \param index the new index
     */
    void comboSelectionChanged( int index );
};

#endif  // WPROPERTYSELECTIONWIDGET_H

