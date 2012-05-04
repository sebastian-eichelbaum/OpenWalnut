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

#include "core/common/WStringUtils.h"

#include "WQtNetworkArrow.h"
#include "WQtNetworkItem.h"
#include "WQtNetworkItemActivator.h"
#include "WQtNetworkScene.h"
#include "WQtNetworkEditor.h"
#include "WQtNetworkColors.h"
#include "WQtNetworkEditorGlobals.h"

WQtNetworkItem::WQtNetworkItem( WQtNetworkEditor *editor, boost::shared_ptr< WModule > module )
    : QGraphicsRectItem(),
    m_isHovered( false ),
    m_isSelected( false ),
    m_busyIsDetermined( false ),
    m_busyPercent( 0.0 ),
    m_busyIndicatorShow( false ),
    m_forceUpdate( true )
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
    }
    else
    {
        m_subtitleFull = "Idle";
    }

    m_subtitle = new QGraphicsTextItem( m_subtitleFull.c_str() );
    m_subtitle->setParentItem( this );
    m_subtitle->setDefaultTextColor( Qt::white );
    QFont f = m_subtitle->font();
    f.setPointSizeF( f.pointSizeF() * 0.75 );
    f.setBold( true );
    m_subtitle->setFont( f );

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
    // this now calculated the optimal size. We keep them for later use
    m_itemBestWidth = boundingRect().width();

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
    delete m_subtitle;
}

int WQtNetworkItem::type() const
{
    return Type;
}

void WQtNetworkItem::updater()
{
    // it is very important to avoid unnecessary changes to pen/brush and similar stuff to avoid permanent updates of the graphics item.
    bool needUpdate = m_forceUpdate;
    m_forceUpdate = false;

    // progress indication is only needed for running modules
    if( m_currentState != Crashed )
    {
        // handle progress indication
        boost::shared_ptr< WProgressCombiner> p = m_module->getRootProgressCombiner();

        // update the progress combiners internal state
        p->update();

        if( p->isPending() )
        {
            m_busyIndicatorShow = true;
            m_busyIsDetermined = p->isDetermined();

            // update subtext
            m_subtitleFull = p->getCombinedNames( true );
            if( m_subtitleFull.empty() ) // if some lazy programmer did not provide names for the progress -> set one
            {
                m_subtitleFull = "Busy";
            }

            // we add the percent-counter to the front because the fitLook method shortens the subtext string if it is too long. This might clip out
            // the percentage if the p->getCombinedNames string is quite long.
            if(m_busyIsDetermined ) // <- of course only add if we have a known percentage
            {
                // NOTE: Percentage of a WProgressCombiner always multiplicatively combines all percentages of the children
                m_subtitleFull = string_utils::toString( static_cast< uint16_t >( p->getProgress() ) ) + "% - " + m_subtitleFull;
            }

            // this method ensures the text is shortened and correctly placed in the iem
            fitLook( m_itemBestWidth, m_itemBestWidth );

            // update indicator
            if( m_busyIsDetermined )
            {
                m_busyPercent = p->getProgress() / 100.0;
            }
            else
            {
                m_busyPercent += 0.025;
                if( m_busyPercent > 1.0 )
                {
                    m_busyPercent = 0.0;
                }
            }
            needUpdate = true;
        }
        else
        {
            // if busy indication was active -> update to remove it again
            needUpdate |= m_busyIndicatorShow;
            m_busyIndicatorShow = false;
            WDataModule::SPtr dataModule = boost::shared_dynamic_cast< WDataModule >( m_module );
            if( dataModule )
            {
                m_subtitleFull = dataModule->getFilename().filename().string();
            }
            else
            {
                m_subtitleFull = "Idle";
            }
            fitLook( m_itemBestWidth, m_itemBestWidth );
        }
    }

    // show crash state as text too
    if( ( m_currentState == Crashed ) && ( m_subtitleFull != "Crashed" ) )
    {
        m_subtitleFull = "Crashed";
        // this method ensures the text is shortened and correctly placed in the iem
        fitLook( m_itemBestWidth, m_itemBestWidth );
        needUpdate = true;
    }

    if( needUpdate )
    {
        update();
    }
}

void WQtNetworkItem::hoverEnterEvent( QGraphicsSceneHoverEvent *event )
{
    Q_UNUSED( event );
    m_isHovered = true;
    update();
}

void WQtNetworkItem::hoverLeaveEvent( QGraphicsSceneHoverEvent *event )
{
    Q_UNUSED( event );
    m_isHovered = false;
    update();
}

void WQtNetworkItem::paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* w )
{
    // This is the default appearance
    QPen newPen = QPen( m_itemColor, 1, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin );
    QColor fillColor = m_itemColor;
    // change appearance due to state changes
    switch( m_currentState )
    {
        case Disabled:
            fillColor = m_itemColor.darker( 300 );
            break;
        case Crashed:
            fillColor = WQtNetworkColors::ModuleCrashed;
            break;
        case Normal:
        default:
            // default behaviour
            break;
    }

    // if hovered:
    if( m_isHovered )
    {
        fillColor = fillColor.lighter();
    }
    // if selected:
    if( m_isSelected )
    {
        newPen = QPen( Qt::black, 2, Qt::DotLine, Qt::SquareCap, Qt::RoundJoin );
    }

    // only set brush and pen if they have changed
    QBrush newBrush = QBrush( fillColor );
    if( newBrush != brush() )
    {
        setBrush( newBrush );
    }
    if( newPen != pen() )
    {
        setPen( newPen );
    }

    QStyleOptionGraphicsItem *o = const_cast<QStyleOptionGraphicsItem*>( option );
    o->state &= ~QStyle::State_Selected;
    QGraphicsRectItem::paint( painter, o, w );

    // strike through crashed modules
    if( m_currentState == Crashed )
    {
        painter->setPen( QPen( Qt::black, 1, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin ) );
        painter->drawLine( QPoint( 0.0, 0.0 ), QPoint( m_width, m_height ) );
        painter->drawLine( QPoint( 0.0, m_height ), QPoint( m_width, 0.0 ) );
    }

    // draw busy indicator
    if( m_busyIndicatorShow )
    {
        float busyBarMarginX = 5.0;
        float busyIndicatorHeight = 2.0;
        painter->setPen( QPen( WQtNetworkColors::BusyIndicatorBackground, busyIndicatorHeight, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin ) );
        painter->drawLine( QPoint( busyBarMarginX, m_height / 2.0 ), QPoint( m_width - busyBarMarginX, m_height / 2.0 ) );
        painter->setPen( QPen( WQtNetworkColors::BusyIndicator, busyIndicatorHeight, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin ) );
        float pos = m_busyPercent * ( m_width - ( 2.0 * busyBarMarginX ) );

        // if the progress indicator is determined (known percentage) -> draw line from 0 to pos
        if( m_busyIsDetermined )
        {
            painter->drawLine( QPoint( busyBarMarginX, m_height / 2.0 ), QPoint( busyBarMarginX + pos, m_height / 2.0 ) );
        }
        else
        {
            painter->drawLine( QPoint( busyBarMarginX + pos, m_height / 2.0 ), QPoint( busyBarMarginX + pos + 5, m_height / 2.0 ) );
        }
    }
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
    m_networkEditor->getScene()->clearSelection();
    setSelected( true );
    QGraphicsItem::mousePressEvent( event );
}

QVariant WQtNetworkItem::itemChange( GraphicsItemChange change, const QVariant &value )
{
    switch( change )
    {
        case ItemSelectedHasChanged:
            m_isSelected = isSelected();
            break;
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

void WQtNetworkItem::fitLook( float maximumWidth, float minimumWidth )
{
    // The purpose of this method is to ensure proper dimensions of the item and the contained text. This method ensures:
    //  * an item maximum size is WNETWORKITEM_MINIMUM_WIDTH or the width of the connectors!
    //  * text exceeding size limits is cut

    m_width = minimumWidth;
    m_height = WNETWORKITEM_MINIMUM_HEIGHT;

    // we need to respect the size minimally needed by ports
    float portWidth = WQtNetworkPort::getMultiplePortWidth( std::max( m_outPorts.size(), m_inPorts.size() ) );

    // the item needs a maximum size constraint to avoid enormously large items
    // NOTE: the specified size max can only be overwritten by the
    float maxWidth = std::max( static_cast< float >( maximumWidth ), portWidth );

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
        subtextMargin = 1.0f * WNETWORKITEM_MARGINY;
    }

    // and another height: the height of text and subtext
    float wholeTextHeight = textHeight + subtextHeight + subtextMargin;

    // get the required width and height
    float maxTextWidth = maxWidth - ( 2.0f * WNETWORKITEM_MARGINX );

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
    }
    float requiredWidth = std::max( portWidth, std::max( subtextWidth, textWidth ) + ( 2.0f * WNETWORKITEM_MARGINX ) );
    float requiredHeight = wholeTextHeight + ( 2.0f * WNETWORKITEM_MARGINY );

    // 3: set the final sizes
    m_height = std::max( requiredHeight, static_cast< float >( WNETWORKITEM_MINIMUM_HEIGHT ) );
    m_width = std::min( std::max( requiredWidth, static_cast< float >( minimumWidth ) ), maxWidth );

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
        qreal y = ( m_height / 2.0 ) - ( subtextMargin );
        m_subtitle->setPos( x, y );
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
}

void WQtNetworkItem::setTextItem( QGraphicsTextItem *text )
{
    m_text = text;
}

QString WQtNetworkItem::getText()
{
    return QString::fromStdString( m_textFull );
}

void WQtNetworkItem::setCrashed()
{
    changeState( Crashed );
}

void WQtNetworkItem::changeState( State state )
{
    m_forceUpdate = ( m_currentState != state );
    m_currentState = state;
    update();
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
        changeState( m_module->isCrashed() ? Crashed : Normal );
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

