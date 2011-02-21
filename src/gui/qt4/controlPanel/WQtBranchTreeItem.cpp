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

#include "WTreeItemTypes.h"
#include "WQtRoiTreeItem.h"
#include "WQtBranchTreeItem.h"

WQtBranchTreeItem::WQtBranchTreeItem( QTreeWidgetItem * parent, boost::shared_ptr< WRMBranch > branch ) :
    QTreeWidgetItem( parent, ROIBRANCH ),
    m_branch( branch )
{
    setFlags( Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDropEnabled );
}

WQtBranchTreeItem::~WQtBranchTreeItem()
{
}

WQtRoiTreeItem* WQtBranchTreeItem::addRoiItem( osg::ref_ptr< WROI > roi )
{
    WQtRoiTreeItem* rti = new WQtRoiTreeItem( this, roi, ROI );

    rti->setText( 0, QString( roi->getName().c_str() ) );
    return rti;
}

boost::shared_ptr< WRMBranch > WQtBranchTreeItem::getBranch()
{
    return m_branch;
}
