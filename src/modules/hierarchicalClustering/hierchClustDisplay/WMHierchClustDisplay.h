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

//---------------------------------------------------------------------------
//
// Project: hClustering
//
// Whole-Brain Connectivity-Based Hierarchical Parcellation Project
// David Moreno-Dominguez
// d.mor.dom@gmail.com
// moreno@cbs.mpg.de
// www.cbs.mpg.de/~moreno//
// This file is also part of OpenWalnut ( http://www.openwalnut.org ).
//
// hClustering is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// http://creativecommons.org/licenses/by-nc/3.0
//
// hClustering is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
//---------------------------------------------------------------------------

#ifndef WMHIERCHCLUSTDISPLAY_H
#define WMHIERCHCLUSTDISPLAY_H

#include <queue>
#include <string>
#include <vector>
#include <map>

#include <osg/Geode>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIEventHandler>

#include "core/dataHandler/WDataHandler.h"
#include "core/dataHandler/WDataSetScalar.h"
#include "core/dataHandler/WDataSetVector.h"
#include "core/dataHandler/WSubject.h"
#include "core/dataHandler/WValueSet.h"
#include "core/graphicsEngine/WGETexture.h"
#include "core/graphicsEngine/WGEManagedGroupNode.h"
#include "core/graphicsEngine/WGEGeodeUtils.h"
#include "core/graphicsEngine/widgets/WOSGButton.h"
//DEPRECATED #include "core/graphicsEngine/WPickHandler.h"
#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"
#include "core/graphicsEngine/shaders/WGEShader.h"

#include "WHtree.h"
#include "WHtreeProcesser.h"
#include "WHtreePartition.h"
#include "WNonBinDendroGeode.h"

const unsigned int MASK_2D = 0xF0000000; //!< used for osgWidget processes
const unsigned int MASK_3D = 0x0F000000; //!< used for osgWidget processes


/**
 * Module to explore hierarchical parcellations of brain data
 * The module loads a tree file from disk, and allows tree processing, visualization of seed voxel locations onto anatomy image
 * exploration of the hierarchical tree, partition selection, and along with partition2Mesh module the projection of partitions
 * back to the brain surface
 *
 * \ingroup modules
 */
class WMHierchClustDisplay: public WModule
{
public:
    //! Constructor
    WMHierchClustDisplay();

    //! Destructor
    virtual ~WMHierchClustDisplay();

    //! Gives back the name of this module.
    //! \return the module's name.
    virtual const std::string getName() const;

    //! Gives back a description of this module.
    //! \return module description.
    virtual const std::string getDescription() const;

    /**
     * new instance of this module
     * Due to the prototype design pattern used to build modules, this method returns a new instance of this method. NOTE: it
     * should never be initialized or modified in some other way. A simple new instance is required.
     * \return the prototype used to create every module in OpenWalnut.
     */
    virtual boost::shared_ptr< WModule > factory() const;

protected:
    //! Entry point after loading the module. Runs in separate thread.
    virtual void moduleMain();

    //! Initialize the connectors this module is using.
    virtual void connectors();

    //! Initialize the properties for this module.
    virtual void properties();

private:
    // === PRIVATE MEMBER FUNCTIONS ===
    /**
     * Small event handler class to catch left mouse buttons clicks in the main view.
     */
    class MainViewEventHandler : public osgGA::GUIEventHandler
    {
    public:
        //! signal from a mouse button being clicked
        typedef boost::signals2::signal< bool ( WVector2f ) > mouseClickSignalType;

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
        void subscribeLeftButtonPush( mouseClickSignalType::slot_type slot );

        /**
         * Registers a function slot to RIGHT BUTTON PUSH events. Whenever the event occurs, the slot is called with current parameters.
         *
         * \param slot Function object having the appropriate signature according to the used SignalType.
         */
        void subscribeRightButtonPush( mouseClickSignalType::slot_type slot );

    private:
        /**
         * Signal used for notification of the LEFT BUTTON PUSH event.
         */
        mouseClickSignalType m_signalLeftButtonPush;
        /**
         * Signal used for notification of the RIGHT BUTTON PUSH event.
         */
        mouseClickSignalType m_signalRightButtonPush;
    };



    /**
     * updates property min/max values after loading a clustering file
     */
    void initProperties();

    /**
     * loads the tree file into the tree object
     * \param treeFile the tree file path
     * \return true if the tree was successfully loaded, false otherwise
     */
    bool loadTree( boost::filesystem::path treeFile );

    /**
     * initializes properties depending on the specific tree structure loaded
     */
    void initTreeData();

    /**
     * recursive function that fills the display colors for a whole branch with a selected color
     * \param root cluster to work on
     * \param color color to paint
     */
    void colorBranch( const size_t &root, const WColor &color );

    /**
     * recursive function that fills the display colors for a whole branch with a custom color from a scheme previously generated
     * \param root cluster to work on
     * \param color color to paint
     */
    void colorBranchCustom( const size_t &root, const WColor &color );

    /**
     * creates a texture, assigning the colouring of the current partition to the respective seed voxels in the
     * displayed anatomy texture. (makes the partitioning visivle in the slide view (usually T1 )
     */
    void initTexture();

    /**
     * renders the triangulated seed voxels
     */
    void renderCubeMesh();

    /**
     * inits the graphical widgets in the 3d scene
     */
    void initWidgets();

    /**
     * updates the graphical widgets in the 3d scene
     */
    void updateWidgets();

    /**
     * handles mouse clicks into the dendrogram
     * \return true if handled
     */
    bool widgetClicked();

    /**
     * handles clicks into the dendrogram
     * \param pos the mouse position during the click
     * \return true if it handled the click
     */
    bool dendrogramClick( const WVector2f& pos );
    //DEPRECATED    void dendrogramClick( WPickInfo pickInfo );

    /**
     * handles right clicks into the dendrogram
     * \param pos the mouse position during the click
     * \return true if it handled the click
     */
    bool dendrogramClickRight( const WVector2f& pos );

    /**
     * after the active cluster list has changed this function updates the texture, cubeMesh, dendrogram and output
     * \param preloadedColors a vector with cluster colors preloaded form the tree file
     */
    void updateSelection( const std::vector< WColor > &preloadedColors = std::vector< WColor >() );

    /**
     * updates the selection colors
     */
    void updateColors();

    /**
     * updates the voxels coordinate output
     */
    void updateOutput2();

    /**
     * writes partition selection(s) in an ascii file
     */
    void writePartition();

    /**
     * Assigns a default color to each node travelling the tree top-down based on dividing first the biggest cluster
     * where the biggest children keeps the color of the parent basing the algorithm on cluster size
     */
    void initializeColorsSize();

    /**
     * Assigns a default color to each node travelling the tree top-down based on hierarchy
     * where the biggest children keeps the color of the parent basing the algorithm on cluster size
     */
    void initializeColorsHdist();

    /**
     * Uses the provided node Id and color to update the current tree default coloring using hierarchical information
     * up: each parent node will be recolored to this color as long as it is the biggest of the children
     * down: the biggest child at each division will be also recolored to this
     * \param thisID ID of the reference node
     * \param thisColor color of the reference node
     */
    void assignColorHierch( const size_t thisID, const WColor thisColor );

    /**
     * manages cluster selection action response
     */
    void clusterSelection();

    /**
     * manages action after the trigger of the color section has been pressed
     */
    void colorAction();

    /**
     * Adds partitions form current selections into the tree object
     */
    void addPartitions();

    /**
     * recolors added partitions from the tree object with current selection set of colors
     */
    void recolorPartitions();

    // === PRIVATE MEMBER DATA ===

    boost::shared_ptr< WModuleInputData< WDataSetSingle > > m_input;    //!< An input connector that accepts order 1 datasets.
    boost::shared_ptr< WModuleOutputData< WDataSetVector > > m_output;    //!< An output connector for the output scalar dsataset
    boost::shared_ptr< WModuleOutputData< WDataSetScalar > > m_output2;    //!< An output connector for the output coordinate vector.
    boost::shared_ptr< WDataSetVector > m_outData;    //!< This is a pointer to the current output1.
    boost::shared_ptr< WDataSetScalar > m_outData2;    //!< This is a pointer to the current output2.

    osgWidget::WindowManager* m_wm;    //!< stores a pointer to the window manager used for osg wdgets and overlay stuff
    osg::Camera* m_camera;    //!< stores the camera object
    int m_oldViewHeight;     //!< stores the old viewport resolution height to check whether a resize happened
    int m_oldViewWidth;    //!< stores the old viewport resolution width to check whether a resize happened

    osg::ref_ptr< WGEGroupNode > m_moduleNode;  //!< Pointer to the modules group node.
    osg::ref_ptr< WGEGroupNode > m_dendrogramNode;  //!< Pointer to the dendrogram group node.
    osg::ref_ptr< WGEGroupNode > m_meshNode;    //!< Pointer to the cube mesh group node.

    /**
     * The shader
     */
    osg::ref_ptr< WGEShader > m_shader;

    boost::shared_ptr< WDataSetSingle > m_anatomy;    //!< This is a pointer to the anatomy dataset the module is currently working on.
    osg::ref_ptr<osg::Texture3D> m_texture;    //!< stores a pointer to the texture we paint in (to overlay to anatomy)
    std::vector< size_t >m_textureLabels;    //!< label vector for texture creation
    boost::shared_ptr< WGridRegular3D > m_grid;    //!< stores a pointer to the grid we use;

    WHtree m_tree;    //!< stores the tree object as loaded from file;
    bool m_treeDirty;    //!< true if the dendrogram needs redrawing
    bool m_labelsDirty;    //!< true if the labels need redrawing
    size_t m_treeZoomRoot;    //!< control variable for tree zoom mode

    WNonBinDendroGeode* m_dendrogramGeode;    //!< stores the dendrogram geode
    float m_horizontalLine; //!< to draw a horizontal line when theres distance partition cutting

    std::vector< WColor >m_nodeColorsCustom; //!< stores preset color for every tree node, so clusters keep same color through different selection
    std::vector< WColor >m_treeDisplayColors; //!< stores a the current colors each tree node should be displayed, given the current selection
    std::map< size_t, WColor >m_selectionColors; //!< stores a the current colors each cluster should be displayed, given the current selection
    std::map< size_t, WColor >m_selectionColorsBackup; //!< stores a backup of m_selectionColors

    std::vector<std::vector<size_t> > m_clusterVoxels; //!< stores the contained seed voxel IDs for each cluster, increases speed for texture drawing
    std::vector<size_t> m_discardedVoxels; //!< stores the contained seed voxel IDs for discarded elements

    std::vector<size_t>m_selectionClusters; //!< stores the currently activated clusters
    std::vector<size_t>m_selectionLeaves; //!< stores the leaves output by the selection

    boost::shared_ptr< WTriangleMesh > m_cubeMesh; //!< Cube-like triangulation of the seed voxels
    boost::shared_ptr< WTriangleMesh > m_discardedMesh; //!< Cube-like triangulation of the discarded voxels

    std::vector< osg::ref_ptr<WOSGButton> >m_selectionLabels; //!< list of buttons for the active cluster selection

    // === MODULE PROPERTIES ===

    boost::shared_ptr< WCondition > m_propTriggerChange; //!< A condition used to notify about changes in several properties.

    WPropGroup m_groupInfoSelected; //!< grouping the selected info
    WPropInt m_infoSelectedID; //!< Info property: clusters in current partition
    WPropInt m_infoSelectedSize; //!< Info property: clusters in current partition
    WPropInt m_infoSelectedDistance; //!< Info property: clusters in current partition

    WPropGroup m_groupInfoPartition; //!< grouping the partition info
    WPropInt m_infoPartNumber; //!< Info property: clusters in current partition
    WPropInt m_infoPartActive; //!< Info property: active clusters in current partition
    WPropInt m_infoPartLeaves; //!< Info property: leaf clusters in current partition
    WPropDouble m_infoCutValue; //!< Info property: cut value for current partition

    WPropGroup m_groupInfoTree; //!< grouping the tree info
    WPropString m_infoTreeName; //!< Info property: current tree name
    WPropInt m_infoCountLeaves; //!< Info property: number of leaves
    WPropInt m_infoCountNodes; //!< Info property: number of nodes
    WPropInt m_infoCountDiscarded; //!< Info property: number of discarded voxels
    WPropInt m_infoMaxLevel; //!< Info property: maximum hierarchical level
    WPropDouble m_infoCpcc; //!< Info property: cpcc value

    WPropGroup m_groupInfoMessage; //!< grouping the message info
    WPropString m_infoPartMessage; //!< Info property: partition output message

    WPropGroup m_groupRead; //!< grouping the tree loading
    WPropFilename m_propReadFilename; //!< The tree file will be loaded form this directory
    WPropTrigger  m_propReadTreeTrigger; //!< This property triggers the actual reading of the tree file

    WPropGroup m_groupDendrogram; //!< grouping the dendrogram manipulation properties
    WPropBool m_propShowDendrogram; //!< controls the display of the dendrogram overlay
    WPropBool m_propDendroPlotByLevel; //!< controls plotting the height of a join (1 plots the tree by hierarchical level, 0 by cluster distance)
    WPropBool m_propTriangleLeaves; //!< specifies a minimum size for a cluster so that too small cluster won't get an own color
    WPropTrigger m_propDendroZoomIn; //!< zoom into tree, sets m_treeZoom true and m_zoomRoot to the currently selected cluster
    WPropTrigger m_propDendroZoomOut; //!< zooms out, m_treeZoom = false, dendrogram shows the whole tree
    boost::shared_ptr< WItemSelection > m_propDendroSideList; //!< A list of dendrogram positions
    WPropSelection m_propDendroSideSelector; //!< Selection property for dendrogram positions
    WPropGroup m_groupDendroManual; //!< grouping the dendrogram manual resizing properties
    WPropInt m_propDendroSizeX; //!< controls the width of the dendrogram
    WPropInt m_propDendroSizeY; //!< controls the height of the dendrogram
    WPropInt m_propDendroOffsetX; //!< controls the horizontal origin of the dendrogram
    WPropInt m_propDendroOffsetY; //!< controls the vertical origin of the dendrogram

    WPropGroup m_groupPartition; //!< grouping the different selection methods
    WPropTrigger m_propPartRoot; //!< triggers the selection of root cluster
    WPropInt m_propSourceCluster; //!< the current subtree root cluster
    WPropSelection m_propPartitionSelector; //!< Selection property for clusters
    boost::shared_ptr< WItemSelection > m_propPartitionSelectionList; //!< A list of cluster selection methods
    WPropSelection m_propConditionSelector; //!< Selection condition for clusters
    boost::shared_ptr< WItemSelection > m_propConditionSelectionsList; //!< A list of condition selection methods
    WPropInt m_propPartNumClusters; //!< number of clusters of the desired partition
    WPropInt m_propPartSizeValue; //!< size cut value for the desired partition
    WPropInt m_propPartSearchDepthValue; //!< depth of search for optimized partitioning
    WPropDouble m_propPartRelativeValue; //!< Relative size cut value for the desired partition
    WPropDouble m_propPartDistanceValue; //!< distance cut value for the desired partition
    WPropInt m_propPartHlevelValue; //!< hierarchical level cut value for the desired partition
    WPropInt m_propPreloadPartitionNr; //!< predefined partition to load
    WPropBool m_propPartExcludeLeaves; //!< defines if base nodes will be subdivided or not
    WPropTrigger m_propDoPartition; //!< triggers the cluster selection update

    WPropGroup m_groupVisualization; //!< grouping the different visualization options
    WPropInt m_propSubselectedCluster; //!< the currently selected cluster
    WPropColor m_propSubselectedcolor; //!< color for subselected cluster
    boost::shared_ptr< WItemSelection > m_propColorSchemeList; //!< List of color schemes
    WPropSelection m_propColorSchemeSelector; //!< selection of color schemes
    boost::shared_ptr< WItemSelection > m_propColorActionList; //!< List of color schemes
    WPropSelection m_propColorActionSelector; //!< selection of color schemes
    WPropTrigger m_propColorActionTrigger; //!< shuffle current colors
    WPropBool m_propShowLabels; //!< show info labels for selected clusters
    boost::shared_ptr< WItemSelection > m_propLabelList; //!< List of label contents
    WPropSelection m_propLabelInfoSelector; //!< selection of info on button labels
    WPropBool m_propShowDiscarded; //!< visualize also discarded voxels
    WPropBool m_propShowCubeMesh; //!< visualize voxel cube triangulation
    WPropBool m_propBoundaries; //!< visualize only parcel boundaries
    WPropColor m_propDiscardedColor; //!< color for discarded voxels
    WPropColor m_propBoundaryColor; //!< color for cluster boundaries
    WPropColor m_propInactiveColor; //!< color for inactive mesh voxels

    WPropGroup m_groupTreeProcess; //!< grouping the different tree processing methods
    WPropSelection m_propProcessSelector; //!< Selection property for processing
    boost::shared_ptr< WItemSelection > m_propProcessSelectionsList; //!< A list of cluster selection methods
    WPropInt m_propSafeSize; //!< maximum size to be pruned
    WPropInt m_propPruneJoinSize; //!< join size for pruning process
    WPropDouble m_propPruneSizeRatio; //!< size ratio for pruning process
    WPropDouble m_propDistanceGap; //!< distance gap for prunin process
    WPropDouble m_propFlatLimit; //!< distance gap for prunin process
    WPropBool m_propKeepBases; //!< dont debinarize basenodes
    WPropInt m_propSmoothSize; //!< desired cluster size fro smoothing
    WPropInt m_propPrunedNumber; //!< number of leaves to be pruned by random pruning
    WPropInt m_propPrunedSeed; //!< rng seed for random pruning
    WPropInt m_propCoarseRatio; //!< ratio of decimation
    WPropTrigger m_propDoProcessing; //!< triggers the tree processing

    WPropGroup m_groupSavedPartition; //!< grouping the partition options
    WPropTrigger  m_propAddPartTrigger; //!< This property triggers the inclusion in the tree of the current partition
    WPropTrigger  m_propRecolorPartTrigger; //!< This property triggers the recoloring partition of the current partitions
    WPropTrigger  m_propClearPartsTrigger; //!< This property triggers the clearing of current partitions in the tree

    WPropGroup m_groupWrite; //!< grouping the tree writing
    WPropFilename m_propWriteTreeFilename; //!< The tree file will be written to this location
    WPropTrigger  m_propWriteTreeTrigger; //!< This property triggers the actual writing of the tree file
    WPropFilename m_propPartitionFile; //!< The partition file will be written to this location
    WPropTrigger  m_propPartitionsTrigger; //!< This property triggers the actual writing of the partition file
};

#endif  // WMHIERCHCLUSTDISPLAY_H


