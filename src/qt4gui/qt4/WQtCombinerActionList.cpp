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

#include <iostream>

#include <string>
#include <vector>

#include <QtGui/QMenu>

#include "core/common/WPreferences.h"
#include "core/kernel/combiner/WApplyCombiner.h"
#include "core/kernel/combiner/WModuleOneToOneCombiner.h"
#include "core/kernel/WModule.h"
#include "core/kernel/WModuleCombiner.h"
#include "guiElements/WQtModuleOneToOneCombinerAction.h"
#include "WMainWindow.h"
#include "WQtCombinerActionList.h"

WQtCombinerActionList::WQtCombinerActionList( QWidget* parent, WIconManager* icons, WCombinerTypes::WCompatiblesList compatibles,
                                              bool advancedText, bool ignoreWhiteList ):
    QList< QAction* >()
{
    // These modules will be allowed to be shown.
    std::string moduleWhiteListString;
    WPreferences::getPreference( "modules.whiteList", &moduleWhiteListString );
    std::vector< std::string > moduleWhiteList = string_utils::tokenize( moduleWhiteListString, "," );

    // These modules will be forbidden to be shown.
    std::string moduleBlackListString;
    WPreferences::getPreference( "modules.blackList", &moduleBlackListString );
    std::vector< std::string > moduleBlackList = string_utils::tokenize( moduleBlackListString, "," );

    // create an action for each group:
    for( WCombinerTypes::WCompatiblesList::iterator groups = compatibles.begin(); groups != compatibles.end(); ++groups )
    {
        // check current prototype against whitelist and blacklist
        if(
            !ignoreWhiteList &&                 // ignore the whitelist?
            moduleWhiteList.size() &&           // whitelist empty?
            ( ( std::find( moduleWhiteList.begin(), moduleWhiteList.end(), groups->first->getName() ) == moduleWhiteList.end() ) ||
            ( std::find( moduleBlackList.begin(), moduleBlackList.end(), groups->first->getName() ) != moduleBlackList.end() ) )
          )
        {
            continue;
        }
        // create a new action for this group
        WQtModuleOneToOneCombinerAction* group = new WQtModuleOneToOneCombinerAction( parent, icons, *groups->second.begin(), advancedText );

        group->setIconVisibleInMenu( true );
        push_back( group );

        // only add a sub menu if there are more than 1 items in the group
        if( ( *groups ).second.size() > 1 )
        {
            QMenu* groupMenu = new QMenu( parent );
            // iterate all the children
            for( WCombinerTypes::WOneToOneCombiners::iterator combiner = ( *groups ).second.begin();
                                                               combiner != ( *groups ).second.end(); ++combiner )
            {
                WQtModuleOneToOneCombinerAction* a = new WQtModuleOneToOneCombinerAction( parent, icons, *combiner, true );

                a->setIconVisibleInMenu( true );
                groupMenu->addAction( a );
            }
            group->setMenu( groupMenu );
        }
    }
}

WQtCombinerActionList::WQtCombinerActionList( QWidget* parent, WIconManager* icons, WCombinerTypes::WDisconnectList disconnects ):
    QList< QAction* >()
{
    // create an action for each group:
    for( WCombinerTypes::WDisconnectList::iterator groups = disconnects.begin(); groups != disconnects.end(); ++groups )
    {
        // create a new action for this group
        WQtModuleOneToOneCombinerAction* group = new WQtModuleOneToOneCombinerAction( parent, icons, *groups->second.begin(), true );

        group->setIconVisibleInMenu( true );
        push_back( group );

        // only add a sub menu if there are more than 1 items in the group
        if( ( *groups ).second.size() > 1 )
        {
            QMenu* groupMenu = new QMenu( parent );
            // iterate all the children
            for( WCombinerTypes::WOneToOneCombiners::iterator combiner = ( *groups ).second.begin();
                                                               combiner != ( *groups ).second.end(); ++combiner )
            {
                WQtModuleOneToOneCombinerAction* a = new WQtModuleOneToOneCombinerAction( parent, icons, *combiner, true );

                a->setIconVisibleInMenu( true );
                groupMenu->addAction( a );
            }
            group->setMenu( groupMenu );
        }
    }
}

WQtCombinerActionList::WQtCombinerActionList():
    QList< QAction* >()
{
    // do nothing here
}

WQtCombinerActionList::~WQtCombinerActionList()
{
    // cleanup
    clear();
}

