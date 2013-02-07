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
#include <algorithm>

#include <QtCore/QSignalMapper>
#include <QtGui/QApplication>
#include <QtGui/QGroupBox>
#include <QtGui/QPushButton>
#include <QtGui/QToolButton>
#include <QtGui/QScrollArea>

#include "../events/WEventTypes.h"
#include "../events/WPropertyChangedEvent.h"
#include "../guiElements/WScaleToolButton.h"

#include "core/common/WPropertyGroupBase.h"
#include "core/common/WLogger.h"

#include "../WGuiConsts.h"

#include "WQtPropertyGroupWidget.h"
#include "WQtPropertyGroupWidget.moc"

WQtPropertyGroupWidget::WQtPropertyGroupWidget( WPropertyGroupBase::SPtr group, size_t depth, QWidget* parent )
    : QWidget( parent ),
    m_name( group->getName().c_str() ),
    m_group( group ),
    m_nestingDepth( depth )
{
    // note: never do layouts as none pointers
    // on destruction of a widget it will try to delete them which will cause crashes
    m_pageLayout = new QVBoxLayout();
    m_pageLayout->setMargin( WGLOBAL_MARGIN );
    m_pageLayout->setSpacing( WGLOBAL_SPACING );
    m_pageLayout->setAlignment( Qt::AlignTop );

    m_controlLayout = new QGridLayout();
    m_controlLayout->setMargin( WGLOBAL_MARGIN );
    m_controlLayout->setSpacing( WGLOBAL_SPACING );
    m_controlLayout->setAlignment( Qt::AlignTop );

    m_pageLayout->addLayout( m_controlLayout );

    // force widgets to horizontally be on top and shrink vertically to match scroll area
    QSizePolicy sizePolicy( QSizePolicy::Expanding, QSizePolicy::Maximum );
    sizePolicy.setHorizontalStretch( 0 );
    sizePolicy.setVerticalStretch( 0 );
    setSizePolicy( sizePolicy );

    // add the groups children
    // read lock, gets unlocked upon destruction (out of scope)
    WPropertyGroupBase::PropertySharedContainerType::ReadTicket propAccess = group->getProperties();
    setName( QString::fromStdString( group->getName() ) );

    // iterate all properties.
    for( WPropertyGroupBase::PropertyConstIterator iter = propAccess->get().begin(); iter != propAccess->get().end(); ++iter )
    {
        addProp( *iter );
    }
    addSpacer();

    // empty groups are hidden too
    // NOTE: the WPropertyGroupBase class fires the update condition if a prop gets added. So it automatically un-hides if a prop is added.
    WPropertyGroupBase::PropertySharedContainerType::ReadTicket r = m_group->getReadTicket();
    bool hide = ( r->get().empty() | m_group->isHidden() );
    r.reset();
    // NOTE: a simple setHidden( group->isHidden() ) causes the QWidgets to popup if hidden is false. This is why we set hidden only if it really
    // is needed
    if( hide )
    {
        setHidden( true );
    }
    // setup the update callback
    m_connection = m_group->getUpdateCondition()->subscribeSignal( boost::bind( &WQtPropertyGroupWidget::propertyChangeNotifier, this ) );
}

WQtPropertyGroupWidget::~WQtPropertyGroupWidget()
{
    // cleanup
    m_connection.disconnect();
}

void WQtPropertyGroupWidget::propertyChangeNotifier()
{
    QCoreApplication::postEvent( this, new WPropertyChangedEvent() );
}

bool WQtPropertyGroupWidget::event( QEvent* event )
{
    // a property changed
    if( event->type() == WQT_PROPERTY_CHANGED_EVENT )
    {
        WPropertyGroupBase::PropertySharedContainerType::ReadTicket r = m_group->getReadTicket();

        // handle the case of a hidden property
        setHidden( r->get().empty() | m_group->isHidden() );
        emit hideSignal( m_group->isHidden() );

        // Remove all items we have a widget for but which is not in the property group anymore
        for( PropertyWidgets::iterator i = m_propWidgets.begin(); i != m_propWidgets.end(); ++i )
        {
            // element in the group?
            WPropertyGroupBase::PropertyContainerType::const_iterator found = std::find( r->get().begin(), r->get().end(), i->first );
            if( found == r->get().end() )
            {
                // NO! Remove the widget. But not yet.
                delete( i->second );
                i->second = NULL;
            }
        }

        // Add all properties as widget of not yet there
        for( WPropertyGroupBase::PropertyContainerType::const_iterator i = r->get().begin(); i != r->get().end(); ++i )
        {
            // is there a widget for this prop?
            if( !m_propWidgets.count( *i ) )
            {
                // NO. Add it.
                addProp( *i );
            }
        }

        return true;
    }

    return QWidget::event( event );
}

void WQtPropertyGroupWidget::addProp( WPropertyBase::SPtr property )
{
    if( property->getType() == PV_GROUP )
    {
        addGroup( property->toPropGroupBase() );
        return;
    }

    // create a widget and increase counter if successful
    WPropertyWidget* widget = WPropertyWidget::construct( property, m_controlLayout, this );
    if( widget )
    {
        m_propWidgets[ property ] = widget;
    }
}

void WQtPropertyGroupWidget::addGroup( WPropertyGroupBase::SPtr prop )
{
    addGroup( new WQtPropertyGroupWidget( prop, m_nestingDepth + 1, this ) );
}

QWidget* WQtPropertyGroupWidget::createPropertyGroupBox( WQtPropertyGroupWidget* widget, bool asScrollArea, QWidget* parent, const QString& title )
{
    QSizePolicy sizePolicy( QSizePolicy::Minimum, QSizePolicy::Maximum );
    sizePolicy.setHorizontalStretch( 0 );
    sizePolicy.setVerticalStretch( 0 );

    // create a scrollbox and group box containing the widget
    QWidget* group = new QWidget();

    QScrollArea* scrollArea = 0;
    QGridLayout* grid = new QGridLayout();
    grid->addWidget( widget, 0, 0 );
    grid->setMargin( WGLOBAL_MARGIN );
    grid->setSpacing( WGLOBAL_SPACING );

    group->setLayout( grid );
    if( asScrollArea )
    {
        scrollArea = new QScrollArea();
        scrollArea->setWidget( group );
        group->show();
    }

    // encapsulate it into an collapsable widget
    QFrame* box = new QFrame( parent );
    box->setFrameShape( QFrame::StyledPanel );
    box->setObjectName( "PropertyGroupBox" );
    QGridLayout* boxLayout = new QGridLayout();
    boxLayout->setMargin( 0 );
    boxLayout->setSpacing( 0 );
    box->setLayout( boxLayout );

    // create a button as title
    WScaleToolButton* boxTitle = new WScaleToolButton( box );
    QString titleText = ( title == "" ) ? widget->getName() : title;
    boxTitle->setText( titleText );
    boxTitle->setToolTip( titleText );
    boxLayout->addWidget( boxTitle, 0, 0 );

    // we need a separate widget to indent the content widget a bit without indenting the title ... yes this really looks like the mess you can
    // do with DIVs in HTML :-/. What we do here is adding a widget which then contains the content widget and indents it. Setting the contents
    // margins directly in the QFrame* box would also indent the title button which is not wanted
    QWidget* boxContent = new QWidget();
    boxContent->setContentsMargins( 5, 0, 0, 0 );
    boxContent->setObjectName( "PropertyGroupBoxContent" );
    QGridLayout* boxContentLayout = new QGridLayout();
    boxContent->setLayout( boxContentLayout );
    boxContentLayout->setMargin( 0 );
    boxContentLayout->setSpacing( 0 );
    boxLayout->addWidget( boxContent, 1, 0 );

    // set the button up
    boxTitle->setSizePolicy( sizePolicy );
    boxTitle->setAutoRaise( true );
    boxTitle->setAutoFillBackground( true );

    // content widget
    QWidget* content = new QWidget();
    content->setObjectName( "PropertyGroupContent" );
    boxContentLayout->addWidget( content, 1, 0 );

    // content layout
    QGridLayout* contentLayout = new QGridLayout();
    content->setLayout( contentLayout );
    contentLayout->setMargin( 0 );
    contentLayout->setSpacing( 0 );

    // some styling
    QPalette palette;   // the palette is used to get default colors of the style/system
    QColor defaultCol = palette.window().color().darker( 120 );
    switch( widget->m_nestingDepth % 10 ) // NOTE: the first level 0 does not need a color as it does not provide a boxtitle, so we begin with 1
    {
        // All these colors are taken from the solarized pallette http://ethanschoonover.com/solarized
        case 1:
            defaultCol = palette.window().color().darker( 150 );
            break;
        case 2:
            defaultCol = QColor( "#268bd2" );
            break;
        case 3:
            defaultCol = QColor( "#2aa198" );
            break;
        case 4:
            defaultCol = QColor( "#859900" );
            break;
        case 5:
            defaultCol = QColor( "#b58900" );
            break;
        case 6:
            defaultCol = QColor( "#cb4b16" );
            break;
        case 7:
            defaultCol = QColor( "#dc322f" );
            break;
        case 8:
            defaultCol = QColor( "#d33682" );
            break;
        case 9:
            defaultCol = QColor( "#6c71c4" );
            break;
    }

    boxTitle->setStyleSheet( "background-color: " + defaultCol.name() + "; font-weight:bold;" );
    box->setStyleSheet( "QFrame#PropertyGroupBox{background-color: " + defaultCol.name() + ";}" );
    content->setStyleSheet( "#PropertyGroupContent{ background-color: "+ palette.window().color().name() +";}" );

    // toggle should cause the body widget to appear/disappear
    QSignalMapper* signalMapper = new QSignalMapper( box );
    signalMapper->setMapping( boxTitle, boxContent );
    connect( boxTitle, SIGNAL( released() ), signalMapper, SLOT( map() ) );
    connect( signalMapper, SIGNAL( mapped( QWidget* ) ), widget, SLOT( switchVisibility( QWidget* ) ) );

    // create a body widget
    if( asScrollArea )
    {
        contentLayout->addWidget( scrollArea, 1, 0 );
    }
    else
    {
        contentLayout->addWidget( group, 1, 0 );
    }

    // hide the box too if the property gets hidden
    box->setHidden( widget->isHidden() );
    connect( widget, SIGNAL( hideSignal( bool ) ), box, SLOT( setHidden( bool ) ) );

    return box;
}

QWidget* WQtPropertyGroupWidget::createPropertyGroupBox( WPropertyGroupBase::SPtr group, const QString& title, size_t depth, QWidget* parent )
{
    QSizePolicy sizePolicy( QSizePolicy::Preferred, QSizePolicy::Maximum );
    sizePolicy.setHorizontalStretch( 0 );
    sizePolicy.setVerticalStretch( 0 );

    WQtPropertyGroupWidget* propWidget = new WQtPropertyGroupWidget( group, depth, parent );
    propWidget->setName( title );
    QWidget* tab =  WQtPropertyGroupWidget::createPropertyGroupBox( propWidget, false, parent, title );
    tab->setSizePolicy( sizePolicy );
    tab->setWindowTitle( title );

    return tab;
}

void WQtPropertyGroupWidget::addGroup( WQtPropertyGroupWidget* widget, bool asScrollArea )
{
    QWidget* box = WQtPropertyGroupWidget::createPropertyGroupBox( widget, asScrollArea, this );

    // insert into layout
    int row = m_controlLayout->rowCount();
    m_controlLayout->addWidget( box, row, 0, 1, 2 );

    // also keep track of group widgets
    m_propWidgets[ widget->getPropertyGroup() ] = box;
}

void WQtPropertyGroupWidget::addSpacer()
{
    m_pageLayout->addStretch();
    setLayout( m_pageLayout );
}

QString WQtPropertyGroupWidget::getName()
{
    return m_name;
}

void WQtPropertyGroupWidget::setName( QString name )
{
    m_name = name;
}

bool WQtPropertyGroupWidget::isEmpty() const
{
    return m_propWidgets.empty();
}

WPropertyGroupBase::SPtr WQtPropertyGroupWidget::getPropertyGroup()
{
    return m_group;
}

void WQtPropertyGroupWidget::switchVisibility( QWidget* who )
{
    who->setVisible( !who->isVisible() );
}
