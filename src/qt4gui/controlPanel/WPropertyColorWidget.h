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

#ifndef WPROPERTYCOLORWIDGET_H
#define WPROPERTYCOLORWIDGET_H

#include <string>

#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QAction>
#include <QtGui/QHBoxLayout>
#include <QtGui/QToolButton>

#include "../guiElements/WScaleLabel.h"

#include "WPropertyWidget.h"

/**
 * Implements a property widget for WPropColor.
 */
class WPropertyColorWidget: public WPropertyWidget
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
    WPropertyColorWidget( WPropColor property, QGridLayout* propertyGrid, QWidget* parent = 0 );

    /**
     * Destructor.
     */
    virtual ~WPropertyColorWidget();

    /**
     * Hide the button for a more compact layout.
     *
     * \param hide true to hide
     */
    void setColorPickerButtonHidden( bool hide = true );

protected:
    /**
     * Internal helper, called to set the color
     * \param color the new color
     */
    virtual void setColor( const QColor& color );

    /**
     * Called whenever the widget should update.
     */
    virtual void update();

    /**
     * Reimplemented to accept color drops
     * \param event the handled event
     */
    virtual void dragEnterEvent( QDragEnterEvent* event );

    /**
     * Reimplemented to accept color drops
     * \param event the handled event
     */
    virtual void dropEvent( QDropEvent* event );

    /**
     * The integer property represented by this widget.
     */
    WPropColor m_colorProperty;

    /**
     * The button field showing the value
     */
    QWidget m_widget;

    /**
     * Color display panel
     */
    QWidget* m_colPanel;

    /**
     * Color picker button
     */
    QToolButton* m_colButton;

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

    /**
     * Color picker
     */
    QAction* m_colorPickerAction;
private:
    /**
     * Helper to convert between QColor and WColor.
     *
     * \param color a color
     *
     * \return the corresponding QColor.
     */
    static QColor toQColor( WColor color );

    /**
     * Helper to convert between QColor and WColor.
     *
     * \param color a color
     *
     * \return the corresponding WColor.
     */
     static WColor toWColor( QColor color );

public slots:

    /**
     * Called when the m_button is clicked
     */
    void buttonClicked();
};

#endif  // WPROPERTYCOLORWIDGET_H

