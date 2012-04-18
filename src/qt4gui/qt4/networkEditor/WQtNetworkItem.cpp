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

#include <algorithm>
#include <string>
#include <iostream>

#include <boost/shared_ptr.hpp>

#include <QtGui/QStyleOptionGraphicsItem>
#include <QtGui/QTextCharFormat>
#include <QtGui/QTextCursor>

#include "WQtNetworkArrow.h"
#include "WQtNetworkItem.h"
#include "WQtNetworkItemActivator.h"
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

    // caption
    m_textFull = module->getName();
    m_text = new QGraphicsTextItem( m_textFull.c_str() );
    m_text->setParentItem( this );
    m_text->setDefaultTextColor( Qt::white );

    // for captions of data modules
    boost::shared_ptr< WDataModule > dataModule;
    dataModule = boost::shared_dynamic_cast< WDataModule >( module );
    if( dataModule )
    {
        m_subtitleFull = dataModule->getFilename().filename().string();
        m_subtitle = new QGraphicsTextItem( m_subtitleFull.c_str() );
        m_subtitle->setParentItem( this );
        m_subtitle->setDefaultTextColor( Qt::white );
        QFont f = m_subtitle->font();
        f.setPointSizeF( f.pointSizeF() * 0.75 );
        f.setBold( true );
        m_subtitle->setFont( f );
    }
    else
    {
        m_subtitle = NULL;
    }

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

    m_hidden = new WQtNetworkItemActivator( m_module );
    m_hidden->setParentItem( this );

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
    delete m_subtitle;
}

int WQtNetworkItem::type() const
{
    return Type;
}

void WQtNetworkItem::update()
{
    // check progress.
    // TODO(ebaum): implement progress updates, crash handling and similar.
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

/**
 * This function cuts away some text and attaches "..." to ensure a maximum width.
 *
 * \param item the item to clip
 * \param maxWidth the maximum width. After this function finished, the item is <=maxWidth.
 * \param fullText the original full text
 */
void clipText( QGraphicsTextItem* item, float maxWidth, std::string fullText )
{
    item->setPlainText( fullText.c_str() );
    //item->adjustSize();

    // get size
    float w = item->boundingRect().width();
    std::string newText = fullText;

    // as long as the width is too large, cut away some letters
    while( w > maxWidth )
    {
        // shorten the text
        newText = newText.substr( 0, newText.length() - 1 );
        item->setPlainText( ( newText + "..." ).c_str() );
        // and measure new size
        w = item->boundingRect().width();
    }
}

void WQtNetworkItem::fitLook()
{
    // The purpose of this method is to ensure proper dimensions of the item and the contained text. This method ensures:
    //  * an item maximum size is WNETWORKITEM_MINIMUM_WIDTH or the width of the connectors!
    //  * text exceeding size limits is cut

    m_width = WNETWORKITEM_MINIMUM_WIDTH;
    m_height = WNETWORKITEM_MINIMUM_HEIGHT;

    // we need to respect the size minimally needed by ports
    float portWidth = WQtNetworkPort::getMultiplePortWidth( std::max( m_outPorts.size(), m_inPorts.size() ) );

    // the item needs a maximum size constraint to avoid enormously large items
    // NOTE: the specified size max can only be overwritten by the
    float maxWidth = std::max( static_cast< float >( WNETWORKITEM_MAXIMUM_WIDTH ), portWidth );

    // the width of the text elements
    float textWidth = 0.0;
    float textHeight = 0.0;

    // the width and height of the subtext elements
    float subtextWidth = 0.0;
    float subtextHeight = 0.0;
    float subtextMargin = 0.0;  // the margin between text and subtext

    // 1: query sizes of sub elements
    if( m_text != 0 )
    {
        textWidth = static_cast< float >( m_text->boundingRect().width() );
        textHeight = static_cast< float >( m_text->boundingRect().height() );
    }
    if( m_subtitle != 0 )
    {
        subtextWidth = static_cast< float >( m_subtitle->boundingRect().width() );
        subtextHeight = static_cast< float >( m_subtitle->boundingRect().height() );
        subtextMargin = 0.5f * WNETWORKITE_MARGINY;
    }

    // and another height: the height of text and subtext
    float wholeTextHeight = textHeight + subtextHeight + subtextMargin;

    // get the required width and height
    float maxTextWidth = maxWidth - ( 2.0f * WNETWORKITE_MARGINX );

    // 2: limit sizes of sub elements if needed (especially the subtext)
    if( ( m_text != 0 ) )
    {
        clipText( m_text, maxTextWidth, m_textFull );
    }
    if( ( m_subtitle != 0 ) )
    {
        clipText( m_subtitle, maxTextWidth, m_subtitleFull );
    }

    // the new text boxes now define the final sizes:
    if( m_text != 0 )
    {
        textWidth = static_cast< float >( m_text->boundingRect().width() );
        textHeight = static_cast< float >( m_text->boundingRect().height() );
    }
    if( m_subtitle != 0 )
    {
        subtextWidth = static_cast< float >( m_subtitle->boundingRect().width() );
        subtextHeight = static_cast< float >( m_subtitle->boundingRect().height() );
        subtextMargin = 0.5f * WNETWORKITE_MARGINY;
    }
    float requiredWidth = std::max( portWidth, std::max( subtextWidth, textWidth ) + ( 2.0f * WNETWORKITE_MARGINX ) );
    float requiredHeight = wholeTextHeight + ( 2.0f * WNETWORKITE_MARGINY );

    // 3: set the final sizes
    m_height = std::max( requiredHeight, static_cast< float >( WNETWORKITEM_MINIMUM_HEIGHT ) );
    m_width = std::min( std::max( requiredWidth, static_cast< float >( WNETWORKITEM_MINIMUM_WIDTH ) ), maxWidth );

    QRectF rect( 0, 0, m_width, m_height );
    m_rect = rect;
    setRect( m_rect );

    // 4: use the sizes and set the positions and sizes of the text elements properly
    if( m_text != 0)
    {
        qreal x = ( m_width / 2.0 ) - ( m_text->boundingRect().width() / 2.0 );
        qreal y = ( m_height / 2.0 ) - ( wholeTextHeight / 2.0 );
        m_text->setPos( x, y );
    }

    if( m_subtitle != 0)
    {
        qreal x = ( m_width / 2.0 ) - ( m_subtitle->boundingRect().width() / 2.0 );
        qreal y = ( m_height / 2.0 ) - ( wholeTextHeight / 2.0 );
        m_subtitle->setPos( x, y + textHeight );
    }

    // 5: handle the ports
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
    return QString::fromStdString( m_textFull );
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

