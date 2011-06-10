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

#ifndef WSETTINGMENU_H
#define WSETTINGMENU_H

#include <string>

#include <QtGui/QMenu>

/**
 * Similar to WSettingAction, this can handle a multi-option setting as QMenu.
 */
class WSettingMenu: public QMenu
{
    Q_OBJECT
public:

    /**
     * Constructs a menu with the specified options and automatically handles updates.
     *
     * \param parent the parent widget owning this menu
     * \param settingName the name of the setting tohandle
     * \param menuName the menu's name
     * \param tooltip the tooltip for the menu
     * \param defaultValue a initial default item
     * \param options a list of exclusive options
     * \param showRestartInfo if true, show an info dialog that changes get applied on next start.
     */
    WSettingMenu( QWidget* parent, std::string settingName, std::string menuName, std::string tooltip, unsigned int defaultValue,
                  const QList< QString >& options, bool showRestartInfo = false );

    /**
     * Destructor.
     */
    virtual ~WSettingMenu();

    /**
     * Returns the current setting.
     *
     * \return current setting
     */
    unsigned int get() const;

signals:

    /**
     * Signal getting emitted if the selected option changes.
     *
     * \param index the new index.
     */
    void change( unsigned int index );

protected:

private:
    /**
     * The name of the setting handled here.
     */
    QString m_settingName;

    /**
     * If true, a change of the setting causes an restart notification dialog.
     */
    bool m_showRestartInfo;

    /**
     * Tracks the currently selected option.
     */
    unsigned int m_currentItem;

private slots:

    /**
     * Handles updates in the option.
     *
     * \param action the triggered action.
     */
    void handleUpdate( QAction* action );
};

#endif  // WSETTINGMENU_H

