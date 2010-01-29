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

#include <string>

#include "WQtRoiTreeItem.h"

WQtRoiTreeItem::WQtRoiTreeItem( QTreeWidgetItem * parent, boost::shared_ptr< WRMROIRepresentation > roi, int type ) :
    QTreeWidgetItem( parent, type ),
    m_roi( roi )
{
}

WQtRoiTreeItem::~WQtRoiTreeItem()
{
}

boost::shared_ptr< WRMROIRepresentation > WQtRoiTreeItem::getRoi()
{
    return m_roi;
}

WQtRoiTreeItem* WQtRoiTreeItem::addRoiItem( boost::shared_ptr< WRMROIRepresentation > roi )
{
    WQtRoiTreeItem* rti = new WQtRoiTreeItem( this, roi, 6 );

    std::string name = "ROI";
    rti->setText( 0, QString( name.c_str() ) );
    return rti;
}
