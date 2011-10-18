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

#ifndef WQTTREEITEM_H
#define WQTTREEITEM_H

#include <string>
#include <vector>

#include <boost/signals2/connection.hpp>

#include <QtGui/QTreeWidgetItem>
#include <QtGui/QProgressBar>
#include <QtCore/QTimer>

#include "WTreeItemTypes.h"
#include "core/kernel/WModule.h"

/**
 * Base class for all items in the control panel tree.
 */
class WQtTreeItem: public QObject,
                   public QTreeWidgetItem
{
    Q_OBJECT

public:
    /**
     * Constructor creates an empty item.
     *
     * \param parent The widget managing this widget
     * \param module The represented module
     * \param type the type used for the treeitem. Used to identify the items.
     */
    WQtTreeItem( QTreeWidgetItem* parent, WTreeItemType type, boost::shared_ptr< WModule > module );

    /**
     * Destructor.
     */
    virtual ~WQtTreeItem();

    /**
     * Get for the module pointer.
     *
     * \return the pointer to the module associated with this item.
     */
    WModule::SPtr getModule();

    /**
     * Returns the name used for this tree item.
     *
     * \return the name.
     */
    std::string getName();

    /**
     * Gets called by the GUI whenever the module was removed from the root container and should now be deleted.
     */
    virtual void gotRemoved();

    /**
     * The name of the input represented by this item.
     *
     * \return handled input
     */
    std::string getHandledInput() const;

    /**
     * Sets the name of the input represented by this item.
     *
     * \param in the input name.
     */
    void setHandledInput( std::string in );

    /**
     * The name of the output represented by this item.
     *
     * \return handled output
     */
    std::string getHandledOutput() const;

    /**
     * Sets the name of the output represented by this item.
     *
     * \param out the output name.
     */
    void setHandledOutput( std::string out );

    /**
     * Handle changes in check state.
     */
    virtual void handleCheckStateChange();

public slots:

    /**
     * Gets called by m_updateTimer in some interval to update the item state, basing on the state of m_module.
     */
    void update();

protected:

    /**
     * Updates the state of the tree item basing on the module's state.
     */
    virtual void updateState();

    /**
     * Updates this item in regular intervals.
     */
    boost::shared_ptr< QTimer > m_updateTimer;

    /**
     * Name of the tree item.
     */
    std::string m_name;

    /**
     * Method updates the tooltip of the tree item.
     *
     * \param progress the current progress as string.
     */
    void updateTooltip( std::string progress );

    /**
     * True if the treeitem and the module gets deleted currently.
     */
    bool m_deleteInProgress;

    /**
     * True if no delete event has been posted yet.
     */
    bool m_needPostDeleteEvent;

    /**
     * The property containing the name of the module.
     */
    WPropString m_nameProp;

    /**
     * The name of the input connector represented by this item.
     */
    std::string m_handledInput;

    /**
     * The output name handled by this item
     */
    std::string m_handledOutput;

private:

    /**
     * The module represented by this tree item.
     */
    boost::shared_ptr< WModule > m_module;

    /**
     * Called when the name property changes.
     */
    void nameChanged();

    /**
     * The output connector update connections. The NEED to be disconnected on destruction manually!
     */
    std::vector< boost::signals2::connection > m_outputUpdateConnections;

    /**
     * Called on output-connector update.
     *
     * \param connector the connector that was updated.
     */
    void slotDataChanged( boost::shared_ptr< WModuleConnector > connector );
};

#endif  // WQTTREEITEM_H
