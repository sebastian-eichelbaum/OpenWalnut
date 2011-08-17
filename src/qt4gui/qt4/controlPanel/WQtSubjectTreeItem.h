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

#ifndef WQTSUBJECTTREEITEM_H
#define WQTSUBJECTTREEITEM_H

#include <string>

#include <QtGui/QTreeWidgetItem>

#include "WQtDatasetTreeItem.h"

/**
 * tree widget item to represent a subject in the dataset brwoser tree widget
 */
class WQtSubjectTreeItem : public QTreeWidgetItem
{
public:
    /**
     * default constructor
     * \param parent the parent widget of this item, i.e. the widget that manages this item
     */
    explicit WQtSubjectTreeItem( QTreeWidget * parent );

    /**
     * destructor
     */
    virtual ~WQtSubjectTreeItem();

    /**
     * add a dataset item below this subject in the tree
     * \param module the module that has the dataset as output
     *
     * \return the tree item representing the module
     */
    WQtDatasetTreeItem* addDatasetItem( boost::shared_ptr< WModule > module );

protected:
private:
};

#endif  // WQTSUBJECTTREEITEM_H
