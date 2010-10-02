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

#ifndef WQTNETWORKEDITOR_H
#define WQTNETWORKEDITOR_H

#include <string>
#include <list>

#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsView>
#include <QtGui/QWidget>
#include <QtGui/QDockWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QListWidget>

#include "../../../kernel/modules/data/WMData.h"
#include "WQtNetworkScene.h"
#include "WQtNetworkItem.h"

// forward declaration
class WMainWindow;

/**
 * container widget to hold the qgraphicsscene
 *
 */
class WQtNetworkEditor : public QDockWidget
{
    Q_OBJECT

public:
    /**
     * default constructor
     *
     * \param title Title will be displayed above the widget
     * \param parent The widget that manages this widget
     */
    explicit WQtNetworkEditor( WMainWindow* parent = 0 );

    /**
     * destructor.
     */
    virtual ~WQtNetworkEditor();

    /**
     * Simple method to create static test "modules"
     */
    void addModule( WModule *module );

    WQtNetworkItem* findItemByModule( WModule *module );

protected:

    /**
     * Reference to the main window of the application.
     */
    WMainWindow* m_mainWindow;

    virtual bool event( QEvent* event );

private:

    WQtNetworkScene* m_scene; //!< QGraphicsScene

    QGraphicsView* m_view; //!< QGraphicsView

    QWidget* m_panel; //!< panel

    QVBoxLayout* m_layout; //!< layout

    std::list< WQtNetworkItem* > m_items;
};

#endif  // WQTNETWORKEDITOR_H
