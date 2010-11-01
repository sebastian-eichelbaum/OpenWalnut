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
#include <iostream>

#include <boost/shared_ptr.hpp>

#include <QtGui/QDockWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsView>
#include <QtGui/QGraphicsItem>
#include <QtGui/QGraphicsItemGroup>
#include <QtGui/QListWidget>
#include <QtGui/QToolBar>

#include "../WMainWindow.h"
#include "WQtNetworkEditor.h"
#include "WQtNetworkItem.h"
#include "WQtNetworkPort.h"
//#include "WQtIconList.h"

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


WQtNetworkEditor::WQtNetworkEditor( WMainWindow* parent )
    : QDockWidget( "NetworkEditor", parent )
{
    m_mainWindow = parent;

    m_panel = new QWidget( this );

    m_view = new QGraphicsView();
    m_view->setDragMode( QGraphicsView::RubberBandDrag );
    m_view->setRenderHint( QPainter::Antialiasing );

    m_scene = new WQtNetworkScene();
    m_scene->setSceneRect( -100.0, -100.0, 200.0, 200.0 );
    m_scene->setSceneRect( m_scene->itemsBoundingRect() );

    m_view->setScene( m_scene );

    //WQtIconList *itemList = new WQtIconList;
    //QToolBar *bar = new QToolBar;
    //QPushButton* dummyButton = new QPushButton;
    //dummyButton->setFixedWidth( 0 );
    //bar->addWidget( dummyButton );

    m_layout = new QVBoxLayout;
//    m_layout->addWidget( bar );
    m_layout->addWidget( m_view );

    m_panel->setLayout( m_layout );

    this->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
    this->setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
    setMinimumSize( 160, 240 );
    setWidget( m_panel );
}

WQtNetworkEditor::~WQtNetworkEditor()
{
}

void WQtNetworkEditor::addModule( boost::shared_ptr< WModule > module )
{
    WQtNetworkItem *netItem = new WQtNetworkItem( module );
    m_items.push_back( netItem );
    m_scene->addItem( netItem );
}
    

bool WQtNetworkEditor::event( QEvent* event )
{    
    // a module got associated with the root container -> add it to the list
    if ( event->type() == WQT_ASSOC_EVENT )
    {
        // convert event to assoc event
        WModuleAssocEvent* e1 = dynamic_cast< WModuleAssocEvent* >( event );     // NOLINT
        if ( e1 )
        {
            WLogger::getLogger()->addLogMessage( "Inserting module " + e1->getModule()->getName() +
                                                " to network editor.",
                                                 "NetworkEditor", LL_DEBUG );
            addModule( e1->getModule() );
        }
        //TODO: disablen des moduls solange nicht rdy!
        return true;
    }
 
    // a module changed its state to "ready" -> activate it in dataset browser
    if ( event->type() == WQT_READY_EVENT )
    {
        // convert event to assoc event
        WModuleReadyEvent* e = dynamic_cast< WModuleReadyEvent* >( event );     // NOLINT
        if ( !e )
        {
            // this should never happen, since the type is set to WQT_READY_EVENT.
            WLogger::getLogger()->addLogMessage( "Event is not an WModueReadyEvent although its type claims it. Ignoring event.",
                                                 "NetworkEditor", LL_WARNING );

            return true;
        }

        WLogger::getLogger()->addLogMessage( "Activating module " + e->getModule()->getName() + " in dataset browser.",
                                             "NetworkEditor", LL_DEBUG );

        // search all the item matching the module
        WQtNetworkItem *item = findItemByModule( e->getModule() );
        item->activate( true );

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
                                                 "DatasetBrowser", LL_WARNING );
            return true;
        }

        // get the module of the input involved in this connection
        boost::shared_ptr< WModule > mIn = e->getInput()->getModule();
        boost::shared_ptr< WModule > mOut = e->getOutput()->getModule();

        WQtNetworkItem *inItem = findItemByModule( mIn );
        WQtNetworkItem *outItem = findItemByModule( mOut );

        WQtNetworkInputPort *ip;
        WQtNetworkOutputPort *op;

        for ( QList< WQtNetworkInputPort* >::const_iterator iter = inItem->getInPorts().begin();
                iter != inItem->getInPorts().end();
                ++iter )
        {
            WQtNetworkInputPort *inP = dynamic_cast< WQtNetworkInputPort* >( *iter );
            if( e->getInput() == inP->getConnector() )
            {
                ip = inP;
            }
        }

        for ( QList< WQtNetworkOutputPort* >::const_iterator iter = outItem->getOutPorts().begin();
                iter != outItem->getOutPorts().end();
                ++iter )
        {
            WQtNetworkOutputPort *outP = dynamic_cast< WQtNetworkOutputPort* >( *iter );
            if( e->getOutput() == outP->getConnector() )
            {
                op = outP;
            }
        }

        WQtNetworkArrow *arrow = new WQtNetworkArrow( op, ip );

        arrow->setZValue( -1000.0 );
        op->addArrow( arrow );
        ip->addArrow( arrow );
        arrow->updatePosition();

        m_scene->addItem( arrow );

    }


    // a module tree item was disconnected from another one
    if ( event->type() == WQT_MODULE_DISCONNECT_EVENT )
    {
        WModuleDisconnectEvent* e = dynamic_cast< WModuleDisconnectEvent* >( event );     // NOLINT
        if ( !e )
        {
            // this should never happen, since the type is set to WQT_MODULE_DISCONNECT_EVENT.
            WLogger::getLogger()->addLogMessage( "Event is not an WModuleDisconnectEvent although its type claims it. Ignoring event.",
                                                 "NetworkEditor", LL_WARNING );
            return true;
        }
 
        WQtNetworkItem *inItem = findItemByModule( e->getInput()->getModule() );
        WQtNetworkItem *outItem = findItemByModule( e->getOutput()->getModule() );

        WQtNetworkInputPort *ip;
        WQtNetworkOutputPort *op;

        for ( QList< WQtNetworkInputPort* >::const_iterator iter = inItem->getInPorts().begin();
            iter != inItem->getInPorts().end();
            ++iter )
        {
           WQtNetworkInputPort *inP = dynamic_cast< WQtNetworkInputPort* >( *iter );
           if( e->getInput() == inP->getConnector() )
           {
               ip = inP;
           }
        }

        for ( QList< WQtNetworkOutputPort* >::const_iterator iter = outItem->getOutPorts().begin();
            iter != outItem->getOutPorts().end();
            ++iter )
       {
           WQtNetworkOutputPort *outP = dynamic_cast< WQtNetworkOutputPort* >( *iter );
           if( e->getOutput() == outP->getConnector() )
           {
               op = outP;
           }
       }

        for ( QList< QGraphicsItem * >::iterator iter = m_scene->items().begin();
                iter != m_scene->items().end();
                ++iter )
        {
            WQtNetworkArrow *ar = dynamic_cast< WQtNetworkArrow* >( *iter );
            if( ar &&
                ar->getStartPort() == op &&
                ar->getEndPort() == ip )
            {
                m_scene->removeItem( ar );
                delete ar;
                break;
            }
        }

        return true;
    }

    // a module was removed from the container
    if ( event->type() == WQT_MODULE_REMOVE_EVENT )
    {
        std::cout << "remove" << std::endl;
        WModuleRemovedEvent* e = dynamic_cast< WModuleRemovedEvent* >( event );
        if ( !e )
        {
            // this should never happen, since the type is set to WQT_MODULE_REMOVE_EVENT.
            WLogger::getLogger()->addLogMessage( "Event is not an WModuleRemovedEvent although its type claims it. Ignoring event.",
                                                 "NetworkEditor", LL_WARNING );
            return true;
        }

        WQtNetworkItem *item = findItemByModule( e->getModule() );
        if( item ){
            item->activate( false );
            e->getModule()->requestStop();
        }


        return true;
    }

    // a module tree item should be deleted
    if ( event->type() == WQT_MODULE_DELETE_EVENT )
    {
        std::cout << "delete event" << std::endl;
        WModuleDeleteEvent* e = dynamic_cast< WModuleDeleteEvent* >( event );
        if ( !e )
        {
            // this should never happen, since the type is set to WQT_MODULE_REMOVE_EVENT.
            WLogger::getLogger()->addLogMessage( "Event is not an WModuleRemovedEvent although its type claims it. Ignoring event.",
                                                 "NetworkEditor", LL_WARNING );
            return true;
        }

        WQtNetworkItem *item = findItemByModule( e->getTreeItem()->getModule() );
        std::cout << item << std::endl;
        std::cout << item->scene() << std::endl;
        std::cout << m_scene << std::endl;
        if( item ){
            m_scene->removeItem( item );
            m_items.remove( item );
            delete item;        
        }

        return true;
    }

    return QDockWidget::event( event );
}

WQtNetworkItem* WQtNetworkEditor::findItemByModule( boost::shared_ptr< WModule > module )
{
    WQtNetworkItem *item;
    for ( std::list< WQtNetworkItem* >::const_iterator iter = m_items.begin(); iter != m_items.end(); ++iter )
    {
       WQtNetworkItem *itemModule = dynamic_cast< WQtNetworkItem* >( *iter );
       if( itemModule &&
           itemModule->getModule() == module )
       {
           item = itemModule;
           break;
       }
    }
    return item;
}
