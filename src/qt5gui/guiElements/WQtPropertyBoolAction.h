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

#ifndef WQTPROPERTYBOOLACTION_H
#define WQTPROPERTYBOOLACTION_H

#include <QAction>

#include "core/common/WPropertyVariable.h"

/**
 * Implements a property action for WPropBool.
 */
class WQtPropertyBoolAction: public QAction
{
    Q_OBJECT
public:
    /**
     * Constructor. Creates a new action appropriate for the specified property.
     *
     * \param property the property to handle
     * \param parent the parent widget.
     */
    WQtPropertyBoolAction( WPropBool property, QWidget* parent = 0 );

    /**
     * Destructor.
     */
    virtual ~WQtPropertyBoolAction();

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
     * Callback for WPropertyBase::getChangeCondition. It emits an event to ensure all updates are done in gui thread.
     */
    virtual void propertyChangeNotifier();

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

private:
public slots:

    /**
     * called whenever the user modifies the action
     */
    void changed();
};

#endif  // WQTPROPERTYBOOLACTION_H

