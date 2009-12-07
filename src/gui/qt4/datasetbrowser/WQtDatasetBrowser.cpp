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

#include "../../../common/WLogger.h"

#include "../../../dataHandler/WDataSet.h"

#include "WQtDatasetBrowser.h"
#include "WQtNumberEdit.h"
#include "WQtCheckBox.h"
#include "WQtModuleHeaderTreeItem.h"

#include "../../../kernel/WModuleFactory.h"
#include "../WMainWindow.h"

#include "../../../modules/data/WMData.h"


WQtDatasetBrowser::WQtDatasetBrowser( WMainWindow* parent )
    : QDockWidget( parent )
{
    m_mainWindow = parent;

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

    WQtModuleHeaderTreeItem* tiModules = new WQtModuleHeaderTreeItem( m_treeWidget );
    tiModules->setText( 0, QString( "Modules" ) );

    connectSlots();
}

WQtDatasetBrowser::~WQtDatasetBrowser()
{
}


void WQtDatasetBrowser::connectSlots()
{
    connect( m_treeWidget, SIGNAL( itemSelectionChanged() ), this, SLOT( selectTreeItem() ) );
    connect( m_treeWidget, SIGNAL( itemClicked( QTreeWidgetItem*, int ) ), this, SLOT( changeTreeItem() ) );
    connect( m_upButton, SIGNAL( pressed() ), this, SLOT( moveTreeItemUp() ) );
    connect( m_downButton, SIGNAL( pressed() ), this, SLOT( moveTreeItemDown() ) );
}


WQtSubjectTreeItem* WQtDatasetBrowser::addSubject( std::string name )
{
    WQtSubjectTreeItem* subject = new WQtSubjectTreeItem( m_treeWidget );
    subject->setText( 0, QString( name.c_str() ) );

    return subject;
}


WQtDatasetTreeItem* WQtDatasetBrowser::addDataset( boost::shared_ptr< WModule > module, int subjectId )
{
    WQtSubjectTreeItem* subject = ( WQtSubjectTreeItem* )m_treeWidget->topLevelItem( subjectId + 1 );
    subject->setExpanded( true );
    emit dataSetBrowserEvent( QString( "textureChanged" ), true );
    emit dataSetBrowserEvent( QString( "dataSetAdded" ), true );
    return subject->addDatasetItem( module );
}

WQtModuleTreeItem* WQtDatasetBrowser::addModule( boost::shared_ptr< WModule > module )
{
    WQtModuleHeaderTreeItem* tiModules = ( WQtModuleHeaderTreeItem* )m_treeWidget->topLevelItem( 0 );
    tiModules->setExpanded( true );
    return tiModules->addModuleItem( module );
}


void WQtDatasetBrowser::selectTreeItem()
{
    // TODO(schurade): qt doc says clear() doesn't delete tabs so this is possibly a memory leak
    m_tabWidget->clear();

    if ( m_treeWidget->selectedItems().size() == 0 || m_treeWidget->selectedItems().at( 0 )->type() == 0 ||
            m_treeWidget->selectedItems().at( 0 )->type() == 2 )
    {
        return;
    }
    boost::shared_ptr< WModule >module;

    if ( m_treeWidget->selectedItems().at( 0 )->type() == 1 )
    {
        module = ( ( WQtDatasetTreeItem* ) m_treeWidget->selectedItems().at( 0 ) )->getModule();
    }
    else
    {
        module = ( ( WQtModuleTreeItem* ) m_treeWidget->selectedItems().at( 0 ) )->getModule();
    }

    // create ribbon menu entry
    std::set< boost::shared_ptr< WModule > > comps = WModuleFactory::getModuleFactory()->getCompatiblePrototypes( module );
    for ( std::set< boost::shared_ptr< WModule > >::iterator iter = comps.begin(); iter != comps.end(); ++iter )
    {
        std::cout << "Name:" << ( *iter )->getName() << std::endl;
    }

    // create properties
    std::vector < WProperty* >props = module->getProperties()->getPropertyVector();

    WQtDSBWidget* tab1 = new WQtDSBWidget( "settings" );

    for ( size_t i = 0; i < props.size(); ++i )
    {
        if ( !props.at(i)->isHidden() )
        {
            switch ( props.at(i)->getType() )
            {
                case P_BOOL:
                {
                    QString name = QString( props.at( i )->getName().c_str() );
                    WQtCheckBox* box = tab1->addCheckBox( name, props.at( i )->getValue<bool>() );
                    connect( box, SIGNAL( checkBoxStateChanged( QString, bool ) ), this, SLOT( slotSetBoolProperty( QString, bool ) ) );
                    break;
                }
                case P_CHAR:
                    break;
                case P_UNSIGNED_CHAR:
                    break;
                case P_INT:
                {
                    QString name = QString( props.at( i )->getName().c_str() );
                    WQtSliderWithEdit* slider = tab1->addSliderInt( name, props.at( i )->getValue<int>(),
                            props.at( i )->getMin<int>(), props.at( i )->getMax<int>() );
                    connect( slider, SIGNAL( signalNumberWithName( QString, int ) ), this, SLOT( slotSetIntProperty( QString, int ) ) );
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
                    QString name = QString( props.at( i )->getName().c_str() );
                    QString text = QString( props.at( i )->getValue<std::string>().c_str() );
                    WQtLineEdit* edit = tab1->addLineEdit( name, text );
                    connect( edit, SIGNAL( lineEditStateChanged( QString, QString ) ), this, SLOT( slotSetStringProperty( QString, QString ) ) );
                    break;
                }
                default:
                    break;
            }
        }
    }

    tab1->addSpacer();
    addTabWidgetContent( tab1 );
}


void WQtDatasetBrowser::changeTreeItem()
{
    if ( m_treeWidget->selectedItems().size() == 1 && m_treeWidget->selectedItems().at( 0 )->type() == 1 )
    {
        boost::shared_ptr< WModule >module =( ( WQtDatasetTreeItem* ) m_treeWidget->selectedItems().at( 0 ) )->getModule();
        if ( m_treeWidget->selectedItems().at( 0 )->checkState( 0 ) )
        {
            module->getProperties()->setValue<bool>( "active", true );
        }
        else
        {
            module->getProperties()->setValue<bool>( "active", false );
        }
        emit dataSetBrowserEvent( QString( "textureChanged" ), true );
    }
    else if ( m_treeWidget->selectedItems().size() == 1 && m_treeWidget->selectedItems().at( 0 )->type() == 3 )
    {
        boost::shared_ptr< WModule >module =( ( WQtModuleTreeItem* ) m_treeWidget->selectedItems().at( 0 ) )->getModule();
        if ( m_treeWidget->selectedItems().at( 0 )->checkState( 0 ) )
        {
            module->getProperties()->setValue<bool>( "active", true );
        }
        else
        {
            module->getProperties()->setValue<bool>( "active", false );
        }
    }
}

void WQtDatasetBrowser::addTabWidgetContent( WQtDSBWidget* content )
{
    m_tabWidget->addTab( content, content->getName() );
}

void WQtDatasetBrowser::slotSetIntProperty( QString name, int value )
{
    boost::shared_ptr< WModule >module =( ( WQtDatasetTreeItem* ) m_treeWidget->selectedItems().at( 0 ) )->getModule();
    module->getProperties()->setValue<int>( name.toStdString(), value );

    emit dataSetBrowserEvent( QString( "textureChanged" ), true );
}

void WQtDatasetBrowser::slotSetBoolProperty( QString name, bool value )
{
    boost::shared_ptr< WModule >module =( ( WQtDatasetTreeItem* ) m_treeWidget->selectedItems().at( 0 ) )->getModule();
    module->getProperties()->setValue<bool>( name.toStdString(), value );

    emit dataSetBrowserEvent( QString( "textureChanged" ), true );
}

void WQtDatasetBrowser::slotSetStringProperty( QString name, QString value )
{
    boost::shared_ptr< WModule >module =( ( WQtDatasetTreeItem* ) m_treeWidget->selectedItems().at( 0 ) )->getModule();
    module->getProperties()->setValue<std::string>( name.toStdString(), value.toStdString() );

    if ( name == "name")
    {
        m_treeWidget->selectedItems().at( 0 )->setText( 0, value );
    }
}

std::vector< boost::shared_ptr< WDataSet > > WQtDatasetBrowser::getDataSetList( int subjectId, bool onlyTextures )
{
    std::vector< boost::shared_ptr< WDataSet > >moduleList;

    if ( m_treeWidget->invisibleRootItem()->childCount() < subjectId + 1)
    {
        return moduleList;
    }
    int count = m_treeWidget->invisibleRootItem()->child( subjectId + 1 )->childCount();

    for ( int i = 0 ; i < count ; ++i )
    {
        boost::shared_ptr< WMData > dm = boost::shared_dynamic_cast< WMData >( ( ( WQtDatasetTreeItem* )m_treeWidget->invisibleRootItem()->child(
                        subjectId + 1 )->child( i ) )->getModule() );

        if ( !onlyTextures || dm->getDataSet()->isTexture() )
        {
            if ( dm->getProperties()->getValue<bool>( "active" ) )
            {
                moduleList.push_back( dm->getDataSet() );
            }
        }
    }
    return moduleList;
}

void WQtDatasetBrowser::moveTreeItemDown()
{
    m_treeWidget->moveTreeItemDown();
    emit dataSetBrowserEvent( QString( "textureChanged" ), true );
}

void WQtDatasetBrowser::moveTreeItemUp()
{
    m_treeWidget->moveTreeItemUp();
    emit dataSetBrowserEvent( QString( "textureChanged" ), true );
}

