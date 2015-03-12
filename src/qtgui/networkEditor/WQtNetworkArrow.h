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

#ifndef WQTNETWORKARROW_H
#define WQTNETWORKARROW_H

#include <string>
#include <iostream>

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsLineItem>
#include <QGraphicsPathItem>
#include <QGraphicsScene>
#include <QPainter>
#include <boost/shared_ptr.hpp>

#include "core/kernel/WModuleConnector.h"

// forward declaration
class WQtNetworkOutputPort;
class WQtNetworkInputPort;

/**
 * This Class is needed for connecting two ports and drawing a line between
 * them.
 */
class WQtNetworkArrow : public QGraphicsPathItem
{
public:
    /**
     * This customize the return value of type()
     */
    enum
    {
       Type = UserType + 4
    };

    /**
     * Constructor
     *
     * \param outPort the outputport that provide the data
     * \param inPort the inputport that recive the data
     */
    explicit WQtNetworkArrow( WQtNetworkOutputPort *outPort, WQtNetworkInputPort *inPort );

    /**
     * Destructor
     */
    ~WQtNetworkArrow();

    /**
     * Reimplementation from QGraphicsItem
     * \return the type of the item as int
     */
    int type() const;

    /**
     * Calculated the new position of the lines endpoints in the scene.
     * Is called everytime the parentItem is changed or after construction.
     *
     * \param deviate pull the line into this direction.
     */
    void updatePosition( QPointF deviate = QPointF() );

    /**
     * Calculated the new position of the lines endpoints in the scene.
     * Is called everytime the parentItem is changed or after construction.
     *
     * \param deviate pull the line into this direction.
     * \param targetPoint the point where to point to.
     */
    void updatePosition( QPointF targetPoint, QPointF deviate );

    /**
     * Returns the WQtNetworkOutputPort where the arrow starts.
     *\return the WQtNetworkInputPort where the connection starts
     */
    WQtNetworkOutputPort* getStartPort();

    /**
     * Returns the WQtNetworkInputPort where the arrow ends.
     *\return the WQtNetworkInputPort where the connection ends
     */
    WQtNetworkInputPort* getEndPort();

    /**
     * Search the next, compatible input port.
     *
     * \param pos the current position for which to search
     * \param maxDistance the maximum distance
     *
     * \return the connector, or NULL if none.
     */
    WQtNetworkInputPort* findNearestCompatibleInput( QPointF pos, float maxDistance = 100 );

    /**
     * Reimplementation form QGraphicsItem, because the arrowhead is added
     * to the line. Its needed that QGraphicsView knows which area needs to
     * be redrawn.
     * \return a QRectF that bounds the drawn object
     */
    QRectF boundingRect() const;

    /**
     * Reimplementation from QGraphicsItem.
     * Return the shape of this item as a QPainterPath in local coordinates
     * \return shape as QPainterPath of this object
     */
    QPainterPath shape() const;

    /**
     * If the item is changed we want to get notified.
     *
     * \param change
     * \param value
     * \return
     */
    QVariant itemChange( GraphicsItemChange change, const QVariant &value );

    /**
     * Start Drag.
     *
     * \param pos the position in scene
     */
    void startDrag( const QPointF& pos );

    /**
     * Update drag position
     *
     * \param pos the position in scene
     */
    void moveDrag( const QPointF& pos );

    /**
     * Called when releasing the mouse.
     *
     * \param pos the position in scene
     */
    void doneDrag( const QPointF& pos );

protected:
    /**
     * Draw some customized stuff in the scene.
     *
     * \param painter
     * \param option
     * \param w
     */
    void paint( QPainter* painter, const QStyleOptionGraphicsItem *option, QWidget *w );

     /**
     * If the cursor enters the arrow, the arrow becomes geen.
     *
     * \param event the hover event
     */
    void hoverEnterEvent( QGraphicsSceneHoverEvent *event );

    /**
     * If the cursor leaves the arrow, the arrow gets his default color.
     *
     * \param event the hover event
     */
    void hoverLeaveEvent( QGraphicsSceneHoverEvent *event );

    /**
     * Double click on port
     *
     * \param mouseEvent the event
     */
    void mouseDoubleClickEvent( QGraphicsSceneMouseEvent* mouseEvent );

    /**
     * Start drawing an arrow temporary.
     *
     * \param mouseEvent the mouse event
     */
    void mousePressEvent( QGraphicsSceneMouseEvent *mouseEvent );

    /**
     * Updates the temporary arrows endpoint.
     * Arrow is colored green when connection possible, red if no connection
     * is possible, or black when cursor doesent covers an WQtNetworkPort.
     *
     * \param mouseEvent the mouse event
     */
    void mouseMoveEvent( QGraphicsSceneMouseEvent *mouseEvent );

    /**
     * Send a connect request to kernel when start- and endport are
     * connectable
     *
     * \param mouseEvent the mouse event
     */
    void mouseReleaseEvent( QGraphicsSceneMouseEvent *mouseEvent );
private:
    /**
     * This method changes the color of the arrow.
     *
     * \param color the choosen color
     */
    void changeColor( QColor color );

    /**
     * Change color and width of the arrow
     *
     * \param color the color
     * \param penWidth the new width.
     */
    void changeColor( QColor color, float penWidth );

    WQtNetworkOutputPort *m_startPort; //!< the start port

    WQtNetworkInputPort *m_endPort; //!< the end port

    QColor m_color; //!< the current color

    QPolygonF m_arrowHead; //!< the arrowhead

    QLineF m_line; //!< the line representing the arrow

    QPointF m_clickPoint; //!< position where the click event was created.

    bool m_snappedOff; //!< gets true once the arrow was pulled far away from original click position.

    bool m_connectionDisconnect; //!< disconnect if true.

    WQtNetworkInputPort* m_connectTo; //!< connect to this port after mouse release.
};
#endif  // WQTNETWORKARROW_H
