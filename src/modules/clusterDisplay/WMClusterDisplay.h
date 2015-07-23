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

#ifndef WMCLUSTERDISPLAY_H
#define WMCLUSTERDISPLAY_H

#include <string>
#include <vector>

#include <osg/Geode>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIEventHandler>

#include "core/common/WHierarchicalTreeFibers.h"
#include "core/graphicsEngine/geodes/WDendrogramGeode.h"
#include "core/graphicsEngine/WFiberDrawable.h"
#include "core/graphicsEngine/WGEManagedGroupNode.h"
#include "core/graphicsEngine/widgets/WOSGButton.h"
#include "core/graphicsEngine/WGECamera.h"
#include "core/kernel/WFiberSelector.h"
#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"

const unsigned int MASK_2D = 0xF0000000; //!< used for osgWidget stuff
const unsigned int MASK_3D = 0x0F000000; //!< used for osgWidget stuff

/**
 * Module offers several fiber selection and coloring options depending on a hierarchical clustering
 *
 * \ingroup modules
 */
class WMClusterDisplay: public WModule
{
public:
    /**
     * constructor
     */
    WMClusterDisplay();

    /**
     * destructor
     */
    virtual ~WMClusterDisplay();

    /**
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Gives back a description of this module.
     * \return description to module.
     */
    virtual const std::string getDescription() const;

    /**
     * Due to the prototype design pattern used to build modules, this method returns a new instance of this method. NOTE: it
     * should never be initialized or modified in some other way. A simple new instance is required.
     *
     * \return the prototype used to create every module in OpenWalnut.
     */
    virtual boost::shared_ptr< WModule > factory() const;

    /**
     * Get the icon for this module in XPM format.
     * \return The icon.
     */
    virtual const char** getXPMIcon() const;

protected:
    /**
     * Entry point after loading the module. Runs in separate thread.
     */
    virtual void moduleMain();

    /**
     * Initialize the connectors this module is using.
     */
    virtual void connectors();

    /**
     * Initialize the properties for this module.
     */
    virtual void properties();


private:
    /**
     * Small event handler class to catch left mouse buttons clicks in the main view.
     */
    class MainViewEventHandler : public osgGA::GUIEventHandler
    {
    public:
        /**
         * Signal for handling left button signals
         */
        typedef boost::signals2::signal< bool ( WVector2f ) > LeftButtonPushSignalType;

        /**
         * The OSG calls this function whenever a new event has occured.
         *
         * \param ea Event class for storing GUI events such as mouse or keyboard interation etc.
         *
         * \return true if the event was handled.
         */
        bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& /* aa */ );

        /**
         * Registers a function slot to LEFT BUTTON PUSH events. Whenever the event occurs, the slot is called with current parameters.
         *
         * \param slot Function object having the appropriate signature according to the used SignalType.
         */
        void subscribeLeftButtonPush( LeftButtonPushSignalType::slot_type slot );

    private:
        /**
         * Signal used for notification of the LEFT BUTTON PUSH event.
         */
        LeftButtonPushSignalType m_signalLeftButtonPush;
    };

    /**
     * helper function to read a text file
     * \param fileName
     * \return string containing the file
     */
    std::vector< std::string > readFile( const std::string fileName );

    /**
     * helper function to load and parse a tree file, the tree is stored in the member variable m_tree
     * \param fileName
     *
     * \return true if successful.
     */
    bool loadTreeAscii( std::string fileName );

    /**
     * inits the cluster navigation widgets
     */
    void initWidgets();

    /**
     * updates all the overlay widgets within the osg thread
     */
    void updateWidgets();

    /**
     * function checks all on screen buttons if they have been clicked
     * \return true if a button was clicked
     */
    bool widgetClicked();

    /**
     * colors subclusters depending on slider settings
     * \param current root cluster to start from
     */
    void colorClusters( size_t current );

    /**
     * sets a set of cluster to a single color
     * \param clusters vector of clusters
     * \param color new color
     */
    void setColor( std::vector<size_t> clusters, WColor color );

    /**
     * function to handle user input
     */
    void handleSelectedClusterChanged();

    /**
     * function to handle user input
     */
    void handleOffsetChanged();

    /**
     * function to handle user input
     */
    void handleSelectedLevelChanged();

    /**
     * function to handle user input
     */
    void handleBiggestClustersChanged();

    /**
     * function to handle user input
     */
    void handleColoringChanged();

    /**
     * function to handle user input
     */
    void handleMinSizeChanged();


    /**
     * function to handle user input
     */
    void handleRoiChanged();

    /**
     * function to handle user input
     */
    void handleCreateRoi();

    /**
     * creates a label depending ont he current labeling setting
     *
     * \param id of the cluster to create the label for
     * \return string of the label
     */
    std::string createLabel( size_t id );

    /**
     * handles clicks into the dendrogram
     * \param pos the mouse position during the click
     * \return true if it handled the click
     */
    bool dendrogramClick( const WVector2f& pos );

    /**
     * helper function to initialize a fiber display node
     */
    void createFiberGeode();

    /**
     * Input connector for a fiber dataset.
     */
    boost::shared_ptr< WModuleInputData< const WDataSetFibers > > m_fiberInput;

    boost::shared_ptr< const WDataSetFibers > m_dataSet; //!< pointer to dataSet to be able to access it throughout the whole module.

    /**
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    /**
     * the currently selected cluster
     */
    WPropInt m_propSelectedCluster;

    /**
     * offset to the currently selected cluster, this allows tree navigation and return to the
     * current cluster
     */
    WPropInt m_propSelectedClusterOffset;

    /**
     * number of biggest sub cluster to determine and show
     */
    WPropInt m_propSubClusters;

    /**
     * number of subclusters to color differently
     */
    WPropInt m_propSubLevelsToColor;

    /**
     * specifies a minimum size for a cluster so that too small cluster won't get an own color
     */
    WPropInt m_propMinSizeToColor;

    /**
     * number of clusters that are selected by the current roi setting and meet the ratio condition
     */
    WPropInt m_propMaxSubClusters;

    /**
     * ratio of how many leafes of a cluster must be inside the roi setting to be considered
     */
    WPropDouble m_propBoxClusterRatio;

    /**
     * controls the display of the tree navigation widget
     */
    WPropBool m_propShowTree;

    /**
     * grouping the dendrogram manipulation properties
     */
    WPropGroup m_groupDendrogram;

    /**
     * controls the display of the dendrogram overlay
     */
    WPropBool m_propShowDendrogram;

    /**
     * if true position and size sliders will have no effect
     */
    WPropBool m_propResizeWithWindow;

    WPropInt m_propDendrogramSizeX; //!< controls the width of the dendrogram
    WPropInt m_propDendrogramSizeY; //!< controls the height of the dendrogram
    WPropInt m_propDendrogramOffsetX; //!< controls the horizontal origin of the dendrogram
    WPropInt m_propDendrogramOffsetY; //!< controls the vertical origin of the dendrogram


    WPropTrigger  m_readTriggerProp; //!< This property triggers the actual reading,
    WPropFilename m_propTreeFile; //!< The tree will be read from this file, i hope we will get a real load button some time

    /**
     * stores the tree object
     */
    WHierarchicalTreeFibers m_tree;

    /**
     * Point to a fiber selector, which is an adapater between the roi manager and the this module
     */
    boost::shared_ptr< WFiberSelector>m_fiberSelector;

    /**
     * stores pointer to the fiber drawer
     */
    osg::ref_ptr< WFiberDrawable > m_fiberDrawable;

    /**
     * The root node used for this modules graphics.
     */
    osg::ref_ptr< WGEManagedGroupNode > m_rootNode;

    std::vector<size_t>m_biggestClusters; //!< stores the currently selected biggest clusters

    /**
     * list of buttons for the tree widget
     */
    std::vector< osg::ref_ptr<WOSGButton> >m_treeButtonList;

    /**
     * list of buttons for biggest cluster selection
     */
    std::vector< osg::ref_ptr<WOSGButton> >m_biggestClustersButtonList;

    WGECamera* m_camera; //!< stores the camera object

    WDendrogramGeode* m_dendrogramGeode; //!< stores the dendrogram geode

    osgWidget::WindowManager* m_wm; //!< stores a pointer to the window manager used for osg wdgets and overlay stuff

    bool m_widgetDirty; //!< true if the widgets need redrawing

    bool m_biggestClusterButtonsChanged; //!< true if the buttons for the biggest clusters need updating

    bool m_dendrogramDirty; //!< true if the dendrogram needs redrawing

    size_t m_rootCluster; //!< currently selected cluster + offset

    size_t m_labelMode; //!< indicates wheter the cluster number, size of custom data should be shown on labels

    int m_oldViewHeight; //!< stores the old viewport resolution to check whether a resize happened

    int m_oldViewWidth; //!< stores the old viewport resolution to check whether a resize happened
};

#endif  // WMCLUSTERDISPLAY_H
