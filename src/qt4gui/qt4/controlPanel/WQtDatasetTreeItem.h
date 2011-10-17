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

#ifndef WQTDATASETTREEITEM_H
#define WQTDATASETTREEITEM_H

#include <QtGui/QTreeWidgetItem>

#include "core/kernel/WModule.h"
#include "WQtTreeItem.h"

/**
 * Tree widget item to represent a dataset in the control panel tree widget. This class is currently empty as all the functionality has been
 * generalized and moved to WQtTreeItem. This class should be removed.
 */
class WQtDatasetTreeItem : public WQtTreeItem
{
public:

    /**
     * Constructor
     *
     * \param parent The parent widget that manages this widget.
     * \param module The module that will be represented by the item.
     */
    WQtDatasetTreeItem( QTreeWidgetItem * parent, boost::shared_ptr< WModule > module );

    /**
     * Destructor
     */
    virtual ~WQtDatasetTreeItem();

protected:

private:
};

#endif  // WQTDATASETTREEITEM_H
