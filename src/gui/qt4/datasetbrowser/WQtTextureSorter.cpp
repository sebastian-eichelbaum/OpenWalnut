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

#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

#include <QtCore/QList>
#include <QtGui/QScrollArea>
#include <QtGui/QVBoxLayout>

#include "../../../common/WLogger.h"
#include "../../../common/WPreferences.h"

#include "../../../dataHandler/WDataSet.h"

#include "WQtTextureSorter.h"
#include "../events/WModuleAssocEvent.h"
#include "../events/WRoiAssocEvent.h"
#include "../events/WModuleReadyEvent.h"
#include "../events/WEventTypes.h"
#include "WQtNumberEdit.h"
#include "WQtNumberEditDouble.h"

#include "../../../kernel/WModuleFactory.h"
#include "../WMainWindow.h"

#include "../../../modules/data/WMData.h"


WQtTextureSorter::WQtTextureSorter( QWidget* parent )
    : QWidget( parent )
{
    m_textureListWidget = new QListWidget( this );
    m_textureListWidget->setToolTip( "List of available textures." );

    m_layout = new QVBoxLayout();

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_downButton = new QPushButton();
    m_downButton->setText( QString( "down" ) );
    m_upButton = new QPushButton();
    m_upButton->setText( QString( "up" ) );

    // TODO(all): reenable these buttons if sorting works again
    m_downButton->setDisabled( true );
    m_upButton->setDisabled( true );

    buttonLayout->addWidget( m_downButton );
    buttonLayout->addWidget( m_upButton );

    m_layout->addWidget( m_textureListWidget );
    m_layout->addLayout( buttonLayout );

    setLayout( m_layout );
}

WQtTextureSorter::~WQtTextureSorter()
{
}

void WQtTextureSorter::moveItemDown()
{
//     m_treeWidget->moveTreeItemDown();
}

void WQtTextureSorter::moveItemUp()
{
//     m_treeWidget->moveTreeItemUp();
}
