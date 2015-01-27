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

#include <QtCore/QList>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QListWidgetItem>
#include <QApplication>
#include <QWidgetAction>

#include "../guiElements/WScaleLabel.h"
#include "../WQtGui.h"
#include "../WMainWindow.h"

#include "WQtRoiHeaderTreeItem.h"
#include "WQtRoiTreeItem.h"
#include "WTreeItemTypes.h"

WQtRoiHeaderTreeItem::WQtRoiHeaderTreeItem( QTreeWidget * parent ) :
    QTreeWidgetItem( parent, ROIHEADER )
{
    setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable );


    // create nice widget
    m_itemWidget = new QWidget( );
    QHBoxLayout* containerLayout = new QHBoxLayout();
    m_itemWidget->setLayout( containerLayout );

    // create a slider for the  for the texture
    QWidget* labelContainer = new QWidget( m_itemWidget );
    QHBoxLayout* labelContainerLayout = new QHBoxLayout();
    labelContainer->setLayout( labelContainerLayout );

    WScaleLabel* l = new WScaleLabel( QString::fromStdString( "ROIs" ), 5, labelContainer );
    l->setTextInteractionFlags( Qt::NoTextInteraction );
    l->setToolTip( "Define a selection of fibers using ROIs which are interpreted as logical formula." );
    labelContainerLayout->addWidget( l );

    // style
    QPalette palette;
    QColor defaultCol = palette.window().color();

    // label color
    QColor labelCol = defaultCol.darker( 120 );

    l->setStyleSheet( "background-color:" + labelCol.name() + ";" );
    labelContainer->setStyleSheet( "background-color:" + labelCol.name() + ";" );

    // fill layout
    containerLayout->addWidget( labelContainer );
    labelContainerLayout->setContentsMargins( 5, 2, 0, 2 );
    labelContainerLayout->setSpacing( 0 );

    // compact layout
    containerLayout->setContentsMargins( 0, 2, 0, 2 );
    containerLayout->setSpacing( 0 );

    // prefer stretching the label
    containerLayout->setStretchFactor( labelContainer, 100 );

    // widget size constraints and policies
    m_itemWidget->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) );
}

WQtRoiHeaderTreeItem::~WQtRoiHeaderTreeItem()
{
}

WQtBranchTreeItem* WQtRoiHeaderTreeItem::addBranch( boost::shared_ptr< WRMBranch> branch )
{
    WQtBranchTreeItem* rti = new WQtBranchTreeItem( this, branch );
    return rti;
}

QWidget* WQtRoiHeaderTreeItem::getWidget() const
{
    return m_itemWidget;
}

