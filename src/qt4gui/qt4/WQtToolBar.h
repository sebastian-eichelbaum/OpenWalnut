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

#ifndef WQTTOOLBAR_H
#define WQTTOOLBAR_H

#include <list>

#include "guiElements/WQtPushButton.h"
#include "WQtToolBarBase.h"

class WMainWindow;

/**
 * This is a toolbar. Its main usage for now is the "compatible modules" toolbar
 */
class WQtToolBar : public WQtToolBarBase
{
public:
    /**
     * Constructs the toolbar.
     * \param title name of the toolbar.
     * \param parent the parent widget of this widget, i.e. the widget that manages it.
     */
    WQtToolBar( const QString & title, WMainWindow* parent );

    /**
     * destructor
     */
    virtual ~WQtToolBar();

    /**
     * Allows addition of new actions to the toolbar. See the Qt Doc of QToolBar for details. Actions have the advantage that they build a
     * uniform interface for menus, toolbars, buttons and menued toolbuttons.
     *
     * \param action the action to add.
     */
    void addAction( QAction* action );

    /**
     * Removes all buttons,
     */
    void clearButtons();

protected:
    /**
     * The list of widgets in this toolbar.
     */
    std::list< QAction* > m_actions;

private:
};

#endif  // WQTTOOLBAR_H
