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
#include <QtGui/QShortcut>
#include <QtGui/QMenu>

#include "../../../common/WLogger.h"
#include "../../../common/WPreferences.h"

#include "../../../dataHandler/WDataSet.h"

#include "../../../kernel/WModuleFactory.h"
#include "../../../kernel/WKernel.h"
#include "../../../modules/data/WMData.h"
#include "../events/WModuleAssocEvent.h"
#include "../events/WRoiAssocEvent.h"
#include "../events/WRoiRemoveEvent.h"
#include "../events/WModuleReadyEvent.h"
#include "../events/WModuleDeleteEvent.h"
#include "../events/WModuleRemovedEvent.h"
#include "../events/WEventTypes.h"
#include "../guiElements/WQtApplyModulePushButton.h"
#include "../WMainWindow.h"
#include "../WQt4Gui.h"
#include "WQtNumberEdit.h"
#include "WQtNumberEditDouble.h"
#include "WQtTextureSorter.h"
#include "WQtBranchTreeItem.h"

#include "WQtDatasetBrowser.h"

WQtDatasetBrowser::WQtDatasetBrowser( WMainWindow* parent )
    : QDockWidget( "Dataset Browser", parent )
{
    m_mainWindow = parent;

    m_panel = new QWidget( this );
    m_moduleTreeWidget = new WQtTreeWidget( m_panel );
    m_moduleTreeWidget->setContextMenuPolicy( Qt::ActionsContextMenu );

    m_moduleTreeWidget->setHeaderLabel( QString( "Dataset Browser" ) );
    m_moduleTreeWidget->setDragEnabled( true );
    m_moduleTreeWidget->viewport()->setAcceptDrops( true );
    m_moduleTreeWidget->setDropIndicatorShown( true );
    m_moduleTreeWidget->setDragDropMode( QAbstractItemView::InternalMove );
    m_moduleTreeWidget->setMinimumHeight( 250 );

    // create context menu for tree items
    m_deleteModuleAction = new QAction( WQt4Gui::getMainWindow()->getIconManager()->getIcon( "remove" ), "Remove Module", m_moduleTreeWidget );
    m_deleteModuleAction->setShortcut( QKeySequence( Qt::Key_Backspace ) );
    connect( m_deleteModuleAction, SIGNAL( triggered() ), this, SLOT( deleteModuleTreeItem() ) );
    m_moduleTreeWidget->addAction( m_deleteModuleAction );

    m_textureSorter = new WQtTextureSorter( this );
    m_textureSorter->setToolTip( "Reorder the textures." );

    m_tabWidget = new QTabWidget( m_panel );
    m_tabWidget2 = new QTabWidget( m_panel );
    m_tabWidget->setMinimumHeight( 220 );

    m_layout = new QVBoxLayout();
    m_layout->addWidget( m_moduleTreeWidget );
    m_layout->addWidget( m_tabWidget2 );

    m_tabWidget2->addTab( m_textureSorter, QString( "Texture Sorter" ) );

    m_roiTreeWidget = new WQtTreeWidget();
    m_roiTreeWidget->setToolTip( "Regions of intrest (ROIs) for selecting fiber  clusters. Branches are combined using logic <b>or</b>, "
"inside the branches the ROIs are combined using logic <b>and</b>." );
    m_roiTreeWidget->setHeaderLabel( QString( "ROIs" ) );
    m_roiTreeWidget->setHeaderHidden( true );
    m_roiTreeWidget->setDragEnabled( true );
    m_roiTreeWidget->viewport()->setAcceptDrops( true );
    m_roiTreeWidget->setDropIndicatorShown( true );
    m_roiTreeWidget->setDragDropMode( QAbstractItemView::InternalMove );
    m_tabWidget2->addTab( m_roiTreeWidget, QString( "ROIs" ) );


    m_layout->addWidget( m_tabWidget );

    m_panel->setLayout( m_layout );

    this->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
    this->setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
    this->setWidget( m_panel );

    m_tiModules = new WQtModuleHeaderTreeItem( m_moduleTreeWidget );
    m_tiModules->setText( 0, QString( "Modules" ) );
    m_tiRois = new WQtRoiHeaderTreeItem( m_roiTreeWidget );
    m_tiRois->setText( 0, QString( "ROIs" ) );

    connectSlots();

    // preset for toolbar text.
    m_showToolBarText = true;
    WPreferences::getPreference( "qt4gui.toolBarIconText", &m_showToolBarText );

    // These modules will be allowed to be shown.
    std::string moduleWhiteList;
    WPreferences::getPreference( "modules.whiteList", &moduleWhiteList );
    m_moduleWhiteList = string_utils::tokenize( moduleWhiteList, "," );

    QShortcut* shortcut = new QShortcut( QKeySequence( Qt::Key_Delete ), m_roiTreeWidget );
    connect( shortcut, SIGNAL( activated() ), this, SLOT( deleteROITreeItem() ) );
}

WQtDatasetBrowser::~WQtDatasetBrowser()
{
}

void WQtDatasetBrowser::connectSlots()
{
    // connect( m_moduleTreeWidget, SIGNAL( itemSelectionChanged() ), this, SLOT( selectTreeItem() ) );
    connect( m_moduleTreeWidget, SIGNAL( itemClicked( QTreeWidgetItem*, int ) ), this, SLOT( selectTreeItem() ) );
    connect( m_moduleTreeWidget, SIGNAL( itemClicked( QTreeWidgetItem*, int ) ), this, SLOT( changeTreeItem() ) );
    connect( m_moduleTreeWidget, SIGNAL( itemClicked( QTreeWidgetItem*, int ) ),  m_roiTreeWidget, SLOT( clearSelection() ) );
    //connect( m_roiTreeWidget, SIGNAL( itemSelectionChanged() ), this, SLOT( selectRoiTreeItem() ) );
    connect( m_roiTreeWidget, SIGNAL( itemClicked( QTreeWidgetItem*, int ) ), this, SLOT( selectRoiTreeItem() ) );
    connect( m_roiTreeWidget, SIGNAL( itemClicked( QTreeWidgetItem*, int ) ), this, SLOT( changeRoiTreeItem() ) );
    connect( m_roiTreeWidget, SIGNAL( itemClicked( QTreeWidgetItem*, int ) ), m_moduleTreeWidget, SLOT( clearSelection() ) );

    connect( m_textureSorter, SIGNAL( textureSelectionChanged( boost::shared_ptr< WDataSet > ) ),
             this, SLOT( selectDataModule( boost::shared_ptr< WDataSet > ) ) );
}

WQtSubjectTreeItem* WQtDatasetBrowser::addSubject( std::string name )
{
    WQtSubjectTreeItem* subject = new WQtSubjectTreeItem( m_moduleTreeWidget );
    subject->setText( 0, QString( name.c_str() ) );

    return subject;
}

bool WQtDatasetBrowser::event( QEvent* event )
{
    // a subject singals a newly registered data set
    if ( event->type() == WQT_UPDATE_TEXTURE_SORTER_EVENT )
    {
        m_textureSorter->update();
    }

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
    if( event->type() == WQT_ROI_REMOVE_EVENT )
    {
        WRoiRemoveEvent* e3 = dynamic_cast< WRoiRemoveEvent* >( event );
        if( e3 )
        {
            removeRoi( e3->getRoi() );
            WLogger::getLogger()->addLogMessage( "Removing roi from dataset browser.", "DatasetBrowser", LL_DEBUG );
        }

        return true;
    }

    // a module tree item should be deleted
    if ( event->type() == WQT_MODULE_DELETE_EVENT )
    {
        WModuleDeleteEvent* e = dynamic_cast< WModuleDeleteEvent* >( event );     // NOLINT
        if ( !e )
        {
            // this should never happen, since the type is set to WQT_Ready_EVENT.
            WLogger::getLogger()->addLogMessage( "Event is not an WModuleDeleteEvent although its type claims it. Ignoring event.",
                                                 "DatasetBrowser", LL_WARNING );
            return true;
        }

        // grab the module reference and print some info
        boost::shared_ptr< WModule > module = e->getTreeItem()->getModule();
        WLogger::getLogger()->addLogMessage( "Deleting module \"" + module->getName() + "\" from Tree.",
                                             "DatasetBrowser", LL_DEBUG );

        // remove it from the tree and free last ref count
        m_moduleTreeWidget->deleteItem( e->getTreeItem() );

        // ref count != 1?
        if ( module.use_count() != 1 )
        {
            wlog::warn( "DatasetBrowser" ) << "Removed module has strange usage count: " << module.use_count() << ". Should be 1 here. " <<
                                              "Module reference is held by someone else.";
        }

        return true;
    }

    // a module was removed from the container
    if ( event->type() == WQT_MODULE_REMOVE_EVENT )
    {
        WModuleRemovedEvent* e = dynamic_cast< WModuleRemovedEvent* >( event );     // NOLINT
        if ( !e )
        {
            // this should never happen, since the type is set to WQT_Ready_EVENT.
            WLogger::getLogger()->addLogMessage( "Event is not an WModuleRemovedEvent although its type claims it. Ignoring event.",
                                                 "DatasetBrowser", LL_WARNING );
            return true;
        }

        // iterate tree items and find proper one
        QTreeWidgetItemIterator it( m_moduleTreeWidget );
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
                item->gotRemoved();
            }

            ++it;
        }

        // be nice and print some info
        WLogger::getLogger()->addLogMessage( "Removing module \"" + e->getModule()->getName() + "\" from Tree.", "DatasetBrowser", LL_DEBUG );

        // stop the module
        e->getModule()->requestStop();
        WLogger::getLogger()->addLogMessage( "Waiting for module \"" + e->getModule()->getName() + "\" to finish before deleting.",
                                             "DatasetBrowser", LL_DEBUG );

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

            return true;
        }

        WLogger::getLogger()->addLogMessage( "Activating module " + e->getModule()->getName() + " in dataset browser.",
                                             "DatasetBrowser", LL_DEBUG );

        // iterate tree items and find proper one
        QTreeWidgetItemIterator it( m_moduleTreeWidget );
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
                selectTreeItem();
            }

            ++it;
        }

        return true;
    }

    return QDockWidget::event( event );
}

WQtDatasetTreeItem* WQtDatasetBrowser::addDataset( boost::shared_ptr< WModule > module, int subjectId )
{
    int c = getFirstSubject();
    WQtSubjectTreeItem* subject = static_cast< WQtSubjectTreeItem* >( m_moduleTreeWidget->topLevelItem( subjectId + c ) );
    subject->setExpanded( true );
    WQtDatasetTreeItem* item = subject->addDatasetItem( module );
    m_moduleTreeWidget->setCurrentItem( item );
    item->setDisabled( true );
    return item;
}

WQtModuleTreeItem* WQtDatasetBrowser::addModule( boost::shared_ptr< WModule > module )
{
    m_tiModules->setExpanded( true );
    WQtModuleTreeItem* item = m_tiModules->addModuleItem( module );
    m_moduleTreeWidget->setCurrentItem( item );
    item->setDisabled( true );
    return item;
}

void WQtDatasetBrowser::addRoi( boost::shared_ptr< WRMROIRepresentation > roi )
{
    WQtRoiTreeItem* newItem;
    WQtBranchTreeItem* branchItem;

    m_tiRois->setExpanded( true );

    if ( m_roiTreeWidget->selectedItems().count() != 0 )
    {
        switch ( m_roiTreeWidget->selectedItems().at( 0 )->type() )
        {
            case ROI :
            {
                branchItem =( static_cast< WQtBranchTreeItem* >( m_roiTreeWidget->selectedItems().at( 0 )->parent() ) );
                break;
            }
            case ROIBRANCH :
            {
                branchItem =( static_cast< WQtBranchTreeItem* >( m_roiTreeWidget->selectedItems().at( 0 ) ) );
                break;
            }
            default:
            {
                branchItem = m_tiRois->addBranch( roi->getBranch() );
                break;
            }
        }
    }
    else
    {
        branchItem = m_tiRois->addBranch( roi->getBranch() );
    }

    m_tabWidget2->setCurrentIndex( m_tabWidget2->indexOf( m_roiTreeWidget ) );
    branchItem->setExpanded( true );
    newItem = branchItem->addRoiItem( roi );
    newItem->setDisabled( false );
    newItem->setSelected( true );
}

void WQtDatasetBrowser::removeRoi( boost::shared_ptr< WRMROIRepresentation > roi )
{
    for( int branchID = 0; branchID < m_tiRois->childCount(); ++branchID )
    {
        QTreeWidgetItem* branchTreeItem = m_tiRois->child( branchID );
        for( int roiID = 0; roiID < branchTreeItem->childCount(); ++roiID )
        {
            WQtRoiTreeItem* roiTreeItem = dynamic_cast< WQtRoiTreeItem* >( branchTreeItem->child( roiID ) );
            if( roiTreeItem && roiTreeItem->getRoi() == roi )
            {
                delete roiTreeItem;

                if( branchTreeItem->childCount() == 0 )
                {
                    delete branchTreeItem;
                }

                break;
            }
        }
    }
}

boost::shared_ptr< WModule > WQtDatasetBrowser::getSelectedModule()
{
    if ( m_moduleTreeWidget->selectedItems().at( 0 )->type() == 1 )
    {
        return ( static_cast< WQtDatasetTreeItem* >( m_moduleTreeWidget->selectedItems().at( 0 ) )->getModule() );
    }
    else if ( m_moduleTreeWidget->selectedItems().at( 0 )->type() == 3 )
    {
        return ( static_cast< WQtModuleTreeItem* >( m_moduleTreeWidget->selectedItems().at( 0 ) )->getModule() );
    }

    return boost::shared_ptr< WModule >();
}

void WQtDatasetBrowser::selectTreeItem()
{
    // TODO(schurade): qt doc says clear() doesn't delete tabs so this is possibly a memory leak
    m_tabWidget->clear();
    // the WQtToolbar also deletes the buttons!
    m_mainWindow->getCompatiblesToolBar()->clearButtons();

    boost::shared_ptr< WModule > module;
    boost::shared_ptr< WProperties > props;
    boost::shared_ptr< WProperties > infoProps;

    if ( m_moduleTreeWidget->selectedItems().size() != 0  )
    {
        switch ( m_moduleTreeWidget->selectedItems().at( 0 )->type() )
        {
            case SUBJECT:
            case MODULEHEADER:
                // deletion of headers and subjects is not allowed
                m_deleteModuleAction->setEnabled( false );
                createCompatibleButtons( module );  // module is NULL at this point
                break;
            case DATASET:
                module = ( static_cast< WQtDatasetTreeItem* >( m_moduleTreeWidget->selectedItems().at( 0 ) ) )->getModule();
                // crashed modules should not provide any props
                if ( module->isCrashed()() )
                {
                    return;
                }

                // enable the delete action as it might be disabled before.
                m_deleteModuleAction->setEnabled( true );

                props = module->getProperties();
                infoProps = module->getInformationProperties();
                createCompatibleButtons( module );

                {
                    boost::shared_ptr< WMData > dataModule = boost::shared_dynamic_cast< WMData >( module );

                    // if the selected module contains a texture, select the corresponding texture in the texture sorter.
                    if( dataModule )
                    {
                        if( dataModule->getDataSet() )
                        {
                            m_textureSorter->selectTexture( dataModule->getDataSet() );
                        }
                    }
                }

                break;
            case MODULE:
                module = ( static_cast< WQtModuleTreeItem* >( m_moduleTreeWidget->selectedItems().at( 0 ) ) )->getModule();
                // NOTE: this hack prevents the navigation slices to be removed as they are buggy and crash OpenWalnut if they get removed
                if ( module->getName() == "Navigation Slices" )
                {
                    m_deleteModuleAction->setEnabled( false );
                }
                else
                {
                    m_deleteModuleAction->setEnabled( true );
                }

                // crashed modules should not provide any props
                if ( module->isCrashed()() )
                {
                    return;
                }
                props = module->getProperties();
                infoProps = module->getInformationProperties();
                createCompatibleButtons( module );
                break;
            case ROIHEADER:
            case ROIBRANCH:
            case ROI:
                break;
            default:
                break;
        }
    }
    buildPropTab( props, infoProps );
}

void WQtDatasetBrowser::selectRoiTreeItem()
{
    // TODO(schurade): qt doc says clear() doesn't delete tabs so this is possibly a memory leak
    m_tabWidget->clear();
    m_mainWindow->getCompatiblesToolBar()->clearButtons();

    boost::shared_ptr< WModule > module;
    boost::shared_ptr< WProperties > props;

    if ( m_roiTreeWidget->selectedItems().size() != 0  )
    {
        switch ( m_roiTreeWidget->selectedItems().at( 0 )->type() )
        {
            case SUBJECT:
            case DATASET:
            case MODULEHEADER:
            case MODULE:
            case ROIHEADER:
                break;
            case ROIBRANCH:
                props = ( static_cast< WQtBranchTreeItem* >( m_roiTreeWidget->selectedItems().at( 0 ) ) )->getBranch()->getProperties();
                break;
            case ROI:
                props = ( static_cast< WQtRoiTreeItem* >( m_roiTreeWidget->selectedItems().at( 0 ) ) )->getRoi()->getProperties();
                break;
            default:
                break;
        }
    }
    WKernel::getRunningKernel()->getRoiManager()->setSelectedRoi( getFirstRoiInSelectedBranch() );
    buildPropTab( props, boost::shared_ptr< WProperties >() );
}

void WQtDatasetBrowser::selectDataModule( boost::shared_ptr< WDataSet > dataSet )
{
    m_moduleTreeWidget->clearSelection();

    QTreeWidgetItemIterator it( m_moduleTreeWidget );
    while( *it )
    {
        if( dynamic_cast< WQtDatasetTreeItem* >( *it ) )
        {
            boost::shared_ptr< WMData > dataModule;
            dataModule = boost::shared_dynamic_cast< WMData >( ( dynamic_cast< WQtDatasetTreeItem* >( *it ) )->getModule() );
            if( dataModule )
            {
                if( dataModule->getDataSet() == dataSet )
                {
                    ( *it )->setSelected( true );
                }
            }
        }
        ++it;
    }

    selectTreeItem();
}

WQtDSBWidget*  WQtDatasetBrowser::buildPropWidget( boost::shared_ptr< WProperties > props )
{
    WQtDSBWidget* tab = new WQtDSBWidget( props->getName() );

    if ( props.get() )
    {
        WProperties::PropertyAccessType propAccess = props->getAccessObject();
        propAccess->beginRead();

        tab->setName( QString::fromStdString( props->getName() ) );

        // iterate all properties. This Locks the properties set -> use endIteration to unlock
        for ( WProperties::PropertyConstIterator iter = propAccess->get().begin(); iter != propAccess->get().end(); ++iter )
        {
            if ( !( *iter )->isHidden() )
            {
                switch ( ( *iter )->getType() )
                {
                    case PV_BOOL:
                        tab->addProp( ( *iter )->toPropBool() );
                        break;
                    case PV_INT:
                        tab->addProp( ( *iter )->toPropInt() );
                        break;
                    case PV_DOUBLE:
                        tab->addProp( ( *iter )->toPropDouble() );
                        break;
                    case PV_STRING:
                        tab->addProp( ( *iter )->toPropString() );
                        break;
                    case PV_PATH:
                        tab->addProp( ( *iter )->toPropFilename() );
                        break;
                    case PV_SELECTION:
                        tab->addProp( ( *iter )->toPropSelection() );
                        break;
                    case PV_COLOR:
                        tab->addProp( ( *iter )->toPropColor() );
                        break;
                    case PV_POSITION:
                        WLogger::getLogger()->addLogMessage( "This property type \"PV_POSITION\" is not yet supported by the GUI.", "DatasetBrowser",
                                LL_WARNING );
                        break;
                    case PV_TRIGGER:
                        tab->addProp( ( *iter )->toPropTrigger() );
                        break;
                    case PV_GROUP:
                        tab->addGroup( buildPropWidget( ( *iter )->toPropGroup() ) );
                        break;
                    default:
                        WLogger::getLogger()->addLogMessage( "This property type is not yet supported.", "DatasetBrowser", LL_WARNING );
                        break;
                }
            }
        }
        propAccess->endRead();
    }

    tab->addSpacer();
    return tab;
}

void WQtDatasetBrowser::buildPropTab( boost::shared_ptr< WProperties > props, boost::shared_ptr< WProperties > infoProps )
{
    WQtDSBWidget* tab = NULL;
    WQtDSBWidget* infoTab = NULL;
    if ( props )
    {
        tab = buildPropWidget( props );
        tab->setName( "Settings" );
    }
    if ( infoProps )
    {
        infoTab = buildPropWidget( infoProps );
        infoTab->setName( "Information" );
    }

    int infoIdx = addTabWidgetContent( infoTab );
    int propIdx = addTabWidgetContent( tab );

    // select the property widget preferably
    if ( propIdx != -1 )
    {
        m_tabWidget->setCurrentIndex( propIdx );
    }
    else if ( infoIdx != -1 )
    {
        m_tabWidget->setCurrentIndex( infoIdx );
    }
}

void WQtDatasetBrowser::createCompatibleButtons( boost::shared_ptr< WModule >module )
{
    // every module may have compatibles: create ribbon menu entry
    // NOTE: if module is NULL, getCompatiblePrototypes returns the list of modules without input connector (nav slices and so on)
    std::vector< boost::shared_ptr< WApplyPrototypeCombiner > > comps = WModuleFactory::getModuleFactory()->getCompatiblePrototypes( module );

    for ( std::vector< boost::shared_ptr< WApplyPrototypeCombiner > >::const_iterator iter = comps.begin(); iter != comps.end(); ++iter )
    {
        if( !m_moduleWhiteList.empty() )
        {
            const std::string tmpName = ( *iter )->getTargetPrototype()->getName();
            if( std::find( m_moduleWhiteList.begin(), m_moduleWhiteList.end(), tmpName ) == m_moduleWhiteList.end() )
            {
                continue; // do nothing for modules that are not in white list
            }
        }

        WQtApplyModulePushButton* button = new WQtApplyModulePushButton( m_mainWindow->getCompatiblesToolBar(), m_mainWindow->getIconManager(),
                                                                         *iter, m_showToolBarText
        );
        m_mainWindow->getCompatiblesToolBar()->addWidget( button );
    }
}

void WQtDatasetBrowser::changeTreeItem()
{
    if ( m_moduleTreeWidget->selectedItems().size() == 1 && m_moduleTreeWidget->selectedItems().at( 0 )->type() == DATASET )
    {
        boost::shared_ptr< WModule > module =( static_cast< WQtDatasetTreeItem* >( m_moduleTreeWidget->selectedItems().at( 0 ) ) )->getModule();
        module->getProperties()->getProperty( "active" )->toPropBool()->set( m_moduleTreeWidget->selectedItems().at( 0 )->checkState( 0 ) );
    }
    else if ( m_moduleTreeWidget->selectedItems().size() == 1 && m_moduleTreeWidget->selectedItems().at( 0 )->type() == MODULE )
    {
        boost::shared_ptr< WModule > module =( static_cast< WQtModuleTreeItem* >( m_moduleTreeWidget->selectedItems().at( 0 ) ) )->getModule();

        module->getProperties()->getProperty( "active" )->toPropBool()->set( m_moduleTreeWidget->selectedItems().at( 0 )->checkState( 0 ) );
    }
}

void WQtDatasetBrowser::changeRoiTreeItem()
{
    if ( m_roiTreeWidget->selectedItems().size() == 1 && m_roiTreeWidget->selectedItems().at( 0 )->type() == ROI )
    {
        boost::shared_ptr< WRMROIRepresentation > roi =( static_cast< WQtRoiTreeItem* >( m_roiTreeWidget->selectedItems().at( 0 ) ) )->getRoi();
        roi->getProperties()->getProperty( "active" )->toPropBool()->set( m_roiTreeWidget->selectedItems().at( 0 )->checkState( 0 ) );
    }
}

int WQtDatasetBrowser::addTabWidgetContent( WQtDSBWidget* content )
{
    if ( !content || content->isEmpty() )
    {
        return -1;
    }

    QScrollArea* sa = new QScrollArea();
    sa->setWidget( content );
    sa->setWidgetResizable( true );

    return m_tabWidget->addTab( sa, content->getName() );
}

void WQtDatasetBrowser::moveTreeItemDown()
{
    m_moduleTreeWidget->moveTreeItemDown();
}

void WQtDatasetBrowser::moveTreeItemUp()
{
    m_moduleTreeWidget->moveTreeItemUp();
}

int WQtDatasetBrowser::getFirstSubject()
{
    int c = 0;
    for ( int i = 0; i < m_moduleTreeWidget->topLevelItemCount() ; ++i )
    {
        if ( m_moduleTreeWidget->topLevelItem( i )->type() == SUBJECT )
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
    if ( m_roiTreeWidget->selectedItems().count() == 0 )
    {
        return roi;
    }
    if ( m_roiTreeWidget->selectedItems().at( 0 )->type() == ROI )
    {
        roi =( static_cast< WQtRoiTreeItem* >( m_roiTreeWidget->selectedItems().at( 0 ) ) )->getRoi();
    }
    return roi;
}

boost::shared_ptr< WRMROIRepresentation > WQtDatasetBrowser::getFirstRoiInSelectedBranch()
{
    boost::shared_ptr< WRMROIRepresentation >roi;
    if ( m_roiTreeWidget->selectedItems().count() == 0 )
    {
        return roi;
    }
    if ( m_roiTreeWidget->selectedItems().at( 0 )->type() == ROI )
    {
        WQtBranchTreeItem* branch = ( static_cast< WQtBranchTreeItem* >( m_roiTreeWidget->selectedItems().at( 0 )->parent() ) );
        roi =( static_cast< WQtRoiTreeItem* >( branch->child( 0 ) ) )->getRoi();
    }
    if ( m_roiTreeWidget->selectedItems().at( 0 )->type() == ROIBRANCH )
    {
        WQtBranchTreeItem* branch = ( static_cast< WQtBranchTreeItem* >( m_roiTreeWidget->selectedItems().at( 0 ) ) );
        if ( branch->childCount() > 0 )
        {
            roi =( static_cast< WQtRoiTreeItem* >( branch->child( 0 ) ) )->getRoi();
        }
    }
    return roi;
}

void WQtDatasetBrowser::deleteModuleTreeItem()
{
    if ( m_moduleTreeWidget->selectedItems().count() > 0 )
    {
        if ( ( m_moduleTreeWidget->selectedItems().at( 0 )->type() == MODULE ) ||
             ( m_moduleTreeWidget->selectedItems().at( 0 )->type() == DATASET ) )
        {
            // remove from the container. It will create a new event in the GUI after it has been removed which is then handled by the tree item.
            // This method deep removes the module ( it also removes depending modules )
            WKernel::getRunningKernel()->getRootContainer()->removeDeep(
                static_cast< WQtTreeItem* >( m_moduleTreeWidget->selectedItems().at( 0 ) )->getModule()
            );
            // select another item
            m_moduleTreeWidget->setCurrentItem( m_moduleTreeWidget->topLevelItem( 0 ) );
        }
    }
}

void WQtDatasetBrowser::deleteROITreeItem()
{
    boost::shared_ptr< WRMROIRepresentation >roi;
    if ( m_roiTreeWidget->selectedItems().count() > 0 )
    {
        if ( m_roiTreeWidget->selectedItems().at( 0 )->type() == ROIBRANCH )
        {
            roi = getFirstRoiInSelectedBranch();
            if ( roi )
            {
                WKernel::getRunningKernel()->getRoiManager()->removeBranch( roi );
            }
            delete m_roiTreeWidget->selectedItems().at( 0 );
        }

        else if ( m_roiTreeWidget->selectedItems().at( 0 )->type() == ROI )
        {
            roi =( static_cast< WQtRoiTreeItem* >( m_roiTreeWidget->selectedItems().at( 0 ) ) )->getRoi();
            if ( roi )
            {
                WKernel::getRunningKernel()->getRoiManager()->removeRoi( roi );
                // Removing the roi from the tree widget is also done by WROIManagerFibers::removeRoi().
            }
        }
    }
    WKernel::getRunningKernel()->getRoiManager()->setSelectedRoi( getFirstRoiInSelectedBranch() );
}
