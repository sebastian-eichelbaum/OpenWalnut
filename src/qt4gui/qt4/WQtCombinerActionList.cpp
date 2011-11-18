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

#include "WQtCombinerActionList.h"

WQtCombinerActionList::WQtCombinerActionList( QWidget* parent, WIconManager* icons, WCombinerTypes::WCompatiblesList compatibles,
                                                              const WQtModuleConfig* exclusionPredicate, bool advancedText ):
    QList< QAction* >()
{
    // create an action for each group:
    for( WCombinerTypes::WCompatiblesList::iterator groups = compatibles.begin(); groups != compatibles.end(); ++groups )
    {
        // check current prototype against whitelist and blacklist
        if( exclusionPredicate && exclusionPredicate->operator()( groups->first ) )
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

