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

#ifndef WQTCOMMANDPROMPTTOOLBAR_H
#define WQTCOMMANDPROMPTTOOLBAR_H

#include <QtGui/QToolBar>

class WQtCommandPrompt;
class WMainWindow;

/**
 * This is a toolbar. It provides a command prompt -like interface for adding, removing and connecting modules
 */
class WQtCommandPromptToolbar: public QToolBar
{
    Q_OBJECT
public:
    /**
     * Constructs the toolbar.
     * \param title name of the toolbar.
     * \param parent the parent widget of this widget, i.e. the widget that manages it.
     */
    explicit WQtCommandPromptToolbar( const QString& title, WMainWindow* parent );

    /**
     * destructor
     */
    virtual ~WQtCommandPromptToolbar();

protected:
private slots:

    /**
     * Toggles the prompt toolbar
     */
    void show();

    /**
     * Exit prompt
     */
    void exit();

private:
    /**
     * The main window parent.
     */
    WMainWindow* m_mainWindow;

    /**
     * The actual prompt edit.
     */
    WQtCommandPrompt* m_prompt;
};

#endif  // WQTCOMMANDPROMPTTOOLBAR_H
