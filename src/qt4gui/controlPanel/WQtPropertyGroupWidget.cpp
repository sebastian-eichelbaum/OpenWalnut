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

#include "core/common/WPropertyGroupBase.h"
#include "core/common/WLogger.h"

#include "../WGuiConsts.h"

#include "WQtPropertyGroupWidget.h"
#include "WQtPropertyGroupWidget.moc"

WQtPropertyGroupWidget::WQtPropertyGroupWidget( WPropertyGroupBase::SPtr group, QWidget* parent )
    : QWidget( parent ),
    m_name( group->getName().c_str() ),
    m_group( group )
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
    addGroup( new WQtPropertyGroupWidget( prop ) );
}

void WQtPropertyGroupWidget::addGroup( WQtPropertyGroupWidget* widget, bool asScrollArea )
{
    // create a scrollbox and group box containing the widget
    QWidget* group = new QWidget( this );

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
    QFrame* box = new QFrame( this );
    box->setFrameShape( QFrame::StyledPanel );
    QGridLayout* boxLayout = new QGridLayout( box );
    boxLayout->setMargin( 0 );
    boxLayout->setSpacing( 0 );

    // create a button as title
    QToolButton* boxTitle = new QToolButton( this );
    boxTitle->setText( widget->getName() );
    boxLayout->addWidget( boxTitle, 0, 0 );

    // set the button up
    QSizePolicy sizePolicy( QSizePolicy::Minimum, QSizePolicy::Maximum );
    sizePolicy.setHorizontalStretch( 0 );
    sizePolicy.setVerticalStretch( 0 );
    boxTitle->setSizePolicy( sizePolicy );
    boxTitle->setAutoRaise( true );
    boxTitle->setAutoFillBackground( true );

    // some styling
    QPalette palette;
    QColor defaultCol = palette.window().color().darker( 150 );
    boxTitle->setStyleSheet( "background-color: " + defaultCol.name() + "; font-weight:bold;" );

    // toggle should cause the body widget to appear/disappear
    QSignalMapper* signalMapper = new QSignalMapper( this );
    signalMapper->setMapping( boxTitle, group );
    connect( boxTitle, SIGNAL( released() ), signalMapper, SLOT( map() ) );
    connect( signalMapper, SIGNAL( mapped( QWidget* ) ), this, SLOT( switchVisibility( QWidget* ) ) );

    // create a body widget
    if( asScrollArea )
    {
        boxLayout->addWidget( scrollArea, 1, 0 );
    }
    else
    {
        boxLayout->addWidget( group, 1, 0 );
    }

    // insert into layout
    int row = m_controlLayout->rowCount();
    m_controlLayout->addWidget( box, row, 0, 1, 2 );

    // hide the box too if the property gets hidden
    box->setHidden( widget->isHidden() );
    connect( widget, SIGNAL( hideSignal( bool ) ), box, SLOT( setHidden( bool ) ) );

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
