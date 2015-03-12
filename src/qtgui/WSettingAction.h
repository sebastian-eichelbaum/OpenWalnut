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

#ifndef WSETTINGACTION_H
#define WSETTINGACTION_H

#include <string>

#include <QAction>
#include <QMessageBox>

#ifndef Q_MOC_RUN
// There is an issue with the moc compiler and some boost headers where
// moc for qt versions < 5.0 is not able to do a macro substitution.
// The issue occurs in the boost library versions 1.48 and higher,
// and is tested in boost version 1.52 using moc version 4.8.4
// Excluding the relevant headers removes the problem.
#include "WQtGui.h"
#endif

class WMainWindow;

/**
 * Class to handle a certain setting with an action. The action is associated with a QSettings value. Please note, that this class is not able to
 * handle async changes in the QSettings object. So, try to avoid multiple actions for one setting.
 */
class WSettingAction: public QAction
{
    Q_OBJECT

public:
    /**
     * Constructs an action which handles a certain setting.
     *
     * \param parent the parent handling this
     * \param settingName the name of the setting to handle
     * \param actionName the name of the action
     * \param defaultValue the default if setting not existing
     * \param tooltip the tooltip text
     * \param showRestartInfo if true, info dialog is shown which says that a restart is needed
     * \param shortcut the shortcut keysequence
     */
    WSettingAction( QObject* parent, std::string settingName, std::string actionName, std::string tooltip, bool defaultValue,
                    bool showRestartInfo = false, const QKeySequence& shortcut = 0 );

    /**
     * Destructor.
     */
    virtual ~WSettingAction();

    /**
     * Gets the current state.
     *
     * \return state
     */
    bool get() const;

signals:

    /**
     * This signal is emitted if this setting changes.
     *
     * \param value the new setting value.
     */
    void change( bool value );

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

private slots:

    /**
     * The state has changed by the action. Handled here.
     *
     * \param state the state
     */
    void stateChange( bool state );
};

#endif  // WSETTINGACTION_H

