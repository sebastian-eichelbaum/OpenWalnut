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

#include <QtGui/QApplication>
#include <QtGui/QGroupBox>
#include <QtGui/QPushButton>
#include <QtGui/QScrollArea>

#include "../events/WEventTypes.h"
#include "../events/WPropertyChangedEvent.h"

#include "core/common/WProperties.h"

#include "../WGuiConsts.h"

#include "WQtPropertyGroupWidget.h"
#include "WQtPropertyGroupWidget.moc"

WQtPropertyGroupWidget::WQtPropertyGroupWidget( WPropGroup group, QWidget* parent )
    : QWidget( parent ),
    m_name( group->getName().c_str() ),
    m_numberOfWidgets( 0 ),
    m_group( group )
{
    // note: never do layouts as none pointers
    // on destruction of a widget it will try to delete them which will cause crashes
    m_pageLayout = new QVBoxLayout();
    m_pageLayout->setMargin( WGLOBAL_MARGIN );
    m_pageLayout->setSpacing( WGLOBAL_SPACING );

    m_controlLayout = new QGridLayout();
    m_controlLayout->setMargin( WGLOBAL_MARGIN );
    m_controlLayout->setSpacing( WGLOBAL_SPACING );

    m_pageLayout->addLayout( m_controlLayout );

    // empty groups are hidden too
    // NOTE: the WProperties class fires the update condition if a prop gets added. So it automatically un-hides if a prop is added.
    WProperties::PropertySharedContainerType::ReadTicket r = m_group->getReadTicket();
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
        WProperties::PropertySharedContainerType::ReadTicket r = m_group->getReadTicket();
        setHidden( r->get().empty() | m_group->isHidden() );
        emit hideSignal( m_group->isHidden() );
        return true;
    }

    return QWidget::event( event );
}

WPropertyBoolWidget* WQtPropertyGroupWidget::addProp( WPropBool property )
{
    ++m_numberOfWidgets;

    return new WPropertyBoolWidget( property, m_controlLayout, this );
}

WPropertyIntWidget* WQtPropertyGroupWidget::addProp( WPropInt property )
{
    ++m_numberOfWidgets;

    return new WPropertyIntWidget( property, m_controlLayout, this );
}

WPropertyDoubleWidget* WQtPropertyGroupWidget::addProp( WPropDouble property )
{
    ++m_numberOfWidgets;

    return new WPropertyDoubleWidget( property, m_controlLayout, this );
}

WPropertyStringWidget* WQtPropertyGroupWidget::addProp( WPropString property )
{
    ++m_numberOfWidgets;

    return new WPropertyStringWidget( property, m_controlLayout, this );
}

WPropertyColorWidget* WQtPropertyGroupWidget::addProp( WPropColor property )
{
    ++m_numberOfWidgets;

    return new WPropertyColorWidget( property, m_controlLayout, this );
}

WPropertyFilenameWidget* WQtPropertyGroupWidget::addProp( WPropFilename property )
{
    ++m_numberOfWidgets;

    return new WPropertyFilenameWidget( property, m_controlLayout, this );
}

WPropertyTriggerWidget* WQtPropertyGroupWidget::addProp( WPropTrigger property )
{
    ++m_numberOfWidgets;

    return new WPropertyTriggerWidget( property, m_controlLayout, this );
}

WPropertyPositionWidget* WQtPropertyGroupWidget::addProp( WPropPosition property )
{
    ++m_numberOfWidgets;

    return new WPropertyPositionWidget( property, m_controlLayout, this );
}

WPropertyMatrix4X4Widget* WQtPropertyGroupWidget::addProp( WPropMatrix4X4 property )
{
    ++m_numberOfWidgets;

    return new WPropertyMatrix4X4Widget( property, m_controlLayout, this );
}

WPropertySelectionWidget* WQtPropertyGroupWidget::addProp( WPropSelection property )
{
    ++m_numberOfWidgets;

    return new WPropertySelectionWidget( property, m_controlLayout, this );
}

WPropertyTransferFunctionWidget* WQtPropertyGroupWidget::addProp( WPropTransferFunction property )
{
    ++m_numberOfWidgets;

    return new WPropertyTransferFunctionWidget( property, m_controlLayout, this );
}

void WQtPropertyGroupWidget::addGroup( WQtPropertyGroupWidget* widget, bool asScrollArea )
{
    ++m_numberOfWidgets;

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
    QPushButton* boxTitle = new QPushButton( this );
    boxLayout->addWidget( boxTitle, 0, 0 );

    // set the button up
    QSizePolicy sizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    sizePolicy.setHorizontalStretch( 0 );
    sizePolicy.setVerticalStretch( 0 );
    sizePolicy.setHeightForWidth( boxTitle->sizePolicy().hasHeightForWidth() );
    boxTitle->setSizePolicy( sizePolicy );
    boxTitle->setCheckable( true );
    boxTitle->setChecked( true );
    boxTitle->setFlat( true );
    QFont font;
    font.setBold( true );
    boxTitle->setFont( font );
    boxTitle->setText( widget->getName() );

    // toggle should cause the body widget to appear/disappear
    connect( boxTitle, SIGNAL( toggled( bool ) ), group, SLOT( setVisible( bool ) ) );

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
    return !m_numberOfWidgets;
}
