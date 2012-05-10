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

#include "core/kernel/WModule.h"

// forward declarations
class WCustomWidget;
class WDataSetDipole;
class WEEG2;
class WEEGSourceCalculator;
class WEEGViewHandler;
class WGEGroupNode;
class WROIBox;
template< class T > class WModuleInputData;

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
     * Get the icon for this module in XPM format.
     *
     * \return the icon.
     */
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
     * Input connector for dipoles of EEG data
     */
    boost::shared_ptr< WModuleInputData< WDataSetDipoles > > m_dipoles;

    /**
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    /**
     * Group for parameters that are normally adjusted using mouse actions.
     */
    WPropGroup m_manualNavigationGroup;

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
     * Property determining whetherwe only show the proof of concept or the real dipoles
     */
    WPropBool m_proofOfConcept;

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
     * The sensitivity of the color map as property. The color map ranges
     * from -m_colorSensitivity to +m_colorSensitivity in microvolt.
     */
    WPropDouble m_colorSensitivity;

    /**
     * event marking a special time position as WFlag
     */
    boost::shared_ptr< WFlag< boost::shared_ptr< WEEGEvent > > > m_event;

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
     * The ROI around the source dipole position at the time determined by
     * m_event.
     * Used to select the fibers around this dipole.
     */
    osg::ref_ptr< WROIBox > m_roi;

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
     * The time of the current event. Used to check whether the event changed.
     */
    double m_currentEventTime;

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
     * calculates a source position at a given time position
     */
    boost::shared_ptr< WEEGSourceCalculator > m_sourceCalculator;

    /**
     * Prepare textures for colormapping EEG signal
     */
    void createColorMap();

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
};

#endif  // WMEEGVIEW_H
