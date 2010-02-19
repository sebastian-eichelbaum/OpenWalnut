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

#include <QtGui/QDoubleSpinBox>
#include <QtGui/QHBoxLayout>

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
     * The integer property represented by this widget.
     */
    WPropDouble m_doubleProperty;

    /**
     * Spin box for doubles
     */
    QDoubleSpinBox m_spin;

    /**
     * Layout used to position the label and the checkbox
     */
    QHBoxLayout m_layout;

private:

public slots:

    /**
     * Called whenever return is pressed or the spin box has a chaged value and looses focus
     */
    void spinChanged();
};

#endif  // WPROPERTYDOUBLEWIDGET_H

