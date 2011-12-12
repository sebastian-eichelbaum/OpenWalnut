//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV-Leipzig and CNCF-CBS
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

#ifndef WMCLUSTERDISPLAYVOXELS_H
#define WMCLUSTERDISPLAYVOXELS_H

#include <queue>
#include <string>
#include <vector>

#include <osg/Geode>

#include "core/common/WHierarchicalTreeVoxels.h"
#include "core/dataHandler/WDataHandler.h"
#include "core/dataHandler/WDataSetScalar.h"
#include "core/graphicsEngine/WGETexture.h"
#include "core/dataHandler/WSubject.h"
#include "core/dataHandler/WValueSet.h"
#include "core/graphicsEngine/geodes/WDendrogramGeode.h"
#include "core/graphicsEngine/WGEManagedGroupNode.h"
#include "core/graphicsEngine/widgets/WOSGButton.h"
#include "core/graphicsEngine/WPickHandler.h"
#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"

const unsigned int MASK_2D = 0xF0000000; //!< used for osgWidget stuff
const unsigned int MASK_3D = 0x0F000000; //!< used for osgWidget stuff

typedef enum
{
    CDV_SINGLE,
    CDV_BIGGEST,
    CDV_X,
    CDV_SIMILARITY,
    CDV_LEVELSFROMTOP,
    CDV_MINBRANCHLENGTH,
    CDV_LOADED
}
CDV_DISPLAYMODE;

/**
 * Allows one to display and interact with clustered voxel data.
 * \ingroup modules
 */
class WMClusterDisplayVoxels: public WModule
{
public:

    /**
     *
     */
    WMClusterDisplayVoxels();

    /**
     *
     */
    virtual ~WMClusterDisplayVoxels();

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
     * loads and parses the clustering text file
     * \param clusterFile to the clustering file
     * \return true if a meta file was successfully loaded, false otherwise
     */
    bool loadClustering( boost::filesystem::path clusterFile );

    /**
     * creates a new texture
     */
    void createTexture();

    /**
     * function to handle user input
     */
    void handleMinSizeChanged();

    /**
     * after the active cluster list has changed this function updates the texture, triangulation and dendrogram
     */
    void updateAll();

    /**
     * creates a triangulation from the selected clusters
     */
    void createMesh();

    /**
     * renders the triangulated clusters
     */
    void renderMesh();

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
     *
     * \return true if handled
     */
    bool widgetClicked();

    /**
     * updates property min/max values after loading a clustering file
     */
    void setPropertyBoundaries();

    /**
     * listenes to the pickhandler and determines if a click into the dendrogram happened
     * \param pickInfo the pickInfo object as sent out by the pickhandler
     */
    void dendrogramClick( WPickInfo pickInfo );

    /**
     * update the output connector on demand, for performance reasons this is not done every time
     * a change to the cluster selection is applied
     */
    void updateOutDataset();

    /**
     * helper function to set the label on the in scene buttons depending on which labeling scheme is selected
     */
    void setButtonLabels();

    /**
     * recursive function that initalizes a color for for every cluster, the biggest of the two children of a cluster
     * inherits the color for its parent
     * \param root cluster to work on
     * \param index color index of the parent
     */
    void initColors( size_t root, size_t index );


    /**
     * An input connector that accepts order 1 datasets.
     */
    boost::shared_ptr< WModuleInputData< WDataSetSingle > > m_input;

    /**
     * An output connector for the output scalar dsataset.
     */
    boost::shared_ptr< WModuleOutputData< WDataSetScalar > > m_output;

    /**
     * This is a pointer to the dataset the module is currently working on.
     */
    boost::shared_ptr< WDataSetSingle > m_dataSet;

    /**
     * Point to the out dataset once it is invalid. Used to deregister from the datahandler
     */
    boost::shared_ptr< WDataSetScalar > m_outDataOld;

    /**
     * This is a pointer to the current output.
     */
    boost::shared_ptr< WDataSetScalar > m_outData;

    /**
     * stores a pointer to the texture we paint in
     */
    osg::ref_ptr<osg::Texture3D>m_texture;

    /**
     * stores a pointer to the texture we paint in
     */
//    osg::ref_ptr< WGETexture3D > m_texture;


    /**
     * label vector for texture creation
     */
    std::vector< size_t >m_data;

    /**
     * stores a pointer to the grid we use;
     */
    boost::shared_ptr< WGridRegular3D > m_grid;

    /**
     * the currently active display mode
     */
    CDV_DISPLAYMODE m_currentDisplayMode;

    /**
     * the currently active display mode as string
     */
    std::string m_currentDisplayModeString;

    /**
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    WPropTrigger  m_propReadTrigger; //!< This property triggers the actual reading,
    WPropFilename m_propClusterFile; //!< The png files will be loaded form this directory

    /**
     * the currently selected cluster
     */
    WPropInt m_propSelectedCluster;

    /**
     * the currently selected cluster
     */
    WPropInt m_propXBiggestClusters;

    /**
     * the similarity value for selecting clusters
     */
    WPropDouble m_propValue;

    /**
     * property to select a loaded partition
     */
    WPropInt m_propSelectedLoadedPartion;

    /**
     * how many levels to go down from top
     */
    WPropInt m_propLevelsFromTop;

    /**
     * how many levels to go down from top
     */
    WPropInt m_propXClusters;


    /**
     * minimum branch length
     */
    WPropDouble m_propMinBranchLength;

    /**
     * minimum branch size
     */
    WPropInt m_propMinBranchSize;

    /**
     * show or hide outliers
     */
    WPropBool m_propHideOutliers;

    /**
     * show or hide outliers
     */
    WPropBool m_propShowSelectedButtons;

    /**
     * specifies a minimum size for a cluster so that too small cluster won't get an own color
     */
    WPropInt m_propMinSizeToColor;

    /**
     * grouping the different selection methods
     */
    WPropGroup m_groupSelection;

    /**
     * grouping the dendrogram manipulation properties
     */
    WPropGroup m_groupDendrogram;

    /**
     * grouping the dendrogram manipulation properties
     */
    WPropGroup m_groupTriangulation;

    /**
     * controls the display of the dendrogram overlay
     */
    WPropBool m_propShowDendrogram;

    /**
     * controls plotting the height of a join
     */
    WPropBool m_propPlotHeightByLevel;

    /**
     * if true position and size sliders will have no effect
     */
    WPropBool m_propResizeWithWindow;

    WPropTrigger m_propZoomIn; //!< zoom into tree, sets m_zoom true and m_zoomRoot to the currently selected cluster
    WPropTrigger m_propZoomOut; //!< zooms out, m_zoom = false, dendrogram shows the whole tree
    size_t m_zoomRoot; //!< control variable for zoom mode
    bool m_zoom; //!< control variable for zoom mode

    WPropInt m_propDendrogramSizeX; //!< controls the width of the dendrogram
    WPropInt m_propDendrogramSizeY; //!< controls the height of the dendrogram
    WPropInt m_propDendrogramOffsetX; //!< controls the horizontal origin of the dendrogram
    WPropInt m_propDendrogramOffsetY; //!< controls the vertical origin of the dendrogram

    /**
     * if true position and size sliders will have no effect
     */
    WPropBool m_propShowVoxelTriangulation;

    /**
     * if true clusters not in the current selection list will be rendered grey in the texture and triangulation
     */
    WPropBool m_showNotInClusters;

    WHierarchicalTreeVoxels m_tree; //!< the tree object as loaded from the file

    osg::ref_ptr< WGEGroupNode > m_moduleNode; //!< Pointer to the modules group node.
    osg::ref_ptr< WGEGroupNode > m_dendrogramNode; //!< Pointer to the dendrogram group node.
    osg::ref_ptr< WGEGroupNode > m_meshNode; //!< Pointer to the mesh group node.

    std::vector<osg::ref_ptr< osg::Geode > > m_outputGeodes; //!< a vector of dendrogram nodes

    std::vector<boost::shared_ptr< WTriangleMesh > >m_triMeshes; //!< triangulation of the active clusters

    boost::shared_ptr< WTriangleMesh >m_nonActiveMesh; //!< triangulation of the voxels not in active clusters

    std::vector<size_t>m_activatedClusters; //!< stores the currently activated clusters

    /**
     * list of buttons for the active cluster selection
     */
    std::vector< osg::ref_ptr<WOSGButton> >m_activeClustersButtonList;

    osg::Camera* m_camera; //!< stores the camera object

    WDendrogramGeode* m_dendrogramGeode; //!< stores the dendrogram geode

    osgWidget::WindowManager* m_wm; //!< stores a pointer to the window manager used for osg wdgets and overlay stuff

    bool m_selectionChanged; //!< true if the selection changed and widgets need redrawing

    bool m_dendrogramDirty; //!< true if the dendrogram needs redrawing

    size_t m_rootCluster; //!< currently selected cluster + offset

    size_t m_labelMode; //!< indicates wheter the cluster number, size of custom data should be shown on labels

    int m_oldViewHeight; //!< stores the old viewport resolution to check whether a resize happened

    int m_oldViewWidth; //!< stores the old viewport resolution to check whether a resize happened

    WPropInt m_infoCountLeafes; //!< info property
    WPropInt m_infoCountClusters; //!< info property
    WPropInt m_infoMaxLevel; //!< info property

    std::vector< std::vector<size_t> >m_loadedPartitions; //!< set partitions loaded from file

    /**
     * updates the output connector on demand, as we don't want to do this every paint command
     */
    WPropTrigger m_buttonUpdateOutput;

    /**
     * A list of cluster selection methods
     */
    boost::shared_ptr< WItemSelection > m_clusterSelectionsList;

    /**
     * Selection property for clusters
     */
    WPropSelection m_clusterSelection;

    /**
     * triggers the cluster selection update
     */
    WPropTrigger m_buttonExecuteSelection;

    /**
     * A list of button labels
     */
    boost::shared_ptr< WItemSelection > m_buttonLabelList;

    /**
     * Selection property for button labels
     */
    WPropSelection m_buttonLabelSelection;

    /**
     * stores a preset color for every cluster, so clusters can keep the same color through different selection methods
     */
    std::vector< WColor >m_clusterColors;

    size_t m_colorIndex; //!< temp variable for the initColors function
};

#endif  // WMCLUSTERDISPLAYVOXELS_H
