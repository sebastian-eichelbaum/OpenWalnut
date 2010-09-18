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

#include <QtGui/QDockWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsView>
#include <QtGui/QGraphicsItem>
#include <QtGui/QGraphicsItemGroup>

#include "../WMainWindow.h"
#include "WQtNetworkEditor.h"
#include "WQtNetworkItem.h"
#include "WQtNetworkPort.h"

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


WQtNetworkEditor::WQtNetworkEditor( QString title, WMainWindow* parent )
    : QDockWidget( title, parent )
{
    m_mainWindow = parent;

    m_panel = new QWidget( this );

    //this->setMinimumSize( 200, 400 );
    //setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Maximum );

    m_view = new QGraphicsView();
    m_view->setDragMode( QGraphicsView::RubberBandDrag );
    m_view->setMinimumSize( QSize( 200, 200 ) );
    m_view->setRenderHint( QPainter::Antialiasing );

    m_scene = new WQtNetworkScene();
    m_scene->setSceneRect( -100.0, -100.0, 200.0, 200.0 );
    m_scene->setSceneRect( m_scene->itemsBoundingRect() );

    m_view->setScene( m_scene );

    m_layout = new QVBoxLayout;
    m_layout->addWidget( m_view );

    m_panel->setLayout( m_layout );

    //addModule(/*Qpoint, String*/);

    addModule();
    //addModule();

    this->setAllowedAreas( Qt::RightDockWidgetArea );
    this->setWidget( m_panel );
}

WQtNetworkEditor::~WQtNetworkEditor()
{
}

void WQtNetworkEditor::addModule( )
{
    WQtNetworkItem *netItem = new WQtNetworkItem();

    WQtNetworkPort *port = new WQtNetworkPort( "A", false );
    port->setParentItem( netItem );

    WQtNetworkPort *iport = new WQtNetworkPort( "B", false );
    iport->setParentItem( netItem );

    WQtNetworkPort *pport = new WQtNetworkPort( "A", true );
    pport->setParentItem( netItem );

    WQtNetworkPort *ipport = new WQtNetworkPort( "B", true );
    ipport->setParentItem( netItem );

    QGraphicsTextItem *text = new QGraphicsTextItem( "test" );
    text->setParentItem( netItem );
    text->setDefaultTextColor( Qt::white );

    netItem->setFlag( QGraphicsItem::ItemIsMovable );
    netItem->setFlag( QGraphicsItem::ItemIsSelectable );
    netItem->setTextItem( text );
    netItem->addPort( port );
    netItem->addPort( iport );
    netItem->addPort( pport );
    netItem->addPort( ipport );

    netItem->fitLook();

    m_scene->addItem( netItem );
}

void WQtNetworkEditor::addModule( WModule *module )
{
    WQtNetworkItem *netItem = new WQtNetworkItem();

    QGraphicsTextItem *text = new QGraphicsTextItem( module->getName().c_str() );
    text->setParentItem( netItem );
    text->setDefaultTextColor( Qt::white );

    WModule::InputConnectorList cons = module->getInputConnectors();
    for ( WModule::InputConnectorList::const_iterator iter = cons.begin(); iter != cons.end(); ++iter )
    {        
        WQtNetworkPort *port = new WQtNetworkPort( iter->get()->getName().c_str(),
                                                  iter->get()->isOutputConnector() );
        port->setParentItem( netItem );
        netItem->addPort( port );
    }
 
    WModule::OutputConnectorList outCons = module->getOutputConnectors();
    for ( WModule::OutputConnectorList::const_iterator iter = outCons.begin(); iter != outCons.end(); ++iter )
    {        
        WQtNetworkPort *port = new WQtNetworkPort( iter->get()->getName().c_str(),
                                                  iter->get()->isOutputConnector() );
        port->setParentItem( netItem );
        netItem->addPort( port );
    }

/*
    WQtNetworkPort *port = new WQtNetworkPort( "A", false );
    port->setParentItem( netItem );

    WQtNetworkPort *iport = new WQtNetworkPort( "B", false );
    iport->setParentItem( netItem );

    WQtNetworkPort *pport = new WQtNetworkPort( "A", true );
    pport->setParentItem( netItem );

    WQtNetworkPort *ipport = new WQtNetworkPort( "B", true );
    ipport->setParentItem( netItem );

    netItem->addPort( port );
    netItem->addPort( iport );
    netItem->addPort( pport );
    netItem->addPort( ipport );
*/

    netItem->setFlag( QGraphicsItem::ItemIsMovable );
    netItem->setFlag( QGraphicsItem::ItemIsSelectable );
    netItem->setTextItem( text );
    
    netItem->fitLook();

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
            addModule( ( e1->getModule() ).get() );
        }
        return true;
    }
    else
    {
        event->ignore();
        return false;
    }
}
