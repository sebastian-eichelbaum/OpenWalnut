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

#ifndef WQTNETWORKEDITORVIEW_H
#define WQTNETWORKEDITORVIEW_H

#include <QtGui/QGraphicsView>

class QGraphicsItem;

/**
 * This class extends the basic functionality of QGraphicsView to allow comfortable panning and zooming.
 */
class WQtNetworkEditorView: public QGraphicsView
{
    Q_OBJECT
public:
    /**
     * Constructs empty view.
     *
     * \param parent parent widgets containing the view
     */
    explicit WQtNetworkEditorView( QWidget* parent = NULL );

    /**
     * The Item to focus on. Use this instead of QGraphicsView::ensureVisible.
     *
     * \param item the item.
     */
    void focusOn( QGraphicsItem* item );

signals:
    /**
     * Emitted whenever the user caused a load event.
     */
    void loadAction();

    /**
     * Emitted whenever the user drops something into the widget
     *
     * \param event event containing further info on the dropped item.
     */
    void dragDrop( QDropEvent* event );

protected:
    /**
     * Sets the current centerpoint.  Also updates the scene's center point.
     * Unlike centerOn, which has no way of getting the floating point center
     * back, setCenter() stores the center point.  It also handles the special
     * sidebar case.  This function will claim the centerPoint to sceneRec ie.
     * the centerPoint must be within the sceneRec.
     *
     * \param centerPoint the center
     */
    void setCenter( const QPointF& centerPoint );

    /**
     * Returns current center point
     *
     * \return
     */
    QPointF getCenter();

    /**
     * Double clicked into the view
     *
     * \param event contains event information.
     */
    virtual void mouseDoubleClickEvent( QMouseEvent* event );

    /**
     * Mouse button pressed.
     *
     * \param event contains event information.
     */
    virtual void mousePressEvent( QMouseEvent* event );

    /**
     * Mouse button released
     *
     * \param event contains event information.
     */
    virtual void mouseReleaseEvent( QMouseEvent* event );

    /**
     * Mouse moved
     *
     * \param event contains event information.
     */
    virtual void mouseMoveEvent( QMouseEvent* event );

    /**
     * Mouse wheel used.
     *
     * \param event contains event information.
     */
    virtual void wheelEvent( QWheelEvent* event );

    /**
     * View resized. This automatically updates the virtual center for zooming after resize
     *
     * \param event contains event information.
     */
    virtual void resizeEvent( QResizeEvent* event );

    /**
     * Key pressed.
     *
     * \param event contains event information.
     */
    void keyPressEvent( QKeyEvent *event );

    /**
     * Mouse leaves the widget.
     *
     * \param event the event
     */
    void leaveEvent( QEvent* event );

    /**
     * Handles the drop event for a tree item.
     *
     * \param event the event.
     */
    virtual void dropEvent( QDropEvent *event );

private:
    /**
     * The currently set centerpoint.
     */
    QPointF m_currentCenterPoint;

    /**
     * To keep track of mouse movement, cache last known mouse event point
     */
    QPoint m_lastPanPoint;

    /**
     * If true, we are in pan mode.
     */
    bool m_panning;

    /**
     * Action list
     */
    WQtCombinerActionList m_addModuleActionList;

    /**
     * The menu containing the add actions in m_addModuleActionList.
     */
    QMenu* m_addMenu;
};

#endif  // WQTNETWORKEDITORVIEW_H

