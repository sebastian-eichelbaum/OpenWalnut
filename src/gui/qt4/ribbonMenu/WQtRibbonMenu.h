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

#ifndef WQTRIBBONMENU_H
#define WQTRIBBONMENU_H

#include <map>

#include <QtGui/QTabWidget>
#include <QtGui/QToolBar>

#include "WQtMenuPage.h"
#include "../guiElements/WQtPushButton.h"
/**
 * implementation of a ribbon menu like widget
 */
class WQtRibbonMenu  : public QToolBar
{
public:
    /**
     * default constructor
     * \param parent the parent widget of this widget, i.e. the widget that manages it.
     */
    explicit WQtRibbonMenu( QWidget* parent );

    /**
     * destructor
     */
    virtual ~WQtRibbonMenu();

    /**
     * Adds a new tab to the ribbon menu and gives it a name.
     * \param name The name of the tab.
     * \param persistent Determines whether the new will be removed when removing non-persistent tabs.
     */
    WQtMenuPage* addTab( QString name, bool persistent = true );

    /**
     * Adds a push button to the menu. The button is identifiable by its name.
     * \param name a name for the new button to identifie it
     * \param tabName the name of the tab to which the button is added.
     * \param icon and icon for the button
     * \param label The optional text that is displayed besides the icon on the button.
     */
    WQtPushButton* addPushButton( QString name, QString tabName, QIcon icon, QString label = 0 );

    /**
     * Get a button by its name.
     * \param name The name that identifies the button.
     */
    WQtPushButton* getButton( QString name );

    /**
     * Removes tabs that are not marked persistent.
     */
    void clearNonPersistentTabs();
protected:
private:
    std::map< QString, WQtMenuPage*> m_tabList; //!< List holding the widgets representing the tabs of the menu.

    std::map< QString, WQtPushButton*> m_buttonList; //!< List holding the buttons that appear in the menu.

    QTabWidget* m_tabWidget; //!< The tab widget beeing the basis for the ribbon menu.
};

#endif  // WQTRIBBONMENU_H
