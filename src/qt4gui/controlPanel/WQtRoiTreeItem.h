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
 * A tree widget item to represent a ROI in the control panel
 */
class WQtRoiTreeItem : public QTreeWidgetItem
{
public:
    /**
     * Constructor
     *
     * \param parent The parent widget.
     * \param roi The ROI taht is represented by this item.
     * \param type
     */
    WQtRoiTreeItem( QTreeWidgetItem * parent, osg::ref_ptr< WROI > roi, WTreeItemType type = ROI );

    /**
     * Destructor
     */
    virtual ~WQtRoiTreeItem();

    /**
     * Getter
     * \return the ROI representation object
     */
    osg::ref_ptr< WROI > getRoi();

protected:
private:
    osg::ref_ptr< WROI > m_roi; //!< Pointer to the ROI represented by this item.
};

#endif  // WQTROITREEITEM_H
