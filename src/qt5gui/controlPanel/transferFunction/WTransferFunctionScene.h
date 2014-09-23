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

#ifndef WTRANSFERFUNCTIONSCENE_H
#define WTRANSFERFUNCTIONSCENE_H

#include "QtGui/QGraphicsScene"

class WTransferFunctionWidget;

/**
 * The scene for our visualization of the transfer function.
 * It differs from the QT implementation because it overloads
 * the Drag&drop methods to allow users to drop colors onto
 * the graph, which adds new nodes in the color representation.
 *
 * This has to be done here according to the QT documentation
 * as we cannot drop something in a location where no object
 * exists.
 */
class WTransferFunctionScene : public QGraphicsScene
{
public:
    /** type of base class for cleaner programming */
    typedef QGraphicsScene BaseClass;

    /**
     * Default constructor.
     *
     * \param parent the parent QObject, typically the QWidget
     */
    explicit WTransferFunctionScene( QObject*parent =0 );

    /**
     * Default destructor
     */
    virtual ~WTransferFunctionScene();

protected:
    /**
     * Checks whether the drag is inside the main rectangle of the scene
     * and whether the dragged object is of type QColor or similar.
     *
     * \param event the event to handle
     */
    virtual void dragMoveEvent( QGraphicsSceneDragDropEvent* event );

    /**
     * Allows drop of QColor-alike objects and creates new color control points
     * in the scene.
     *
     * \param event the event to handle
     */
    virtual void dropEvent( QGraphicsSceneDragDropEvent* event );

private:
    /** storage of the widget as this currently manages the data */
    WTransferFunctionWidget *myparent;
};

#endif  // WTRANSFERFUNCTIONSCENE_H

