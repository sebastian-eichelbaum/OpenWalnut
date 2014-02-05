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

#ifndef WQTNETWORKITEMACTIVATOR_H
#define WQTNETWORKITEMACTIVATOR_H

#include <boost/signals2.hpp>

#include <QtGui/QGraphicsPolygonItem>
#include <QtGui/QGraphicsLineItem>

#include "WQtNetworkArrow.h"
#include "WQtNetworkItemActivator.h"

/**
 * Small graphics item that respresents whether a module in the network editor
 * is active or not. Clicking on it can toggle the state.
 */
class WQtNetworkItemActivator : public QGraphicsPolygonItem
{
public:
    /**
     * Constructor for indicator showing if module is active.
     *
     * \param module The module whose state is represented by this indicator.
     */
    explicit WQtNetworkItemActivator( boost::shared_ptr< WModule > module );

    /**
     * Destructor
     */
    ~WQtNetworkItemActivator();

    /**
     * This customize the return value of type()
     */
    enum
    {
        Type = UserType + 13
    };

    /**
     * Reimplementation from QGraphicsItem
     * \return the type of the item as int
     */
    virtual int type() const;

    /**
     * Actually paints the port.
     *
     * \param painter the painter
     * \param option style options
     * \param widget parent widget.
     */
    virtual void paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget );
protected:
    /**
     * Start drawing an arrow temporary.
     *
     * \param mouseEvent the mouse event
     */
    void mousePressEvent( QGraphicsSceneMouseEvent *mouseEvent );

    boost::shared_ptr< WModule > m_module; //!< the module

    QColor m_activeColor; //!< color used for active indicator
    QColor m_inactiveColor; //!< color used for inactive indicator

    /**
     * The connection used for notification.
     */
    boost::signals2::connection m_notifierConnection;

    /**
     * Callback getting called by the module's active property to know about state changes
     */
    void activeChangeNotifier();

    /**
     * If true, the module state changed. Update the item.
     */
    bool m_needStateUpdate;

    /**
     * Setups tooltip and brush acccording to state.
     */
    void handleActiveState();
};
#endif  // WQTNETWORKITEMACTIVATOR_H
