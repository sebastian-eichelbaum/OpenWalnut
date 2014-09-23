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

#ifndef WQTNETWORKITEMGRID_H
#define WQTNETWORKITEMGRID_H

#include <map>

#include <boost/shared_ptr.hpp>

#include <QtCore/QObject>

#include <QGraphicsItem>
#include <QWidget>
#include <QtCore/QPoint>

/**
 * Compare two QPoints. Needed for std::map.
 *
 * \param lhs left operator
 * \param rhs right operator
 *
 * \return result lhs < rhs
 */
inline bool operator< ( const QPoint& lhs, const QPoint& rhs )
{
    return ( lhs.x() < rhs.x() ) || ( !( rhs.x() < lhs.x() ) && ( lhs.y() < rhs.y() ) );
}

/**
 * Implement a virtual grid for placing QGraphicsItems. It itself is a QGraphicsItem to allow drawing the grid or parts of the grid.
 *
 * \note all coordinates are originating from zero
 */
class WQtNetworkItemGrid: public QGraphicsObject
{
    Q_OBJECT
public:
    /**
     * Convenience typedef for a boost::shared_ptr< WQtNetworkItemGrid >.
     */
    typedef boost::shared_ptr< WQtNetworkItemGrid > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WQtNetworkItemGrid >.
     */
    typedef boost::shared_ptr< const WQtNetworkItemGrid > ConstSPtr;

    /**
     * Default constructor.
     */
    WQtNetworkItemGrid();

    /**
     * Destructor.
     */
    virtual ~WQtNetworkItemGrid();

    /**
     * The bounding area of the item.
     *
     * \return the bounding rect
     */
    virtual QRectF boundingRect() const;

    /**
     * Returns the boundaries in grid coordinates. If you need world coords, use \ref boundingRect instead.
     *
     * \return the grid bounds
     */
    virtual QRect getGridBoundingRect() const;

    /**
     * Update the m_dimensions. This method rebuilds the bounding area of the grid by re-reading all items in the grid. This is useful, whenever
     * you do a lot of complex operations while suppressing all updates. Then use this method to force a complete update of the dimensions.
     */
    void updateDimensions();

    /**
     * Paint the item.
     *
     * \param painter implements drawing commands
     * \param option provides style options for the item, such as its state, exposed area and its level-of-detail hints
     * \param widget the widget on which is painted. Can be NULL.
     */
    virtual void paint( QPainter* painter, const QStyleOptionGraphicsItem* option,
                        QWidget* widget );

    /**
     * Return the index of the first empty column.
     *
     * \return the index
     */
    int getFirstFreeColumn() const;

    /**
     * Set the specified item to the specified position.
     *
     * \param item the item to set
     * \param col the column ( x coordinate )
     * \param row the row ( y coordinate )
     *
     * \return false if there already is an element
     */
    bool setItem( QGraphicsItem* item, int col, int row );

    /**
     * Set the specified item to the specified position.
     *
     * \param item the item to set
     * \param cell the column ( x coordinate )
     *
     * \return false if there already is an element
     */
    bool setItem( QGraphicsItem* item, QPoint cell );

    /**
     * Remove the item at the given position.
     *
     * \param col the column ( x coordinate )
     * \param row the row ( y coordinate )
     */
    void removeItem( int col, int row );

    /**
     * Remove the item from the list.
     *
     * \param item the item
     *
     * \return true if found and removed.
     */
    bool removeItem( QGraphicsItem* item );

    /**
     * Move item from its current position to the specified one. If the item does not exist right now, it will get moved from its current
     * position. If the target position is occupied right now, nothing happens and false will be returned.
     *
     * \param item the item
     * \param col target columns
     * \param row target row
     *
     * \return true if successful. False if the target was occupied.
     */
    bool moveItem( QGraphicsItem* item, int col, int row );

    /**
     * Move item from its current position to the specified one. If the item does not exist right now, it will get moved from its current
     * position. If the target position is occupied right now, nothing happens and false will be returned.
     *
     * \param item the item
     * \param cell target cell
     *
     * \return true if successful. False if the target was occupied.
     */
    bool moveItem( QGraphicsItem* item, QPoint cell );

    /**
     * Is there an element at the given position?
     *
     * \param col the column ( x coordinate )
     * \param row the row ( y coordinate )
     *
     * \return true if there is an item already
     */
    bool isOccupied( int col, int row );

    /**
     * Is there an element at the given position?
     *
     * \param cell the cell
     *
     * \return true if there is an item already
     */
    bool isOccupied( QPoint cell );

    /**
     * Map the grid coordinate to world coordinates.
     *
     * \param col the column ( x coordinate )
     * \param row the row ( y coordinate )
     *
     * \return the top,left corner of the grid box
     */
    virtual QPointF mapToWorld( int col, int row );

    /**
     * Map the grid coordinate to world coordinates.
     *
     * \param gridCoord the grid coordinates
     *
     * \return the world coords
     */
    virtual QPointF mapToWorld( QPoint gridCoord );

    /**
     * Build a rect which covers the cell area in world space.
     *
     * \param cell the cell to create the rect for
     *
     * \return the rect covering the cell
     */
    virtual QRectF mapCellAreaToWorld( QPoint cell );

    /**
     * Build a rect which covers the cell area in world space.
     *
     * \param row the row
     * \param col the col
     *
     * \return the rect covering the cell
     */
    virtual QRectF mapCellAreaToWorld( int row, int col );

    /**
     * Find the position of the specified item.
     *
     * \param item item to search
     *
     * \return QPoint() if not found. Else, the position in grid coordinates.
     */
    virtual QPoint whereIs( QGraphicsItem* item );

    /**
     * Check whether the item is managed in the grid.
     *
     * \param item the item
     *
     * \return true if managed
     */
    virtual bool isInGrid( QGraphicsItem* item );

    /**
     * Find the nearest cell matching the specified world coordinates.
     *
     * \param worldSpace the coordinates to find the next cell for.
     *
     * \return cell coordinates
     */
    virtual QPoint findNearestCell( QPointF worldSpace );

    /**
     * Move the item to the position physically. This does not modify the grid. It only moves the QGraphicsItem in the scene.
     *
     * \param row target row
     * \param col target col
     * \param animate if true, the item uses its build in animations
     * \param item item to move
     */
    virtual void physicalMoveTo( QGraphicsItem* item, int col, int row, bool animate = true );

    /**
     * Move the item to the position physically. This does not modify the grid. It only moves the QGraphicsItem in the scene.
     *
     * \param cell target cell
     * \param animate if true, the item uses its build in animations
     * \param item item to move
     */
    virtual void physicalMoveTo( QGraphicsItem* item, QPoint cell, bool animate );

    /**
     * Highlights the specified cell. Note that only one cell can be highlighted. If you call this method with another cell, the old highlight
     * fades out.
     *
     * \param cell the cell to highlight
     * \param color the color of the highlight
     */
    void highlightCell( QPoint cell, QColor color );

    /**
     * Turns off highlight
     */
    void highlightCell();

    /**
     * Allows for temporarily disabling bounds update signal. This is needed sometimes, where bounds are updated during another Qt event, causing the
     * update event again, causing the Qt event again, causing the update event again, segfault.
     *
     * \note Always remember to enable again.
     *
     * \param disable true to disable.
     */
    void disableBoundsUpdate( bool disable = true );

public slots:
    /**
     * Allows blending in the underlaying layout structure.
     */
    void blendIn();

    /**
     * Allows blending out the underlaying layout structure.
     */
    void blendOut();
signals:
    /**
     * emitted when the grid shrinks or grows to update scene bounds
     */
    void updatedBounds();

protected:
    /**
     * Return the element at the given position. If the position is invalid, NULL is returned.
     *
     * \param col the column ( x coordinate )
     * \param row the row ( y coordinate )
     *
     * \return the item or NULL if not valid
     */
    virtual QGraphicsItem* at( int col, int row );

    /**
     * Really sets the item to the grid position. Ignores previous items. You should manage any previous existing item at that grid position for
     * yourself.
     *
     * \param item the item
     * \param col the column ( x coordinate )
     * \param row the row ( y coordinate )
     * \param suppressUpdate if true, the call will not call updateDimensionsAdd. Useful when using this method in another, more complex operation.
     *
     * \return the previous item at this position, or NULL if there was none.
     */
    virtual QGraphicsItem* setItemImpl( QGraphicsItem* item, int col, int row, bool suppressUpdate = false );

    /**
     * Remove the item from the grid if existing.
     *
     * \param item item to remove
     * \param suppressUpdate if true, the call will not call updateDimensionsRemove. Useful when using this method in another, more complex operation.
     *
     * \return true if something was removed. False if the item is not in the grid.
     */
    virtual bool removeItemImpl( QGraphicsItem* item, bool suppressUpdate = false );

private:
    /**
     * List of lists of QGraphicsItem
     */
    typedef std::map< QPoint, QGraphicsItem* > Grid;

    /**
     * The virtual grid. Column-first. Means outer list is the list of x coordinates, containing a list of all items in this column.
     */
    Grid m_grid;

    /**
     * Maps between item and its grid coordinates
     */
    typedef std::map< QGraphicsItem*, QPoint > ItemCoordinateMap;

    /**
     * Keeps track of the item coordinates.
     */
    ItemCoordinateMap m_gridReverse;

    /**
     * The largest number of entries in a column/row
     */
    QRect m_dimensions;

    /**
     * Bounding Rect of the Grid in world coordinates.
     */
    QRectF m_bb;

    /**
     * Use to update bounding box when you modify the m_grid dimensions.
     */
    void updateBoundingRect();

    /**
     * Current cell to highlight.
     */
    QPoint m_highlightCell;

    /**
     * Color of the highlight
     */
    QColor m_highlightColor;

    /**
     * En/Dis-able highlughting
     */
    bool m_highlightCellEnabled;

    /**
     * Timer used for blend in effects of the grid.
     */
    QTimeLine* m_blendInTimer;

    /**
     * Timer used for blend in effects of the grid.
     */
    QTimeLine* m_blendOutTimer;

    /**
     * Disable updateBounds signal.
     */
    bool m_disableUpdateBounds;

private slots:
    /**
     * Called when the animation timers tick and progress in timeline. Used to blend the item in.
     *
     * \param value the value between 0 and 1
     */
    void animationBlendInTick( qreal value );

    /**
     * Called when the animation timers tick and progress in timeline. Used to blend the item out.
     *
     * \param value the value between 0 and 1
     */
    void animationBlendOutTick( qreal value );
};

#endif  // WQTNETWORKITEMGRID_H

