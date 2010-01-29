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

#include <QtGui/QTreeWidgetItem>
#include <QtGui/QProgressBar>
#include <QtCore/QTimer>

#include "WQtTreeItem.h"
#include "../../../graphicsEngine/WROI.h"
#include "../../../modules/fiberDisplay2/WRMROIRepresentation.h"

/**
 * TODO(schurade): Document this!
 */
class WQtRoiTreeItem : public QTreeWidgetItem
{
public:
    /**
     * TODO(schurade): Document this!
     */
    WQtRoiTreeItem( QTreeWidgetItem * parent, boost::shared_ptr< WRMROIRepresentation > roi, int type = 5 );

    /**
     * TODO(schurade): Document this!
     */
    ~WQtRoiTreeItem();

    boost::shared_ptr< WRMROIRepresentation > getRoi();

    /**
     * Add a roi to the tree view.
     * \param roi The new roi.
     */
    WQtRoiTreeItem* addRoiItem( boost::shared_ptr< WRMROIRepresentation > roi );


protected:
private:
    boost::shared_ptr< WRMROIRepresentation > m_roi;
};

#endif  // WQTROITREEITEM_H
