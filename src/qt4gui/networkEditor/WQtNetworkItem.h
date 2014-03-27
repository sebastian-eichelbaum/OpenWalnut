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

#ifndef WQTNETWORKITEM_H
#define WQTNETWORKITEM_H

#include <string>

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtCore/QTimeLine>
#include <QtGui/QGraphicsObject>
#include <QtGui/QGraphicsTextItem>
#include <QtGui/QGraphicsItemAnimation>
#include <QtGui/QPainter>
#include <QtGui/QColor>

#include "core/kernel/WModule.h"

#include "WQtNetworkInputPort.h"
#include "WQtNetworkOutputPort.h"
#include "WQtNetworkEditorGlobals.h"

class WQtNetworkEditor;
class WQtNetworkItemActivator;

/**
 * This class represents a WModule as QGraphicsItem and
 * contains a reference to its in- and outports.
 */
class WQtNetworkItem: public QGraphicsObject
{
    Q_OBJECT

public:
    /**
     * Constructs new item in the network scene.
     *
     * \param editor the editor containing this item
     * \param module the module represented by the item
     */
    WQtNetworkItem( WQtNetworkEditor* editor, boost::shared_ptr< WModule > module );

    /**
     * Destructor.
     */
    virtual ~WQtNetworkItem();

    /**
     * This customize the return value of type()
     */
    enum
    {
       Type = UserType + 20
    };

    /**
     * Reimplementation from QGraphicsItem
     * \return the type of the item as int
     */
    int type() const;

    /**
     * The bounding area of the item.
     *
     * \return the bounding rect
     */
    virtual QRectF boundingRect() const;

    /**
     * Add a port to the item.
     *
     * \param inPort the input port
     */
    void addInputPort( WQtNetworkInputPort* inPort );

    /**
     * Add a port to the item.
     *
     * \param outPort the output port
     */
    void addOutputPort( WQtNetworkOutputPort* outPort );

    /**
     * Returns the item inports.
     * \return the item inports
     */
    QList< WQtNetworkInputPort* > getInPorts();

    /**
     * Returns the item outports
     * \return the item outports
     */
    QList< WQtNetworkOutputPort* > getOutPorts();

    /**
     * This method aligns the in- and outports as well as the modulename
     * in a regular way.
     *
     * \param maximumWidth the maximal width of the item.
     * \param minimumWidth the minimal width of the item.
     */
    void fitLook( float maximumWidth = WNETWORKITEM_MAXIMUM_WIDTH, float minimumWidth = WNETWORKITEM_MINIMUM_WIDTH );

    /**
     * Set the QGraphicsTextItem ( the caption ) of the item
     *
     * \param text the caption
     */
    void setTextItem( QGraphicsTextItem* text );

    /**
     * Get the caption as QString
     *
     * \return the caption.
     */
    QString getText();

    /**
     * Get the WModule represented by this object.
     * \return the related WModule
     */
    boost::shared_ptr< WModule > getModule();

    /**
     * Here the module can be enabled when the WModule is ready.
     * \param active true if module is ready.
     */
    void activate( bool active );

    /**
     * Can be used for polling the module states. It is called by a timer.
     */
    virtual void updater();

    /**
     * Mark this module as crashed.
     */
    void setCrashed();

    /**
     * Animate removal and finally, instruct the owning network editor to remove the item.
     *
     * \return the timer for the removal animation. Use this to start the animation.
     */
    QTimeLine* die();

    /**
     * Move item to specified position smoothly, via animation.
     *
     * \param pos position in world space
     */
    void animatedMoveTo( QPointF pos );

    /**
     * Move item to specified position smoothly, via animation.
     *
     * \param x x coord in world space
     * \param y y coord in world space
     */
    void animatedMoveTo( qreal x, qreal y );

    /**
     * Move item to specified position smoothly, no animation.
     *
     * \param pos position in world space
     */
    void moveTo( QPointF pos );

    /**
     * Move item to specified position smoothly, no animation.
     *
     * \param x x coord in world space
     * \param y y coord in world space
     */
    void moveTo( qreal x, qreal y );

    /**
     * Marks the item as already layed out.
     *
     * \param layedOut the layout flag
     */
    void setLayedOut( bool layedOut = true );

    /**
     * Checks whether the item was layed out earlier.
     *
     * \return true if already positioned
     */
    bool wasLayedOut() const;

    /**
     * Mark item as manually placed. This flag is used by the layouter to avoid re-layouting these kind of items.
     *
     * \param manual true if manually placed.
     */
    void setManuallyPlaced( bool manual = true );

    /**
     * Item was placed by hand. Used as a flag to avoid re-layouting these items.
     *
     * \return true if manually placed.
     */
    bool wasManuallyPlaced() const;

public slots:
    /**
     * Called whenever the item moves around.
     */
    void positionChanged();

signals:
    /**
     * The item is now dead. Animation completed and item was removed from scene. Do not use this to delete the item.
     *
     * \param item the item.
     */
    void dead( WQtNetworkItem* item );
protected:
    /**
     * If the item is changed we want to get notified.
     *
     * \param change
     * \param value
     * \return
     */
    virtual QVariant itemChange( GraphicsItemChange change, const QVariant& value );

    /**
     * If the WQtNetworkItem is moved, then the contained ports have to update
     * the connected WQtNetworkArrows for correct alignment.
     *
     * \param mouseEvent the mouse event
     */
    virtual void mouseMoveEvent( QGraphicsSceneMouseEvent* mouseEvent );

    /**
     * Reimplemented from QGraphicsRectItem. Used to enable selection of network items
     * with right-click and left-click.
     *
     * \param event the mouse event
     **/
    virtual void mousePressEvent( QGraphicsSceneMouseEvent *event );

    /**
     * Mouse was released.
     *
     * \param event the mouse event
     */
    virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent* event );

    /**
     * Called upon double click.
     *
     * \param event the click event
     */
    virtual void mouseDoubleClickEvent( QGraphicsSceneMouseEvent* event );

    /**
     * If the cursor enters the item, the item gets a green color.
     *
     * \param event the hover event
     */
    virtual void hoverEnterEvent( QGraphicsSceneHoverEvent* event );

    /**
     * If the cursor leaves the item, the item gets his default color.
     *
     * \param event the hover event
     */
    virtual void hoverLeaveEvent( QGraphicsSceneHoverEvent* event );

    /**
     * Draw some customized stuff in the scene.
     *
     * \param painter
     * \param option
     * \param w
     */
    virtual void paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* w );

private:
    /**
     * Denotes the current state this item is in.
     */
    enum State
    {
        Disabled = 0,
        Normal,
        Crashed
    };

    /**
     * Update the visual state of the item. Draw the item according to the state specified.
     *
     * \param state the state
     */
    void changeState( State state );

    boost::shared_ptr< WModule > m_module; //!< the module

    QList< WQtNetworkInputPort* > m_inPorts; //!< the input ports of the item

    QList< WQtNetworkOutputPort* > m_outPorts; //!< the output ports of the item

    QRectF m_rect; //!< the size of the items rect

    float m_width; //!< the width of the rect

    float m_height; //!< the height of the rect

    std::string m_textFull; //!< always contains the unclipped text of m_text
    QGraphicsTextItem* m_text; //!< the caption
    std::string m_subtitleFull; //!< always contains the unclipped text of m_subtitle
    QGraphicsTextItem* m_subtitle; //!< the caption

    WQtNetworkEditor* m_networkEditor; //!< the related WQtNetworkEditor

    QColor m_itemColor; //!< the color of the item. Depends on type (source, sink, ...).

    State m_currentState; //!< denotes the state the item currently is in

    WQtNetworkItemActivator *m_hidden; //!< indicator showing if module's graphics are activated (allows to activate it)

    /**
     * If true, the item is hovered.
     */
    bool m_isHovered;

    /**
     * If true, the item is selected.
     */
    bool m_isSelected;

    /**
     * If true, a percentage is shown. If not, an animated bar walking around.
     */
    bool m_busyIsDetermined;

    /**
     * Counter used for busy indication.
     */
    float m_busyPercent;

    /**
     * If true, the busy indication is active.
     */
    bool m_busyIndicatorShow;

    /**
     * If true, the next call to update() will force a redraw. Avoid setting this to true permanently.
     */
    bool m_forceUpdate;

    /**
     * Optimal width for this module. Calculated during construction.
     */
    float m_itemBestWidth;

    /**
     * The property toolbox window if any.
     */
    QWidget* m_propertyToolWindow;

    /**
     * Animation for appearance and deletion
     */
    QGraphicsItemAnimation* m_animation;

    /**
     * Timer for the animation.
     */
    QTimeLine* m_animationTimer;

    /**
     * Animation for appearance and deletion
     */
    QGraphicsItemAnimation* m_removalAnimation;

    /**
     * Timer for the animation.
     */
    QTimeLine* m_removalAnimationTimer;

    /**
     * Keep track of the mouse dragging of this item
     */
    QPointF m_dragStartPosition;

    /**
     * Dragging?
     */
    bool m_dragging;

    /**
     * Start position of dragging
     */
    QPointF m_draggingStart;

    /**
     * Mark item as already positioned.
     */
    bool m_wasLayedOut;

    /**
     * Item has been placed manually. This flag is used by the layouter to avoid moving around manually placed items.
     */
    bool m_wasManuallyPlaced;

    /**
     * If true, the mouse events do not cause a drag and move operation.
     */
    bool m_noDrag;

private slots:
    /**
     * Called when the m_removalAnimationTimer finishes.
     */
    void removalAnimationDone();

    /**
     * Called when the animation timers tick and progress in timeline. Used to blend the item out.
     *
     * \param value the value between 0 and 1
     */
    void animationBlendOutTick( qreal value );

    /**
     * Called when the animation timers tick and progress in timeline. Used to blend the item in.
     *
     * \param value the value between 0 and 1
     */
    void animationBlendInTick( qreal value );

    /**
     * Called whenever a animated move was finished.
     */
    void moveFinished();
};
#endif  // WQTNETWORKITEM_H
