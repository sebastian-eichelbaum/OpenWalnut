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

#ifndef WPROPERTYMATRIX4X4WIDGET_H
#define WPROPERTYMATRIX4X4WIDGET_H

#include <string>

#include <QLineEdit>
#include <QSlider>
#include <QHBoxLayout>

#include "../guiElements/WScaleLabel.h"

#include "WPropertyWidget.h"

class WScaleLabel;

/**
 * Implements a property widget for MATRIX4X4.
 */
class WPropertyMatrix4X4Widget: public WPropertyWidget
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
    WPropertyMatrix4X4Widget( WPropMatrix4X4 property, QGridLayout* propertyGrid, QWidget* parent = 0 );

    /**
     * Destructor.
     */
    virtual ~WPropertyMatrix4X4Widget();

protected:
    /**
     * Called whenever the widget should update.
     */
    virtual void update();

    /**
     * The integer property represented by this widget.
     */
    WPropMatrix4X4 m_matrixProperty;

    /**
     * The edit field showing the value of the slider
     */
    QLineEdit m_edits[16];

    /**
     * Layout used to position the label and the widgets
     */
    QVBoxLayout m_layout;

    /**
     * Used to show the property as text.
     */
    WScaleLabel m_asText;

    /**
     * The layout used for the pure output (information properties)
     */
    QHBoxLayout m_infoLayout;

    /**
     * Updates the property using the edit X Y and Z widgets.
     *
     * \param validateOnly true if the prop should not really be set. Just validated.
     */
    void setPropertyFromWidgets( bool validateOnly = false );

private:
public slots:

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
};

#endif  // WPROPERTYMATRIX4X4WIDGET_H

