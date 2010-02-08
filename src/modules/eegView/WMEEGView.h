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

#ifndef WMEEGVIEW_H
#define WMEEGVIEW_H

#include <string>

#include "../../dataHandler/WEEG.h"
#include "../../graphicsEngine/WGEGroupNode.h"
#include "../../graphicsEngine/WEvent.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"


/**
 * Test module to open a new widget and display EEG data
 * \ingroup modules
 */
class WMEEGView : public WModule
{
public:
    /**
     * default constructor
     */
    WMEEGView();

    /**
     * destructor
     */
    virtual ~WMEEGView();

    /**
     * Due to the prototype design pattern used to build modules, this method
     * returns a new instance of this method.
     * NOTE: it should never be initialized or modified in some other way. A
     * simple new instance is required.
     *
     * \return the prototype used to create every module in OpenWalnut.
     */
    virtual boost::shared_ptr< WModule > factory() const;

    /**
     * Gets the name of this module.
     *
     * \return the name.
     */
    virtual const std::string getName() const;

    /**
     * Gets the description for this module.
     *
     * \return the description
     */
    virtual const std::string getDescription() const;

    /**
     * Determine what to do if a property was changed.
     *
     * \param propertyName Name of the property.
     */
    void slotPropertyChanged( std::string propertyName );

protected:
    /**
     * Initialize connectors in this function.
     */
    virtual void connectors();

    /**
     * Initialize properties in this function.
     */
    virtual void properties();

    /**
     * Gets called whenever a connector gets connected to the specified input.
     * Sets the m_dataChanged flag.
     */
    virtual void notifyConnectionEstablished(
        boost::shared_ptr< WModuleConnector > /*here*/, boost::shared_ptr< WModuleConnector > /*there*/ );

    /**
     * Gets called when the data on one input connector changed.
     * Sets the m_dataChanged flag.
     */
    virtual void notifyDataChange(
        boost::shared_ptr< WModuleConnector > /*input*/, boost::shared_ptr< WModuleConnector > /*output*/ );

    /**
     * Entry point after loading the module. Runs in separate thread.
     */
    virtual void moduleMain();

private:
    /**
     * Input connector for a EEG dataset
     */
    boost::shared_ptr< WModuleInputData< WEEG > > m_input;

    /**
     * Pointer to the loaded EEG dataset
     */
    boost::shared_ptr< WEEG > m_eeg;

    /**
     * Custom widget which is used by this module to display its data.
     */
    boost::shared_ptr< WCustomWidget > m_widget;

    /**
     * OSG node for all 2D stuff of this module. All other OSG nodes should be
     * placed as child to this node.
     */
    osg::ref_ptr< WGEGroupNode > m_rootNode2d;

    /**
     * OSG node for all 3D stuff of this module. All other OSG nodes should be
     * placed as child to this node.
     */
    osg::ref_ptr< WGEGroupNode > m_rootNode3d;

    /**
     * Bool flag which gets set when the data was changed.
     * The module threads waits for this flag and performs a redraw.
     */
    WBoolFlag m_dataChanged;

    /**
     * Bool flag to represent the state which is selected in the GUI.
     * The module thread waits for changes of this flag and hiddes/displays its
     * data according to it.
     */
    WBoolFlag m_isActive;

    /**
     * The current active-state. Whether the widget is open and usable.
     */
    bool m_wasActive;

    /**
     * Event marking a special time position
     */
    WEvent m_event;

    /**
     * Flag from WMarkHandler which contains a new event position.
     */
    WFlag< double >* m_eventPositionFlag;

    /**
     * Opens a custom widget and connects the m_node with it.
     *
     * \returns whether the custom widget could be opened successfully
     */
    bool openCustomWidget();

    /**
     * Disconnects the m_node from the custom widget and closes the widget.
     */
    void closeCustomWidget();

    /**
     * Removes all Nodes from m_rootNode and adds new ones based on the current
     * data stored in m_eeg.
     */
    void redraw();

    /**
     * Changes an WEvent to the given time position
     *
     * \param event the WEvent which should be changed
     * \param time the new time position
     */
    void updateEvent( WEvent* event, double time );
};

#endif  // WMEEGVIEW_H
