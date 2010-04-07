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

#include "WQtDatasetTreeItem.h"

WQtDatasetTreeItem::WQtDatasetTreeItem( QTreeWidgetItem * parent, boost::shared_ptr< WModule > module )
    : WQtTreeItem( parent, DATASET, module )
{
    // replace the name by the filename
    boost::shared_ptr< WPropertyBase > p = module->getProperties()->findProperty( "filename" );
    m_name = "Dataset";
    if ( p.get() )
    {
        m_name = p->toPropString()->get();
    }

    // remove the path up to the file name
    if ( m_name != "" )
    {
        m_name = string_utils::tokenize( m_name, "/" ).back();
    }

    setText( 0, m_name.c_str() );
}

WQtDatasetTreeItem::~WQtDatasetTreeItem()
{
}

