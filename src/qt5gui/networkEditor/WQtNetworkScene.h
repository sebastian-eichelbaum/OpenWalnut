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

#ifndef WQTNETWORKSCENE_H
#define WQTNETWORKSCENE_H

#include <QGraphicsScene>
#include <QGraphicsItem>

/**
 * The scene containing the whole graph
 */
class WQtNetworkScene: public QGraphicsScene
{
    Q_OBJECT
public:
    /**
     * Constructor.
     *
     * \param parent the parent which handles the scene
     */
    explicit WQtNetworkScene( QObject* parent = NULL );

    /**
     * Destructor
     */
    virtual ~WQtNetworkScene();

protected:
    /**
     * Event handler when pulling something inside the scene.
     *
     * \param event the event.
     */
    virtual void dragEnterEvent( QGraphicsSceneDragDropEvent* event );

    /**
     * Event handler when pulling something in the scene.
     *
     * \param event the event.
     */
    virtual void dragMoveEvent( QGraphicsSceneDragDropEvent* event );

private:
};
#endif  // WQTNETWORKSCENE_H

