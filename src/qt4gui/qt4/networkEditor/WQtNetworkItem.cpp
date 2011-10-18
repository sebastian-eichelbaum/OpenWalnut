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

#include "WQtNetworkArrow.h"
#include "WQtNetworkItem.h"
#include "WQtNetworkScene.h"
#include "WQtNetworkEditor.h"
#include "WQtNetworkEditorGlobals.h"

WQtNetworkItem::WQtNetworkItem( WQtNetworkEditor *editor, boost::shared_ptr< WModule > module )
    : QGraphicsRectItem()
{
    m_networkEditor = editor;
    m_module = module;
    changeColor( Qt::white );

    setCacheMode( DeviceCoordinateCache );

    //caption
    m_text = new QGraphicsTextItem( module->getName().c_str() );
    m_text->setParentItem( this );
    m_text->setDefaultTextColor( Qt::white );


    m_inPorts = QList< WQtNetworkInputPort* > ();
    m_outPorts = QList< WQtNetworkOutputPort* > ();

    //add input ports
    WModule::InputConnectorList cons = module->getInputConnectors();
    for( WModule::InputConnectorList::const_iterator iter = cons.begin(); iter != cons.end(); ++iter )
    {
        WQtNetworkInputPort *port = new WQtNetworkInputPort( *iter );
        port->setParentItem( this );
        this->addInputPort( port );
    }

    //add output ports
    WModule::OutputConnectorList outCons = module->getOutputConnectors();
    for( WModule::OutputConnectorList::const_iterator iter = outCons.begin(); iter != outCons.end(); ++iter )
    {
        WQtNetworkOutputPort *port = new WQtNetworkOutputPort( *iter );
        port->setParentItem( this );
        this->addOutputPort( port );
    }

    activate( false );

    fitLook();

    m_layoutNode = NULL;
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

    if( m_color != Qt::darkBlue )
    {
        changeColor( Qt::darkBlue );
    }

    std::string tooltip = "";
    /*if( m_module->isCrashed()() )
    {
        tooltip += "<b>A problem occured. The module has been stopped. </b><br/><br/>";
    }
    tooltip += "<b>Module: </b>" + m_module->getName() + "<br/>";
    //tooltip += "<b>Progress: </b>" + progress + "<br/>";
    tooltip += "<b>Connectors: </b>";

    // also list the connectors
    std::string conList = "";
    WModule::InputConnectorList consIn = m_module->getInputConnectors();
    WModule::OutputConnectorList consOut = m_module->getOutputConnectors();
    conList += "<table><tr><th>Name</th><th>Description</th><th>Type (I/O)</th><th>Connected</th></tr>";
    int conCount = 0;
    for( WModule::InputConnectorList::const_iterator it = consIn.begin(); it != consIn.end(); ++it )
    {
        ++conCount;
        conList += "<tr><td><b>" + ( *it )->getName() + "&nbsp;</b></td><td>" + ( *it )->getDescription() + "&nbsp;</td>";
        conList += "<td><center>In</center></td>";
        conList += ( *it )->isConnected() ? "<td><center>Yes</center></td>" : "<td><center>No</center></td>";
        conList += "</tr>";
    }
    for( WModule::OutputConnectorList::const_iterator it = consOut.begin(); it != consOut.end(); ++it )
    {
        ++conCount;
        conList += "<tr><td><b>" + ( *it )->getName() + "&nbsp;</b></td><td>" + ( *it )->getDescription() + "&nbsp;</td>";
        conList += "<td><center>Out</center></td>";
        conList += ( *it )->isConnected() ? "<td></center>Yes</center></td>" : "<td><center>No</center></td>";
        conList += "</tr>";
    }
    conList += "</table>";

    tooltip += conCount ? "Yes" + conList + "<br/><br/>" : "None<br/>";
    tooltip += "<b>Module Description: </b><br/>" + m_module->getDescription();

    */
    setToolTip( tooltip.c_str() );
}

void WQtNetworkItem::hoverLeaveEvent( QGraphicsSceneHoverEvent *event )
{
    Q_UNUSED( event );
    if( m_color != Qt::gray )
    {
        changeColor( Qt::gray );
    }
}

void WQtNetworkItem::paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* w )
{
    if( isSelected() && m_color != Qt::darkGreen )
    {
        changeColor( Qt::darkGreen );
    }

    QStyleOptionGraphicsItem *o = const_cast<QStyleOptionGraphicsItem*>( option );
    o->state &= ~QStyle::State_Selected;
    QGraphicsRectItem::paint( painter, o, w );

    //glass highlight
    //snippet from "voreen-2.5" source
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
    // TODO check the comment
    //m_networkEditor->itemMoved();
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
            changeColor( Qt::gray );
        case ItemPositionHasChanged:
            foreach( WQtNetworkPort *port, m_inPorts )
            {
                port->updateArrows();
            }
            foreach( WQtNetworkPort *port, m_outPorts )
            {
                port->updateArrows();
            }
            // TODO check the comment
            //m_networkEditor->itemMoved();
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

    changeColor( Qt::gray );
}

void WQtNetworkItem::setTextItem( QGraphicsTextItem *text )
{
    m_text = text;
}

QString WQtNetworkItem::getText()
{
    return m_text->toPlainText();
}

void WQtNetworkItem::changeColor( QColor color )
{
    m_color = color;
    m_gradient.setStart( 0, 0 );
    m_gradient.setFinalStop( 0, m_height );

    m_gradient.setColorAt( 0.0, m_color );
    m_gradient.setColorAt( 0.3, Qt::black );
    m_gradient.setColorAt( 0.7, Qt::black );
    m_gradient.setColorAt( 1.0, m_color );
    setBrush( m_gradient );
    setPen( QPen( m_color, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
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
        changeColor( Qt::gray );
    }
    if( active == false )
    {
        setAcceptsHoverEvents( false );
        setFlag( QGraphicsItem::ItemIsSelectable, false );
        setFlag( QGraphicsItem::ItemIsMovable, false );
        changeColor( Qt::white );
    }
}

/*void WQtNetworkItem::calculateForces()
{
    if( !scene() || scene()->mouseGrabberItem() == this )
    {
        m_newPos = pos();
    }

    // Sum up all forces pushing this item away
    qreal xvel = 0;
    qreal yvel = 0;
    foreach( QGraphicsItem *item, scene()->items() )
    {
        WQtNetworkItem *networkItem = qgraphicsitem_cast<WQtNetworkItem *>( item );
        if( !networkItem )
            continue;

        QLineF line( mapFromItem( networkItem, 0, 0 ), QPointF( 0, 0 ) );
        qreal dx = line.dx();
        qreal dy = line.dy();
        double l = 2.0 * ( dx * dx + dy * dy );
        if( l > 0 )
        {
            xvel += ( dx * 250.0 ) / l;
            yvel += ( dy * 250.0 ) / l;
        }
    }

    // Now subtract all forces pulling items together
    double weight = 1;
    QPointF pos;

    foreach( WQtNetworkPort *port, m_inPorts )
    {
        weight += port->getNumberOfArrows();
    }
    foreach( WQtNetworkPort *port, m_outPorts )
    {
        weight += port->getNumberOfArrows();
    }
    weight *= 50;


    foreach( WQtNetworkPort *port, m_inPorts )
    {
       QList< WQtNetworkArrow *> arrowlist = port->getArrowList();
       foreach( WQtNetworkArrow *arrow, arrowlist )
       {
            pos = mapFromItem( arrow->getStartPort()->parentItem(), 0, 0 );
            xvel += pos.x() / weight;
            yvel += pos.y() / weight;
       }
    }

    foreach( WQtNetworkPort *port, m_outPorts )
    {
       QList< WQtNetworkArrow *> arrowlist = port->getArrowList();
       foreach( WQtNetworkArrow *arrow, arrowlist )
       {
            pos = mapFromItem( arrow->getEndPort()->parentItem(), 0, 0 );
            xvel += pos.x() / weight;
            yvel += pos.y() / weight;
       }
    }

    WQtNetworkScene *scn = dynamic_cast< WQtNetworkScene *>( this->scene() );
    if( scn != NULL )
    {
        pos = mapFromItem( scn->getFakeItem(), 0, 0 );
        xvel += pos.x() / weight;
        yvel += pos.y() / weight;
    }

    if( qAbs( xvel ) < 0.1 && qAbs( yvel ) < 0.1 )
    {
        xvel = yvel = 0;
    }

    QRectF sceneRect = this->scene()->sceneRect();
    m_newPos = this->pos() + QPoint( xvel, yvel );
    m_newPos.setX( qMin( qMax( m_newPos.x(), sceneRect.left() + m_rect.x() ), sceneRect.right() - m_rect.x() ) );
    m_newPos.setY( qMin( qMax( m_newPos.y(), sceneRect.top() + m_rect.y() ), sceneRect.bottom() - m_rect.y() ) );
}*/

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

