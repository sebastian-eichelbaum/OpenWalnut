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

#include <list>

#include <QtGui/QAction>
#include <QtGui/QPushButton>
#include <QtGui/QMenu>

#include "../../common/WPreferences.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleCombiner.h"

#include "WMainWindow.h"
#include "WQtToolBar.h"

#include "WQtCombinerToolbar.h"

WQtCombinerToolbar::WQtCombinerToolbar( WMainWindow* parent, WCombinerTypes::WCompatiblesList compatibles )
    : QToolBar( "Compatible Modules", parent ),
      m_parent( parent ),
      m_actionList( WQtCombinerActionList( this, parent->getIconManager(), compatibles ) )
{
    // setup toolbar
    setAllowedAreas( Qt::AllToolBarAreas );

    // this sets the toolbar style
    int compToolBarStyle = parent->getToolbarStyle(); // this defaults to the global toolbar style
    WPreferences::getPreference( "qt4gui.compatiblesToolBarStyle", &compToolBarStyle );
    if ( ( compToolBarStyle < 0 ) || ( compToolBarStyle > 3 ) ) // ensure a valid value
    {
        compToolBarStyle = 0;
    }

    // cast and set
    setToolButtonStyle( static_cast< Qt::ToolButtonStyle >( compToolBarStyle ) );

    // create the list of actions possible
    addActions( m_actionList );

    insertDummyButton();
}

WQtCombinerToolbar::~WQtCombinerToolbar()
{
}

void WQtCombinerToolbar::clear()
{
    // as QT is not able to delete the actions that are removed by clear(), we need to take care about it
    for ( WQtCombinerActionList::iterator it = m_actionList.begin(); it != m_actionList.end(); ++it )
    {
        delete ( *it );
    }
    m_actionList.clear();
    QToolBar::clear();
}

void WQtCombinerToolbar::makeEmpty()
{
    clear();
    insertDummyButton();
}

void WQtCombinerToolbar::updateButtons( WCombinerTypes::WCompatiblesList compatibles )
{
    clear();
    // create the list of actions possible
    m_actionList = WQtCombinerActionList( this, m_parent->getIconManager(), compatibles );
    addActions( m_actionList );
}

void WQtCombinerToolbar::insertDummyButton()
{
    // The following makes the bar having button size.
    QPushButton* dummyButton = new QPushButton;
    if ( ( m_parent->toQtToolBarArea( m_parent->getCompatiblesToolbarPos() ) ==  Qt::TopToolBarArea ) ||
         ( m_parent->toQtToolBarArea( m_parent->getCompatiblesToolbarPos() ) ==  Qt::BottomToolBarArea ) )
    {
        dummyButton->setFixedWidth( 0 );
        dummyButton->setFixedHeight( 30 );
    }
    else
    {
        dummyButton->setFixedWidth( 48 );   // well this size must be more than 32 as there might be these little submenu arrows besides the icon
        dummyButton->setFixedHeight( 0 );
    }

    addWidget( dummyButton );
}
