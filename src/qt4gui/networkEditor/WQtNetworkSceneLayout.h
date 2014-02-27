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

#ifndef WQTNETWORKSCENELAYOUT_H
#define WQTNETWORKSCENELAYOUT_H

#include <boost/shared_ptr.hpp>

class WQtNetworkEditor;
class WQtNetworkScene;
class WQtNetworkItem;
class WQtNetworkItemGrid;

/**
 * Layout manager class for a QGraphicsScene. As the QGraphicsScene itself does not provide virtual methods for item addition and removal, we
 * need to write a separate layout class.
 */
class WQtNetworkSceneLayout
{
public:
    /**
     * Convenience typedef for a boost::shared_ptr< WQtNetworkSceneLayout >.
     */
    typedef boost::shared_ptr< WQtNetworkSceneLayout > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WQtNetworkSceneLayout >.
     */
    typedef boost::shared_ptr< const WQtNetworkSceneLayout > ConstSPtr;

    /**
     * Default constructor.
     *
     * \param scene the scene to manage
     * \param grid the grid to work in
     */
    WQtNetworkSceneLayout( WQtNetworkScene* scene, WQtNetworkItemGrid* grid );

    /**
     * Destructor.
     */
    virtual ~WQtNetworkSceneLayout();

    /**
     * Add an item to the layout
     *
     * \param item the item to add.
     */
    virtual void addItem( WQtNetworkItem* item );

    /**
     * Remove the item from the layout
     *
     * \param item the item to remove
     */
    virtual void removeItem( WQtNetworkItem* item );

    /**
     * Two module got connected.
     *
     * \param outItem the output module
     * \param inItem the input module
     */
    virtual void connection( WQtNetworkItem* outItem, WQtNetworkItem* inItem );

    /**
     * Two modules got disconnected.
     *
     * \param outItem the output module
     * \param inItem the input module
     */
    void disconnection( WQtNetworkItem* outItem, WQtNetworkItem* inItem );

    /**
     * Snap the item to the nearest layout element. The coordinates specified are in world space. The snapping is temporary. You can undo it or
     * accept it later to actually modify layout.
     *
     * \param item the item
     * \param worldCoords world space coordinates
     */
    void snapTemporarily( WQtNetworkItem* item, QPointF worldCoords );

    /**
     * Accept the current item position and update layout accordingly.
     *
     * \param item the item
     */
    void snapAccept( WQtNetworkItem* item );

    /**
     * Allows blending in the underlaying layout structure.
     */
    void blendIn();

    /**
     * Allows blending out the underlaying layout structure.
     */
    void blendOut();

    /**
     * Return the bounding box of this layout.
     *
     * \return the bb
     */
    QRectF getBoundingBox();

    /**
     * Get the grid used for the layout.
     *
     * \return the grid.
     */
    WQtNetworkItemGrid* getGrid() const;
protected:
private:
    WQtNetworkScene* m_scene; //!< the scene managed by this layouter

    WQtNetworkItemGrid* m_grid; //!< we use a grid to place the items
};

#endif  // WQTNETWORKSCENELAYOUT_H

