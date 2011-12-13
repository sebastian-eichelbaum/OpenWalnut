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

#ifndef WQTCOMMANDPROMPT_H
#define WQTCOMMANDPROMPT_H

#include <QtGui/QLineEdit>

/**
 * This is a toolbar. It provides a command prompt -like interface for adding, removing and connecting modules
 */
class WQtCommandPrompt: public QLineEdit
{
    Q_OBJECT
public:
    /**
     * Constructs the prompt.
     *
     * \param parent the parent widget of this widget, i.e. the widget that manages it.
     */
    explicit WQtCommandPrompt( QWidget* parent );

    /**
     * destructor
     */
    virtual ~WQtCommandPrompt();

signals:

    /**
     * Emited if the prompt should quit. On ESC or after commiting a command.
     */
    void done();

protected:
    /**
     * Converts the given key press event into a line edit action.
     *
     * \param event the key
     */
    virtual void keyPressEvent( QKeyEvent* event );

    /**
     * Event handler
     *
     * \param event the event
     *
     * \return true if event was handled.
     */
    bool event( QEvent* event );

private slots:
    /**
     * This aborts. The typed command will not be sent.
     */
    void abort();

    /**
     * Commit the current command.
     */
    void commit();

private:
};

#endif  // WQTCOMMANDPROMPT_H
