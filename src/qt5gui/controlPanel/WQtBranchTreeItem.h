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

#ifndef WQTBRANCHTREEITEM_H
#define WQTBRANCHTREEITEM_H

#include <QtCore/QTimer>
#include <QProgressBar>
#include <QTreeWidgetItem>

#include "core/graphicsEngine/WROI.h"

#include "core/kernel/WRMBranch.h"

#include "WQtTreeItem.h"

class WQtRoiTreeItem;

/**
 * This class represents a ROI branch in the tree widget
 */
class WQtBranchTreeItem  : public QTreeWidgetItem
{
public:
    /**
     * default constructor
     *
     * \param parent
     * \param branch
     */
    explicit WQtBranchTreeItem( QTreeWidgetItem * parent, boost::shared_ptr< WRMBranch > branch );

    /**
     * destructor
     */
    virtual ~WQtBranchTreeItem();

    /**
     * Add a ROI to the tree view.
     * \param roi The new ROI.
     *
     * \return Pointer to the new ROI tree item.
     */
    WQtRoiTreeItem* addRoiItem( osg::ref_ptr< WROI > );

    /**
     * getter
     * \return the branch representation object
     */
    boost::shared_ptr< WRMBranch > getBranch();

    /**
     * Create a representation widget for this item.
     *
     * \return the widget.
     */
    QWidget* getWidget() const;

    /**
     * Update internal Roi Manager sorting using the sorting of the children of this tree item.
     */
    void updateRoiManagerSorting();
protected:
private:
    boost::shared_ptr< WRMBranch > m_branch; //!< ROI

    /**
     * Widget representing the item.
     */
    QWidget* m_itemWidget;
};

#endif  // WQTBRANCHTREEITEM_H
