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

#include "WQtModuleTreeItem.h"

WQtModuleTreeItem::WQtModuleTreeItem( QTreeWidgetItem * parent, boost::shared_ptr< WModule > module ) :
    QTreeWidgetItem( parent, 3 )
{
    m_module = module;

    if ( module->getProperties()->getValue< bool > ( "active" ) )
    {
        this->setCheckState( 0, Qt::Checked );
    }
    else
    {
        this->setCheckState( 0, Qt::Unchecked );
    }
this->setFlags( Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled );
}

WQtModuleTreeItem::~WQtModuleTreeItem()
{
}

boost::shared_ptr< WModule > WQtModuleTreeItem::getModule()
{
    return m_module;
}
