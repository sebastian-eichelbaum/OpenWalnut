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

#include <cassert>
#include <iostream>
#include <vector>
#include <map>

#include "WQtRibbonMenu.h"


WQtRibbonMenu::WQtRibbonMenu( QWidget* parent )
    : QToolBar( parent )
{
    this->setAllowedAreas( Qt::TopToolBarArea | Qt::BottomToolBarArea );

    m_tabWidget = new QTabWidget();
    this->addWidget( m_tabWidget );
}

WQtRibbonMenu::~WQtRibbonMenu()
{
}


WQtMenuPage* WQtRibbonMenu::addTab( QString name, bool persistent )
{
    // don't add a tab with the same name twice
    assert( m_tabList.count( name ) == 0 );

    WQtMenuPage* tab = new WQtMenuPage( name, persistent );

    m_tabWidget->addTab( tab, name );

    m_tabList[name] = tab;

    return tab;
}

WQtPushButton* WQtRibbonMenu::addPushButton( QString name, QString tabName, QIcon icon, QString label )
{
    assert( m_tabList.count( tabName ) == 1 );
    assert( m_buttonList.count( name ) == 0 );

    WQtPushButton* button = new WQtPushButton( icon, label );

    button->setName( name );

    m_tabList[tabName]->addButton( button );

    m_buttonList[name] = button;

    return button;
}

WQtPushButton* WQtRibbonMenu::getButton( QString name )
{
    assert( m_buttonList.count( name ) == 1 );
    return m_buttonList[name];
}

void WQtRibbonMenu::clearNonPersistentTabs()
{
    for ( std::map< QString, WQtMenuPage*>::iterator iter = m_tabList.begin(); iter != m_tabList.end(); ++iter )
    {
        int idx = m_tabWidget->indexOf( ( *iter ).second );
        if ( !( *iter ).second->isPersistent() )
        {
            std::vector< QString > buttons = ( *iter ).second->getButtons();

            for ( std::vector< QString >::iterator bIter = buttons.begin(); bIter != buttons.end(); ++bIter )
            {
                m_buttonList.erase( *bIter );
            }
            m_tabWidget->removeTab( idx );
            m_tabList.erase( iter );
        }
    }
}

