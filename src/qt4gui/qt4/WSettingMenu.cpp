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

#include <QtGui/QMessageBox>

#include "WQt4Gui.h"
#include "WMainWindow.h"

#include "WSettingMenu.h"
#include "WSettingMenu.moc"

WSettingMenu::WSettingMenu( QWidget* parent, std::string settingName, std::string menuName, std::string tooltip, unsigned int defaultValue,
                            const QList< QString >& options, bool showRestartInfo ):
    QMenu( QString::fromStdString( menuName ), parent ),
    m_settingName( QString::fromStdString( settingName ) ),
    m_showRestartInfo( showRestartInfo )
{
    m_currentItem = WQt4Gui::getSettings().value( m_settingName, defaultValue ).toUInt();

    // setup this menu
    setToolTip( QString::fromStdString( tooltip ) );

    QActionGroup* actionGroup = new QActionGroup( parent );

    // add an action for each option to the menu and the action group
    unsigned int i = 0;
    for( QList< QString >::const_iterator iter = options.begin(); iter != options.end(); ++iter )
    {
        QAction* action = new QAction( *iter, parent );
        action->setCheckable( true );
        action->setData( i );
        action->setActionGroup( actionGroup );
        addAction( action );

        // is this currently active?
        if( i == m_currentItem )
        {
            action->setChecked( true );
        }

        ++i;
    }

    // get notified about changes
    connect( actionGroup, SIGNAL( triggered( QAction* ) ), this, SLOT( handleUpdate( QAction* ) ) );
}

WSettingMenu::~WSettingMenu()
{
}

void WSettingMenu::handleUpdate( QAction* action )
{
    // update setting and emit signal
    m_currentItem = action->data().toUInt();
    WQt4Gui::getSettings().setValue( m_settingName, m_currentItem );

    // does this setting require restart?
    if( m_showRestartInfo )
    {
        QMessageBox::information( WQt4Gui::getMainWindow(), QString( "Restart required" ), QString( "This setting is applied after restart." ) );
    }

    emit change( m_currentItem );
}

unsigned int WSettingMenu::get() const
{
    return m_currentItem;
}

