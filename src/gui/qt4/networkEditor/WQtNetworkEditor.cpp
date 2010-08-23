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


WQtNetworkEditor::WQtNetworkEditor( QString title, WMainWindow* parent )
    : QDockWidget( title, parent )
{
    m_mainWindow = parent;

    m_panel = new QWidget( this );

    //this->setMinimumSize( 200, 400 );
    //setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Maximum );

    m_view = new QGraphicsView();
    m_view->setDragMode( QGraphicsView::RubberBandDrag );
    m_view->setMinimumSize( QSize(200, 200 ) );
    m_view->setRenderHint(QPainter::Antialiasing);
    
    m_scene = new WQtNetworkScene();
    m_scene->setSceneRect( -100.0, -100.0, 200.0, 200.0 );
    m_scene->setSceneRect(m_scene->itemsBoundingRect());

    m_view->setScene( m_scene );

    m_layout = new QVBoxLayout;
    m_layout->addWidget( m_view );

    m_panel->setLayout( m_layout );

    addModule(/*Qpoint, String*/);

    addModule();
    addModule();

    this->setAllowedAreas( Qt::RightDockWidgetArea );
    this->setWidget( m_panel );
}

WQtNetworkEditor::~WQtNetworkEditor()
{
}

void WQtNetworkEditor::addModule()
{
    WQtNetworkItem *netItem = new WQtNetworkItem();

    WQtNetworkPort *port = new WQtNetworkPort();
    port->setType( false );
    port->setParentItem( netItem );
    port->setName( "input" );
    WQtNetworkPort *iport = new WQtNetworkPort();
    iport->setType( false );
    iport->setParentItem( netItem );

    WQtNetworkPort *pport = new WQtNetworkPort();
    pport->setType( true );
    pport->setParentItem( netItem );
    WQtNetworkPort *ipport = new WQtNetworkPort();
    ipport->setType( true );
    ipport->setParentItem( netItem );

    QGraphicsTextItem *text = new QGraphicsTextItem( "Module " );
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
