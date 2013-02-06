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

#ifndef WPROPERTYWIDGET_H
#define WPROPERTYWIDGET_H

#include <string>

#include <boost/shared_ptr.hpp>

#include <QtGui/QWidget>
#include <QtGui/QStackedWidget>
#include <QtGui/QLabel>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>

#include "core/common/WPropertyBase.h"
#include "core/common/WPropertyTypes.h"

#include "../guiElements/WScaleLabel.h"

/**
 * Class building the base for all widgets representing properties. It simply contains the handled property object.
 */
class WPropertyWidget: public QStackedWidget
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
    WPropertyWidget( boost::shared_ptr< WPropertyBase > property, QGridLayout* propertyGrid, QWidget* parent = 0 );

    /**
     * Destructor.
     */
    virtual ~WPropertyWidget();

    /**
     * Returns the handled property.
     *
     * \return the property
     */
    boost::shared_ptr< WPropertyBase > getProperty();

    /**
     * This method marks this widget as invalid. This is useful to let the user know about wrong inputs.
     *
     * \param invalid true whenever the property widget should be marked as invalid
     */
    virtual void invalidate( bool invalid = true );

    /**
     * Gets the tooltip that should be used for this widget.
     *
     * \return the tooltip.
     */
    virtual std::string getTooltip() const;

    /**
     * Returns the parameter widget for this property.
     *
     * \return the widget
     */
    QWidget* getParameterWidgets();

    /**
     * Returns the info widget for this property.
     *
     * \return the widget.
     */
    QWidget* getInformationWidgets();

    /**
     * Constructs a proper widget for the specified property. If the widget type is not supported, NULL is returned.
     *
     * \param property the property to create the widget for
     * \param propertyGrid grid to provide to he widget
     * \param parent parent of the widget
     *
     * \return widget
     */
    static WPropertyWidget* construct( WPropertyBase::SPtr property, QGridLayout* propertyGrid = NULL, QWidget* parent = NULL );

    /**
     * Request an update of the property widget. Can be called from any thread.
     */
    virtual void requestUpdate();

protected:
    /**
     * Called whenever the widget should update itself.
     */
    virtual void update() = 0;

    /**
     * The property handled by the widget.
     */
    boost::shared_ptr< WPropertyBase > m_property;

    /**
     * The grid used to layout label and widget.
     */
    QGridLayout* m_propertyGrid;

    /**
     * The label used to name the property
     */
    WScaleLabel m_label;

    /**
     * Separator after each property
     */
    QFrame m_separator;

    /**
     * If set to true, the widgets uses the control layout to combine the widget with a label
     */
    bool m_useLabel;

    /**
     * The widget containing a layout and provides the edit widgets for the property.
     */
    QWidget m_parameterWidgets;

    /**
     * The widget containing a layout and provides the widgets for showing information properties.
     */
    QWidget m_informationWidgets;

    /**
     * Flag denoting whether the widget is set to an invalid value.
     */
    bool m_invalid;

    /**
     * Custom event dispatcher. Gets called by QT's Event system every time an event got sent to this widget. This event handler
     * processes property change events.
     *
     * \note QT Doc says: use event() for custom events.
     * \param event the event that got transmitted.
     *
     * \return true if the event got handled properly.
     */
    virtual bool event( QEvent* event );

    /**
     * The connection for propertyChangeNotifier().
     */
    boost::signals2::connection m_connection;

    /**
     * The color to use for separators.
     */
    QColor m_sepCol;

    /**
     * The color to use for the property labels
     */
    QColor m_labelCol;

    /**
     * The color to use for property widgets
     */
    QColor m_propertyCol;
private:
};

#endif  // WPROPERTYWIDGET_H

