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

#include "../WQt4Gui.h"
#include "../WMainWindow.h"

#include "WQtDockWidget.h"
#include "WQtDockWidget.moc"

WQtDockWidget::WQtDockWidget( const QString& title, QWidget* parent, Qt::WindowFlags flags ):
    QDockWidget( title, parent, flags )
{
    construct();
}

WQtDockWidget::WQtDockWidget( QWidget* parent, Qt::WindowFlags flags ):
    QDockWidget( parent, flags )
{
    construct();
}

WQtDockWidget::~WQtDockWidget()
{
    // cleanup
}

void WQtDockWidget::setupButton( QToolButton* btn )
{
    btn->setToolButtonStyle( Qt::ToolButtonIconOnly );
    btn->setContentsMargins( 0, 0, 0, 0 );
    btn->setFixedHeight( 24 );
    btn->setAutoRaise( true );
    btn->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed ) );
}

void WQtDockWidget::construct()
{
    // some standard dock features
    setAllowedAreas( Qt::AllDockWidgetAreas );
    setFeatures( QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );

    // create titlebar widget and its layout
    m_titleWidget = new QWidget( this );
    QHBoxLayout* titleWidgetLayout = new QHBoxLayout( m_titleWidget );
    titleWidgetLayout->setMargin( 0 );
    titleWidgetLayout->setSpacing( 0 );
    m_titleWidget->setSizePolicy( QSizePolicy( QSizePolicy::Ignored, QSizePolicy::Fixed ) );

    // title
    m_title = new WScaleLabel( " " + windowTitle(), 3, m_titleWidget );
    m_title->setTextInteractionFlags( Qt::NoTextInteraction );

    // close Btn
    m_closeBtn = new QToolButton( m_titleWidget );
    QAction* closeAction = new QAction( WQt4Gui::getMainWindow()->getIconManager()->getIcon( "popup_close" ), "Close", m_titleWidget );
    connect( closeAction, SIGNAL( triggered( bool ) ), this, SLOT( close() ) );
    m_closeBtn->setDefaultAction( closeAction );
    setupButton( m_closeBtn );
    m_closeBtn->setMinimumSize( 24, 24 );
    m_closeBtn->setMaximumSize( 24, 24 );

    // create the container for the actions shown in the titlebar directly
    m_tools = new QWidget( m_titleWidget );
    m_toolsLayout = new QHBoxLayout( m_tools );
    m_tools->setLayout( m_toolsLayout );
    m_tools->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
    m_toolsLayout->setMargin( 0 );
    m_toolsLayout->setSpacing( 0 );
    m_tools->setContentsMargins( 0, 0, 0, 0 );
    m_tools->setMinimumSize( 1, 24 );

    // create the container for the actions dropped out the titlebar to save some space
    m_toolsMenu = new QWidget( m_titleWidget );
    QHBoxLayout* toolsMenuLayout = new QHBoxLayout( m_toolsMenu );
    m_toolsMenu->setLayout( toolsMenuLayout );
    toolsMenuLayout->setMargin( 0 );
    toolsMenuLayout->setSpacing( 0 );
    m_toolsMenu->setContentsMargins( 0, 0, 0, 0 );

    // create a button that shows the m_toolsMenu container widget
    m_moreBtn = new QToolButton( m_titleWidget );
    m_moreBtn->setHidden( true );
    setupButton( m_moreBtn );
    m_moreBtn->setPopupMode( QToolButton::InstantPopup );
    m_moreBtn->setIcon(  WQt4Gui::getMainWindow()->getIconManager()->getIcon( "popup_more" ) );
    QMenu* moreMenu = new QMenu();
    QWidgetAction* moreAction = new QWidgetAction( m_toolsMenu );
    moreAction->setDefaultWidget( m_toolsMenu );
    moreMenu->addAction( moreAction );
    m_moreBtn->setMenu( moreMenu );

    // fill layout
    titleWidgetLayout->addWidget( m_title );
    titleWidgetLayout->addStretch( 100000 );
    titleWidgetLayout->addWidget( m_tools );
    titleWidgetLayout->addWidget( m_moreBtn );
    titleWidgetLayout->addWidget( m_closeBtn );

    // thats it. we now have the title bar
    setTitleBarWidget( m_titleWidget );
}

