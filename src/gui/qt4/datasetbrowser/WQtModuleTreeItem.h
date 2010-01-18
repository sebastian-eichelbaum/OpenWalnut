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

#ifndef WQTMODULETREEITEM_H
#define WQTMODULETREEITEM_H

#include <QtGui/QTreeWidgetItem>
#include <QtGui/QProgressBar>
#include <QtCore/QTimer>

#include "WQtTreeItem.h"
#include "../../../kernel/WModule.h"

/**
 * TODO(schurade): Document this!
 */
class WQtModuleTreeItem: public WQtTreeItem
{
public:
    /**
     * TODO(schurade): Document this!
     * \param parent The widget managing this widget
     * \param module The represented module
     */
    WQtModuleTreeItem( QTreeWidgetItem * parent, boost::shared_ptr< WModule > module );

    /**
     * Destructor.
     */
    virtual ~WQtModuleTreeItem();

protected:

private:
};

#endif  // WQTMODULETREEITEM_H
