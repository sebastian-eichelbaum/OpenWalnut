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

#ifndef WMODULEDELETEEVENT_H
#define WMODULEDELETEEVENT_H

#include <QtCore/QEvent>

#include "../controlPanel/WQtTreeItem.h"

/**
 * Event signalling a module item should be deleted.
 */
class WModuleDeleteEvent: public QEvent
{
public:
    /**
     * Creates a new event instance denoting that the specified module got deleted in the root container.
     *
     * \param treeItem the tree item that switched its state.
     */
    explicit WModuleDeleteEvent( WQtTreeItem* treeItem );

    /**
     * Destructor.
     */
    virtual ~WModuleDeleteEvent();

    /**
     * Getter for the tree item that got outdated.
     *
     * \return the tree item
     */
    WQtTreeItem* getTreeItem();

protected:
    /**
     * The tree item sent this event.
     */
    WQtTreeItem* m_item;
private:
};

#endif  // WMODULEDELETEEVENT_H

