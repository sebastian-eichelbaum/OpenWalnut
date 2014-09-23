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

#include <cmath>

#include <algorithm>

#include <QtGui/QPainter>

#include "core/common/WLogger.h"

#include "WQtNetworkEditorGlobals.h"
#include "WQtNetworkItem.h"

#include "WQtNetworkItemGrid.h"

WQtNetworkItemGrid::WQtNetworkItemGrid():
    QGraphicsObject(),
    m_highlightCellEnabled( false ),
    m_disableUpdateBounds( false )
{
    // initialize members
    setZValue( -1.0 );
    setOpacity( 0.0 );

    // blend timer
    m_blendInTimer = new QTimeLine( WNETWORKITEM_GRIDBLENDIN_DURATION );
    connect( m_blendInTimer, SIGNAL( valueChanged( qreal ) ), this, SLOT( animationBlendInTick( qreal ) ) );

    m_blendOutTimer = new QTimeLine( WNETWORKITEM_GRIDBLENDOUT_DURATION );
    connect( m_blendOutTimer, SIGNAL( valueChanged( qreal ) ), this, SLOT( animationBlendOutTick( qreal ) ) );
}

WQtNetworkItemGrid::~WQtNetworkItemGrid()
{
    // cleanup
    delete m_blendInTimer;
    delete m_blendOutTimer;
}

QRectF WQtNetworkItemGrid::boundingRect() const
{
    return m_bb;
}

void WQtNetworkItemGrid::updateBoundingRect()
{
    float boxWidth = WNETWORKITEM_XSPACE + WNETWORKITEM_MAXIMUM_WIDTH;
    float boxHeight = WNETWORKITEM_YSPACE + WNETWORKITEM_MAXIMUM_HEIGHT;

    QRect dimensions = m_dimensions;

    // always include hightlight cell to force QGraphicsScene to render it properly
    if( m_highlightCellEnabled )
    {
        QRect h( m_highlightCell.x(), m_highlightCell.y(), 1, 1 );
        dimensions = dimensions.united( h );
    }

    // NOTE: we always add a row and column at each side for the visual representation of the grid
    float minX = ( dimensions.x() - 1 ) * boxWidth;
    float minY = ( dimensions.y() - 1 ) * boxHeight;
    float maxX = ( dimensions.width() + 2 ) * boxWidth;
    float maxY = ( dimensions.height() + 2 ) * boxHeight;

    // done. Create rect and emit signal
    m_bb = QRectF( minX, minY, maxX, maxY );

    if( !m_disableUpdateBounds )
    {
        emit updatedBounds();
    }
}

void WQtNetworkItemGrid::paint( QPainter* painter, const QStyleOptionGraphicsItem* /* option */,
                                QWidget* /* widget */ )
{
    QRectF bb = boundingRect();

    float sx = WNETWORKITEM_XSPACE + WNETWORKITEM_MAXIMUM_WIDTH;
    float sy = WNETWORKITEM_YSPACE + WNETWORKITEM_MAXIMUM_HEIGHT;

    float lineWidth = 1.0;

    // draw a nice grid
    for( float y = sy; y < bb.height(); y = y + sy )
    {
        QRectF r( bb.x(), bb.y() + y, bb.width(), lineWidth );
        QLinearGradient gradient( bb.topLeft(), bb.topRight() );
        gradient.setColorAt( 0.0, QColor( 0, 0, 0, 0 ) );
        gradient.setColorAt( 0.4, QColor( 0, 0, 0, 128 ) );
        gradient.setColorAt( 0.6, QColor( 0, 0, 0, 128 ) );
        gradient.setColorAt( 1.0, QColor( 0, 0, 0, 0 ) );
        painter->fillRect( r, gradient );
    }

    for( float x = sx; x < bb.width(); x = x + sx )
    {
        QRectF r( bb.x() + x, bb.y(), lineWidth, bb.height() );
        QLinearGradient gradient( bb.topLeft(), bb.bottomLeft() );
        gradient.setColorAt( 0.0, QColor( 0, 0, 0, 0 ) );
        gradient.setColorAt( 0.4, QColor( 0, 0, 0, 128 ) );
        gradient.setColorAt( 0.6, QColor( 0, 0, 0, 128 ) );
        gradient.setColorAt( 1.0, QColor( 0, 0, 0, 0 ) );
        painter->fillRect( r, gradient );
    }

    if( m_highlightCellEnabled )
    {
        QRectF cellRect = mapCellAreaToWorld( m_highlightCell );

        // draw highlight
        // QRadialGradient radialGrad( cellRect.center(), cellRect.height() );
        // radialGrad.setColorAt( 0, m_highlightColor );
        // radialGrad.setColorAt( 0.2, QColor( 0,0,0,0 ) );
        // radialGrad.setColorAt( 0.8, QColor( 0,0,0,0 ) );
        // radialGrad.setColorAt( 1, m_highlightColor );
        painter->fillRect( cellRect, m_highlightColor );
    }
}

void WQtNetworkItemGrid::physicalMoveTo( QGraphicsItem* item, int col, int row, bool animate )
{
    QPointF p = mapToWorld( col, row );

    p.rx() += WNETWORKITEM_XSPACE * 0.5;
    p.ry() += WNETWORKITEM_YSPACE * 0.5;

    WQtNetworkItem* ni = dynamic_cast< WQtNetworkItem* >( item );
    if( animate && ni )
    {
        ni->animatedMoveTo( p );
    }
    else if( ni )
    {
        ni->moveTo( p );
    }
    else
    {
        item->setPos( p );
    }
}

void WQtNetworkItemGrid::physicalMoveTo( QGraphicsItem* item, QPoint cell, bool animate )
{
    physicalMoveTo( item, cell.x(), cell.y(), animate );
}

bool WQtNetworkItemGrid::setItem( QGraphicsItem* item, int col, int row )
{
    // already owned?
    if( isOccupied( col, row ) )
    {
        return false;
    }

    // insert element into the corresponding position
    setItemImpl( item, col, row );

    // move element physically
    physicalMoveTo( item, col, row, false );

    return true;
}

bool WQtNetworkItemGrid::setItem( QGraphicsItem* item, QPoint cell )
{
    return setItem( item, cell.x(), cell.y() );
}

void WQtNetworkItemGrid::removeItem( int col, int row )
{
    removeItemImpl( m_grid[ QPoint( col, row ) ] );
}

bool WQtNetworkItemGrid::removeItem( QGraphicsItem* item )
{
    return removeItemImpl( item );
}

bool WQtNetworkItemGrid::moveItem( QGraphicsItem* item, int col, int row )
{
    // already in use?
    if( isOccupied( col, row ) )
    {
        return false;
    }

    // clear old entry?
    removeItemImpl( item, true );

    // set new
    setItemImpl( item, col, row, true );

    // move element physically
    physicalMoveTo( item, col, row, true );

    // grid might be larger/smaller now
    updateDimensions();

    return true;
}

bool WQtNetworkItemGrid::moveItem( QGraphicsItem* item, QPoint cell )
{
    return moveItem( item, cell.x(), cell.y() );
}

bool WQtNetworkItemGrid::isOccupied( int col, int row )
{
    return at( col, row );
}

bool WQtNetworkItemGrid::isOccupied( QPoint cell )
{
    return isOccupied( cell.x(), cell.y() );
}

QGraphicsItem* WQtNetworkItemGrid::at( int col, int row )
{
    Grid::iterator it = m_grid.find( QPoint( col, row ) );
    if( it == m_grid.end() )
    {
        return NULL;
    }
    return ( *it ).second;
}

QGraphicsItem* WQtNetworkItemGrid::setItemImpl( QGraphicsItem* item, int col, int row, bool suppressUpdate )
{
    // keep the item that might be there
    QGraphicsItem* prev = m_grid[ QPoint( col, row ) ];

    // set the new item to the grid
    m_grid[ QPoint( col, row ) ] = item;
    // and store the coordinates for the item
    m_gridReverse[ item ] = QPoint( col, row );

    // grid might be larger now
    if( !suppressUpdate )
    {
        updateDimensions();
    }

    return prev;
}

bool WQtNetworkItemGrid::removeItemImpl( QGraphicsItem* item, bool suppressUpdate )
{
    // item exists?
    ItemCoordinateMap::iterator it = m_gridReverse.find( item );
    if( it == m_gridReverse.end() )
    {
        return false;
    }

    // ok found. Where?
    QPoint where = ( *it ).second;
    // remove from old position
    m_grid.erase( where );

    // remove from reverse list
    m_gridReverse.erase( it );

    // grid might be smaller now
    if( !suppressUpdate )
    {
        updateDimensions();
    }

    return true;
}

int WQtNetworkItemGrid::getFirstFreeColumn() const
{
    // we assume that the remove and move functions clean up empty lists
    return m_dimensions.x() + m_dimensions.width();
}

QPointF WQtNetworkItemGrid::mapToWorld( int col, int row )
{
    return QPointF( col * ( WNETWORKITEM_XSPACE + WNETWORKITEM_MAXIMUM_WIDTH ),
                    row * ( WNETWORKITEM_YSPACE + WNETWORKITEM_MAXIMUM_HEIGHT ) );
}

QPointF WQtNetworkItemGrid::mapToWorld( QPoint gridCoord )
{
    return mapToWorld( gridCoord.x(), gridCoord.y() );
}

QRectF WQtNetworkItemGrid::mapCellAreaToWorld( int col, int row )
{
    QPointF c = mapToWorld( col, row );
    return QRectF( c.x(), c.y(), WNETWORKITEM_XSPACE + WNETWORKITEM_MAXIMUM_WIDTH, WNETWORKITEM_YSPACE + WNETWORKITEM_MAXIMUM_HEIGHT );
}

QRectF WQtNetworkItemGrid::mapCellAreaToWorld( QPoint cell )
{
    return mapCellAreaToWorld( cell.x(), cell.y() );
}

QPoint WQtNetworkItemGrid::whereIs( QGraphicsItem* item )
{
    ItemCoordinateMap::iterator it = m_gridReverse.find( item );
    if( it == m_gridReverse.end() )
    {
        return QPoint();
    }
    return ( *it ).second;
}

bool WQtNetworkItemGrid::isInGrid( QGraphicsItem* item )
{
    ItemCoordinateMap::iterator it = m_gridReverse.find( item );
    return ( it != m_gridReverse.end() );
}

void WQtNetworkItemGrid::updateDimensions()
{
    // NOTE: this is very important to allow QGraphicsScene updating its index before geometry change
    prepareGeometryChange();

    QRect oldDim = m_dimensions;
    QPoint min;
    QPoint max;

    bool first = true;
    for( Grid::const_iterator it = m_grid.begin(); it != m_grid.end(); ++it )
    {
        // empty grid cell?
        if( !( *it ).second )
        {
            continue;
        }

        QPoint p = ( *it ).first;

        // extend bounds by this point
        if( first )
        {
            first = false;
            min = p;
            max = p;
        }
        else
        {
            min.rx() = std::min( p.x(), min.x() );
            min.ry() = std::min( p.y(), min.y() );
            max.rx() = std::max( p.x(), max.x() );
            max.ry() = std::max( p.y(), max.y() );
        }
    }

    if( !first )    // there really was something in the grid
    {
        m_dimensions = QRect( min, max );
    }
    else
    {
        m_dimensions = QRect();
    }

    // changed?
    if( oldDim != m_dimensions )
    {
        // Debug
        // wlog::debug( "new BB") << m_dimensions.topLeft().x() << ", " << m_dimensions.topLeft().y()
        //                        << m_dimensions.bottomRight().x() << ", " << m_dimensions.bottomRight().y() << " ---- " << m_dimensions.width();
        updateBoundingRect();
    }
}

QPoint WQtNetworkItemGrid::findNearestCell( QPointF worldSpace )
{
    float boxWidth = WNETWORKITEM_XSPACE + WNETWORKITEM_MAXIMUM_WIDTH;
    float boxHeight = WNETWORKITEM_YSPACE + WNETWORKITEM_MAXIMUM_HEIGHT;

    // NOTE: (0,0) in world space overlaps with (0,0) in grid space
    return QPoint( std::floor( worldSpace.x() / boxWidth ),
                   std::floor( worldSpace.y() / boxHeight ) );
}

void WQtNetworkItemGrid::highlightCell( QPoint cell, QColor color )
{
    // NOTE: this is very important to allow QGraphicsScene updating its index before geometry change
    prepareGeometryChange();

    // also update bounding area of item or QGraphicsScene might not draw the highlight cell
    m_highlightCell = cell;
    m_highlightColor = color;
    m_highlightCellEnabled = true;
    updateBoundingRect();
    update();
}

void WQtNetworkItemGrid::highlightCell()
{
    // NOTE: this is very important to allow QGraphicsScene updating its index before geometry change
    prepareGeometryChange();
    m_highlightCellEnabled = false;
    updateBoundingRect();
    update();
}

void WQtNetworkItemGrid::blendIn()
{
    // if currently blending out, stop it and continue with blending in at this position
    if( m_blendOutTimer->state() == QTimeLine::Running )
    {
        m_blendOutTimer->stop();
        float doneRatio = static_cast< float >( m_blendOutTimer->currentTime() ) / static_cast< float >( m_blendInTimer->duration() );
        float remaining = doneRatio * static_cast< float >( m_blendInTimer->duration() );
        m_blendInTimer->setCurrentTime( static_cast< int >( remaining ) );
    }
    else
    {
        m_blendInTimer->setCurrentTime( 0 );
    }

    m_blendInTimer->start();
}

void WQtNetworkItemGrid::blendOut()
{
    // if currently blending out, stop it and continue with blending in at this position
    if( m_blendInTimer->state() == QTimeLine::Running )
    {
        m_blendInTimer->stop();
        float doneRatio = static_cast< float >( m_blendInTimer->currentTime() ) / static_cast< float >( m_blendInTimer->duration() );
        float remaining = doneRatio * static_cast< float >( m_blendOutTimer->duration() );
        m_blendOutTimer->setCurrentTime( static_cast< int >( remaining ) );
    }
    else
    {
        m_blendOutTimer->setCurrentTime( 0 );
    }

    m_blendOutTimer->resume();
}

void WQtNetworkItemGrid::animationBlendInTick( qreal value )
{
    setOpacity( value * value );
}

void WQtNetworkItemGrid::animationBlendOutTick( qreal value )
{
    setOpacity( 1.0 - ( value * value ) );
}

void WQtNetworkItemGrid::disableBoundsUpdate( bool disable )
{
    m_disableUpdateBounds = disable;
}

QRect WQtNetworkItemGrid::getGridBoundingRect() const
{
    return m_dimensions;
}

