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

#include <boost/shared_ptr.hpp>

#include <QtGui/QWidget>
#include <QtGui/QLabel>
#include <QtGui/QGridLayout>

#include "../../../common/WPropertyBase.h"
#include "../../../common/WPropertyTypes.h"

/**
 * Class building the base for all widgets representing properties. It simply contains the handled property object.
 */
class WPropertyWidget: public QWidget
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

protected:

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
    QLabel m_label;

private:
};

#endif  // WPROPERTYWIDGET_H

