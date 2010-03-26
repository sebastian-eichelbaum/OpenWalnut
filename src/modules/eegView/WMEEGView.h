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

#include <osg/Texture1D>
#include <osgSim/ScalarsToColors>

#include "../../dataHandler/WEEG2.h"
#include "../../graphicsEngine/WEvent.h"
#include "../../graphicsEngine/WGEGroupNode.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"
#include "WEEGViewHandler.h"


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

    virtual const char** getXPMIcon() const;

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
    boost::shared_ptr< WModuleInputData< WEEG2 > > m_input;

    /**
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    /**
     * Property determining whether electode positions should be drawn.
     */
    WPropBool m_drawElectrodes;

    /**
     * Property determining whether the head surface should be drawn.
     */
    WPropBool m_drawHeadSurface;

    /**
     * Property determining whether electrode labels should be drawn.
     */
    WPropBool m_drawLabels;

    /**
     * the width of the label display in pixel as property
     */
    WPropInt m_labelsWidth;

    /**
     * the time position in seconds where to start the graph at the left edge as
     * property
     */
    WPropDouble m_timePos;

    /**
     * the width of the graph in seconds as property
     */
    WPropDouble m_timeRange;

    /**
     * the width of the graph in pixel as property
     */
    WPropInt m_graphWidth;

    /**
     * the y position in pixel at the lower edge as property
     */
    WPropDouble m_yPos;

    /**
     * the distance between two curves of the graph in pixel as property
     */
    WPropDouble m_ySpacing;

    /**
     * the sensitivity of the graph in microvolt per pixel as property
     */
    WPropDouble m_ySensitivity;

    /**
     * Pointer to the loaded EEG dataset
     */
    boost::shared_ptr< WEEG2 > m_eeg;

    /**
     * Custom widget which is used by this module to display its data.
     */
    boost::shared_ptr< WCustomWidget > m_widget;

    /**
     * GUI event handler used for interactive changing of many properties
     */
    osg::ref_ptr< WEEGViewHandler > m_handler;

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
     * OSG node for the 3D display of the electrode positions
     */
    osg::ref_ptr< osg::Node > m_electrodesNode;

    /**
     * OSG node for the 3D display of the head surface
     */
    osg::ref_ptr< osg::Node > m_headSurfaceNode;

    /**
     * OSG node for the 3D display of the electrode labels
     */
    osg::ref_ptr< osg::Node > m_labelsNode;

    /**
     * Bool flag which gets set when the data was changed.
     * The module threads waits for this flag and performs a redraw.
     */
    WBoolFlag m_dataChanged;

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
     * A ScalarsToColors object mapping the potentials at the electrodes to
     * colors. Used for the display of electrode positions and the head surface.
     */
    osg::ref_ptr< osgSim::ScalarsToColors > m_colorMap;

    /**
     * A 1D texture containing the color map as image. Used for the
     * interpolation on the head surface.
     */
    osg::ref_ptr< osg::Texture1D > m_colorMapTexture;

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
     * Removes all Nodes from m_rootNode2d and m_rootNode3d and adds new ones
     * based on the current data stored in m_eeg.
     */
    void redraw();

    /**
     * Draw the electrode positions in 3D.
     *
     * \return an OSG Node containing the electrode positions
     */
    osg::ref_ptr< osg::Node > drawElectrodes();

    /**
     * Draw the head surface in 3D.
     *
     * \return an OSG Node containing the head surface
     */
    osg::ref_ptr< osg::Node > drawHeadSurface();

    /**
     * Draw the electrode labels in 3D.
     *
     * \return an OSG Node containing the electrode labels
     */
    osg::ref_ptr< osg::Node > drawLabels();

    /**
     * Changes an WEvent to the given time position
     *
     * \param event the WEvent which should be changed
     * \param time the new time position
     */
    void updateEvent( WEvent* event, double time );

    /**
     * OSG Update Callback to change the color of a ShapeDrawable when an event
     * position changed.
     * \note Only add it to a ShapeDrawable!
     */
    class UpdateColorOfShapeDrawableCallback : public osg::Drawable::UpdateCallback
    {
    public:
        /**
         * Constructor
         *
         * \param channelID the number of the channel
         * \param event the event on which the ShapeDrawable updates
         * \param eeg the WEEG dataset
         * \param colorMap the object mapping the electrode potentials to colors
         */
        UpdateColorOfShapeDrawableCallback( size_t channelID,
                                            const WEvent* event,
                                            boost::shared_ptr< const WEEG2 > eeg,
                                            osg::ref_ptr< const osgSim::ScalarsToColors > colorMap );

        /**
         * Callback method called by the NodeVisitor.
         * Changes the color of the drawable according to the event.
         *
         * \param drawable The drawable this callback is connected to. Should be
         *                 a ShapeDrawable.
         */
        virtual void update( osg::NodeVisitor* /*nv*/, osg::Drawable* drawable );

    protected:
    private:
        /**
         * The number of the channel
         */
        const size_t m_channelID;

        /**
         * The time position which is currently used.
         * The color is updated if the new time from the m_event is different to
         * this.
         */
        double m_currentTime;

        /**
         * The event on which the ShapeDrawable updates
         */
        const WEvent* const m_event;

        /**
         * The WEEG dataset
         */
        const boost::shared_ptr< const WEEG2 > m_eeg;

        /**
         * The ScalarsToColors object mapping the potentials at the electrodes
         * to colors.
         */
        const osg::ref_ptr< const osgSim::ScalarsToColors > m_colorMap;
    };

    /**
     * OSG Update Callback to change the color of a Geometry by changing its 1D
     * texture coordinates when an event position changed.
     * \note Only add it to a Geometry with a 1D texture!
     */
    class UpdateColorOfGeometryCallback : public osg::Drawable::UpdateCallback
    {
    public:
        /**
         * Constructor
         *
         * \param event the event on which the Geometry updates
         * \param eeg the WEEG dataset
         */
        UpdateColorOfGeometryCallback( const WEvent* event, boost::shared_ptr< const WEEG2 > eeg );

        /**
         * Callback method called by the NodeVisitor.
         * Changes the color of the drawable according to the event.
         *
         * \param drawable The drawable this callback is connected to. Should be
         *                 a Geometry with a 1D texture.
         */
        virtual void update( osg::NodeVisitor* /*nv*/, osg::Drawable* drawable );

    protected:
    private:
        /**
         * The time position which is currently used.
         * The color is updated if the new time from the m_event is different to
         * this.
         */
        double m_currentTime;

        /**
         * The event on which the Geometry updates
         */
        const WEvent* const m_event;

        /**
         * The WEEG dataset
         */
        const boost::shared_ptr< const WEEG2 > m_eeg;
    };
};

#endif  // WMEEGVIEW_H
