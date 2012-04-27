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

#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>

#include "../../../core/common/WLogger.h"

#include "WQtNetworkEditorView.h"
#include "WQtNetworkEditorView.moc"

WQtNetworkEditorView::WQtNetworkEditorView( QWidget* parent ):
    QGraphicsView( parent )
{
    setDragMode( QGraphicsView::NoDrag );
    setRenderHint( QPainter::Antialiasing );
    setMinimumSize( 20, 20 );

    setCenter( QPointF( 0.0, 0.0 ) );
    setResizeAnchor( QGraphicsView::AnchorUnderMouse );
}

void WQtNetworkEditorView::setCenter( const QPointF& centerPoint )
{
    m_currentCenterPoint = centerPoint;
    centerOn( m_currentCenterPoint );
}

QPointF WQtNetworkEditorView::getCenter()
{
    return m_currentCenterPoint;
}

void WQtNetworkEditorView::leaveEvent( QEvent* event )
{
    // are we in pan mode?
    if( !m_lastPanPoint.isNull() )
    {
        // if the widget is left by the mouse during pan: deactivate pan
        m_lastPanPoint = QPoint();
    }

    // forward
    QGraphicsView::leaveEvent( event );
}

void WQtNetworkEditorView::mousePressEvent( QMouseEvent* event )
{
    // only pan if no element is hit
    if( items( event->pos() ).size() != 0 )
    {
        QGraphicsView::mousePressEvent( event );
        return;
    }

    // for panning the view
    m_lastPanPoint = event->pos();
    setCursor( Qt::ClosedHandCursor );
    QGraphicsView::mousePressEvent( event );
}

void WQtNetworkEditorView::mouseReleaseEvent( QMouseEvent* event )
{
    setCursor( Qt::ArrowCursor );
    m_lastPanPoint = QPoint();
    QGraphicsView::mouseReleaseEvent( event );
}

void WQtNetworkEditorView::mouseMoveEvent( QMouseEvent* event )
{
    // are we in pan mode?
    if( !m_lastPanPoint.isNull() )
    {
        // get how much we panned
        QPointF delta = mapToScene( m_lastPanPoint ) - mapToScene( event->pos() );
        m_lastPanPoint = event->pos();

        // update the center ie. do the pan
        setCenter( getCenter() + delta );

        // during pan, avoid anyone else to handle this event
        event->accept();
        return;
    }
    QGraphicsView::mouseMoveEvent( event );
}

void WQtNetworkEditorView::wheelEvent( QWheelEvent* event )
{
    // get the position of the mouse before scaling, in scene coords
    QPointF pointBeforeScale( mapToScene( event->pos() ) );

    // get the original screen centerpoint
    QPointF screenCenter = getCenter();

    // scale the view ie. do the zoom
    double scaleFactor = 1.15;
    if( event->delta() > 0 )
    {
        // zoom in
        scale( scaleFactor, scaleFactor );
    }
    else
    {
        // zooming out
        scale( 1.0 / scaleFactor, 1.0 / scaleFactor );
    }

    // get the position after scaling, in scene coords
    QPointF pointAfterScale( mapToScene( event->pos() ) );

    // get the offset of how the screen moved
    QPointF offset = pointBeforeScale - pointAfterScale;

    // adjust to the new center for correct zooming
    QPointF newCenter = screenCenter + offset;
    setCenter( newCenter );

    // we do not forward this event to avoid the scrollbox to scroll around while zooming
    event->accept();
    // QGraphicsView::wheelEvent( event );
}

void WQtNetworkEditorView::resizeEvent( QResizeEvent* event )
{
    // get the rectangle of the visible area in scene coords
    QRectF visibleArea = mapToScene( rect() ).boundingRect();
    setCenter( visibleArea.center() );

    // call the subclass resize so the scrollbars are updated correctly
    QGraphicsView::resizeEvent( event );
}

void WQtNetworkEditorView::keyPressEvent( QKeyEvent *event )
{
    // scale the view ie. do the zoom
    double scaleFactor = 1.15;
    if( event->matches( QKeySequence::ZoomIn ) )
    {
        // zoom in
        scale( scaleFactor, scaleFactor );
    }
    if( event->matches( QKeySequence::ZoomOut ) )
    {
        // zooming out
        scale( 1.0 / scaleFactor, 1.0 / scaleFactor );
    }
    QGraphicsView::keyPressEvent( event );
}
