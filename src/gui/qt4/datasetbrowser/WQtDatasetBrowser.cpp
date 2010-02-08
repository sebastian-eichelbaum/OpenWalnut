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
#include "../events/WModuleAssocEvent.h"
#include "../events/WRoiAssocEvent.h"
#include "../events/WModuleReadyEvent.h"
#include "../events/WEventTypes.h"
#include "WQtNumberEdit.h"
#include "WQtNumberEditDouble.h"

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

    m_tiModules = new WQtModuleHeaderTreeItem( m_treeWidget );
    m_tiModules->setText( 0, QString( "Modules" ) );
    m_tiRois = new WQtRoiHeaderTreeItem( m_treeWidget );
    m_tiRois->setText( 0, QString( "ROIs" ) );

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

bool WQtDatasetBrowser::event( QEvent* event )
{
    // a module got associated with the root container -> add it to the list
    if ( event->type() == WQT_ASSOC_EVENT )
    {
        // convert event to assoc event
        WModuleAssocEvent* e1 = dynamic_cast< WModuleAssocEvent* >( event );     // NOLINT
        if ( e1 )
        {
            WLogger::getLogger()->addLogMessage( "Inserting module " + e1->getModule()->getName() + " to dataset browser.",
                                                 "DatasetBrowser", LL_DEBUG );

            // finally add the module
            // TODO(schurade): is this differentiation between data and "normal" modules really needed?
            if ( boost::shared_dynamic_cast< WMData >( e1->getModule() ).get() )
            {
                addDataset( e1->getModule(), 0 );
            }
            else
            {
                addModule( e1->getModule() );
            }
        }
        return true;
    }
    if ( event->type() == WQT_ROI_ASSOC_EVENT)
    {
        WRoiAssocEvent* e2 = dynamic_cast< WRoiAssocEvent* >( event );     // NOLINT
        if ( e2 )
        {
            addRoi( e2->getRoi() );
            WLogger::getLogger()->addLogMessage( "Inserting roi to dataset browser.", "DatasetBrowser", LL_DEBUG );
        }

        return true;
    }

    // a module changed its state to "ready" -> activate it in dataset browser
    if ( event->type() == WQT_READY_EVENT )
    {
        // convert event to assoc event
        WModuleReadyEvent* e = dynamic_cast< WModuleReadyEvent* >( event );     // NOLINT
        if ( !e )
        {
            // this should never happen, since the type is set to WQT_Ready_EVENT.
            WLogger::getLogger()->addLogMessage( "Event is not an WModueReadyEvent although its type claims it. Ignoring event.",
                                                 "DatasetBrowser", LL_WARNING );
        }

        WLogger::getLogger()->addLogMessage( "Activating module " + e->getModule()->getName() + " in dataset browser.",
                                             "DatasetBrowser", LL_DEBUG );

        // iterate tree items and find proper one
        QTreeWidgetItemIterator it( m_treeWidget );
        while ( *it )
        {
            WQtTreeItem* item = dynamic_cast< WQtTreeItem* >( *it );
            boost::shared_ptr< WModule > module = boost::shared_ptr< WModule >();
            if ( item )
            {
                module = item->getModule();
            }

            // if the pointer is NULL the item was none of the above
            if ( !module.get() )
            {
                ++it;
                continue;
            }

            // we found it
            if ( e->getModule() == module )
            {
                // activate it
                item->setDisabled( false );

                // if the type number is 1 (dataset item) emit change event
                if ( item->type() == 1 )
                {
                    emit dataSetBrowserEvent( QString( "textureChanged" ), true );
                    emit dataSetBrowserEvent( QString( "dataSetAdded" ), true );
                }
            }

            ++it;
        }
    }

    return QDockWidget::event( event );
}

void WQtDatasetBrowser::addModule2( boost::shared_ptr< WModule > module, int subjectId )
{
    if ( boost::shared_dynamic_cast< WMData >( module ) )
    {
        WQtSubjectTreeItem* subject = ( WQtSubjectTreeItem* )m_treeWidget->topLevelItem( subjectId + 1 );
        subject->setExpanded( true );
        WQtDatasetTreeItem* item = subject->addDatasetItem( module );
        item->setDisabled( true );
    }
    else
    {
        m_tiModules->setExpanded( true );
        WQtModuleTreeItem* item = m_tiModules->addModuleItem( module );
        item->setDisabled( true );
    }
}

WQtDatasetTreeItem* WQtDatasetBrowser::addDataset( boost::shared_ptr< WModule > module, int subjectId )
{
    int c = getFirstSubject();
    WQtSubjectTreeItem* subject = ( WQtSubjectTreeItem* )m_treeWidget->topLevelItem( subjectId + c );
    subject->setExpanded( true );
    WQtDatasetTreeItem* item = subject->addDatasetItem( module );
    item->setDisabled( true );
    return item;
}

WQtModuleTreeItem* WQtDatasetBrowser::addModule( boost::shared_ptr< WModule > module )
{
    m_tiModules->setExpanded( true );
    WQtModuleTreeItem* item = m_tiModules->addModuleItem( module );
    item->setDisabled( true );
    return item;
}

void WQtDatasetBrowser::addRoi( boost::shared_ptr< WRMROIRepresentation > roi )
{
    if ( m_treeWidget->selectedItems().count() != 0 )
    {
        switch ( m_treeWidget->selectedItems().at( 0 )->type() )
        {
            case ROI :
            {
                WQtRoiTreeItem* roiItem =( ( WQtRoiTreeItem* ) m_treeWidget->selectedItems().at( 0 ) );
                m_tiRois->setExpanded( true );
                roiItem->setExpanded( true );
                WQtRoiTreeItem* item = roiItem->addRoiItem( roi );
                item->setDisabled( false );
                break;
            }
            case SUBROI :
            {
                WQtRoiTreeItem* roiItem =( ( WQtRoiTreeItem* ) m_treeWidget->selectedItems().at( 0 )->parent() );
                m_tiRois->setExpanded( true );
                roiItem->setExpanded( true );
                WQtRoiTreeItem* item = roiItem->addRoiItem( roi );
                item->setDisabled( false );
                break;
            }
            default:
            {
                m_tiRois->setExpanded( true );
                WQtRoiTreeItem* item = m_tiRois->addRoiItem( roi );
                item->setDisabled( false );
                break;
            }
        }
    }
    else
    {
        m_tiRois->setExpanded( true );
        WQtRoiTreeItem* item = m_tiRois->addRoiItem( roi );
        item->setDisabled( false );
    }
}

boost::shared_ptr< WModule > WQtDatasetBrowser::getSelectedModule()
{
    if ( m_treeWidget->selectedItems().at( 0 )->type() == 1 )
    {
        return ( ( WQtDatasetTreeItem* ) m_treeWidget->selectedItems().at( 0 ) )->getModule();
    }
    else if ( m_treeWidget->selectedItems().at( 0 )->type() == 3 )
    {
        return ( ( WQtModuleTreeItem* ) m_treeWidget->selectedItems().at( 0 ) )->getModule();
    }

    return boost::shared_ptr< WModule >();
}

void WQtDatasetBrowser::selectTreeItem()
{
    // TODO(schurade): qt doc says clear() doesn't delete tabs so this is possibly a memory leak
    m_tabWidget->clear();
    m_mainWindow->getCompatiblesToolBar()->clearButtons();

    boost::shared_ptr< WModule >module;
    boost::shared_ptr< WProperties2 > props;

    if ( m_treeWidget->selectedItems().size() != 0  )
    {
        switch ( m_treeWidget->selectedItems().at( 0 )->type() )
        {
            case SUBJECT:
                break;
            case DATASET:
                module = ( static_cast< WQtDatasetTreeItem* >( m_treeWidget->selectedItems().at( 0 ) ) )->getModule();
                props = module->getProperties2();
                createCompatibleButtons( module );
                break;
            case MODULEHEADER:
                break;
            case MODULE:
                module = ( static_cast< WQtModuleTreeItem* >( m_treeWidget->selectedItems().at( 0 ) ) )->getModule();
                props = module->getProperties2();
                createCompatibleButtons( module );
                break;
            case ROIHEADER:
                break;
            case ROI:
            case SUBROI:
                // TODO(ebaum): update rois to use new properties
                //props = ( static_cast< WQtRoiTreeItem* >( m_treeWidget->selectedItems().at( 0 ) ) )->getRoi()->getProperties()->getPropertyVector();
                //props = module->getProperties2();
                break;
            default:
                break;
        }
    }

    // TODO(seralph): remove the above stuff if all modules use wpropertyvariable instead of wproperty
    WQtDSBWidget* tab = new WQtDSBWidget( "Settings" );

    if ( props.get() )
    {
        // iterate all properties. This Locks the properties set -> use endIteration to unlock
        for ( WProperties2::PropertyIterator iter = props->beginIteration(); iter != props->getPropertyIteratorEnd(); ++iter )
        {
            if ( !( *iter )->isHidden() )
            {
                switch ( ( *iter )->getType() )
                {
                    case PV_BOOL:
                        tab->addPropBool( ( *iter )->toPropBool() );
                        break;
                    case PV_INT:
                        tab->addPropInt( ( *iter )->toPropInt() );
                        break;
                    case PV_DOUBLE:
                        tab->addPropDouble( ( *iter )->toPropDouble() );
                        break;
                    case PV_STRING:
                        tab->addPropString( ( *iter )->toPropString() );
                        break;
                    case PV_PATH:
                        WLogger::getLogger()->addLogMessage( "This property type \"PV_PATH\" is not yet supported by the GUI.", "DatasetBrowser",
                                LL_WARNING );
                        break;
                    case PV_LIST:
                        WLogger::getLogger()->addLogMessage( "This property type \"PV_LIST\" is not yet supported by the GUI.", "DatasetBrowser",
                                LL_WARNING );
                        break;
                    case PV_COLOR:
                        WLogger::getLogger()->addLogMessage( "This property type \"PV_COLOR\" is not yet supported by the GUI.", "DatasetBrowser",
                                LL_WARNING );
                        break;
                    case PV_POSITION:
                        WLogger::getLogger()->addLogMessage( "This property type \"PV_POSITION\" is not yet supported by the GUI.", "DatasetBrowser",
                                LL_WARNING );
                        break;
                    default:
                        WLogger::getLogger()->addLogMessage( "This property type is not yet supported.", "DatasetBrowser", LL_WARNING );
                        break;
                }
            }
        }
        props->endIteration();
    }
    tab->addSpacer();
    addTabWidgetContent( tab );
}


void WQtDatasetBrowser::createCompatibleButtons( boost::shared_ptr< WModule >module )
{
    // every module may have compatibles: create ribbon menu entry
    std::set< boost::shared_ptr< WModule > > comps = WModuleFactory::getModuleFactory()->getCompatiblePrototypes( module );
    for ( std::set< boost::shared_ptr< WModule > >::iterator iter = comps.begin(); iter != comps.end(); ++iter )
    {
        WQtPushButton* button = m_mainWindow->getCompatiblesToolBar()->addPushButton( QString( ( *iter )->getName().c_str() ),
                m_mainWindow->getIconManager()->getIcon( ( *iter )->getName().c_str() ), QString( ( *iter )->getName().c_str() ) );

        connect( button, SIGNAL( pushButtonPressed( QString ) ), m_mainWindow, SLOT( slotActivateModule( QString ) ) );
    }
}

void WQtDatasetBrowser::changeTreeItem()
{
    if ( m_treeWidget->selectedItems().size() == 1 && m_treeWidget->selectedItems().at( 0 )->type() == DATASET )
    {
        boost::shared_ptr< WModule >module =( static_cast< WQtDatasetTreeItem* >( m_treeWidget->selectedItems().at( 0 ) ) )->getModule();
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
    else if ( m_treeWidget->selectedItems().size() == 1 && m_treeWidget->selectedItems().at( 0 )->type() == MODULE )
    {
        boost::shared_ptr< WModule >module =( static_cast< WQtModuleTreeItem* >( m_treeWidget->selectedItems().at( 0 ) ) )->getModule();
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
    getPropOfSelected()->setValue<int>( name.toStdString(), value );

    emit dataSetBrowserEvent( QString( "textureChanged" ), true );
}

void WQtDatasetBrowser::slotSetDoubleProperty( QString name, double value )
{
    getPropOfSelected()->setValue<double>( name.toStdString(), value );

    emit dataSetBrowserEvent( QString( "textureChanged" ), true );
}

void WQtDatasetBrowser::slotSetBoolProperty( QString name, bool value )
{
    getPropOfSelected()->setValue<bool>( name.toStdString(), value );


    emit dataSetBrowserEvent( QString( "textureChanged" ), true );
}

void WQtDatasetBrowser::slotSetStringProperty( QString name, QString value )
{
    getPropOfSelected()->setValue<std::string>( name.toStdString(), value.toStdString() );

    if ( name == "Name")
    {
        m_treeWidget->selectedItems().at( 0 )->setText( 0, value );
    }
}

boost::shared_ptr< WProperties > WQtDatasetBrowser::getPropOfSelected()
{
    boost::shared_ptr< WModule > module;
    boost::shared_ptr< WProperties > props;
    if ( m_treeWidget->selectedItems().size() != 0  )
    {
        switch ( m_treeWidget->selectedItems().at( 0 )->type() )
        {
            case SUBJECT:
                break;
            case DATASET:
                module = ( static_cast< WQtModuleTreeItem* >( m_treeWidget->selectedItems().at( 0 ) ) )->getModule();
                props = module->getProperties();
                break;
            case MODULE:
                module = ( static_cast< WQtModuleTreeItem* >( m_treeWidget->selectedItems().at( 0 ) ) )->getModule();
                props = module->getProperties();
                break;
            case MODULEHEADER:
                break;
            case ROIHEADER:
                break;
            case ROI:
            case SUBROI:
                props = ( static_cast< WQtRoiTreeItem* >( m_treeWidget->selectedItems().at( 0 ) ) )->getRoi()->getProperties();
                break;
            default:
                break;
        }
    }
    return props;
}

std::vector< boost::shared_ptr< WDataSet > > WQtDatasetBrowser::getDataSetList( int subjectId, bool onlyTextures )
{
    int c = getFirstSubject();
    std::vector< boost::shared_ptr< WDataSet > >moduleList;

    if ( m_treeWidget->invisibleRootItem()->childCount() < subjectId + c)
    {
        return moduleList;
    }
    int count = m_treeWidget->invisibleRootItem()->child( subjectId + c )->childCount();

    for ( int i = 0 ; i < count ; ++i )
    {
        boost::shared_ptr< WMData > dm = boost::shared_dynamic_cast< WMData >( ( static_cast< WQtDatasetTreeItem* >(
                            m_treeWidget->invisibleRootItem()->child( subjectId + c )->child( i ) ) )->getModule() );

        if ( dm->isReady()() && ( !onlyTextures || dm->getDataSet()->isTexture() ) )
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

int WQtDatasetBrowser::getFirstSubject()
{
    int c = 0;
    for ( int i = 0; i < m_treeWidget->topLevelItemCount() ; ++i )
    {
        if ( m_treeWidget->topLevelItem( i )->type() == SUBJECT )
        {
            break;
        }
        ++c;
    }
    return c;
}

boost::shared_ptr< WRMROIRepresentation > WQtDatasetBrowser::getSelectedRoi()
{
    boost::shared_ptr< WRMROIRepresentation >roi;
    if ( m_treeWidget->selectedItems().count() == 0 )
    {
        return roi;
    }
    if ( m_treeWidget->selectedItems().at( 0 )->type() == ROI )
    {
        roi =( static_cast< WQtRoiTreeItem* >( m_treeWidget->selectedItems().at( 0 ) ) )->getRoi();
        std::cout << "return this" << std::endl;
    }
    if ( m_treeWidget->selectedItems().at( 0 )->type() == SUBROI )
    {
        roi =( static_cast< WQtRoiTreeItem* >( m_treeWidget->selectedItems().at( 0 )->parent() ) )->getRoi();
        std::cout << "return parent" << std::endl;
    }
    return roi;
}
