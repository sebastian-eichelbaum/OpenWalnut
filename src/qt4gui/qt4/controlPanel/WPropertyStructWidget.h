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

#ifndef WPROPERTYSTRUCTWIDGET_H
#define WPROPERTYSTRUCTWIDGET_H

#include <string>

#include <QtGui/QCheckBox>
#include <QtGui/QAction>
#include <QtGui/QHBoxLayout>

#include <core/common/WPropertyGroupBase.h>

#include "WPropertyWidget.h"

/**
 * Implements a property widget for arbitrary WPropStruct. It only needs the WPropertyGroupBase interface, so can be used for other props
 * providing this interface. But this widget assumes that the number of props inside the group is fixed.
 */
class WPropertyStructWidget: public WPropertyWidget
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
    WPropertyStructWidget( WPropertyGroupBase::SPtr property, QGridLayout* propertyGrid, QWidget* parent = 0 );

    /**
     * Destructor.
     */
    virtual ~WPropertyStructWidget();

protected:

    /**
     * Called whenever the widget should update.
     */
    virtual void update();

    /**
     * The struct property
     */
    WPropertyGroupBase::SPtr m_struct;

    /**
     * Layout used to position the label and the checkbox
     */
    QHBoxLayout m_layout;

private:
    /**
     * List of widgets of thiss type
     */
    typedef std::vector< WPropertyWidget* > WidgetList;

    /**
     * list of widgets maintained to forward update calls
     */
    WidgetList m_widgets;
};

#endif  // WPROPERTYSTRUCTWIDGET_H

