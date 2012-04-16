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

#include "core/kernel/combiner/WDisconnectCombiner.h"
#include "core/kernel/WKernel.h"
#include "core/kernel/WModule.h"
#include "core/kernel/WModuleFactory.h"
#include "../controlPanel/WQtControlPanel.h"
#include "../events/WEventTypes.h"
#include "../events/WModuleAssocEvent.h"
#include "../events/WModuleConnectEvent.h"
#include "../events/WModuleDeleteEvent.h"
#include "../events/WModuleDisconnectEvent.h"
#include "../events/WModuleReadyEvent.h"
#include "../events/WModuleRemovedEvent.h"

#include "WQtNetworkEditor.h"
#include "WQtNetworkEditor.moc"

WQtNetworkEditor::WQtNetworkEditor( WMainWindow* parent )
    : QDockWidget( "Module Graph", parent )
{
    setObjectName( "Module Graph Dock" );
    m_mainWindow = parent;

    m_view = new WQtNetworkEditorView();
    m_view->setMinimumSize( 20, 20 );
    this->setFocusProxy( m_view );

    m_scene = new WQtNetworkScene();
    m_scene->setSceneRect( m_scene->itemsBoundingRect() );

    m_view->setScene( m_scene );

    this->setAllowedAreas( Qt::AllDockWidgetAreas );
    this->setFeatures( QDockWidget::DockWidgetClosable |QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
    setWidget( m_view );
    connect( m_scene, SIGNAL( selectionChanged() ), this, SLOT( selectItem() ) );

    m_layout = new WNetworkLayout();

    // as the QGraphicsItems are not derived from QObject, they cannot utilize the event system. We need to provide some possibility to update
    // them regularly. We use a timer here.
    QTimer* updater = new QTimer( this );
    updater->start( 50 );
    connect( updater, SIGNAL( timeout() ), this, SLOT( updateCylce() ) );
}

WQtNetworkEditor::~WQtNetworkEditor()
{
    delete m_layout;
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

void WQtNetworkEditor::deleteSelectedItems()
{
    QList< WQtNetworkItem *> itemList;
    QList< WQtNetworkArrow *> arrowList;
    foreach( QGraphicsItem *item, m_scene->selectedItems() )
    {
        if( item->type() == WQtNetworkItem::Type )
        {
            WQtNetworkItem *netItem = qgraphicsitem_cast<WQtNetworkItem *>( item );
            itemList.append( netItem );
        }
        else if( item->type() == WQtNetworkArrow::Type )
        {
            WQtNetworkArrow *netArrow = qgraphicsitem_cast<WQtNetworkArrow *>( item );
            arrowList.append( netArrow );
        }
    }

    foreach( WQtNetworkArrow *ar, arrowList )
    {
        if( ar != 0 )
        {
            boost::shared_ptr< WDisconnectCombiner > disconnectCombiner =
                boost::shared_ptr< WDisconnectCombiner >( new WDisconnectCombiner(
                            ar->getStartPort()->getConnector()->getModule(),
                            ar->getStartPort()->getConnector()->getName(),
                            ar->getEndPort()->getConnector()->getModule(),
                            ar->getEndPort()->getConnector()->getName() ) );
            disconnectCombiner->run();
            disconnectCombiner->wait();
        }
    }

    foreach( WQtNetworkItem *it, itemList )
    {
        if( it != 0 )
        {
            WKernel::getRunningKernel()->getRootContainer()->remove( it->getModule() );
            m_scene->removeItem( it );
        }
    }
    itemList.clear();
    arrowList.clear();
}

void WQtNetworkEditor::updateCylce()
{
    for( QList< WQtNetworkItem* >::const_iterator i = m_items.begin(); i != m_items.end(); ++i )
    {
        ( *i )->update();
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
            m_layout->addItem( item );
            m_scene->addItem( item );
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

            m_layout->connectItems( outItem, inItem );
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
            m_layout->disconnectNodes( outItem, inItem );

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
            m_layout->removeItem( item );

            if( item->scene() != NULL )
            {
                m_scene->removeItem( item );
            }
            m_items.removeAll( item );
            delete item;
        }

        return true;
    }

    return QDockWidget::event( event );
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

