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
#include <string>

#include <QtCore/QList>

#include "../../../common/WLogger.h"

#include "WQtDatasetBrowser.h"
#include "WQtNumberEdit.h"

WQtDatasetBrowser::WQtDatasetBrowser( QWidget* parent )
    : QDockWidget( parent )
{
    m_panel = new QWidget( this );
    m_treeWidget = new WQtTreeWidget( m_panel );

    m_treeWidget->setHeaderLabel( QString( "Dataset Browser" ) );
    m_treeWidget->setDragEnabled( true );
    m_treeWidget->viewport()->setAcceptDrops( true );
    m_treeWidget->setDropIndicatorShown( true );
    m_treeWidget->setDragDropMode( QAbstractItemView::InternalMove );

    m_tabWidget = new QTabWidget( m_panel );

    m_layout = new QVBoxLayout();

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_downButton = new QPushButton();
    m_downButton->setText( QString( "down" ) );
    m_upButton = new QPushButton();
    m_upButton->setText( QString( "up" ) );
    buttonLayout->addWidget( m_downButton );
    buttonLayout->addWidget( m_upButton );

    //========================================================================
    // TODO(Schurade): only for testing, will be removed soon
    WQtDSBWidget* tab1 = new WQtDSBWidget( "settings" );
    tab1->addPushButton( "button1" );
    tab1->addPushButton( "button2" );
    tab1->addCheckBox( "box1:", true );
    tab1->addCheckBox( "box2:", false );
    tab1->addLineEdit( "text:", "some text" );
    tab1->addSliderInt( "slider:" , 50, 0, 100 );
    addTabWidgetContent( tab1 );
    //========================================================================

    m_layout->addWidget( m_treeWidget );
    m_layout->addLayout( buttonLayout );
    m_layout->addWidget( m_tabWidget );

    m_panel->setLayout( m_layout );

    this->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
    this->setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
    this->setWidget( m_panel );

    connectSlots();
}

WQtDatasetBrowser::~WQtDatasetBrowser()
{
}


void WQtDatasetBrowser::connectSlots()
{
    connect( m_treeWidget, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ), this, SLOT( selectTreeItem() ) );
    connect( m_upButton, SIGNAL( pressed() ), m_treeWidget, SLOT( moveTreeItemUp() ) );
    connect( m_downButton, SIGNAL( pressed() ), m_treeWidget, SLOT( moveTreeItemDown() ) );
}


WQtSubjectTreeItem* WQtDatasetBrowser::addSubject( std::string name )
{
    WQtSubjectTreeItem* subject = new WQtSubjectTreeItem( m_treeWidget );
    subject->setText( 0, QString( name.c_str() ) );

    return subject;
}


WQtDatasetTreeItem* WQtDatasetBrowser::addDataset( std::string name, int subjectId )
{
    WQtSubjectTreeItem* subject = ( WQtSubjectTreeItem* )m_treeWidget->topLevelItem( subjectId );
    return subject->addDatasetItem( name );
}

void WQtDatasetBrowser::selectTreeItem()
{
    WLogger::getLogger()->addLogMessage( "tree item clicked: " );
    WLogger::getLogger()->addLogMessage( std::string( m_treeWidget->selectedItems().at( 0 )->text( 0 ).toAscii() ) );
    ( ( WQtDatasetTreeItem* ) m_treeWidget->selectedItems().at( 0 ) )->emitSelect();
}

void WQtDatasetBrowser::addTabWidgetContent( WQtDSBWidget* content )
{
    m_tabWidget->addTab( content, content->getName() );
}
