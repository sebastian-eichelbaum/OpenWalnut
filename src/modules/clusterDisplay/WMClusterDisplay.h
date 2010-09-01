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

#ifndef WMCLUSTERDISPLAY_H
#define WMCLUSTERDISPLAY_H

#include <queue>
#include <string>
#include <vector>

#include <osg/Geode>

#include "../../graphicsEngine/WGEManagedGroupNode.h"

#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"
#include "../../kernel/WModuleOutputData.h"

#include "WDendrogram.h"
#include "WOSGButton.h"
#include "WHierarchicalTree.h"

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
     * helper function to read a text file
     * \param fileName
     * \return string containing the file
     */
    std::vector< std::string > readFile( const std::string fileName );

    /**
     * helper function to load and parse a tree file, the tree is stored in the member variable m_tree
     * \param fileName
     */
    void loadTreeAscii( std::string fileName );

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
    void handleRoiChanged();

    /**
     * creates a label depending ont he current labeling setting
     *
     * \param id of the cluster to create the label for
     * \return string of the label
     */
    std::string createLabel( size_t id );

    /**
     * Input connector for a fiber dataset.
     */
    boost::shared_ptr< WModuleInputData< const WDataSetFibers > > m_fiberInput;

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
     * controls the display of the dendrogram overlay
     */
    WPropBool m_propShowDendrogram;

    WPropTrigger  m_readTriggerProp; //!< This property triggers the actual reading,
    WPropFilename m_propTreeFile; //!< The tree will be read from this file, i hope we will get a real load button some time


    /**
     * stores the tree object
     */
    WHierarchicalTree m_tree;

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

    osg::Camera* m_camera; //!< stores the camera object

    WDendrogram* m_dendrogramGeode; //!< stores the dendrogram geode

    osgWidget::WindowManager* m_wm; //!< stores a pointer to the window manager used for osg wdgets and overlay stuff

    bool m_widgetDirty; //!< true if the widgets need redrawing

    bool m_biggestClusterButtonsChanged; //!< true if the buttons for the biggest clusters need updating

    bool m_dendrogramDirty; //!< true if the dendrogram needs redrawing

    size_t m_rootCluster; //!< currently selected cluster + offset

    size_t m_labelMode; //!< indicates wheter the cluster number, size of custom data should be shown on labels

    int m_oldViewHeight; //!< stores the old viewport resolution to check whether a resize happened

    int m_oldViewWidth; //!< stores the old viewport resolution to check whether a resize happened

    /**
     * Node callback to change position and appearance of the plane within the OSG thread
     */
    class SafeUpdateCallback : public osg::NodeCallback
    {
    public: // NOLINT

        /**
         * Constructor.
         *
         * \param module just set the creating module as pointer for later reference.
         */
        explicit SafeUpdateCallback( WMClusterDisplay* module ): m_module( module )
        {
        };

        /**
         * operator () - called during the update traversal.
         *
         * \param node the osg node
         * \param nv the node visitor
         */
        virtual void operator()( osg::Node* node, osg::NodeVisitor* nv );

        /**
         * Pointer used to access members of the module to modify the node.
         * Please do not use shared_ptr here as this would prevent deletion of the module as the callback contains
         * a reference to it. It is safe to use a simple pointer here as callback get deleted before the module.
         */
        WMClusterDisplay* m_module;
    };
};

inline void WMClusterDisplay::SafeUpdateCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    m_module->updateWidgets();

    traverse( node, nv );
}

#endif  // WMCLUSTERDISPLAY_H