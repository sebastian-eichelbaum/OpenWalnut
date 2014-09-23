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

#include "core/common/WLogger.h"

#include "WQtDockWidget.h"
#include "WQtDockTitleWidget.h"

#define MagicWidgetMinSize 24

WQtDockTitleWidget::WQtDockTitleWidget( WQtDockWidget* parent ):
    QWidget( parent ),
    m_dockParent( parent )
{
    construct();
}

void WQtDockTitleWidget::setupButton( QToolButton* btn )
{
    btn->setToolButtonStyle( Qt::ToolButtonIconOnly );
    btn->setAutoRaise( true );
}

void WQtDockTitleWidget::setupSizeConstraints( QWidget* widget )
{
    widget->setContentsMargins( 0, 0, 0, 0 );
    widget->setFixedHeight( MagicWidgetMinSize );
    widget->setMinimumSize( MagicWidgetMinSize, MagicWidgetMinSize );
    widget->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed ) );
}

void WQtDockTitleWidget::construct()
{
    // create titlebar widget and its layout
    QHBoxLayout* titleWidgetLayout = new QHBoxLayout( this );
    titleWidgetLayout->setMargin( 0 );
    titleWidgetLayout->setSpacing( 0 );
    setSizePolicy( QSizePolicy( QSizePolicy::Ignored, QSizePolicy::Fixed ) );

    // title
    m_title = new WScaleLabel( " " + m_dockParent->windowTitle(), 5, this );
    m_title->setTextInteractionFlags( Qt::NoTextInteraction );

    // close Btn
    m_closeBtn = new QToolButton( this );
    QAction* closeAction = new QAction( WQt4Gui::getMainWindow()->getIconManager()->getIcon( "popup_close" ), "Close", this );
    connect( closeAction, SIGNAL( triggered( bool ) ), m_dockParent, SLOT( close() ) );
    m_closeBtn->setDefaultAction( closeAction );
    setupButton( m_closeBtn );
    setupSizeConstraints( m_closeBtn );
    m_closeBtn->setMinimumSize( MagicWidgetMinSize, MagicWidgetMinSize );
    m_closeBtn->setMaximumSize( MagicWidgetMinSize, MagicWidgetMinSize );

    // create the container for the actions shown in the titlebar directly
    m_tools = new QWidget( this );
    m_toolsLayout = new QHBoxLayout( m_tools );
    m_tools->setLayout( m_toolsLayout );
    m_tools->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
    m_toolsLayout->setMargin( 0 );
    m_toolsLayout->setSpacing( 0 );
    m_tools->setContentsMargins( 0, 0, 0, 0 );
    m_tools->setMinimumSize( 1, MagicWidgetMinSize );

    // create the container for the actions dropped out the titlebar to save some space
    m_toolsMenu = new QWidget( this );
    m_toolsMenuLayout = new QHBoxLayout( m_toolsMenu );
    m_toolsMenu->setLayout( m_toolsMenuLayout );
    m_toolsMenuLayout->setMargin( 0 );
    m_toolsMenuLayout->setSpacing( 0 );
    m_toolsMenu->setContentsMargins( 0, 0, 0, 0 );

    // create a button that shows the m_toolsMenu container widget
    m_moreBtn = new QToolButton( this );
    setupButton( m_moreBtn );
    setupSizeConstraints( m_moreBtn );
    m_moreBtn->setFixedWidth( 32 );
    m_moreBtn->setPopupMode( QToolButton::InstantPopup );
    m_moreBtn->setIcon(  WQt4Gui::getMainWindow()->getIconManager()->getIcon( "popup_more" ) );
    QMenu* moreMenu = new QMenu();
    QWidgetAction* moreAction = new QWidgetAction( m_toolsMenu );
    moreAction->setDefaultWidget( m_toolsMenu );
    moreMenu->addAction( moreAction );
    m_moreBtn->setMenu( moreMenu );

    // help button
    m_helpBtn = new QToolButton( this );
    QAction* helpAction = new QAction( WQt4Gui::getMainWindow()->getIconManager()->getIcon( "questionmark" ), "Help", this );
    connect( helpAction, SIGNAL( triggered( bool ) ), m_dockParent, SLOT( showHelp() ) );
    m_helpBtn->setDefaultAction( helpAction );
    setupButton( m_helpBtn );
    setupSizeConstraints( m_helpBtn );
    m_helpBtn->setMinimumSize( MagicWidgetMinSize, MagicWidgetMinSize );
    m_helpBtn->setMaximumSize( MagicWidgetMinSize, MagicWidgetMinSize );
    m_helpBtn->setVisible( false );

    // fill layout
    titleWidgetLayout->addWidget( m_title );
    titleWidgetLayout->addStretch( 1 );
    titleWidgetLayout->addWidget( m_tools );
    titleWidgetLayout->addWidget( m_moreBtn );
    titleWidgetLayout->addWidget( m_helpBtn );
    titleWidgetLayout->addWidget( m_closeBtn );
}

void WQtDockTitleWidget::resizeEvent( QResizeEvent* event )
{
    updateLayouts( event->size().width() );
    QWidget::resizeEvent( event );
}

void WQtDockTitleWidget::addTitleAction( QAction* action, bool instantPopup )
{
    QToolButton* actionBtn = new QToolButton( this );
    actionBtn->setDefaultAction( action );
    setupButton( actionBtn );
    setupSizeConstraints( actionBtn );

    if( instantPopup )
    {
        actionBtn->setPopupMode( QToolButton::InstantPopup );
    }

    // we keep track of the widgets:
    m_titleActionWidgets.push_back( actionBtn );

    // update the layouts
    updateLayouts( width() );
}

void WQtDockTitleWidget::addTitleButton( QToolButton* button )
{
    setupButton( button );
    setupSizeConstraints( button );

    addTitleWidget( button );
}

void WQtDockTitleWidget::addTitleWidget( QWidget* widget )
{
    setupSizeConstraints( widget );

    // we keep track of the widgets:
    m_titleActionWidgets.push_back( widget );

    // update the layouts
    updateLayouts( width() );
}

void WQtDockTitleWidget::removeTitleWidget( QWidget* widget )
{
    if( widget )
    {
        m_toolsLayout->removeWidget( widget );
        m_toolsMenuLayout->removeWidget( widget );
        m_titleActionWidgets.removeAll( widget );
        // update the layouts
        updateLayouts( width() );
    }
}

void WQtDockTitleWidget::removeTitleAction( QAction* action )
{
    // find the widget for this action
    QToolButton* btn = NULL;
    for( QList< QWidget* >::iterator i = m_titleActionWidgets.begin(); i != m_titleActionWidgets.end(); ++i )
    {
        QToolButton* btnCandidate = dynamic_cast< QToolButton* >( *i );
        if( btnCandidate && ( btnCandidate->defaultAction() == action ) )
        {
            btn = btnCandidate;
        }
    }

    removeTitleWidget( btn );
}

void WQtDockTitleWidget::addTitleSeperator()
{
    // use a qframe
    QFrame* line = new QFrame();
    line->setFrameShape( QFrame::VLine );
    line->setFrameShadow( QFrame::Sunken );
    line->setFixedWidth( 5 );

    // add it
    m_titleActionWidgets.push_back( line );
    // update the layouts
    updateLayouts( width() );
}

void WQtDockTitleWidget::updateLayouts( int width )
{
    // calculate the size of widgets and the title and the mandatory close button
    int minRequired = m_title->calculateSize( m_title->text().length() ) +
                      m_moreBtn->minimumSize().width() +
                      m_helpBtn->isVisible() * m_helpBtn->minimumSize().width() +
                      m_closeBtn->minimumSize().width();

    // check and move items
    int curWidth = minRequired;
    QList< QWidget* > visible;
    QList< QWidget* > hidden;
    QList< QWidget* >* currentList = &visible;
    for( QList< QWidget* >::iterator i = m_titleActionWidgets.begin(); i != m_titleActionWidgets.end(); ++i )
    {
        curWidth += ( *i )->sizeHint().width();
        if( curWidth >= width )
        {
            // we reached the size limit.
            currentList = &hidden;
        }
        currentList->push_back( *i );
    }

    // move all visible items to the m_toolsLayout
    for( QList< QWidget* >::iterator i = visible.begin(); i != visible.end(); ++i )
    {
        m_toolsMenuLayout->removeWidget( *i );
        m_toolsLayout->addWidget( *i );
    }

    // move all visible items to the m_toolsMenuLayout
    for( QList< QWidget* >::iterator i = hidden.begin(); i != hidden.end(); ++i )
    {
        m_toolsLayout->removeWidget( *i );
        m_toolsMenuLayout->addWidget( *i );
    }

    // hide more button if nothing needs to be hidden
    m_moreBtn->setHidden( !( hidden.size() ) );

    updateGeometry();
}

void WQtDockTitleWidget::updateHelp()
{
    // hide button if no help is available.
    if( m_dockParent->getHelpContext() == "" )
    {
        m_helpBtn->setVisible( false );
        return;
    }
    m_helpBtn->setVisible( true );
}

void WQtDockTitleWidget::disableCloseButton( bool disable )
{
    m_closeBtn->setDisabled( disable );
    m_closeBtn->setVisible( !disable );
}
