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

#include <boost/shared_ptr.hpp>

#include <QtGui/QWidget>
#include <QtGui/QDockWidget>
#include <QtGui/QVBoxLayout>

#include "core/kernel/WModule.h"

#include "../WQtCombinerToolbar.h"
#include "layout/WNetworkLayout.h"
#include "WQtNetworkEditorView.h"
#include "WQtNetworkItem.h"
#include "WQtNetworkScene.h"

// forward declaration
class WMainWindow;
class WNetworkLayout;

/**
 * Container widget to hold the WQtNetworkScene
 */
class WQtNetworkEditor : public QDockWidget
{
    Q_OBJECT
public:
    /**
     * constructor
     *
     * \param parent The widget that manages this widget
     */
    explicit WQtNetworkEditor( WMainWindow* parent = 0 );

    /**
     * destructor.
     */
    virtual ~WQtNetworkEditor();

    /**
     * Simple search the WQtNetworkItem that belongs to the WModule
     *
     * \param module a WModule
     * \return WQtNetworkItem belongs to the WModule
     */
    WQtNetworkItem* findItemByModule( boost::shared_ptr< WModule > module );

    /**
     * Connect SIGNALS with SLOTS
     */
    void connectSlots();

    /**
     * Query a list of selected items.
     *
     * \return the list
     **/
    QList< QGraphicsItem* > selectedItems() const;

    /**
     * Clears the selection.
     */
    void clearSelection();

    /**
     * Select the item representing the given module. If module not found or NULL, nothing is selected.
     *
     * \param module the module
     */
    void selectByModule( WModule::SPtr module );

    /**
     * Returns the current scene.
     *
     * \return the scene
     */
    WQtNetworkScene* getScene();
protected:
    /**
     * Reference to the main window of the application.
     */
    WMainWindow* m_mainWindow;

    /**
     * Everytime a module is associated, ready, connected, disconnected, removed or
     * deleted the kernels emits a signal and here we look how to behave.
     *
     * \param event the event that should be handled here.
     * \return if event was accepted or not
     */
    virtual bool event( QEvent* event );

private:
    WQtNetworkScene* m_scene; //!< QGraphicsScene

    WNetworkLayout* m_layout; //!< the object that handels the layout

    QList< WQtNetworkItem* > m_items; //!< a list of the WQtNetworkItems in the WQtNetworkScene

    /**
     * Action which uses a compatibles list (submenu) to connect a selected item with other existing modules.
     */
    QAction* m_connectWithModuleAction;

    /**
     * Action which uses a compatibles list (submenu) to connect a selected item with other prototypes.
     */
    QAction* m_connectWithPrototypeAction;

    /**
     * Action which disconnects a connector from the module.
     */
    QAction* m_disconnectAction;

    /**
     * The view controlling several scene transformations.
     */
    WQtNetworkEditorView* m_view;

private slots:

    /**
     * Determines possible Connections and the propertytab.
     */
    void selectItem();

    /**
     * Called by a timer to allow updates of all module items.
     */
    void updateCylce();
};

#endif  // WQTNETWORKEDITOR_H
