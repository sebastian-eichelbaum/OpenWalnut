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
#include <string>
#include <vector>

#include <QtGui/QAction>
#include <QtGui/QMenu>
#include <QtGui/QPushButton>

#include "../../common/WPreferences.h"

#include "WMainWindow.h"
#include "WQtToolBar.h"
#include "guiElements/WQtApplyModuleAction.h"

#include "WQtCombinerToolbar.h"

WQtCombinerToolbar::WQtCombinerToolbar( WMainWindow* parent, WModuleFactory::CompatiblesList compatibles )
    : QToolBar( "Compatible Modules", parent )
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
    // These modules will be allowed to be shown.
    std::string moduleWhiteListString;
    WPreferences::getPreference( "modules.whiteList", &moduleWhiteListString );
    std::vector< std::string > moduleWhiteList = string_utils::tokenize( moduleWhiteListString, "," );


    // cast and set
    setToolButtonStyle( static_cast< Qt::ToolButtonStyle >( compToolBarStyle ) );

    // create an action for each group:
    for ( WModuleFactory::CompatiblesList::const_iterator groups = compatibles.begin(); groups != compatibles.end(); ++groups )
    {
        if( moduleWhiteList.size()
            && std::find( moduleWhiteList.begin(), moduleWhiteList.end(), groups->first->getName() ) == moduleWhiteList.end() )
        {
            continue;
        }
        // create a new action for this group
        WQtApplyModuleAction* group = new WQtApplyModuleAction( this,
                                                                parent->getIconManager(),
                                                                *( *groups ).second.begin() );
        addAction( group );

        // only add a sub menu if there are more than 1 items in the group
        if ( ( *groups ).second.size() > 1 )
        {
            QMenu* groupMenu = new QMenu( this );
            // iterate all the children
            for ( WModuleFactory::CompatibleCombiners::const_iterator combiner = ( *groups ).second.begin();
                                                                      combiner != ( *groups ).second.end(); ++combiner )
            {
                WQtApplyModuleAction* a = new WQtApplyModuleAction( this,
                                                                    parent->getIconManager(),
                                                                    ( *combiner ),
                                                                    true );
                a->setIconVisibleInMenu( true );
                groupMenu->addAction( a );
            }
            group->setMenu( groupMenu );
        }
    }

    // The following makes the bar having button size from the beginning.
    QPushButton* dummyButton = new QPushButton;

    if ( ( parent->getToolbarPos() ==  Qt::TopToolBarArea ) || ( ( parent->getToolbarPos() ==  Qt::BottomToolBarArea ) ) )
    {
        dummyButton->setFixedWidth( 0 );
        dummyButton->setFixedHeight( 32 );
    }
    else
    {
        dummyButton->setFixedWidth( 48 );   // well this size must be more than 32 as there might be these little submenu arrows besides the icon
        dummyButton->setFixedHeight( 0 );
    }

    addWidget( dummyButton );
}

WQtCombinerToolbar::~WQtCombinerToolbar()
{
}

Qt::ToolBarArea WQtCombinerToolbar::getCompatiblesToolbarPos()
{
    int compatiblesToolbarPos = -1;
    WPreferences::getPreference( "qt4gui.compatiblesToolBarPos", &compatiblesToolbarPos );
    Qt::ToolBarArea pos = Qt::TopToolBarArea;
    switch ( compatiblesToolbarPos )
    {
        case 0:
            pos = Qt::TopToolBarArea;
            break;
        case 1:
            pos = Qt::BottomToolBarArea;
            break;
        case 2:
            pos = Qt::LeftToolBarArea;
            break;
        case 3:
            pos = Qt::RightToolBarArea;
            break;
        default:
            pos = WMainWindow::getToolbarPos();
            break;
    }

    return pos;
}

