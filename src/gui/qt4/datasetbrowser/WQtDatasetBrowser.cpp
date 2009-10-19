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
#include <string>

#include <QtCore/QList>

#include "../../../common/WLogger.h"

#include "WQtDatasetBrowser.h"
#include "WQtNumberEdit.h"
#include "WQtCheckBox.h"

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
    connect( m_treeWidget, SIGNAL( itemSelectionChanged() ), this, SLOT( selectTreeItem() ) );
    connect( m_treeWidget, SIGNAL( itemClicked( QTreeWidgetItem*, int ) ), this, SLOT( changeTreeItem() ) );
    connect( m_upButton, SIGNAL( pressed() ), m_treeWidget, SLOT( moveTreeItemUp() ) );
    connect( m_downButton, SIGNAL( pressed() ), m_treeWidget, SLOT( moveTreeItemDown() ) );
}


WQtSubjectTreeItem* WQtDatasetBrowser::addSubject( std::string name )
{
    WQtSubjectTreeItem* subject = new WQtSubjectTreeItem( m_treeWidget );
    subject->setText( 0, QString( name.c_str() ) );

    return subject;
}


WQtDatasetTreeItem* WQtDatasetBrowser::addDataset( boost::shared_ptr< WModule > module, int subjectId )
{
    WQtSubjectTreeItem* subject = ( WQtSubjectTreeItem* )m_treeWidget->topLevelItem( subjectId );
    return subject->addDatasetItem( module );
}

void WQtDatasetBrowser::selectTreeItem()
{
    if ( m_treeWidget->selectedItems().size() == 0 || m_treeWidget->selectedItems().at( 0 )->type() != 1 )
    {
        return;
    }

    boost::shared_ptr< WModule >module =( ( WQtDatasetTreeItem* ) m_treeWidget->selectedItems().at( 0 ) )->getModule();
    std::map < std::string, WProperty* >*props = module->getProperties()->getProperties();
    std::map < std::string, WProperty* >::const_iterator propIt = props->begin();

    WQtDSBWidget* tab1 = new WQtDSBWidget( "settings" );

    while ( propIt != props->end() )
    {
        switch ( propIt->second->getType() )
        {
            case P_BOOL:
            {
                WQtCheckBox* box = tab1->addCheckBox( propIt->second->getName(), propIt->second->getValue<bool>() );
                box->getboostSignal()->connect( boost::bind( &WQtDatasetBrowser::slotSetBoolProperty, this, _1, _2 ) );
                break;
            }
            case P_CHAR:
                break;
            case P_UNSIGNED_CHAR:
                break;
            case P_INT:
            {
                WQtSliderWithEdit* slider = tab1->addSliderInt( propIt->second->getName(), propIt->second->getValue<int>(),
                        propIt->second->getMin<int>(), propIt->second->getMax<int>() );
                slider->getboostSignal()->connect( boost::bind( &WQtDatasetBrowser::slotSetIntProperty, this, _1, _2 ) );
                break;
            }
            case P_UNSIGNED_INT:
                break;
            case P_FLOAT:
                break;
            case P_DOUBLE:
                break;
            case P_STRING:
            {
                WQtLineEdit* edit = tab1->addLineEdit( propIt->second->getName(), propIt->second->getValue<std::string>() );
                edit->getboostSignal()->connect( boost::bind( &WQtDatasetBrowser::slotSetStringProperty, this, _1, _2 ) );
                break;
            }
            default:
                break;
        }
        ++propIt;
    }
    // TODO(schurade): qt doc says clear() doesn't delete tabs so this is possibly a memory leak
    m_tabWidget->clear();
    addTabWidgetContent( tab1 );
}

void WQtDatasetBrowser::changeTreeItem()
{
    if ( m_treeWidget->selectedItems().size() == 0 || m_treeWidget->selectedItems().at( 0 )->type() != 1 )
    {
        return;
    }
    boost::shared_ptr< WModule >module =( ( WQtDatasetTreeItem* ) m_treeWidget->selectedItems().at( 0 ) )->getModule();

    if ( m_treeWidget->selectedItems().at( 0 )->checkState( 0 ) )
    {
        module->getProperties()->setValue<bool>( "active", true );
    }
    else
    {
        module->getProperties()->setValue<bool>( "active", false );
    }
    selectTreeItem();
}

void WQtDatasetBrowser::addTabWidgetContent( WQtDSBWidget* content )
{
    m_tabWidget->addTab( content, content->getName() );
}

void WQtDatasetBrowser::slotSetIntProperty( std::string name, int value )
{
    boost::shared_ptr< WModule >module =( ( WQtDatasetTreeItem* ) m_treeWidget->selectedItems().at( 0 ) )->getModule();
    module->getProperties()->setValue<int>( name, value );
}

void WQtDatasetBrowser::slotSetBoolProperty( std::string name, bool value )
{
    boost::shared_ptr< WModule >module =( ( WQtDatasetTreeItem* ) m_treeWidget->selectedItems().at( 0 ) )->getModule();
    module->getProperties()->setValue<bool>( name, value );

    if ( name == "active")
    {
        if ( value )
        {
            m_treeWidget->selectedItems().at( 0 )->setCheckState( 0, Qt::Checked );
        }
        else
        {
            m_treeWidget->selectedItems().at( 0 )->setCheckState( 0, Qt::Unchecked );
        }
    }
}

void WQtDatasetBrowser::slotSetStringProperty( std::string name, std::string value )
{
    boost::shared_ptr< WModule >module =( ( WQtDatasetTreeItem* ) m_treeWidget->selectedItems().at( 0 ) )->getModule();
    module->getProperties()->setValue<std::string>( name, value );

    if ( name == "name")
    {
        m_treeWidget->selectedItems().at( 0 )->setText( 0,  QString( value.c_str() ) );
    }
}
