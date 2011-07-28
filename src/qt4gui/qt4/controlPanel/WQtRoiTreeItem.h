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

#ifndef WQTROITREEITEM_H
#define WQTROITREEITEM_H

#include <QtCore/QTimer>
#include <QtGui/QProgressBar>
#include <QtGui/QTreeWidgetItem>

#include "core/graphicsEngine/WROI.h"

#include "WQtTreeItem.h"

/**
 * a tree widget item to represent a roi in the control panel
 */
class WQtRoiTreeItem : public QTreeWidgetItem
{
public:
    /**
     * constructor
     *
     * \param parent
     * \param roi
     * \param type
     */
    WQtRoiTreeItem( QTreeWidgetItem * parent, osg::ref_ptr< WROI > roi, WTreeItemType type = ROI );

    /**
     * destructor
     */
    virtual ~WQtRoiTreeItem();

    /**
     * getter
     * \return the roi representation object
     */
    osg::ref_ptr< WROI > getRoi();

protected:
private:
    osg::ref_ptr< WROI > m_roi; //!< roi
};

#endif  // WQTROITREEITEM_H
