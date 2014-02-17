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
#include <QtGui/QScrollBar>
#include <QtGui/QWheelEvent>
#include <QtGui/QGraphicsItem>

#include "core/common/WLogger.h"
#include "core/kernel/WModuleFactory.h"
#include "../WQtCombinerActionList.h"
#include "../WQt4Gui.h"
#include "../WMainWindow.h"
#include "../guiElements/WQtMenuFiltered.h"

#include "WQtNetworkEditorGlobals.h"
#include "WQtNetworkEditorView.h"
#include "WQtNetworkEditorView.moc"

WQtNetworkEditorView::WQtNetworkEditorView( QWidget* parent ):
    QGraphicsView( parent )
{
    m_panning = false;

    setDragMode( QGraphicsView::NoDrag );
    setRenderHint( QPainter::Antialiasing );
    setMinimumSize( 20, 20 );

    setResizeAnchor( QGraphicsView::AnchorUnderMouse );
    setAcceptDrops( true );

    // we need a list of all modules
    m_addModuleActionList = WQtCombinerActionList( this, WQt4Gui::getMainWindow()->getIconManager(),
                                                   WModuleFactory::getModuleFactory()->getAllPrototypes(),
                                                   0, false );
    m_addMenu = new WQtMenuFiltered( this );
    m_addMenu->addActions( m_addModuleActionList );

    m_autoPanTimer = new QTimeLine( WNETWORKITEM_VIEWPAN_DURATION );
    connect( m_autoPanTimer, SIGNAL( valueChanged( qreal ) ), this, SLOT( autoPanTick( qreal ) ) );
}

void WQtNetworkEditorView::focusOn( QGraphicsItem* item )
{
    ensureVisibleSmooth( item );
    // centerOn( item );
}

void WQtNetworkEditorView::ensureVisibleSmooth( QGraphicsItem* item , int xmargin, int ymargin )
{
    // rect of the item inside the scene -> convert from scene to viewport
    QRectF viewRect = mapFromScene( matrix().mapRect( item->sceneBoundingRect() ) ).boundingRect();

    // actually visible area
    qreal top = verticalScrollBar()->value();
    qreal left = horizontalScrollBar()->value();
    qreal width = viewport()->width();
    qreal height = viewport()->height();

    m_autoPanOrig = QPoint( left, top );
    m_autoPanTarget = QPoint();

    // our rect somehow outside the left
    if( viewRect.left() < xmargin )
    {
        m_autoPanTarget.rx() = viewRect.left() - xmargin;
    }
    // ... the right
    if( viewRect.right() + xmargin > width )
    {
        m_autoPanTarget.rx() = viewRect.right() + xmargin - width;
    }
    // ... above
    if( viewRect.top() < ymargin )
    {
        m_autoPanTarget.ry() = viewRect.top() - ymargin;
    }

    // ... below
    if( viewRect.bottom() + ymargin > height )
    {
        m_autoPanTarget.ry() = viewRect.bottom() + ymargin - height;
    }

    m_autoPanTimer->stop();
    m_autoPanTimer->start();
}

void WQtNetworkEditorView::autoPanTick( qreal value )
{
    moveTo( m_autoPanOrig + value * m_autoPanTarget );
}

void WQtNetworkEditorView::moveBy( const QPointF& delta )
{
    QScrollBar* horiz = horizontalScrollBar();
    QScrollBar* vert = verticalScrollBar();
    horiz->setValue( horiz->value() - delta.x() );
    vert->setValue( vert->value() - delta.y() );
}

void WQtNetworkEditorView::moveTo( const QPointF& target )
{
    horizontalScrollBar()->setValue( target.x() );
    verticalScrollBar()->setValue( target.y() );
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

void WQtNetworkEditorView::mouseDoubleClickEvent( QMouseEvent* event )
{
    // only apply if no item  is hit
    if( items( event->pos() ).size() != 0 )
    {
        QGraphicsView::mouseDoubleClickEvent( event );
        return;
    }

    // open the add menu when a modifier was pressed
    if( ( event->modifiers() == Qt::ShiftModifier ) ||
        ( event->modifiers() == Qt::ControlModifier )
      )
    {
        m_addMenu->popup( event->globalPos() );
    }
    else if( event->modifiers() == Qt::NoModifier )
    {
        // plain double-click -> open file
        emit loadAction();
    }
}

void WQtNetworkEditorView::mousePressEvent( QMouseEvent* event )
{
    // only pan if no element is hit
    if( items( event->pos() ).size() != 0 )
    {
        QGraphicsView::mousePressEvent( event );
        return;
    }

    // also ignore middle mouse button
    if( event->button() == Qt::MidButton )
    {
        event->accept();
        return;
    }

    if( event->button() == Qt::LeftButton )
    {
        // for panning the view
        m_lastPanPoint = event->pos();
        m_panning = true;
        setCursor( Qt::ClosedHandCursor );
    }
    QGraphicsView::mousePressEvent( event );
}

void WQtNetworkEditorView::mouseReleaseEvent( QMouseEvent* event )
{
    // middle mouse button release: open add-menu
    if( event->button() == Qt::MidButton )
    {
        m_addMenu->popup( event->globalPos() );
        return;
    }

    setCursor( Qt::ArrowCursor );
    m_lastPanPoint = QPoint();
    m_panning = false;
    QGraphicsView::mouseReleaseEvent( event );
}

void WQtNetworkEditorView::mouseMoveEvent( QMouseEvent* event )
{
    // are we in pan mode?
    if( m_panning )
    {
        // get how much we panned
        QPointF delta = mapToScene( m_lastPanPoint ) - mapToScene( event->pos() );
        m_lastPanPoint = event->pos();

        // update the center ie. do the pan
        moveBy( -delta );

        // during pan, avoid anyone else to handle this event
        event->accept();
        return;
    }
    QGraphicsView::mouseMoveEvent( event );
}

void WQtNetworkEditorView::wheelEvent( QWheelEvent* event )
{
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

    // we do not forward this event to avoid the scrollbox to scroll around while zooming
    event->accept();
}

void WQtNetworkEditorView::resizeEvent( QResizeEvent* event )
{
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

void WQtNetworkEditorView::dropEvent( QDropEvent* event )
{
    // strangely this does not communicate down to WMainWindow although all parents are set properly. Even more strange is the fact that the
    // dropEvent does not get fired inside the WQtNetworkScene.
    emit dragDrop( event );
    QGraphicsView::dropEvent( event );
}

