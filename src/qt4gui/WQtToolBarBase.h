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

#ifndef WQTTOOLBARBASE_H
#define WQTTOOLBARBASE_H

#include <QtGui/QToolBar>
#include <QtGui/QMenu>

class WMainWindow;
class WSettingMenu;

/**
 * Base class for toolbars.
 */
class WQtToolBarBase: public QToolBar
{
    Q_OBJECT
public:
    /**
     * Constructs the toolbar.
     * \param title name of the toolbar.
     * \param parent the parent widget of this widget, i.e. the widget that manages it.
     */
    WQtToolBarBase( const QString & title, WMainWindow* parent );

    /**
     * Destructor.
     */
    virtual ~WQtToolBarBase();

    /**
     * Returns a menu for styling the menu items. All the handling is done internally. Just use the menu.
     *
     * \param title the title of the menu.
     *
     * \return the menu
     */
    QMenu* getStyleMenu( QString title = QString( "Toolbar Style" ) ) const;

protected:
private:
    /**
     * The main window parent.
     */
    WMainWindow* m_mainWindow;

    /**
     * The options for toolbar style.
     */
    WSettingMenu* m_styleOptionMenu;

private slots:

    /**
     * Used to update the style of this toolbar.
     *
     * \param index the new index in the option list.
     */
    void handleStyleUpdate( unsigned int index );
};

#endif  // WQTTOOLBARBASE_H

