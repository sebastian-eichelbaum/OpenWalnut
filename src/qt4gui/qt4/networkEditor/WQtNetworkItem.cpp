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

#include <QtGui/QStyleOptionGraphicsItem>
#include <QtGui/QTextCharFormat>
#include <QtGui/QTextCursor>

#include "WQtNetworkArrow.h"
#include "WQtNetworkItem.h"
#include "WQtNetworkScene.h"
#include "WQtNetworkEditor.h"
#include "WQtNetworkColors.h"
#include "WQtNetworkEditorGlobals.h"

WQtNetworkItem::WQtNetworkItem( WQtNetworkEditor *editor, boost::shared_ptr< WModule > module )
    : QGraphicsRectItem()
{
    m_networkEditor = editor;
    m_module = module;

    setCacheMode( DeviceCoordinateCache );

    // for captions of data modules
    std::string dataNameString;

    boost::shared_ptr< WDataModule > dataModule;
    dataModule = boost::shared_dynamic_cast< WDataModule >( module );    if( dataModule )
    {
        dataNameString = std::string( "\n(" ) + string_utils::tokenize( dataModule->getFilename().string(), "/" ).back() + ")";
    }

    // caption
    m_text = new QGraphicsTextItem( ( module->getName().c_str() + dataNameString  ).c_str() );
    m_text->setParentItem( this );
    m_text->setDefaultTextColor( Qt::white );

    m_inPorts = QList< WQtNetworkInputPort* > ();
    m_outPorts = QList< WQtNetworkOutputPort* > ();

    //add input ports
    WModule::InputConnectorList cons = module->getInputConnectors();
    bool hasInput = cons.size();
    for( WModule::InputConnectorList::const_iterator iter = cons.begin(); iter != cons.end(); ++iter )
    {
        WQtNetworkInputPort *port = new WQtNetworkInputPort( *iter );
        port->setParentItem( this );
        this->addInputPort( port );
    }

    //add output ports
    WModule::OutputConnectorList outCons = module->getOutputConnectors();
    bool hasOutput = outCons.size();
    for( WModule::OutputConnectorList::const_iterator iter = outCons.begin(); iter != outCons.end(); ++iter )
    {
        WQtNetworkOutputPort *port = new WQtNetworkOutputPort( *iter );
        port->setParentItem( this );
        this->addOutputPort( port );
    }

    // Standard processing modules with in- and outputs are colored this way:
    m_itemColor = WQtNetworkColors::Module;
    if( !hasInput && !hasOutput )
    {
        // neither inputs nor outputs
        m_itemColor = WQtNetworkColors::StandaloneModule;
    }
    else if( !hasInput )
    {
        // no inputs -> source
        m_itemColor = WQtNetworkColors::SourceModule;
    }
    else if( !hasOutput )
    {
        // no outputs but inputs -> sink
        m_itemColor = WQtNetworkColors::SinkModule;
    }

    activate( false );

    fitLook();

    m_layoutNode = NULL;

    changeState( Disabled );
}

WQtNetworkItem::~WQtNetworkItem()
{
    foreach( WQtNetworkPort *port, m_inPorts )
    {
        delete port;
    }

    foreach( WQtNetworkPort *port, m_outPorts )
    {
        delete port;
    }
    delete m_text;
    //delete m_subtitle;
}

int WQtNetworkItem::type() const
{
    return Type;
}

void WQtNetworkItem::hoverEnterEvent( QGraphicsSceneHoverEvent *event )
{
    Q_UNUSED( event );

    if( !isSelected() )
    {
        changeState( Hovered );
    }
}

void WQtNetworkItem::hoverLeaveEvent( QGraphicsSceneHoverEvent *event )
{
    Q_UNUSED( event );

    if( !isSelected() )
    {
        changeState( Idle );
    }
}

void WQtNetworkItem::paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* w )
{
    if( isSelected() )
    {
        changeState( Selected );
    }

    QStyleOptionGraphicsItem *o = const_cast<QStyleOptionGraphicsItem*>( option );
    o->state &= ~QStyle::State_Selected;
    QGraphicsRectItem::paint( painter, o, w );

    painter->setBrush( QBrush( Qt::white ) );
    painter->setPen( QPen( QBrush( Qt::white ), 0.01 ) );
    painter->setOpacity( 0.30 );
    QRectF rect( 0, 0, m_width, m_height/2.0 );
    painter->drawRect( rect );
}

void WQtNetworkItem::mouseMoveEvent( QGraphicsSceneMouseEvent *mouseEvent )
{
    QGraphicsItem::mouseMoveEvent( mouseEvent );

    foreach( WQtNetworkPort *port, m_inPorts )
    {
        port->updateArrows();
    }
    foreach( WQtNetworkPort *port, m_outPorts )
    {
        port->updateArrows();
    }
}

void WQtNetworkItem::mousePressEvent( QGraphicsSceneMouseEvent *event )
{
    QGraphicsItem::mousePressEvent( event );
    setSelected( true );
}

QVariant WQtNetworkItem::itemChange( GraphicsItemChange change, const QVariant &value )
{
    switch( change )
    {
        case ItemSelectedHasChanged:
            changeState( Idle );
        case ItemPositionHasChanged:
            foreach( WQtNetworkPort *port, m_inPorts )
            {
                port->updateArrows();
            }
            foreach( WQtNetworkPort *port, m_outPorts )
            {
                port->updateArrows();
            }
        default:
            break;
    }

    return QGraphicsItem::itemChange( change, value );
}

void WQtNetworkItem::addInputPort( WQtNetworkInputPort *port )
{
    m_inPorts.append( port );
}

void WQtNetworkItem::addOutputPort( WQtNetworkOutputPort *port )
{
    m_outPorts.append( port );
}

QList< WQtNetworkInputPort *> WQtNetworkItem::getInPorts()
{
    return m_inPorts;
}

QList< WQtNetworkOutputPort *> WQtNetworkItem::getOutPorts()
{
    return m_outPorts;
}

WNetworkLayoutNode * WQtNetworkItem::getLayoutNode()
{
    return m_layoutNode;
}

void WQtNetworkItem::fitLook()
{
    if( m_text != 0)
    {
        m_width = m_text->boundingRect().width() + 10;
        m_height = m_text->boundingRect().height() + 10;
        if( m_width < WNETWORKITEM_MINIMUM_WIDTH )
        {
            m_width = WNETWORKITEM_MINIMUM_WIDTH;
        }
        if( m_height < WNETWORKITEM_MINIMUM_HEIGHT )
        {
            m_height = WNETWORKITEM_MINIMUM_HEIGHT;
        }
        QRectF rect( 0, 0, m_width, m_height );
        m_rect = rect;
        setRect( m_rect );

        qreal x = ( m_width / 2.0 ) - ( m_text->boundingRect().width() / 2.0 );
        qreal y = ( m_height / 2.0 ) - ( m_text->boundingRect().height() / 2.0 );
        m_text->setPos( x, y );
    }

    int portNumber = 1;
    foreach( WQtNetworkPort *port, m_inPorts )
    {
        port->alignPosition( m_inPorts.size(), portNumber, m_rect, false );
        portNumber++;
    }

    portNumber = 1;
    foreach( WQtNetworkPort *port, m_outPorts )
    {
        port->alignPosition( m_outPorts.size(), portNumber, m_rect, true );
        portNumber++;
    }

    changeState( Idle );
}

void WQtNetworkItem::setTextItem( QGraphicsTextItem *text )
{
    m_text = text;
}

QString WQtNetworkItem::getText()
{
    return m_text->toPlainText();
}

void WQtNetworkItem::changeState( State state )
{
    m_currentState = state;

    // This is the default appearance
    QPen pen = QPen( m_itemColor, 1, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin );
    QLinearGradient gradient;
    gradient.setStart( 0, 0 );
    gradient.setFinalStop( 0, m_height );
    gradient.setColorAt( 0.0, m_itemColor );
    gradient.setColorAt( 1.0, m_itemColor );

    // change appearance due to state changes
    switch( state )
    {
        case Disabled:
            gradient.setColorAt( 0.0, m_itemColor.darker( 300 ) );
            gradient.setColorAt( 1.0, m_itemColor.darker( 300 ) );
            break;
        case Idle:
            // default behaviour
            break;
        case Hovered:
            gradient.setColorAt( 0.0, m_itemColor.lighter() );
            gradient.setColorAt( 1.0, m_itemColor.lighter() );
            break;
        case Selected:
            pen = QPen( Qt::black, 2, Qt::DotLine, Qt::SquareCap, Qt::RoundJoin );
            break;
        default:
            break;
    }

    setBrush( gradient );
    setPen( pen );
}

boost::shared_ptr< WModule > WQtNetworkItem::getModule()
{
    return m_module;
}

void WQtNetworkItem::activate( bool active )
{
    setEnabled( active );

    if( active == true )
    {
        setAcceptsHoverEvents( true );
        setFlag( QGraphicsItem::ItemIsSelectable );
        setFlag( QGraphicsItem::ItemIsMovable );
        changeState( Idle );
    }
    if( active == false )
    {
        setAcceptsHoverEvents( false );
        setFlag( QGraphicsItem::ItemIsSelectable, false );
        setFlag( QGraphicsItem::ItemIsMovable, false );
        changeState( Disabled );
    }
}

bool WQtNetworkItem::advance()
{
    if( m_newPos == pos() )
        return false;

    setPos( m_newPos );

    foreach( WQtNetworkPort *port, m_inPorts )
    {
        port->updateArrows();
    }
    foreach( WQtNetworkPort *port, m_outPorts )
    {
        port->updateArrows();
    }

    return true;
}

