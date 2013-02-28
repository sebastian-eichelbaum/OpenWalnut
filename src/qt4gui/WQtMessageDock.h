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

#ifndef WQTMESSAGEDOCK_H
#define WQTMESSAGEDOCK_H

#include <QtGui/QDockWidget>
#include <QtGui/QListWidget>
#include <QtGui/QComboBox>

#include "core/common/WLogEntry.h"

#include "guiElements/WQtDockWidget.h"
#include "WQtMessagePopup.h"

/**
 * Dock widget for showing messages and logs
 */
class WQtMessageDock: public WQtDockWidget
{
    Q_OBJECT
public:
    /**
     * Constructor
     *
     * \param dockTitle the title
     * \param parent parent widget
     */
    WQtMessageDock( QString dockTitle, QWidget* parent );

    /**
     * Destructor.
     */
    virtual ~WQtMessageDock();

    /**
     * Add a message to the dock
     *
     * \param title the title
     * \param message the message
     * \param type the type
     */
    void addMessage( QString title, QString message, WQtMessagePopup::MessageType type );

    /**
     * Add a message to the dock
     *
     * \param sender who sent the message
     * \param message the message
     * \param type the type
     */
    void addLogMessage( QString sender, QString message, WQtMessagePopup::MessageType type );

    /**
     * Add message to the dock
     *
     * \param entry use this log entry
     */
    void addLogMessage( const WLogEntry& entry );

    /**
     * Save state to settings
     */
    void saveSettings();

protected slots:
    /**
     * Handles changes in the filter combo
     */
    void handleFilterUpdate();
private:
    /**
     * The list
     */
    QListWidget* m_logList;

    /**
     * The message filter
     */
    QComboBox* m_filterCombo;
};

#endif  // WQTMESSAGEDOCK_H

