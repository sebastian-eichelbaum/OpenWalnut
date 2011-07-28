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

#ifndef WQTMODULEHEADERTREEITEM_H
#define WQTMODULEHEADERTREEITEM_H

#include <string>

#include <QtGui/QTreeWidgetItem>

#include "WQtModuleTreeItem.h"

/**
 * TODO(schurade): Document this!
 */
class WQtModuleHeaderTreeItem : public QTreeWidgetItem
{
public:
    /**
     * TODO(schurade): Document this!
     * \param parent The widget managing this widget.
     */
    explicit WQtModuleHeaderTreeItem( QTreeWidget * parent );

    /**
     * TODO(schurade): Document this!
     */
    virtual ~WQtModuleHeaderTreeItem();

    /**
     * Add an module to the tree view.
     * \param module The new module.
     */
    WQtModuleTreeItem* addModuleItem( boost::shared_ptr< WModule > module );

protected:
private:
};

#endif  // WQTMODULEHEADERTREEITEM_H
