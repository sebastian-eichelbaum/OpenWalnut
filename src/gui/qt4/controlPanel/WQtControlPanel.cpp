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
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>

#include <QtCore/QList>
#include <QtGui/QMenu>
#include <QtGui/QScrollArea>
#include <QtGui/QShortcut>

#include "../../../common/WLogger.h"
#include "../../../common/WPreferences.h"
#include "../../../dataHandler/WDataSet.h"
#include "../../../kernel/modules/data/WMData.h"
#include "../../../kernel/WKernel.h"
#include "../../../kernel/WModule.h"
#include "../../../kernel/WModuleFactory.h"
#include "../events/WEventTypes.h"
#include "../events/WModuleAssocEvent.h"
#include "../events/WModuleConnectEvent.h"
#include "../events/WModuleDeleteEvent.h"
#include "../events/WModuleDisconnectEvent.h"
#include "../events/WModuleReadyEvent.h"
#include "../events/WModuleRemovedEvent.h"
#include "../events/WRoiAssocEvent.h"
#include "../events/WRoiRemoveEvent.h"
#include "../WMainWindow.h"
#include "../WQt4Gui.h"
#include "../WQtCombinerActionList.h"
#include "WQtBranchTreeItem.h"
#include "WQtControlPanel.h"
#include "WQtTextureSorter.h"

WQtControlPanel::WQtControlPanel( WMainWindow* parent )
    : QDockWidget( "Control Panel", parent ),
    m_ignoreSelectionChange( false )
{
    m_mainWindow = parent;

    m_panel = new QWidget( this );
    m_moduleTreeWidget = new WQtTreeWidget( m_panel );
    m_moduleTreeWidget->setContextMenuPolicy( Qt::ActionsContextMenu );

    m_moduleTreeWidget->setHeaderLabel( QString( "Module Tree" ) );
    m_moduleTreeWidget->setDragEnabled( false );
    m_moduleTreeWidget->viewport()->setAcceptDrops( true );
    m_moduleTreeWidget->setDropIndicatorShown( true );
    m_moduleTreeWidget->setMinimumHeight( 250 );

    // create context menu for tree items

    // a separator to clean up the tree widget's context menu
    QAction* separator = new QAction( m_moduleTreeWidget );
    separator->setSeparator( true );
    m_moduleTreeWidget->addAction( separator );

    m_connectWithPrototypeAction = new QAction( "Connect with Prototype", m_moduleTreeWidget );
    m_moduleTreeWidget->addAction( m_connectWithPrototypeAction );
    m_connectWithModuleAction = new QAction( "Connect with Module", m_moduleTreeWidget );
    m_moduleTreeWidget->addAction( m_connectWithModuleAction );
    m_disconnectAction = new QAction( "Disconnect", m_moduleTreeWidget );
    m_moduleTreeWidget->addAction( m_disconnectAction );

    // a separator to clean up the tree widget's context menu
    m_moduleTreeWidget->addAction( separator );

    m_deleteModuleAction = new QAction( WQt4Gui::getMainWindow()->getIconManager()->getIcon( "remove" ), "Remove Module", m_moduleTreeWidget );
    m_deleteModuleAction->setShortcut( QKeySequence( Qt::Key_Backspace ) );
    connect( m_deleteModuleAction, SIGNAL( triggered() ), this, SLOT( deleteModuleTreeItem() ) );
    m_moduleTreeWidget->addAction( m_deleteModuleAction );

    m_textureSorter = new WQtTextureSorter( this );
    m_textureSorter->setToolTip( "Reorder the textures." );

    m_tabWidget = new QTabWidget( m_panel );
    m_tabWidget2 = new QTabWidget( m_panel );
    m_tabWidget->setMinimumHeight( 220 );

    // should the Tree, Texture Sorter and the ROI Display be combined in one tab widget?
    bool combineThem = false;
    WPreferences::getPreference( "qt4gui.combineTreeAndRoiAndTextureSorter", &combineThem );

    m_layout = new QVBoxLayout();
    if ( !combineThem )
    {
        m_layout->addWidget( m_moduleTreeWidget );
    }
    else
    {
        m_tabWidget2->addTab( m_moduleTreeWidget, QString( "Modules" ) );
    }
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
    m_tiModules->setText( 0, QString( "Subject-independent Modules" ) );
    m_tiModules->setToolTip( 0, "Subject-independent modules and modules for which no parent module could be detected." );
    m_tiRois = new WQtRoiHeaderTreeItem( m_roiTreeWidget );
    m_tiRois->setText( 0, QString( "ROIs" ) );

    connectSlots();

    QShortcut* shortcut = new QShortcut( QKeySequence( Qt::Key_Delete ), m_roiTreeWidget );
    connect( shortcut, SIGNAL( activated() ), this, SLOT( deleteROITreeItem() ) );
}

WQtControlPanel::~WQtControlPanel()
{
}

void WQtControlPanel::connectSlots()
{
    connect( m_moduleTreeWidget, SIGNAL( itemSelectionChanged() ), this, SLOT( selectTreeItem() ) );
    // connect( m_moduleTreeWidget, SIGNAL( itemClicked( QTreeWidgetItem*, int ) ), this, SLOT( selectTreeItem() ) );
    connect( m_moduleTreeWidget, SIGNAL( itemClicked( QTreeWidgetItem*, int ) ), this, SLOT( changeTreeItem() ) );
    connect( m_moduleTreeWidget, SIGNAL( itemClicked( QTreeWidgetItem*, int ) ),  m_roiTreeWidget, SLOT( clearSelection() ) );
    //connect( m_roiTreeWidget, SIGNAL( itemSelectionChanged() ), this, SLOT( selectRoiTreeItem() ) );
    connect( m_roiTreeWidget, SIGNAL( itemClicked( QTreeWidgetItem*, int ) ), this, SLOT( selectRoiTreeItem() ) );
    connect( m_roiTreeWidget, SIGNAL( itemClicked( QTreeWidgetItem*, int ) ), this, SLOT( changeRoiTreeItem() ) );
    connect( m_roiTreeWidget, SIGNAL( itemClicked( QTreeWidgetItem*, int ) ), m_moduleTreeWidget, SLOT( clearSelection() ) );

    connect( m_textureSorter, SIGNAL( textureSelectionChanged( boost::shared_ptr< WDataSet > ) ),
             this, SLOT( selectDataModule( boost::shared_ptr< WDataSet > ) ) );
}

void WQtControlPanel::addToolbar( QToolBar* tb )
{
    m_layout->insertWidget( 0, tb );
}

WQtSubjectTreeItem* WQtControlPanel::addSubject( std::string name )
{
    WQtSubjectTreeItem* subject = new WQtSubjectTreeItem( m_moduleTreeWidget );
    subject->setText( 0, QString::fromStdString( name ) );
    subject->setToolTip( 0, QString::fromStdString( "All data and modules that are children of this tree item belong to the subject \"" +
                name + "\"." ) );

    return subject;
}

bool WQtControlPanel::event( QEvent* event )
{
    // a subject singals a newly registered data set
    if ( event->type() == WQT_UPDATE_TEXTURE_SORTER_EVENT )
    {
        m_textureSorter->update();
    }

    if ( event->type() == WQT_ROI_ASSOC_EVENT )
    {
        WRoiAssocEvent* e2 = dynamic_cast< WRoiAssocEvent* >( event );     // NOLINT
        if ( e2 )
        {
            addRoi( e2->getRoi() );
            WLogger::getLogger()->addLogMessage( "Inserting ROI to control panel.", "ControlPanel", LL_DEBUG );
        }

        return true;
    }
    if( event->type() == WQT_ROI_REMOVE_EVENT )
    {
        WRoiRemoveEvent* e3 = dynamic_cast< WRoiRemoveEvent* >( event );
        if( e3 )
        {
            removeRoi( e3->getRoi() );
            WLogger::getLogger()->addLogMessage( "Removing ROI from control panel.", "ControlPanel", LL_DEBUG );
        }

        return true;
    }

    // a module got associated with the root container -> add it to the list
    if ( event->type() == WQT_ASSOC_EVENT )
    {
        // convert event to assoc event
        WModuleAssocEvent* e1 = dynamic_cast< WModuleAssocEvent* >( event );     // NOLINT
        if ( e1 )
        {
            WLogger::getLogger()->addLogMessage( "Inserting module " + e1->getModule()->getName() + " to control panel.",
                                                 "ControlPanel", LL_DEBUG );

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

    // a module changed its state to "ready" -> activate it in control panel
    if ( event->type() == WQT_READY_EVENT )
    {
        // convert event to assoc event
        WModuleReadyEvent* e = dynamic_cast< WModuleReadyEvent* >( event );     // NOLINT
        if ( !e )
        {
            // this should never happen, since the type is set to WQT_READY_EVENT.
            WLogger::getLogger()->addLogMessage( "Event is not an WModueReadyEvent although its type claims it. Ignoring event.",
                                                 "ControlPanel", LL_WARNING );

            return true;
        }

        WLogger::getLogger()->addLogMessage( "Activating module " + e->getModule()->getName() + " in control panel.",
                                             "ControlPanel", LL_DEBUG );

        // search all the item matching the module
        std::list< WQtTreeItem* > items = findItemsByModule( e->getModule() );
        for ( std::list< WQtTreeItem* >::const_iterator iter = items.begin(); iter != items.end(); ++iter )
        {
            ( *iter )->setSelected( true );
            ( *iter )->setDisabled( false );
        }

        selectTreeItem();

        return true;
    }

    // a module tree item was connected to another one
    if ( event->type() == WQT_MODULE_CONNECT_EVENT )
    {
        WModuleConnectEvent* e = dynamic_cast< WModuleConnectEvent* >( event );     // NOLINT
        if ( !e )
        {
            // this should never happen, since the type is set to WQT_MODULE_CONNECT_EVENT.
            WLogger::getLogger()->addLogMessage( "Event is not an WModuleConnectEvent although its type claims it. Ignoring event.",
                                                 "ControlPanel", LL_WARNING );
            return true;
        }

        // get the module of the input involved in this connection
        boost::shared_ptr< WModule > mIn = e->getInput()->getModule();
        boost::shared_ptr< WModule > mOut = e->getOutput()->getModule();

        // NOTE: the following is ugly. We definitely should rethink our GUI

        // at this moment items for each input connector are inside the tree.
        // search the items not yet associated with some other module for the first item
        std::list< WQtTreeItem* > items = findItemsByModule( mIn, m_tiModules );
        for ( std::list< WQtTreeItem* >::const_iterator iter = items.begin(); iter != items.end(); ++iter )
        {
            ( *iter )->setHidden( false );
            ( *iter )->setHandledInput( e->getInput()->getName() );
            ( *iter )->setHandledOutput( e->getOutput()->getName() );

            // move it to the module with the involved output
            std::list< WQtTreeItem* > possibleParents = findItemsByModule( mOut );
            for ( std::list< WQtTreeItem* >::const_iterator parIter = possibleParents.begin(); parIter != possibleParents.end(); ++parIter )
            {
                // remove child from tiModules
                m_tiModules->removeChild( *iter );
                ( *parIter )->addChild( *iter );
                ( *parIter )->setExpanded( true );
            }

            // job done.
            break;
        }
    }

    // a module tree item was disconnected from another one
    if ( event->type() == WQT_MODULE_DISCONNECT_EVENT )
    {
        WModuleDisconnectEvent* e = dynamic_cast< WModuleDisconnectEvent* >( event );     // NOLINT
        if ( !e )
        {
            // this should never happen, since the type is set to WQT_MODULE_DISCONNECT_EVENT.
            WLogger::getLogger()->addLogMessage( "Event is not an WModuleDisconnectEvent although its type claims it. Ignoring event.",
                                                 "ControlPanel", LL_WARNING );
            return true;
        }

        // get the module of the input involved in this connection
        boost::shared_ptr< WModule > mIn = e->getInput()->getModule();
        boost::shared_ptr< WModule > mOut = e->getOutput()->getModule();

        // NOTE: the following is ugly. We definitely should rethink our GUI

        // at this moment items for each input connector are inside the tree.
        // search all items an find those containing a children which belongs to the connection input
        std::list< WQtTreeItem* > items = findItemsByModule( mOut );
        for ( std::list< WQtTreeItem* >::const_iterator iter = items.begin(); iter != items.end(); ++iter )
        {
            // each of them can contain a child with the involved input
            std::list< WQtTreeItem* > childs = findItemsByModule( mIn, *iter );
            for ( std::list< WQtTreeItem* >::const_iterator citer = childs.begin(); citer != childs.end(); ++citer )
            {
                if ( ( *citer )->getHandledInput() == e->getInput()->getName() )
                {
                    ( *iter )->removeChild( *citer );

                    // move it back to the reservoir in m_tiModules
                    m_tiModules->addChild( *citer );
                    ( *citer )->setHidden( true );
                    ( *citer )->setHandledInput( "" );
                    ( *citer )->setHandledOutput( "" );
                }
            }
        }

        // we need to ensure that at least one item for mIn is visible somewhere
        items = findItemsByModule( mIn );
        bool oneVisible = false;
        for ( std::list< WQtTreeItem* >::const_iterator iter = items.begin(); iter != items.end(); ++iter )
        {
            oneVisible = oneVisible || !( *iter )->isHidden();
        }
        if ( !oneVisible )
        {
            ( *items.begin() )->setHidden( false );
        }
    }

    // a module tree item should be deleted
    if ( event->type() == WQT_MODULE_DELETE_EVENT )
    {
        WModuleDeleteEvent* e = dynamic_cast< WModuleDeleteEvent* >( event );
        if ( !e )
        {
            // this should never happen, since the type is set to WQT_MODULE_DELETE_EVENT.
            WLogger::getLogger()->addLogMessage( "Event is not an WModuleDeleteEvent although its type claims it. Ignoring event.",
                                                 "ControlPanel", LL_WARNING );
            return true;
        }

        // grab the module reference and print some info
        boost::shared_ptr< WModule > module = e->getTreeItem()->getModule();
        WLogger::getLogger()->addLogMessage( "Deleting module \"" + module->getName() + "\" from Tree.",
                                             "ControlPanel", LL_DEBUG );

        // remove it from the tree and free last ref count
        m_moduleTreeWidget->deleteItem( e->getTreeItem() );

        // ref count != 1? (only if there are now tree items left)
        bool lastTreeItem = !findItemsByModule( module ).size();
        if ( lastTreeItem && ( module.use_count() != 1 ) )
        {
            wlog::warn( "ControlPanel" ) << "Removed module has strange usage count: " << module.use_count() << ". Should be 1 here. " <<
                                              "Module reference is held by someone else.";
        }

        return true;
    }

    // a module was removed from the container
    if ( event->type() == WQT_MODULE_REMOVE_EVENT )
    {
        WModuleRemovedEvent* e = dynamic_cast< WModuleRemovedEvent* >( event );
        if ( !e )
        {
            // this should never happen, since the type is set to WQT_MODULE_REMOVE_EVENT.
            WLogger::getLogger()->addLogMessage( "Event is not an WModuleRemovedEvent although its type claims it. Ignoring event.",
                                                 "ControlPanel", LL_WARNING );
            return true;
        }

        // iterate tree items and find proper one
        std::list< WQtTreeItem* > items = findItemsByModule( e->getModule() );
        for ( std::list< WQtTreeItem* >::const_iterator iter = items.begin(); iter != items.end(); ++iter )
        {
            ( *iter )->gotRemoved();
        }

        // be nice and print some info
        WLogger::getLogger()->addLogMessage( "Removing module \"" + e->getModule()->getName() + "\" from Tree.", "ControlPanel", LL_DEBUG );

        // stop the module
        e->getModule()->requestStop();
        WLogger::getLogger()->addLogMessage( "Waiting for module \"" + e->getModule()->getName() + "\" to finish before deleting.",
                                             "ControlPanel", LL_DEBUG );

        return true;
    }

    return QDockWidget::event( event );
}

std::list< WQtTreeItem* > WQtControlPanel::findItemsByModule( boost::shared_ptr< WModule > module, QTreeWidgetItem* where )
{
    std::list< WQtTreeItem* > l;

    // iterate tree items and find proper one
    QTreeWidgetItemIterator it( where );
    while ( *it )
    {
        WQtTreeItem* item = dynamic_cast< WQtTreeItem* >( *it );
        boost::shared_ptr< WModule > itemModule = boost::shared_ptr< WModule >();
        if ( item )
        {
            itemModule = item->getModule();
        }

        // if the pointer is NULL the item was none of the above
        if ( !itemModule.get() )
        {
            ++it;
            continue;
        }

        // we found it
        if ( module == itemModule )
        {
            l.push_back( item );
        }

        ++it;
    }
    return l;
}

std::list< WQtTreeItem* > WQtControlPanel::findItemsByModule( boost::shared_ptr< WModule > module )
{
    return findItemsByModule( module, m_moduleTreeWidget->invisibleRootItem() );
}

WQtDatasetTreeItem* WQtControlPanel::addDataset( boost::shared_ptr< WModule > module, int subjectId )
{
    int c = getFirstSubject();
    WQtSubjectTreeItem* subject = static_cast< WQtSubjectTreeItem* >( m_moduleTreeWidget->topLevelItem( subjectId + c ) );
    subject->setExpanded( true );
    WQtDatasetTreeItem* item = subject->addDatasetItem( module );
    m_moduleTreeWidget->setCurrentItem( item );
    item->setDisabled( true );

    return item;
}

WQtModuleTreeItem* WQtControlPanel::addModule( boost::shared_ptr< WModule > module )
{
    m_tiModules->setExpanded( true );
    WQtModuleTreeItem* item;
    // for each input, create an item
    m_moduleTreeWidget->setCurrentItem( NULL );
    bool firstItem = true;
    WModule::InputConnectorList cons = module->getInputConnectors();
    for ( WModule::InputConnectorList::const_iterator iter = cons.begin(); iter != cons.end(); ++iter )
    {
        // every module gets added to tiModules first. The connection events then move these things to the right parent
        item = m_tiModules->addModuleItem( module );
        item->setDisabled( true );
        item->setExpanded( true );

        // all but the first item get hidden by default. They get visible after a connection event has been fired
        if ( !firstItem )
        {
            item->setHidden( true );
        }

        firstItem = false;
    }

    // this module has not inputs. So we simply add it to the tiModules
    if ( firstItem )
    {
        item = m_tiModules->addModuleItem( module );
        item->setDisabled( true );
    }

    return item;
}

void WQtControlPanel::addRoi( osg::ref_ptr< WROI > roi )
{
    WQtRoiTreeItem* newItem;
    WQtBranchTreeItem* branchItem;

    m_tiRois->setExpanded( true );
    bool found = false;

    // go through all branches
    for( int branchID = 0; branchID < m_tiRois->childCount(); ++branchID )
    {
        branchItem = dynamic_cast< WQtBranchTreeItem* >( m_tiRois->child( branchID ) );
        // if branch == roi branch
        if ( branchItem->getBranch() == WKernel::getRunningKernel()->getRoiManager()->getBranch( roi ) )
        {
            found = true;
            break;
        }
    }

    if ( !found )
    {
        branchItem = m_tiRois->addBranch( WKernel::getRunningKernel()->getRoiManager()->getBranch( roi ) );
    }

    m_tabWidget2->setCurrentIndex( m_tabWidget2->indexOf( m_roiTreeWidget ) );
    branchItem->setExpanded( true );
    newItem = branchItem->addRoiItem( roi );
    newItem->setDisabled( false );
    newItem->setSelected( true );
    WKernel::getRunningKernel()->getRoiManager()->setSelectedRoi( getSelectedRoi() );
}

void WQtControlPanel::removeRoi( osg::ref_ptr< WROI > roi )
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
    WKernel::getRunningKernel()->getRoiManager()->setSelectedRoi( getSelectedRoi() );
}

boost::shared_ptr< WModule > WQtControlPanel::getSelectedModule()
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

void WQtControlPanel::selectTreeItem()
{
    if ( m_ignoreSelectionChange )
    {
        return;
    }

    // TODO(schurade): qt doc says clear() doesn't delete tabs so this is possibly a memory leak
    m_tabWidget->clear();

    boost::shared_ptr< WModule > module;
    boost::shared_ptr< WProperties > props;
    boost::shared_ptr< WProperties > infoProps;

    WQtCombinerToolbar* newToolbar = NULL;

    if ( m_moduleTreeWidget->selectedItems().size() != 0  )
    {
        // disable delete action for tree items that have children.
        if( m_moduleTreeWidget->selectedItems().at( 0 )->childCount() != 0 )
        {
            m_deleteModuleAction->setEnabled( false );
        }
        else
        {
            m_deleteModuleAction->setEnabled( true );
        }

        switch ( m_moduleTreeWidget->selectedItems().at( 0 )->type() )
        {
            case SUBJECT:
            case MODULEHEADER:
                // deletion of headers and subjects is not allowed
                m_deleteModuleAction->setEnabled( false );
                newToolbar = createCompatibleButtons( module );  // module is NULL at this point
                break;
            case DATASET:
                module = ( static_cast< WQtDatasetTreeItem* >( m_moduleTreeWidget->selectedItems().at( 0 ) ) )->getModule();
                // crashed modules should not provide any props
                if ( module->isCrashed()() )
                {
                    return;
                }

                props = module->getProperties();
                infoProps = module->getInformationProperties();
                newToolbar = createCompatibleButtons( module );

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
                {
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

                    // this is ugly since it causes the property tab to update multiple times if multiple items get selected by this call
                    // but it highlights all the same items which is nice and wanted here
                    // Set the ignore flag to avoid that this method gets called several times
                    m_ignoreSelectionChange = true;
                    std::list< WQtTreeItem* > items = findItemsByModule( module );
                    for ( std::list< WQtTreeItem* >::const_iterator iter = items.begin(); iter != items.end(); ++iter )
                    {
                        ( *iter )->setSelected( true );
                    }
                    m_ignoreSelectionChange = false;

                    // crashed modules should not provide any props
                    if ( module->isCrashed()() )
                    {
                        return;
                    }
                    props = module->getProperties();
                    infoProps = module->getInformationProperties();
                    newToolbar = createCompatibleButtons( module );
                }
                break;
            case ROIHEADER:
            case ROIBRANCH:
            case ROI:
                break;
            default:
                break;
        }
    }

    // set the new toolbar
    // NOTE: setCompatiblesToolbar removes the toolbar if NULL is specified.
    m_mainWindow->setCompatiblesToolbar( newToolbar );

    buildPropTab( props, infoProps );
}

void WQtControlPanel::selectRoiTreeItem()
{
    // TODO(schurade): qt doc says clear() doesn't delete tabs so this is possibly a memory leak
    m_tabWidget->clear();
    m_mainWindow->setCompatiblesToolbar();

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
                WKernel::getRunningKernel()->getRoiManager()->setSelectedRoi( getSelectedRoi() );
                break;
            case ROIBRANCH:
                props = ( static_cast< WQtBranchTreeItem* >( m_roiTreeWidget->selectedItems().at( 0 ) ) )->getBranch()->getProperties();
                WKernel::getRunningKernel()->getRoiManager()->setSelectedRoi( getFirstRoiInSelectedBranch() );
                break;
            case ROI:
                props = ( static_cast< WQtRoiTreeItem* >( m_roiTreeWidget->selectedItems().at( 0 ) ) )->getRoi()->getProperties();
                WKernel::getRunningKernel()->getRoiManager()->setSelectedRoi( getSelectedRoi() );
                break;
            default:
                break;
        }
    }
    buildPropTab( props, boost::shared_ptr< WProperties >() );
}

void WQtControlPanel::selectDataModule( boost::shared_ptr< WDataSet > dataSet )
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

WQtPropertyGroupWidget*  WQtControlPanel::buildPropWidget( boost::shared_ptr< WProperties > props )
{
    WQtPropertyGroupWidget* tab = new WQtPropertyGroupWidget( props->getName() );

    if ( props.get() )
    {
        // read lock, gets unlocked upon destruction (out of scope)
        WProperties::PropertySharedContainerType::ReadTicket propAccess = props->getProperties();

        tab->setName( QString::fromStdString( props->getName() ) );

        // iterate all properties.
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
                        tab->addProp( ( *iter )->toPropPosition() );
                        break;
                    case PV_TRIGGER:
                        tab->addProp( ( *iter )->toPropTrigger() );
                        break;
                    case PV_GROUP:
                        tab->addGroup( buildPropWidget( ( *iter )->toPropGroup() ) );
                        break;
                    default:
                        WLogger::getLogger()->addLogMessage( "This property type is not yet supported.", "ControlPanel", LL_WARNING );
                        break;
                }
            }
        }
    }

    tab->addSpacer();
    return tab;
}

void WQtControlPanel::buildPropTab( boost::shared_ptr< WProperties > props, boost::shared_ptr< WProperties > infoProps )
{
    WQtPropertyGroupWidget* tab = NULL;
    WQtPropertyGroupWidget* infoTab = NULL;
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

WQtCombinerToolbar* WQtControlPanel::createCompatibleButtons( boost::shared_ptr< WModule >module )
{
    // every module may have compatibles: create ribbon menu entry
    // NOTE: if module is NULL, getCompatiblePrototypes returns the list of modules without input connector (nav slices and so on)
    WCombinerTypes::WCompatiblesList comps = WModuleFactory::getModuleFactory()->getCompatiblePrototypes( module );

    // build the prototype menu
    QMenu* m = new QMenu( m_moduleTreeWidget );
    m->addActions( WQtCombinerActionList( m, m_mainWindow->getIconManager(), comps ) );
    m_connectWithPrototypeAction->setDisabled( !comps.size() );  // disable if no entry inside
    delete( m_connectWithPrototypeAction->menu() ); // ensure that combiners get free'd
    m_connectWithPrototypeAction->setMenu( m );

    // build the module menu
    WCombinerTypes::WCompatiblesList containerComps = WKernel::getRunningKernel()->getRootContainer()->getPossibleConnections( module );
    m = new QMenu( m_moduleTreeWidget );
    m->addActions( WQtCombinerActionList( m, m_mainWindow->getIconManager(), containerComps, true, true ) );
    m_connectWithModuleAction->setDisabled( !containerComps.size() );  // disable if no entry inside
    delete m_connectWithModuleAction->menu();
    m_connectWithModuleAction->setMenu( m );

    // build the disconnect menu
    WCombinerTypes::WDisconnectList disconnects;
    if ( module )
    {
       disconnects = module->getPossibleDisconnections();
    }
    m = new QMenu( m_moduleTreeWidget );
    m->addActions(  WQtCombinerActionList( m, m_mainWindow->getIconManager(), disconnects ) );
    m_disconnectAction->setDisabled( !disconnects.size() );  // disable if no entry inside
    delete( m_disconnectAction->menu() ); // ensure that combiners get free'd
    m_disconnectAction->setMenu( m );

    return new WQtCombinerToolbar( m_mainWindow, comps );
}

void WQtControlPanel::changeTreeItem()
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

void WQtControlPanel::changeRoiTreeItem()
{
    if ( m_roiTreeWidget->selectedItems().size() == 1 && m_roiTreeWidget->selectedItems().at( 0 )->type() == ROI )
    {
        osg::ref_ptr< WROI > roi = ( static_cast< WQtRoiTreeItem* >( m_roiTreeWidget->selectedItems().at( 0 ) ) )->getRoi();
        roi->getProperties()->getProperty( "active" )->toPropBool()->set( m_roiTreeWidget->selectedItems().at( 0 )->checkState( 0 ) );
    }
}

int WQtControlPanel::addTabWidgetContent( WQtPropertyGroupWidget* content )
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

int WQtControlPanel::getFirstSubject()
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

osg::ref_ptr< WROI > WQtControlPanel::getSelectedRoi()
{
    osg::ref_ptr< WROI > roi;
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

osg::ref_ptr< WROI > WQtControlPanel::getFirstRoiInSelectedBranch()
{
    osg::ref_ptr< WROI >roi;
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

QAction* WQtControlPanel::toggleViewAction() const
{
    QAction* result = QDockWidget::toggleViewAction();
    QList< QKeySequence > shortcut;
#if defined( __APPLE__ )
    shortcut.append( QKeySequence( Qt::CTRL + Qt::Key_F9 ) ); // In Mac OS X F9 is already taken by window managment
#else
    shortcut.append( QKeySequence( Qt::Key_F9 ) );
#endif
    result->setShortcuts( shortcut );
    return result;
}

void WQtControlPanel::deleteModuleTreeItem()
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

void WQtControlPanel::deleteROITreeItem()
{
    osg::ref_ptr< WROI >roi;
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

void WQtControlPanel::selectUpperMostEntry()
{
    m_tiModules->setSelected( true );
}
