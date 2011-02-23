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
#include <QtGui/QLabel>
#include <QtGui/QMenu>

#include "../../common/WPreferences.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleCombiner.h"

#include "WMainWindow.h"
#include "WQtToolBar.h"

#include "WQtCombinerToolbar.h"

WQtCombinerToolbar::WQtCombinerToolbar( WMainWindow* parent, const WQtCombinerActionList& compatibles )
    : QToolBar( "Compatible Modules", parent ),
      m_parent( parent )
{
    // setup toolbar
    setAllowedAreas( Qt::AllToolBarAreas );
    setObjectName( QString( "Compatible Modules" ) );

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
    addActions( compatibles );
    insertDummyButton();
}

WQtCombinerToolbar::WQtCombinerToolbar( WMainWindow* parent )
    : QToolBar( "Compatible Modules", parent ),
      m_parent( parent )
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

    // reserve size
    insertDummyButton();
}

WQtCombinerToolbar::~WQtCombinerToolbar()
{
}

void WQtCombinerToolbar::makeEmpty()
{
    clear();
    insertDummyButton();
}

void WQtCombinerToolbar::updateButtons( const WQtCombinerActionList& compatibles )
{
    clear();
    addActions( compatibles );
    insertDummyButton();
}

void WQtCombinerToolbar::insertDummyButton()
{
    // this is needed to ensure a constant height or width (if toolbar is in vertical mode). We use a label as an empty label has no graphical
    // representation. The width and height are set both because we do not know a priori how the toolbar is oriented as it might change after
    // creation.
    QLabel* dummy = new QLabel;
    dummy->setFixedWidth( 50 );
    dummy->setFixedHeight( 30 );
    addWidget( dummy );
}
