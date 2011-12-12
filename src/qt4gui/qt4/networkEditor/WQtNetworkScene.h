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

#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsItem>

/**
 * The scene containing the whole graph
 */
class WQtNetworkScene: public QGraphicsScene
{
    Q_OBJECT
public:
    /**
     * Constructor
     */
    explicit WQtNetworkScene();

    /**
     * Destructor
     */
    virtual ~WQtNetworkScene();

    /**
     * Set a fakeitem to the scene to avoid that groups of connected items
     * or single items in the scene push away each other.
     *
     * \param fake the fake item
     */
    void setFakeItem( QGraphicsItem* fake );

    /**
     * Return the fakeitem in the scene to calculate forces in a more
     * beautiful way.
     *
     * \return the fake item
     */
    QGraphicsItem* getFakeItem();

protected:
    /**
     * Emits a Signal to notify the WQtNetworkEditor to build a default WQtCombinerToolbar
     *
     * \param mouseEvent the mouse event
     */
    void mousePressEvent( QGraphicsSceneMouseEvent* mouseEvent );

private:
    QGraphicsItem *m_fakeItem; //!< the fakeitem for the forcebased layout
};
#endif  // WQTNETWORKSCENE_H

