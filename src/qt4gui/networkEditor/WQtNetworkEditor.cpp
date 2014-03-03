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
#include <cstdlib>
#include <iostream>

#include <boost/shared_ptr.hpp>

#include <QtGui/QKeyEvent>
#include <QtGui/QGraphicsView>
#include <QtGui/QGraphicsItem>
#include <QtGui/QGraphicsItemGroup>

#include "../WMainWindow.h"
#include "WQtNetworkPort.h"
#include "WQtNetworkItemGrid.h"

#include "core/kernel/combiner/WDisconnectCombiner.h"
#include "core/kernel/WKernel.h"
#include "core/kernel/WModule.h"
#include "core/kernel/WModuleFactory.h"
#include "core/kernel/WProjectFile.h"

#include "../controlPanel/WQtControlPanel.h"
#include "../events/WEventTypes.h"
#include "../events/WModuleAssocEvent.h"
#include "../events/WModuleConnectEvent.h"
#include "../events/WModuleDeleteEvent.h"
#include "../events/WModuleDisconnectEvent.h"
#include "../events/WModuleReadyEvent.h"
#include "../events/WModuleRemovedEvent.h"
#include "../events/WModuleCrashEvent.h"

#include "WQtNetworkEditorProjectFileIO.h"

#include "WQtNetworkEditor.h"
#include "WQtNetworkEditor.moc"

WQtNetworkEditor::WQtNetworkEditor( WMainWindow* parent ):
    WQtDockWidget( "Modules", parent )
{
    // register network editor project IO parser
    WProjectFile::registerParser( WProjectFileIO::SPtr( new WQtNetworkEditorProjectFileIO( this ) ) );

    setObjectName( "Module Graph Dock" );
    m_mainWindow = parent;

    setAcceptDrops( true ); // enable drag and drop events

    m_view = new WQtNetworkEditorView( this );
    m_view->setMinimumSize( 20, 20 );
    this->setFocusProxy( m_view );

    m_scene = new WQtNetworkScene( this );

    // our virtual grid used to place items
    m_grid = new WQtNetworkItemGrid;

    m_scene->addItem( m_grid );

    // QGraphicsScene provides no virtual methods for adding/removing items, thus we use a separate layouter instead of implementing this in
    // WQtNetworkScene
    m_layout = new WQtNetworkSceneLayout( m_scene, m_grid );

    // update view rect whenever the grid updates its dimensions
    connect( m_grid, SIGNAL( updatedBounds() ), this, SLOT( updateSceneRect() ) );

    m_view->setScene( m_scene );

    setWidget( m_view );
    connect( m_scene, SIGNAL( selectionChanged() ), this, SLOT( selectItem() ) );
    connect( m_view, SIGNAL( loadAction() ), m_mainWindow, SLOT( openLoadDialog() ) );

    // as the QGraphicsItems are not derived from QObject, they cannot utilize the event system. We need to provide some possibility to update
    // them regularly. We use a timer here.
    QTimer* updater = new QTimer( this );
    updater->start( 100 );
    connect( updater, SIGNAL( timeout() ), this, SLOT( updateCylce() ) );
}

WQtNetworkEditor::~WQtNetworkEditor()
{
}

QList<QGraphicsItem *> WQtNetworkEditor::selectedItems() const
{
    return m_scene->selectedItems();
}

void WQtNetworkEditor::clearSelection()
{
    m_scene->clearSelection();
}

void WQtNetworkEditor::selectByModule( WModule::SPtr module )
{
    WQtNetworkItem* item = findItemByModule( module );
    if( item && !item->isSelected() )
    {
        m_scene->clearSelection();
        item->setSelected( true );
    }
}

void WQtNetworkEditor::selectItem()
{
    boost::shared_ptr< WModule > module;
    if( m_scene->selectedItems().size() != 0 &&
         m_scene->selectedItems().at( 0 )->type() == WQtNetworkItem::Type )
    {
        if( m_scene->selectedItems().at(0)->type() == WQtNetworkItem::Type )
        {
            module = ( static_cast< WQtNetworkItem* >( m_scene->selectedItems().at( 0 ) ) )->getModule();
            m_mainWindow->getControlPanel()->setActiveModule( module );
            return;
        }
    }

    m_mainWindow->getControlPanel()->deactivateModuleSelection();
}

void WQtNetworkEditor::updateCylce()
{
    for( QList< WQtNetworkItem* >::const_iterator i = m_items.begin(); i != m_items.end(); ++i )
    {
        ( *i )->updater();
    }
}

bool WQtNetworkEditor::event( QEvent* event )
{
    // list to store the connectors temporarily
    QList< WQtNetworkInputPort* > tmpIn;
    QList< WQtNetworkOutputPort* > tmpOut;

    // a module got associated with the root container -> add it to the list
    if( event->type() == WQT_ASSOC_EVENT )
    {
        // convert event to assoc event
        WModuleAssocEvent* e1 = dynamic_cast< WModuleAssocEvent* >( event );     // NOLINT
        if( e1 )
        {
            WLogger::getLogger()->addLogMessage( "Inserting \"" + e1->getModule()->getName() + "\".",
                                                 "NetworkEditor", LL_DEBUG );
            WQtNetworkItem *item = new WQtNetworkItem( this, e1->getModule() );
            m_items.push_back( item );
            m_scene->addItem( item );
            m_layout->addItem( item );

            // This is needed here for some reason. You know how to make this better? Tell us.
            // REASON: although the WQtNetworkItemGrid updates its bounding box, the ensureVisible call does not move to the new item.
            // Also calling the next method directly from the grid causes seg-faults when leaving the widget area while dragging the item.
            m_scene->setSceneRect( m_layout->getBoundingBox() );

            // make visible
            QList< QGraphicsView* > allViews = m_scene->views();
            foreach( QGraphicsView* v, allViews )
            {
                WQtNetworkEditorView* nv = dynamic_cast< WQtNetworkEditorView* >( v );
                if( nv )
                {
                    nv->focusOn( item );
                }
                else
                {
                    v->ensureVisible( item );
                }
            }
        }
        return true;
    }

    // a module changed its state to "ready" -> activate it in dataset browser
    if( event->type() == WQT_READY_EVENT )
    {
        // convert event to ready event
        WModuleReadyEvent* e = dynamic_cast< WModuleReadyEvent* >( event );     // NOLINT
        if( !e )
        {
            // this should never happen, since the type is set to WQT_READY_EVENT.
            WLogger::getLogger()->addLogMessage( "Event is not an WModueReadyEvent although its type claims it. Ignoring event.",
                                                 "NetworkEditor", LL_WARNING );

            return true;
        }

        WLogger::getLogger()->addLogMessage( "Activating \"" + e->getModule()->getName() + "\".",
                                             "NetworkEditor", LL_DEBUG );

        // search all the item matching the module
        WQtNetworkItem *item = findItemByModule( e->getModule() );
        if( item != 0 )
        {
            item->activate( true );
        }

        return true;
    }

    // a module tree item was connected to another one
    if( event->type() == WQT_MODULE_CONNECT_EVENT )
    {
        WModuleConnectEvent* e = dynamic_cast< WModuleConnectEvent* >( event );     // NOLINT
        if( !e )
        {
            // this should never happen, since the type is set to WQT_MODULE_CONNECT_EVENT.
            WLogger::getLogger()->addLogMessage( "Event is not an WModuleConnectEvent although its type claims it. Ignoring event.",
                                                 "NetworkEditor", LL_WARNING );
            return true;
        }

        WLogger::getLogger()->addLogMessage( "Connecting \"" + e->getInput()->getModule()->getName() +
                "\" and \"" + e->getOutput()->getModule()->getName() + "\".", "NetworkEditor", LL_DEBUG );

        boost::shared_ptr< WModule > mIn;
        boost::shared_ptr< WModule > mOut;

        if( e->getInput()->isInputConnector() == true &&
            e->getOutput()->isOutputConnector() == true )
        {
            mIn = e->getInput()->getModule();
            mOut = e->getOutput()->getModule();
        }
        else if( e->getInput()->isOutputConnector() == true &&
                    e->getOutput()->isInputConnector() == true )
        {
            mIn = e->getOutput()->getModule();
            mOut = e->getInput()->getModule();
        }
        else
        {
            return true;
        }

        WQtNetworkItem *inItem = findItemByModule( mIn );
        WQtNetworkItem *outItem = findItemByModule( mOut );

        WQtNetworkInputPort *ip = NULL;
        WQtNetworkOutputPort *op = NULL;

        tmpIn = inItem->getInPorts();
        for( QList< WQtNetworkInputPort* >::const_iterator iter = tmpIn.begin();
                iter != tmpIn.end();
                ++iter )
        {
            WQtNetworkInputPort* inP = *iter;
            if( e->getInput() == inP->getConnector() )
            {
                ip = inP;
            }
        }

        tmpOut = outItem->getOutPorts();
        for( QList< WQtNetworkOutputPort* >::const_iterator iter = tmpOut.begin();
                iter != tmpOut.end();
                ++iter )
        {
            WQtNetworkOutputPort* outP = *iter;
            if( e->getOutput() == outP->getConnector() )
            {
                op = outP;
            }
        }

        if( ip != NULL &&
            op != NULL )
        {
            WQtNetworkArrow *arrow = new WQtNetworkArrow( op, ip );

            arrow->setZValue( -1000.0 );
            op->addArrow( arrow );
            ip->addArrow( arrow );
            arrow->updatePosition();

            m_scene->addItem( arrow );

            // also update the layouter
            m_layout->connection( outItem, inItem );
        }
    }

    // a module tree item was disconnected from another one
    if( event->type() == WQT_MODULE_DISCONNECT_EVENT )
    {
        WModuleDisconnectEvent* e = dynamic_cast< WModuleDisconnectEvent* >( event );     // NOLINT
        if( !e )
        {
            // this should never happen, since the type is set to WQT_MODULE_DISCONNECT_EVENT.
            WLogger::getLogger()->addLogMessage( "Event is not an WModuleDisconnectEvent although its type claims it. Ignoring event.",
                                                 "NetworkEditor", LL_WARNING );
            return true;
        }

        WLogger::getLogger()->addLogMessage( "Disconnecting \"" + e->getInput()->getCanonicalName() +
                                             "\" and \"" + e->getOutput()->getCanonicalName() +
                                             "\"." , "NetworkEditor", LL_DEBUG );

        boost::shared_ptr< WModule > mIn;
        boost::shared_ptr< WModule > mOut;

        if( e->getInput()->isInputConnector() == true &&
            e->getOutput()->isOutputConnector() == true )
        {
            mIn = e->getInput()->getModule();
            mOut = e->getOutput()->getModule();
        }
        else if( e->getInput()->isOutputConnector() == true &&
                    e->getOutput()->isInputConnector() == true )
        {
            mIn = e->getOutput()->getModule();
            mOut = e->getInput()->getModule();
        }
        else
        {
            return true;
        }


        WQtNetworkItem *inItem = findItemByModule( mIn );
        WQtNetworkItem *outItem = findItemByModule( mOut );

        WQtNetworkInputPort *ip = NULL;
        WQtNetworkOutputPort *op = NULL;

        tmpIn = inItem->getInPorts();
        for( QList< WQtNetworkInputPort* >::const_iterator iter = tmpIn.begin();
            iter != tmpIn.end();
            ++iter )
        {
           WQtNetworkInputPort *inP = *iter;
           if( e->getInput() == inP->getConnector() )
           {
               ip = inP;
           }
        }
        tmpOut = outItem->getOutPorts();
        for( QList< WQtNetworkOutputPort* >::const_iterator iter = tmpOut.begin();
                iter != tmpOut.end();
                ++iter )
        {
            WQtNetworkOutputPort *outP = *iter;
            if( e->getOutput() == outP->getConnector() )
            {
                op = outP;
            }
        }

        WQtNetworkArrow *ar = NULL;

        QList< QGraphicsItem* > tmpItems = m_scene->items();
        for( QList< QGraphicsItem* >::const_iterator iter = tmpItems.begin();
                iter != tmpItems.end();
                ++iter )
        {
            ar = dynamic_cast< WQtNetworkArrow* >( *iter );
            if( ar &&
                ar->getStartPort() == op &&
                ar->getEndPort() == ip )
            {
                break;
            }
        }
        if( ar )
        {
            op->removeArrow( ar );
            ip->removeArrow( ar );
            if( ar->scene() != NULL )
            {
                m_scene->removeItem( ar );
            }
            delete ar;
        }
        else
        {
            WLogger::getLogger()->addLogMessage( "Arrow not found!.", "NetworkEditor", LL_ERROR );
        }

        // update layouter
        m_layout->disconnection( outItem, inItem );

        return true;
    }

    // a module was removed from the container
    if( event->type() == WQT_MODULE_REMOVE_EVENT )
    {
        WModuleRemovedEvent* e = dynamic_cast< WModuleRemovedEvent* >( event );
        if( !e )
        {
            // this should never happen, since the type is set to WQT_MODULE_REMOVE_EVENT.
            WLogger::getLogger()->addLogMessage( "Event is not an WModuleRemovedEvent although its type claims it. Ignoring event.",
                                                 "NetworkEditor", LL_WARNING );
            return true;
        }

        WLogger::getLogger()->addLogMessage( "Removing \"" + e->getModule()->getName() + "\".",
                                             "NetworkEditor", LL_DEBUG );


        WQtNetworkItem *item = findItemByModule( e->getModule() );
        if( item != 0 )
        {
            item->activate( false );
            e->getModule()->requestStop();
        }

        return true;
    }

    // a module tree item should be deleted
    if( event->type() == WQT_MODULE_DELETE_EVENT )
    {
        WModuleDeleteEvent* e = dynamic_cast< WModuleDeleteEvent* >( event );
        if( !e )
        {
            // this should never happen, since the type is set to WQT_MODULE_REMOVE_EVENT.
            WLogger::getLogger()->addLogMessage( "Event is not an WModuleRemovedEvent although"
                                                 "its type claims it. Ignoring event.",
                                                 "NetworkEditor", LL_WARNING );
            return true;
        }

        WLogger::getLogger()->addLogMessage( "Deleting \"" + e->getTreeItem()->getModule()->getName() + "\".",
                                             "NetworkEditor", LL_DEBUG );

        WQtNetworkItem *item = findItemByModule( e->getTreeItem()->getModule() );

        if( item != 0 )
        {
            m_items.removeAll( item );
            // NOTE: the die-animation also removes the item from the layout. (in  WQtNetworkItem::removalAnimationDone )
            item->die();
        }

        return true;
    }

    if( event->type() == WQT_CRASH_EVENT )
    {
        // change module state
        WModuleCrashEvent* e = dynamic_cast< WModuleCrashEvent* >( event );
        if( !e )
        {
            // this should never happen, since the type is set to WQT_MODULE_REMOVE_EVENT.
            WLogger::getLogger()->addLogMessage( "Event is not an WModuleCrashEvent although"
                                                 "its type claims it. Ignoring event.",
                                                 "NetworkEditor", LL_WARNING );
            return true;
        }

        WLogger::getLogger()->addLogMessage( "Marking \"" + e->getModule()->getName() + "\" as crashed.",
                                             "NetworkEditor", LL_DEBUG );

        WQtNetworkItem *item = findItemByModule( e->getModule() );
        item->setCrashed();

        return true;
    }

    return WQtDockWidget::event( event );
}

WQtNetworkItem* WQtNetworkEditor::findItemByModule( boost::shared_ptr< WModule > module )
{
    for( QList< WQtNetworkItem* >::const_iterator iter = m_items.begin(); iter != m_items.end(); ++iter )
    {
       WQtNetworkItem *itemModule = dynamic_cast< WQtNetworkItem* >( *iter );
       // pointers are compared, not realy a good idea but atm. there is no better way to do this
       if( itemModule && itemModule->getModule().get() == module.get() )
       {
           return itemModule;
       }
    }
    return NULL;
}

WQtNetworkScene* WQtNetworkEditor::getScene()
{
    return m_scene;
}

WQtNetworkEditorView* WQtNetworkEditor::getView()
{
    return m_view;
}

WQtNetworkSceneLayout* WQtNetworkEditor::getLayout()
{
    return m_layout;
}

void WQtNetworkEditor::updateSceneRect()
{
    m_scene->setSceneRect( m_layout->getBoundingBox() );
}
