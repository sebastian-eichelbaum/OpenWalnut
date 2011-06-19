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

#include "WMainWindow.h"

#include "WSettingAction.h"
#include "WSettingAction.moc"

WSettingAction::WSettingAction( QObject* parent, std::string settingName, std::string actionName, std::string tooltip, bool defaultValue,
                                bool showRestartInfo, const QKeySequence& shortcut ):
    QAction( QString::fromStdString( actionName ), parent ),
    m_settingName( QString::fromStdString( settingName ) ),
    m_showRestartInfo( showRestartInfo )
{
    // set the user-specified shortcut
    QList<QKeySequence> shortcuts;
    shortcuts.push_back( shortcut );
    setShortcuts( shortcuts );

    // binary action
    setCheckable( true );

    setToolTip( QString::fromStdString( tooltip ) );

    // set with current value
    setChecked( WQt4Gui::getSettings().value( QString::fromStdString( settingName ), defaultValue ).toBool() );

    // handle the change
    connect( this, SIGNAL( toggled( bool ) ), this, SLOT( stateChange( bool ) ) );
}

WSettingAction::~WSettingAction()
{
}

bool WSettingAction::get() const
{
    return isChecked();
}

void WSettingAction::stateChange( bool state )
{
    // store the value
    WQt4Gui::getSettings().setValue( m_settingName, state );

    // does this setting require restart?
    if( m_showRestartInfo )
    {
        QMessageBox::information( WQt4Gui::getMainWindow(), QString( "Restart required" ), QString( "This setting is applied after restart." ) );
    }

    emit change( state );
}

