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

#include <queue>
#include <string>
#include <vector>
#include <utility>
#include <map>
#include <algorithm>
#include <list>

#include <boost/regex.hpp>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIEventHandler>
#include <osgGA/TrackballManipulator>
#include <osgGA/StateSetManipulator>
#include <osgViewer/ViewerEventHandlers>
#include <osgWidget/Util> //NOLINT
#include <osgWidget/ViewerEventHandlers> //NOLINT
#include <osgWidget/WindowManager> //NOLINT
#include <osg/LightModel>

#include "core/common/WStringUtils.h"
#include "core/common/WPathHelper.h"
#include "core/common/WPropertyHelper.h"
#include "core/common/algorithms/WMarchingLegoAlgorithm.h"
#include "core/graphicsEngine/WGEColormapping.h"
#include "core/graphicsEngine/WGEUtils.h"
#include "core/kernel/WKernel.h"
#include "core/kernel/WSelectionManager.h"
#include "core/kernel/WROIManager.h"
//#include "core/ui/WUIViewWidget.h"


#include "WMHierchClustDisplay.h"


bool WMHierchClustDisplay::MainViewEventHandler::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& /* aa */ )
{
//    wlog::debug( "WMHierchClustDisplay::MainViewEventHandler" ) << "handle";
    if( ea.getEventType() == osgGA::GUIEventAdapter::PUSH && ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON )
    {
        return ( true == m_signalLeftButtonPush( WVector2f( ea.getX(), ea.getY() ) ) );
    }
    if( ea.getEventType() == osgGA::GUIEventAdapter::PUSH && ea.getButton() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON )
    {
        return ( true == m_signalRightButtonPush( WVector2f( ea.getX(), ea.getY() ) ) );
    }
    return false;
}

void WMHierchClustDisplay::MainViewEventHandler::subscribeLeftButtonPush( mouseClickSignalType::slot_type slot )
{
    m_signalLeftButtonPush.connect( slot );
}

void WMHierchClustDisplay::MainViewEventHandler::subscribeRightButtonPush( mouseClickSignalType::slot_type slot )
{
    m_signalRightButtonPush.connect( slot );
}

WMHierchClustDisplay::WMHierchClustDisplay():
    WModule(),
    m_moduleNode( new WGEGroupNode() ),
    m_dendrogramNode( new WGEGroupNode() ),
    m_meshNode( new WGEGroupNode() ),
    m_treeDirty( false )
{
}

WMHierchClustDisplay::~WMHierchClustDisplay()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMHierchClustDisplay::factory() const
{
    // See "src/modules/template/" for an extensively documented example.
    return boost::shared_ptr< WModule >( new WMHierchClustDisplay() );
}

const std::string WMHierchClustDisplay::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "Hierarchical Clustering Display";
}

const std::string WMHierchClustDisplay::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    // See "src/modules/template/" for an extensively documented example.
    return "Visualizes hierarchical clustering in dendrogram tree, texture and 3D seed voxels view";
}

void WMHierchClustDisplay::connectors()
{
    // the input dataset is just used as source for resolurtion and transformation matrix
    m_input = boost::shared_ptr< WModuleInputData < WDataSetSingle  > >(
                new WModuleInputData< WDataSetSingle >( shared_from_this(), "in", "The input dataset." ) );
    addConnector( m_input );

    m_output = boost::shared_ptr< WModuleOutputData < WDataSetVector > >(
                new WModuleOutputData< WDataSetVector >( shared_from_this(), "colors out", "The extracted image." ) );
    addConnector( m_output );

    m_output2 = boost::shared_ptr< WModuleOutputData < WDataSetScalar  > >(
                new WModuleOutputData< WDataSetScalar >( shared_from_this(), "coords out", "The coordinate vector." ) );
    addConnector( m_output2 );

    WModule::connectors();
} // end connectors()

void WMHierchClustDisplay::properties()
{
    m_propTriggerChange = boost::shared_ptr< WCondition >( new WCondition() );

    // Info properties
    m_groupInfoSelected = m_infoProperties->addPropertyGroup( "Selected cluster", "" ); //NOLINT
    m_infoSelectedID = m_groupInfoSelected->addProperty( "ID: ", "", 0 );
    m_infoSelectedSize = m_groupInfoSelected->addProperty( "Size: ", "", 0 );
    m_infoSelectedDistance = m_groupInfoSelected->addProperty( "Distance: ", "", 0 );
    m_groupInfoPartition = m_infoProperties->addPropertyGroup( "Partition", "" ); //NOLINT
    m_infoPartNumber = m_groupInfoPartition->addProperty( "Partition clusters: ", "", 0 );
    m_infoPartActive = m_groupInfoPartition->addProperty( "Partition active nodes: ", "", 0 );
    m_infoPartLeaves = m_groupInfoPartition->addProperty( "Partition leaves: ", "", 0 );
    m_infoCutValue = m_groupInfoPartition->addProperty( "Cut value: ", "", 0.0 );
    m_groupInfoTree = m_infoProperties->addPropertyGroup( "Tree", "" ); //NOLINT
    m_infoTreeName = m_groupInfoTree->addProperty( "Current Tree: ", "", std::string() );
    m_infoCountLeaves = m_groupInfoTree->addProperty( "Total Leaves: ", "", 0 );
    m_infoCountNodes  = m_groupInfoTree->addProperty( "Total Nodes: ", "", 0 );
    m_infoCountDiscarded  = m_groupInfoTree->addProperty( "Total Discarded: ", "", 0 );
    m_infoMaxLevel = m_groupInfoTree->addProperty( "Max Hierarch. Level: ", "", 0 );
    m_infoCpcc = m_groupInfoTree->addProperty( "CPCC: ", "", 0.0 );
    m_groupInfoMessage = m_infoProperties->addPropertyGroup( "Operation Messages", "" );
    m_infoPartMessage = m_groupInfoMessage->addProperty( "Part",  "", std::string() );

    // Group Read
    m_groupRead = m_properties->addPropertyGroup( "Read file",  "Groups the tree loading options" ); //NOLINT
    m_propReadFilename = m_groupRead->addProperty( "Tree file", "", boost::filesystem::path( "" ) );
    m_propReadTreeTrigger = m_groupRead->addProperty( "Load tree",  "Load tree", WPVBaseTypes::PV_TRIGGER_READY, m_propTriggerChange );

    // Group Dendrogram
    m_groupDendrogram = m_properties->addPropertyGroup( "Dendrogram", "Properties only related to the dendrogram." );
    m_propShowDendrogram = m_groupDendrogram->addProperty( "Show dendrogram", "", true, m_propTriggerChange );
    m_propDendroPlotByLevel = m_groupDendrogram->addProperty( "Height by hierarchical level", "", false, m_propTriggerChange );
    m_propTriangleLeaves = m_groupDendrogram->addProperty( "\"Triangle\" leaves", "", false, m_propTriggerChange );

    m_propDendroZoomIn = m_groupDendrogram->addProperty( "Zoom in", "Press!", WPVBaseTypes::PV_TRIGGER_READY, m_propTriggerChange );
    m_propDendroZoomOut = m_groupDendrogram->addProperty( "Zoom out", "Press!", WPVBaseTypes::PV_TRIGGER_READY, m_propTriggerChange );
    m_propDendroSideList = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_propDendroSideList->addItem( "Fill", "" );
    m_propDendroSideList->addItem( "Left", "" );
    m_propDendroSideList->addItem( "Right", "" );
    m_propDendroSideList->addItem( "Manual", "" );
    m_propDendroSideSelector = m_groupDendrogram->addProperty( "Position",
                                                               "selection",
                                                               m_propDendroSideList->getSelectorFirst(),
                                                               m_propTriggerChange );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_propDendroSideSelector );

    m_groupDendroManual = m_groupDendrogram->addPropertyGroup( "Manual size",  "manual resizing of the dendrogram." );
    m_propDendroSizeX = m_groupDendroManual->addProperty( "Width", "", 100, m_propTriggerChange );
    m_propDendroSizeX->setMin( 0 );
    m_propDendroSizeY = m_groupDendroManual->addProperty( "Height", "", 100, m_propTriggerChange );
    m_propDendroSizeY->setMin( 0 );
    m_propDendroOffsetX = m_groupDendroManual->addProperty( "X-offset", "", 100, m_propTriggerChange );
    m_propDendroOffsetY = m_groupDendroManual->addProperty( "Y-offset", "", 100, m_propTriggerChange );
    m_groupDendrogram->setHidden( true );

    // Group Partition
    m_groupPartition = m_properties->addPropertyGroup( "Cluster selections",  "Groups the different cluster selection methods" ); //NOLINT
    m_propPartRoot = m_groupPartition->addProperty( "Select tree root", "Press!", WPVBaseTypes::PV_TRIGGER_READY, m_propTriggerChange );
    m_propSourceCluster = m_groupPartition->addProperty( "Partition root",
                                                         "Selects an intermediate node as partition root.",
                                                         0,
                                                         m_propTriggerChange );
    m_propSourceCluster->setMin( 0 );
    m_propPartitionSelectionList = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_propPartitionSelectionList->addItem( "Classic: Horizontal", "Use horizonztal cut" );
    m_propPartitionSelectionList->addItem( "Classic: Hierarch value", "Cut by hierarchical value" );
    m_propPartitionSelectionList->addItem( "Search: min Cluster size diff", "Search for the partition with minimal cluster size difference" );
    m_propPartitionSelectionList->addItem( "Search: Spread-Separation", "Search for the partition with best Spread-Separation value" );
    m_propPartitionSelectionList->addItem( "Branch length", "Select clusters with branches higher than a definded value" );
    m_propPartitionSelectionList->addItem( "Predefined partitions", "Select partitions previously saved in the tree file" );
    //UNUSED m_propPartitionSelectionList->addItem( "Classic: Size Restrict.", "" );
    //UNUSED m_propPartitionSelectionList->addItem( "Search: min Intra-Clust Dist", "" );
    //UNUSED m_propPartitionSelectionList->addItem( "Search: min w-Intra-Clust Dist", "" );
    //UNUSED m_propPartitionSelectionList->addItem( "Search: Max Inter-Clust Dist", "" );
    //UNUSED m_propPartitionSelectionList->addItem( "Search: Max w-Inter-Clust Dist", "" );
    //UNUSED m_propPartitionSelectionList->addItem( "Branch lenght normalized", "" );
    m_propPartitionSelector = m_groupPartition->addProperty( "Selection method", "selection",
                                                           m_propPartitionSelectionList->getSelectorFirst(), m_propTriggerChange );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_propPartitionSelector );

    m_propConditionSelectionsList = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_propConditionSelectionsList->addItem( "Cluster #", "find partition with # clusters" );
    m_propConditionSelectionsList->addItem( "Cut level", "find partition thresholding the method-related absolute value" );
    m_propConditionSelectionsList->addItem( "Cut level (%)", "find partition thresholding the method-related relative value to its maximum" );
    m_propConditionSelector = m_groupPartition->addProperty( "Selection condition", "condition",
                                                           m_propConditionSelectionsList->getSelectorFirst(), m_propTriggerChange );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_propConditionSelector );


    m_propPartNumClusters = m_groupPartition->addProperty( "# of clusters", "", 5, m_propTriggerChange );
    m_propPartNumClusters->setMin( 1 );
    m_propPartSizeValue = m_groupPartition->addProperty( "Cluster size", "", 500, m_propTriggerChange );
    m_propPartSizeValue->setMin( 1 );
    m_propPartRelativeValue = m_groupPartition->addProperty( "% value", "", 10.0, m_propTriggerChange );
    m_propPartRelativeValue->setMin( 0.0 );
    m_propPartRelativeValue->setMax( 100.0 );
    m_propPartHlevelValue = m_groupPartition->addProperty( "H.level cut value", "", 2, m_propTriggerChange );
    m_propPartHlevelValue->setMin( 1 );
    m_propPartDistanceValue = m_groupPartition->addProperty( "Distance value", "", 1.0, m_propTriggerChange );
    m_propPartDistanceValue->setMin( 0.0001 );
    m_propPartDistanceValue->setMax( 1.0 );
    m_propPartSearchDepthValue = m_groupPartition->addProperty( "Search depth", "", 1, m_propTriggerChange );
    m_propPartSearchDepthValue->setMin( 1 );
    m_propPartSearchDepthValue->setMax( 10 );
    m_propPreloadPartitionNr = m_groupPartition->addProperty( "partition to load", "", 0, m_propTriggerChange );
    m_propPreloadPartitionNr->setMin( 0 );


    m_propPartExcludeLeaves = m_groupPartition->addProperty( "Exclude leaves",
                                                             "Base nodes only containing single leaves will not be subdivided",
                                                             true,
                                                             m_propTriggerChange );
    m_propDoPartition = m_groupPartition->addProperty( "Find Partition", "Press!", WPVBaseTypes::PV_TRIGGER_READY, m_propTriggerChange );
    m_groupPartition->setHidden( true );

    // Group Visualization
    m_groupVisualization = m_properties->addPropertyGroup( "Visualization",  "Groups the visualization options" ); //NOLINT
    m_propSubselectedCluster = m_groupVisualization->addProperty( "Selected cluster", "", 0, m_propTriggerChange );
    m_propSubselectedcolor = m_groupVisualization->addProperty( "Selected color",
                                                                "Selected cluster colors",
                                                                WColor( 1.0, 1.0, 1.0, 1.0 ),
                                                                m_propTriggerChange );
    m_propColorSchemeList = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_propColorSchemeList->addItem( "Custom", "Use custom color values, default is hierarchical tree coloring, can be modified" );
    m_propColorSchemeList->addItem( "Partition", "Disregard custom value and assign color values to selected partition" );
    m_propColorSchemeSelector= m_groupVisualization->addProperty( "Color Scheme",
                                                                  "",
                                                                  m_propColorSchemeList->getSelectorFirst(),
                                                                  m_propTriggerChange );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_propColorSchemeSelector );
    m_propColorActionList = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_propColorActionList->addItem( "Shuffle partition", "" );
    m_propColorActionList->addItem( "Redo: Size", "" );
    m_propColorActionList->addItem( "Redo: Hierch.Dist", "" );
    m_propColorActionList->addItem( "Apply: single selected", "" );
    m_propColorActionList->addItem( "Apply: hierarchical selected", "" );
    m_propColorActionList->addItem( "Apply: single partition", "" );
    m_propColorActionList->addItem( "Apply: hierarchical partition", "" );
    m_propColorActionSelector= m_groupVisualization->addProperty( "Color Action",
                                                                  "",
                                                                  m_propColorActionList->getSelectorFirst(),
                                                                  m_propTriggerChange );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_propColorActionSelector );
    m_propColorActionTrigger= m_groupVisualization->addProperty( "Take action", "Press!", WPVBaseTypes::PV_TRIGGER_READY, m_propTriggerChange );
    m_propShowLabels = m_groupVisualization->addProperty( "Show cluster labels",
                                                          "Shows the an info label per selected cluster",
                                                          false,
                                                          m_propTriggerChange );
    m_propLabelList = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_propLabelList->addItem( "Node-ID", "" );
    m_propLabelList->addItem( "Size", "" );
    m_propLabelList->addItem( "Distance", "" );
    m_propLabelList->addItem( "H. Level", "" );
    m_propLabelInfoSelector= m_groupVisualization->addProperty( "Label Info", "selection", m_propLabelList->getSelectorFirst(), m_propTriggerChange );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_propLabelInfoSelector );
    m_propShowDiscarded = m_groupVisualization->addProperty( "Show discarded voxels",
                                                             "( mesh triangulation is re-done )",
                                                             false, m_propTriggerChange );
    m_propBoundaries = m_groupVisualization->addProperty( "Just boundaries", "Only show boundary colors", false, m_propTriggerChange );
    m_propShowCubeMesh = m_groupVisualization->addProperty( "Render seed voxels",
                                                            "Shows the seed voxel cube triangulation",
                                                            false, m_propTriggerChange );
    m_propDiscardedColor = m_groupVisualization->addProperty( "Discarded color", "", WColor( 0.0, 0.0, 0.0, 1.0 ), m_propTriggerChange );
    m_propBoundaryColor =  m_groupVisualization->addProperty( "Boundary color", "", WColor( 0.4, 0.4, 0.4, 1.0 ), m_propTriggerChange );
    m_propInactiveColor = m_groupVisualization->addProperty( "Inactive color", "",  WColor( 0.4, 0.4, 0.4, 1.0 ), m_propTriggerChange );
    m_groupVisualization->setHidden( true );

    // Group Tree Processing
    m_groupTreeProcess = m_properties->addPropertyGroup( "Tree Processing", "Grouping the different tree processing methods" ); //NOLINT
    m_propProcessSelectionsList = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_propProcessSelectionsList->addItem( "Prune by size ratio",
                                          "Clusters smaller than SafeSize that join clusters bigger than SizeRatio times its size will be pruned" );
    m_propProcessSelectionsList->addItem( "Prune by joining size",
                                          "Clusters smaller than SafeSize that join clusters bigger than JoinSize will be pruned" );
    m_propProcessSelectionsList->addItem( "Prune by joining level",
                                          "Clusters smaller than SafeSize that join at a distance level higher than DistanceGap will be pruned" );
    m_propProcessSelectionsList->addItem( "Prune randomly", "Pruned Number leaves randomly chosen will be pruned" );
    m_propProcessSelectionsList->addItem( "Collapse nodes, linear", "Nodes with branches shorter than DistanceGap * nodeLevel will be collapsed" );
    m_propProcessSelectionsList->addItem( "Flatten selection", "All inner nodes within the currently selected clusters will be collapsed" );
    m_propProcessSelectionsList->addItem( "Prune selection", "currently selected clusters will be eliminated (including contained leaves)" );
    m_propProcessSelector = m_groupTreeProcess->addProperty( "Processing method", "processing",
                                                             m_propProcessSelectionsList->getSelectorFirst(),
                                                             m_propTriggerChange );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_propProcessSelector );
    m_propSafeSize = m_groupTreeProcess->addProperty( "Safe Size", "Maximum size to be pruned", 1, m_propTriggerChange );
    m_propSafeSize->setMin( 1 );
    m_propSafeSize->setMax( 500 );
    m_propPruneJoinSize= m_groupTreeProcess->addProperty( "Join Size", "", 500, m_propTriggerChange );
    m_propPruneJoinSize->setMin( 1 );
    m_propPruneJoinSize->setMax( 500 );
    m_propPruneSizeRatio= m_groupTreeProcess->addProperty( "Size Ratio", "", 100.0, m_propTriggerChange );
    m_propPruneSizeRatio->setMin( 2.0 );
    m_propPruneSizeRatio->setMax( 1000.0 );
    m_propDistanceGap= m_groupTreeProcess->addProperty( "Distance Gap", "", 0.1, m_propTriggerChange );
    m_propDistanceGap->setMin( 0.0 );
    m_propDistanceGap->setMax( 1 );
    m_propDistanceGap->setMax( 0.5 );
    m_propFlatLimit= m_groupTreeProcess->addProperty( "Flatten limit", "", 0.3, m_propTriggerChange );
    m_propFlatLimit->setMin( 0.0 );
    m_propDistanceGap->setMax( 1 );
    m_propDistanceGap->setMax( 0.7 );
    m_propPrunedNumber= m_groupTreeProcess->addProperty( "Pruned number", "Number of leaves to prune", 500, m_propTriggerChange );
    m_propPrunedNumber->setMin( 1 );
    m_propPrunedNumber->setMax( 500 );
    m_propPrunedSeed= m_groupTreeProcess->addProperty( "Pruned seed", "Seed for the random generator", 0, m_propTriggerChange );
    m_propPrunedSeed->setMin( 0 );
    m_propPrunedSeed->setMax( 500 );
    m_propKeepBases= m_groupTreeProcess->addProperty( "keep base nodes", "", false, m_propTriggerChange );
    m_propDoProcessing= m_groupTreeProcess->addProperty( "Do Process", "Press!", WPVBaseTypes::PV_TRIGGER_READY, m_propTriggerChange );
    m_groupTreeProcess->setHidden( true );


    // Group Partition
    m_groupSavedPartition = m_properties->addPropertyGroup( "Add selected partitions",  "Groups the tree partition options" ); //NOLINT
    m_propAddPartTrigger = m_groupSavedPartition->addProperty( "Add partition",  "Add partition",
                                                               WPVBaseTypes::PV_TRIGGER_READY,
                                                               m_propTriggerChange );
    m_propRecolorPartTrigger = m_groupSavedPartition->addProperty( "Recolor partition",
                                                                   "Recolor partition",
                                                                   WPVBaseTypes::PV_TRIGGER_READY,
                                                                   m_propTriggerChange );
    m_propClearPartsTrigger = m_groupSavedPartition->addProperty( "Clear partitions",
                                                                  "Clear partitions",
                                                                  WPVBaseTypes::PV_TRIGGER_READY,
                                                                  m_propTriggerChange );
    m_groupSavedPartition->setHidden( true );

    // Group Write
    m_groupWrite = m_properties->addPropertyGroup( "Write Files",  "Groups the file writing options" ); //NOLINT
    m_propWriteTreeFilename = m_groupWrite->addProperty( "Output Tree file", "", boost::filesystem::path( "" ) );
    m_propWriteTreeTrigger = m_groupWrite->addProperty( "Write Tree",  "Press!", WPVBaseTypes::PV_TRIGGER_READY, m_propTriggerChange );
    m_propPartitionFile = m_groupWrite->addProperty( "Output Partition file", "", boost::filesystem::path( "" ) );
//    WPropertyHelper::PC_PATHEXISTS::addTo( dirname );
//    WPropertyHelper::PC_ISDIRECTORY::addTo( dirname );
    m_propPartitionsTrigger = m_groupWrite->addProperty( "Write Partition",  "Press!", WPVBaseTypes::PV_TRIGGER_READY, m_propTriggerChange );
    m_groupWrite->setHidden( true );

    WModule::properties();
} // end properties()

void WMHierchClustDisplay::initWidgets()
{
    osg::ref_ptr<osgViewer::View> viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getView();
    int height = viewer->getCamera()->getViewport()->height();
    int width = viewer->getCamera()->getViewport()->width();

    m_oldViewHeight = height;
    m_oldViewWidth = width;

    // create shader
    m_shader = osg::ref_ptr< WGEShader >( new WGEShader( "WMHierchClustDisplay", m_localPath ) );

    m_moduleNode = osg::ref_ptr< WGEManagedGroupNode >( new WGEManagedGroupNode( m_active ) );
    m_dendrogramNode = osg::ref_ptr< WGEManagedGroupNode >( new WGEManagedGroupNode( m_active ) );
    m_meshNode = osg::ref_ptr< WGEManagedGroupNode >( new WGEManagedGroupNode( m_active ) );
    m_moduleNode->insert( m_dendrogramNode );
    m_shader->apply( m_meshNode );
    m_moduleNode->insert( m_meshNode );

    m_wm = new osgWidget::WindowManager( viewer, 0.0f, 0.0f, MASK_2D );

    m_camera = new osg::Camera();
    m_camera->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );

    m_camera->setProjectionMatrix( osg::Matrix::ortho2D( 0.0, width, 0.0f, height ) );
    m_camera->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
    m_camera->setViewMatrix( osg::Matrix::identity() );
    m_camera->setClearMask( GL_DEPTH_BUFFER_BIT );
    m_camera->setRenderOrder( osg::Camera::POST_RENDER );

    m_dendrogramNode->addChild( m_camera );
    m_camera->addChild( m_wm );

    viewer->addEventHandler( new osgWidget::MouseHandler( m_wm ) );
    viewer->addEventHandler( new osgWidget::KeyboardHandler( m_wm ) );
    viewer->addEventHandler( new osgWidget::ResizeHandler( m_wm, m_camera ) );
    viewer->addEventHandler( new osgWidget::CameraSwitchHandler( m_wm, m_camera ) );
    viewer->addEventHandler( new osgViewer::StatsHandler() );
    viewer->addEventHandler( new osgViewer::WindowSizeHandler() );
    viewer->addEventHandler( new osgGA::StateSetManipulator( viewer->getCamera()->getOrCreateStateSet() ) );

    m_wm->resizeAllWindows();

    m_moduleNode->addUpdateCallback( new WGEFunctorCallback< osg::Node >( boost::bind( &WMHierchClustDisplay::updateWidgets, this ) ) );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_moduleNode );
} // end initWidgets()



void WMHierchClustDisplay::initProperties()
{
    infoLog() << "Init properties...";

    infoLog() << m_tree.getName();

    // init gui appearance
    m_groupRead->setHidden( false );
    m_groupDendrogram->setHidden( false );
    m_propTriangleLeaves->setHidden( false );
    m_groupPartition->setHidden( false );
    m_propPartNumClusters->setHidden( false );
    m_propPartDistanceValue->setHidden( true );
    m_propPartSizeValue->setHidden( true );
    m_propPartSearchDepthValue->setHidden( true );
    m_propPartRelativeValue->setHidden( true );
    m_propPartHlevelValue->setHidden( true );
    m_propPreloadPartitionNr->setHidden( true );
    m_groupDendroManual->setHidden( true );
    m_groupVisualization->setHidden( false );
    m_propDiscardedColor->setHidden( true );
    m_propBoundaryColor->setHidden( true );
    m_propInactiveColor->setHidden( true );
    m_propLabelInfoSelector->setHidden( true );
    m_groupWrite->setHidden( false );
    m_groupTreeProcess->setHidden( false );
    m_propSafeSize->setHidden( false );
    m_propPruneSizeRatio->setHidden( false );
    m_propPruneJoinSize->setHidden( true );
    m_propDistanceGap->setHidden( true );
    m_propPrunedNumber->setHidden( true );
    m_propPrunedSeed->setHidden( true );
    m_propFlatLimit->setHidden( true );
    m_propKeepBases->setHidden( true );
    m_groupSavedPartition->setHidden( false );

    // set boundaries
    const WHnode& root( m_tree.getRoot() );
    infoLog() << root.printAllData();
    m_propSubselectedCluster->removeConstraint( PC_MAX );
    m_propSubselectedCluster->setMax( root.getID() );
    m_propSourceCluster->removeConstraint( PC_MAX );
    m_propSourceCluster->setMax( root.getID() );
    m_propPartNumClusters->removeConstraint( PC_MAX );
    m_propPartNumClusters->setMax( root.getSize() );
    m_propPartDistanceValue->removeConstraint( PC_MAX );
    m_propPartDistanceValue->setMax( root.getDistLevel() );
    m_propPartHlevelValue->removeConstraint( PC_MAX );
    m_propPartHlevelValue->setMax( root.getHLevel() );
    m_propPartSizeValue->removeConstraint( PC_MAX );
    m_propPartSizeValue->setMax( root.getSize() );
    std::vector<size_t> baseNodes( m_tree.getRootBaseNodes() );
    m_tree.sortBySize( &baseNodes );
    m_propPartSizeValue->removeConstraint( PC_MIN );
    m_propPartSizeValue->setMin( m_tree.getNode( baseNodes.front() ).getSize() );
    m_propPartRelativeValue->removeConstraint( PC_MIN );
    m_propPartRelativeValue->setMin( m_tree.getNode( baseNodes.front() ).getSize()*100/( ( double )m_tree.getNumLeaves() ) );
    m_propPrunedNumber->removeConstraint( PC_MAX );
    m_propPrunedNumber->setMax( root.getSize() );
    m_propPruneJoinSize->removeConstraint( PC_MAX );
    m_propPruneJoinSize->setMax( root.getSize()/3 );
    m_propSafeSize->removeConstraint( PC_MAX );
    m_propSafeSize->setMax( root.getSize()/3 );
    m_propPreloadPartitionNr->removeConstraint( PC_MAX );
    m_propPreloadPartitionNr->setMax( m_tree.getSelectedValues().size() );
    m_propPreloadPartitionNr->removeConstraint( PC_MIN );
    if( m_tree.getSelectedValues().size() > 0 )
    {
        m_propPreloadPartitionNr->setMin( 1 );
        m_propPreloadPartitionNr->set( 1 );
    }
    else
    {
        m_propPreloadPartitionNr->setMin( 0 );
        m_propPreloadPartitionNr->set( 0 );
    }



    // init properties
    m_propSourceCluster->set( root.getID() );
    m_propSourceCluster->changed( true );
    m_propPartNumClusters->set( 1 );
    m_propPartNumClusters->changed( true );
    m_propPreloadPartitionNr->changed( true );
    m_propShowDendrogram->set( true );
    m_propShowDendrogram->changed( true );
    m_propDendroPlotByLevel->set( false );
    m_propDendroPlotByLevel->changed( true );
    m_propTriangleLeaves->set( true );
    m_propTriangleLeaves->changed( true );
    m_propDendroSideSelector->set( m_propDendroSideList->getSelectorFirst() );
    m_propDendroSideSelector->changed( true );
    m_propShowDiscarded->set( false );
    m_propShowDiscarded->changed( true );
    m_propBoundaries->set( false );
    m_propBoundaries->changed( true );
    m_propShowCubeMesh->set( false );
    m_propShowCubeMesh->changed( true );
    m_propPartSizeValue->set( root.getSize() );
    m_propPartSizeValue->changed( true );
    m_propPartHlevelValue->set( root.getHLevel() );
    m_propPartHlevelValue->changed( true );
    m_propPartitionSelector->set( m_propPartitionSelectionList->getSelectorFirst() );
    m_propPartitionSelector->changed( true );
    m_propConditionSelector->set( m_propConditionSelectionsList->getSelectorFirst() );
    m_propConditionSelector->changed( true );
    m_propPartExcludeLeaves->set( true );
    m_propPartExcludeLeaves->changed( true );
    m_propLabelInfoSelector->set( m_propLabelList->getSelectorFirst() );
    m_propLabelInfoSelector->changed( true );
    m_propColorSchemeSelector->set( m_propColorSchemeList->getSelectorFirst() );
    m_propColorSchemeSelector->changed( true );
    m_propColorActionSelector->set( m_propColorActionList->getSelectorFirst() );
    m_propColorActionSelector->changed( true );
    m_propProcessSelector->set( m_propProcessSelectionsList->getSelectorFirst() );
    m_propProcessSelector->changed( true );
    m_propShowLabels->set( false );
    m_propShowLabels->changed( true );
    m_propKeepBases->set( false );
    m_propKeepBases->changed( true );


    // init variables
    m_treeZoomRoot = root.getID();
    m_selectionClusters.clear();
    m_selectionClusters.push_back( m_propSourceCluster->get( true ) );
    m_selectionColors.clear();
    m_selectionColorsBackup.clear();
    m_selectionLeaves.clear();
    m_treeDirty = false;
    m_labelsDirty = false;
    m_horizontalLine = 0;
    m_treeDisplayColors.clear();


    // init info properties
    m_infoSelectedID->set( root.getID() );
    m_infoSelectedSize->set( root.getSize() );
    m_infoPartNumber->set( 1 );
    m_infoPartActive->set( 1 );
    m_infoPartLeaves->set( 0 );
    m_infoCutValue->set( root.getDistLevel() );
    m_infoTreeName->set( m_tree.getName() );
    m_infoCountLeaves->set( m_tree.getNumLeaves() );
    m_infoCountNodes->set( m_tree.getNumNodes() );
    m_infoCountDiscarded->set( m_tree.getNumDiscarded() );
    m_infoMaxLevel->set( root.getHLevel() );
    m_infoCpcc->set( m_tree.getCpcc() );
    m_infoPartMessage->set( "" );

    infoLog() << "End Init properties...";

    return;
} // end initProperties()

void WMHierchClustDisplay::initTreeData()
{
    infoLog() << "Init tree data...";

    boost::shared_ptr< WProgress > progressInitTreeData = boost::shared_ptr< WProgress >( new WProgress( "Initializing tree data..." ) );
    m_progress->addSubProgress( progressInitTreeData );

    // change to nifti coordinates if necessary
    if( m_tree.convert2grid( HC_NIFTI ) )
    {
        infoLog() << "Tree was converted to nifti coordinates";
    }

    // initialize colors for each cluster, assign new color to biggest cluster
    initializeColorsSize();
    m_treeDisplayColors.clear();
    m_treeDisplayColors.resize( m_tree.getNumNodes() );

    //save a vector of seed voxel indices for each cluster
    m_clusterVoxels.clear();
    m_clusterVoxels.resize( m_tree.getNumNodes() );
    for( size_t i = 0; i < m_clusterVoxels.size(); ++i )
    {
        std::vector<WHcoord> clusterCoords( m_tree.getCoordinates4node( i ) );
        m_clusterVoxels[i].reserve( clusterCoords.size() );
        for( size_t j = 0; j < clusterCoords.size(); ++j )
        {
            size_t voxelID( m_grid->getVoxelNum( clusterCoords[j].m_x, clusterCoords[j].m_y, clusterCoords[j].m_z ) );
            m_clusterVoxels[i].push_back( voxelID );
        }
    }
    m_discardedVoxels.clear();
    std::list<WHcoord> discarded( m_tree.getDiscarded() );
    for( std::list<WHcoord>::iterator listIter( discarded.begin() ); listIter != discarded.end(); ++listIter )
    {
        m_discardedVoxels.push_back( m_grid->getVoxelNum( listIter->m_x, listIter->m_y, listIter->m_z ) );
    }


    // create mesh for seed voxel triangulation
    {
        std::vector< size_t > seedVoxelMask;
        seedVoxelMask.resize( m_grid->size(), 0 );
        for( std::vector<size_t>::iterator iter( m_clusterVoxels.back().begin() ); iter != m_clusterVoxels.back().end(); ++iter )
        {
            seedVoxelMask[*iter] = 1;
        }
        WMarchingLegoAlgorithm mlAlgo;
        m_cubeMesh = mlAlgo.genSurfaceOneValue( m_grid->getNbCoordsX(),
                                                m_grid->getNbCoordsY(),
                                                m_grid->getNbCoordsZ(),
                                                m_grid->getTransformationMatrix(),
                                                &seedVoxelMask, 1 );
    }
    // create mesh for discarded voxels
    {
        std::vector< size_t > discardedMask;
        discardedMask.resize( m_grid->size(), 0 );
        for( std::vector<size_t>::iterator iter( m_discardedVoxels.begin() ); iter != m_discardedVoxels.end(); ++iter )
        {
            discardedMask[*iter] = 1;
        }
        WMarchingLegoAlgorithm mlAlgo;
        m_discardedMesh = mlAlgo.genSurfaceOneValue( m_grid->getNbCoordsX(),
                                                     m_grid->getNbCoordsY(),
                                                     m_grid->getNbCoordsZ(),
                                                     m_grid->getTransformationMatrix(),
                                                     &discardedMask, 1 );
    }

    progressInitTreeData->finish();

    // update seed coordinates output
    updateOutput2();

    return;
} // end initTreeData()


void WMHierchClustDisplay::assignColorHierch( const size_t thisID, const WColor thisColor )
{
    // color up the tree
    size_t currentID( thisID );
    while( currentID != 0 )
    {
        if( m_tree.getNode( currentID ).isRoot() )
        {
            break;
        }
        size_t parentID =  m_tree.getNode( currentID ).getParent().second;
        std::vector<nodeID_t> kids( m_tree.getNode( parentID ).getChildren() );
        m_tree.sortBySize( &kids );
        if( kids.back().second == currentID )
        {
            m_nodeColorsCustom[parentID] = thisColor;
            currentID = parentID;
        }
        else
        {
            break;
        }
    }

    // color down the tree
    currentID = thisID;
    m_nodeColorsCustom[currentID] = thisColor;
    // give same color to all the biggest clusters in lower divisions.
    while( true )
    {
        std::vector<nodeID_t> kids( m_tree.getNode( currentID ).getChildren() );
        m_tree.sortBySize( &kids );
        // if bigger son is a leaf ( should not happen) stop
        if( !kids.back().first )
            break;
        currentID = kids.back().second;
        m_nodeColorsCustom[currentID] = thisColor;
        if( m_tree.getNode( currentID ).getHLevel() == 1 )
            break;
    }
} // end assignColorHierch()


void WMHierchClustDisplay::initializeColorsHdist()
{
    // initialize colors for each cluster, assign new color to highest cluster in the tree
    size_t colorIndex( 0 );
    m_nodeColorsCustom.clear();
    m_nodeColorsCustom.resize( m_tree.getNumNodes() );
    std::list<size_t> worklist;
    for( size_t i = 0; i < m_tree.getNumNodes(); ++i )
    {
        worklist.push_front( i );
    }
    while( !worklist.empty() )
    {
        size_t currentID( worklist.front() );
        worklist.pop_front();
        m_nodeColorsCustom[currentID] = wge::getNthHSVColor( colorIndex );
        if( m_tree.getNode( currentID ).getHLevel() == 1 )
            continue;
        // give same color to all the biggest clusters in lower divisions.
        while( true )
        {
            std::vector<nodeID_t> kids( m_tree.getNode( currentID ).getChildren() );
            m_tree.sortBySize( &kids );
            // if bigger son is a leaf ( should not happen) stop
            if( !kids.back().first )
                break;
            currentID = kids.back().second;
            m_nodeColorsCustom[currentID] = wge::getNthHSVColor( colorIndex );
            worklist.remove( currentID );
            if( m_tree.getNode( currentID ).getHLevel() == 1 )
                break;
        }
        // next cluster will ahve a different color
        ++colorIndex;
    }
} // end assignColorsHdist()


void WMHierchClustDisplay::initializeColorsSize()
{
    // initialize colors for each cluster, assign new color to biggest cluster
    size_t colorIndex( 0 );
    std::list<size_t> worklist;
    m_nodeColorsCustom.clear();
    m_nodeColorsCustom.resize( m_tree.getNumNodes() );
    {
        std::vector<size_t> initOrder;
        initOrder.reserve( m_tree.getNumNodes() );
        for( size_t i = 0; i < m_tree.getNumNodes(); ++i )
        {
            initOrder.push_back( i );
        }
        m_tree.sortBySize( &initOrder );
        for( size_t i = 0; i < initOrder.size(); ++i )
        {
            worklist.push_front( initOrder[i] );
        }
    }

    while( !worklist.empty() )
    {
        size_t currentID( worklist.front() );
        worklist.pop_front();
        m_nodeColorsCustom[currentID] = wge::getNthHSVColor( colorIndex );
        if( m_tree.getNode( currentID ).getHLevel() == 1 )
            continue;
        // give same color to all the biggest clusters in lower divisions.
        while( true )
        {
            std::vector<nodeID_t> kids( m_tree.getNode( currentID ).getChildren() );
            m_tree.sortBySize( &kids );
            // if bigger son is a leaf ( should not happen) stop
            if( !kids.back().first )
                break;
            currentID = kids.back().second;
            m_nodeColorsCustom[currentID] = wge::getNthHSVColor( colorIndex );
            worklist.remove( currentID );
            if( m_tree.getNode( currentID ).getHLevel() == 1 )
                break;
        }
        // next cluster will ahve a different color
        ++colorIndex;
    }
} // end assignColorsSize()

void WMHierchClustDisplay::initTexture()
{
    infoLog() << "Init texture";
    boost::shared_ptr< WProgress > progressInitTexture = boost::shared_ptr< WProgress >( new WProgress( "Initializing output texture..." ) );
    m_progress->addSubProgress( progressInitTexture );

    osg::ref_ptr< osg::Image > ima = new osg::Image;
    ima->allocateImage( m_grid->getNbCoordsX(), m_grid->getNbCoordsY(), m_grid->getNbCoordsZ(), GL_RGB, GL_UNSIGNED_BYTE );
    unsigned char* data = ima->data();
    m_textureLabels.clear();
    m_textureLabels.resize( m_grid->getNbCoordsX() * m_grid->getNbCoordsY() * m_grid->getNbCoordsZ(), 0 );

    for( unsigned int i = 0; i < m_grid->size() * 3; ++i )
    {
        data[i] = 0.0;
    }

    m_texture = osg::ref_ptr<osg::Texture3D>( new osg::Texture3D );
    m_texture->setFilter( osg::Texture3D::MIN_FILTER, osg::Texture3D::LINEAR );
    m_texture->setFilter( osg::Texture3D::MAG_FILTER, osg::Texture3D::LINEAR );
    m_texture->setWrap( osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_BORDER );
    m_texture->setWrap( osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_BORDER );
    m_texture->setWrap( osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_BORDER );
    m_texture->setImage( ima );
    m_texture->setResizeNonPowerOfTwoHint( false );

    WKernel::getRunningKernel()->getSelectionManager()->setTexture( m_texture, m_grid );
    WKernel::getRunningKernel()->getSelectionManager()->setShader( 0 );
    WKernel::getRunningKernel()->getSelectionManager()->setUseTexture( true );

    WDataHandler::getDefaultSubject()->getChangeCondition()->notify();

    progressInitTexture->finish();

    infoLog() << "End Init texture";

    return;
} // end initTexture()


void WMHierchClustDisplay::moduleMain()
{
// DEPRECATED    boost::signals2::connection con = WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getPickHandler()->getPickSignal()->
// DEPRECATED             connect( boost::bind( &WMHierchClustDisplay::dendrogramClick, this, _1 ) );

    osg::ref_ptr< MainViewEventHandler > eh( new MainViewEventHandler );
    eh->subscribeLeftButtonPush( boost::bind( &WMHierchClustDisplay::dendrogramClick, this, _1 ) );

    eh->subscribeRightButtonPush( boost::bind( &WMHierchClustDisplay::dendrogramClickRight, this, _1 ) );

    WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getView()->addEventHandler( eh );


    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_propTriggerChange );
    m_moduleState.add( m_active->getUpdateCondition() );

    ready();

    // wait for a dataset to be connected, most likely an anatomy dataset
    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();
        if( m_shutdownFlag() )
        {
            break;
        }
        boost::shared_ptr< WDataSetSingle > newDataSet = m_input->getData();
        bool dataChanged = ( m_anatomy != newDataSet );
        bool dataValid   = ( newDataSet != NULL );
        if( dataValid )
        {
            if( dataChanged )
            {
                m_anatomy = newDataSet;
                m_grid = boost::dynamic_pointer_cast< WGridRegular3D >( m_anatomy->getGrid() );
                break;
            }
        }
    }

    // wait for a tree file to be loaded
    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();
        if( m_shutdownFlag() )
        {
            break;
        }
        if( m_propReadTreeTrigger->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            infoLog() << "Loading tree file";

            boost::filesystem::path fileName = m_propReadFilename->get();

            if( m_tree.readTree( fileName.string() ) )
            {
                m_propReadTreeTrigger->set( WPVBaseTypes::PV_TRIGGER_READY, true );
                break;
            }
            else
            {
                infoLog() << "Invalid tree file";
                m_propReadTreeTrigger->set( WPVBaseTypes::PV_TRIGGER_READY, true );
                continue;
            }
        }
    }

    // initialize
    if( !m_shutdownFlag() )
    {
        initWidgets();
        initTreeData();
        initProperties();
        initTexture();
        updateSelection();
    }

    // === MAIN MODULE LOOP ===


    // main loop, respond to controls
    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        // if close signal is received, finish
        if( m_shutdownFlag() )
            break;

        // === READ TREE OPTIONS ===

        // if another tree is loaded, reinitialize
        if( m_propReadTreeTrigger->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            infoLog() << "Loading tree file";

            boost::shared_ptr< WProgress > progressLoadTree = boost::shared_ptr< WProgress >( new WProgress( "Loading tree from file..." ) );
            m_progress->addSubProgress( progressLoadTree );

            boost::filesystem::path fileName = m_propReadFilename->get();

            if( !m_tree.readTree( fileName.string() ) )
            {
                infoLog() << "Invalid tree file";
                progressLoadTree->finish();
                m_propReadTreeTrigger->set( WPVBaseTypes::PV_TRIGGER_READY, false );
                continue;
            }
            progressLoadTree->finish();

            std::string message( "Tree \""+ m_tree.getName()+"\" loaded. " +
                                 string_utils::toString( m_tree.getNumLeaves() )+" leaves and " +
                                 string_utils::toString( m_tree.getNumNodes() )+" nodes" );
            m_infoPartMessage->set( message );

            initTreeData();
            initProperties();
            updateSelection();
            m_propReadTreeTrigger->set( WPVBaseTypes::PV_TRIGGER_READY, false );
        }

        // === DENDROGRAM OPTIONS ===


        // if an action that changes the on screen tree is triggered, set tree to redraw
        if( m_propTriangleLeaves->changed( true ) || m_propShowDendrogram->changed( true ) )
        {
            m_treeDirty = true;
        }
        if( m_propDendroSizeX->changed( true )   ||
            m_propDendroSizeY->changed( true )   ||
            m_propDendroOffsetX->changed( true ) ||
            m_propDendroOffsetY->changed( true )
            )
        {
            m_treeDirty = true;
            m_labelsDirty = true;
        }

        // only show triangle leaves option if tree is being plotted by distance level
        if( m_propDendroPlotByLevel->changed( true ) )
        {
            m_treeDirty = true;

            if( m_propDendroPlotByLevel->get( true ) )
            {
                m_propTriangleLeaves->setHidden( true );
            }
            else
            {
                m_propTriangleLeaves->setHidden( false );
            }
        }

        // if zoom controls are changed, make changes and redraw
        if( m_propDendroZoomIn->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            m_treeZoomRoot = m_propSourceCluster->get( true );
            m_propDendroZoomIn->set( WPVBaseTypes::PV_TRIGGER_READY, true );
            m_treeDirty = true;
        }
        if( m_propDendroZoomOut->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            m_treeZoomRoot = m_tree.getRoot().getID();
            m_propDendroZoomOut->set( WPVBaseTypes::PV_TRIGGER_READY, true );
            m_treeDirty = true;
        }

        // if auto resize option is changed, set to redraw and hide or extend the manual settings
        if( m_propDendroSideSelector->changed( true ) )
        {
            m_labelsDirty = true;
            m_treeDirty = true;

            if( m_propDendroSideSelector->get( true ).getItemIndexOfSelected( 0 ) == 3 )
            {
                m_groupDendroManual->setHidden( false );
            }
            else
            {
                m_groupDendroManual->setHidden( true );
            }
        }


        // === CLUSTER SELECTION OPTIONS ===


        // if root button is pressed update
        if( m_propPartRoot->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            m_propSourceCluster->set( m_tree.getRoot().getID() );
            m_selectionClusters.clear();
            m_selectionClusters.push_back( m_propSourceCluster->get( true ) );
            m_horizontalLine = 0;


            m_propPartRoot->set( WPVBaseTypes::PV_TRIGGER_READY, true );
            std::string message( "Single cluster at distance: " +
                                 string_utils::toString( m_tree.getRoot().getDistLevel() ) );
            m_infoPartMessage->set( message );
            m_infoPartNumber->set( 1 );
            m_infoPartActive->set( 1 );
            m_infoPartLeaves->set( 0 );
            m_infoCutValue->set( m_tree.getRoot().getDistLevel() );

            updateSelection();
            m_propSubselectedCluster->set( m_propSourceCluster->get( true ) );
            m_propSubselectedcolor->set( m_selectionColors.begin()->second );
        }

        // if root cluster bas changed
        if( m_propSourceCluster->changed( true ) )
        {
            m_selectionClusters.clear();
            m_horizontalLine = 0;
            m_selectionClusters.push_back( m_propSourceCluster->get( true ) );
            m_propPartNumClusters->removeConstraint( PC_MAX );
            m_propPartNumClusters->setMax( m_tree.getNode( m_propSourceCluster->get( true ) ).getSize() );
            m_propPartDistanceValue->removeConstraint( PC_MAX );
            m_propPartDistanceValue->setMax( m_tree.getNode( m_propSourceCluster->get( true ) ).getDistLevel() );
            m_propPartHlevelValue->removeConstraint( PC_MAX );
            m_propPartHlevelValue->setMax( m_tree.getNode( m_propSourceCluster->get( true ) ).getHLevel() );
            m_propPartSizeValue->removeConstraint( PC_MAX );
            m_propPartSizeValue->setMax( m_tree.getNode( m_propSourceCluster->get( true ) ).getSize() );

            std::string message( "Single cluster at distance: " +
                                 string_utils::toString( m_tree.getNode( m_propSourceCluster->get( true ) ).getDistLevel() ) );
            m_infoPartMessage->set( message );
            m_infoPartNumber->set( 1 );
            m_infoPartActive->set( 1 );
            m_infoPartLeaves->set( 0 );
            m_infoCutValue->set( m_tree.getNode( m_propSourceCluster->get( true ) ).getDistLevel() );

            updateSelection();
            m_propSubselectedCluster->set( m_propSourceCluster->get( true ) );
            m_propSubselectedcolor->set( m_selectionColors.begin()->second );
        }

        // if cluster selection has changed
        if( m_propPartitionSelector->changed( true ) )
        {
            m_propConditionSelector->setHidden( true );
            m_propPartNumClusters->setHidden( true );
            m_propPartDistanceValue->setHidden( true );
            m_propPartSizeValue->setHidden( true );
            m_propPartSearchDepthValue->setHidden( true );
            m_propPartRelativeValue->setHidden( true );
            m_propPartHlevelValue->setHidden( true );
            m_propPreloadPartitionNr->setHidden( true );
            m_horizontalLine = 0;
            switch( m_propPartitionSelector->get( true ).getItemIndexOfSelected( 0 ) )
            {
                case 0:
                case 1:
                    m_propConditionSelector->setHidden( false );
                    m_propPartNumClusters->setHidden( false );
                    m_propConditionSelector->set( m_propConditionSelectionsList->getSelector( 0 ) );
                    break;
                case 2:
                case 3:
                    m_propConditionSelector->setHidden( false );
                    m_propPartNumClusters->setHidden( false );
                    m_propConditionSelector->set( m_propConditionSelectionsList->getSelector( 0 ) );
                    m_propPartSearchDepthValue->setHidden( false );
                    break;
                case 4:
                    m_propPartDistanceValue->setHidden( false );
                    m_propConditionSelector->set( m_propConditionSelectionsList->getSelector( 1 ) );
                    break;
                case 5:
                    m_propPreloadPartitionNr->setHidden( false );
                    break;
//UNUSED                case 6:
//UNUSED                    m_propConditionSelector->setHidden( false );
//UNUSED                    m_propPartNumClusters->setHidden( false );
//UNUSED                    m_propConditionSelector->set( m_propConditionSelectionsList->getSelector( 0 ) );
//UNUSED                    break;
//UNUSED                case 7:
//UNUSED                case 8:
//UNUSED                case 9:
//UNUSED                case 10:
//UNUSED                    m_propConditionSelector->setHidden( false );
//UNUSED                    m_propPartNumClusters->setHidden( false );
//UNUSED                    m_propConditionSelector->set( m_propConditionSelectionsList->getSelector( 0 ) );
//UNUSED                    m_propPartSearchDepthValue->setHidden( false );
//UNUSED                    break;
//UNUSED                case 11:
//UNUSED                    m_propPartDistanceValue->setHidden( false );
//UNUSED                    m_propConditionSelector->set( m_propConditionSelectionsList->getSelector( 1 ) );
//UNUSED                    break;
                default:
                    break;
            }
        }

        // if condition selection has changed
        if( m_propConditionSelector->changed( true ) )
        {
            m_propPartNumClusters->setHidden( true );
            m_propPartDistanceValue->setHidden( true );
            m_propPartSizeValue->setHidden( true );
            m_propPartRelativeValue->setHidden( true );
            m_propPartHlevelValue->setHidden( true );
            m_propPreloadPartitionNr->setHidden( true );
            switch( m_propConditionSelector->get( true ).getItemIndexOfSelected( 0 ) )
            {
                case 0:
                    m_propPartNumClusters->setHidden( false );
                    break;
                case 1:

                    switch( m_propPartitionSelector->get( true ).getItemIndexOfSelected( 0 ) )
                    {
                        case 0:
                            m_propPartDistanceValue->setHidden( false );
                            break;
                        case 1:
                            m_propPartHlevelValue->setHidden( false );
                            break;
                        case 2:
                            m_propPartSizeValue->setHidden( false );
                        break;
                        case 3:
                        case 4:
                            m_propPartDistanceValue->setHidden( false );
                            break;
                        case 5:
                            break;
//UNUSED                        case 6:
//UNUSED                            m_propPartSizeValue->setHidden( false );
//UNUSED                            break;
//UNUSED                        case 7:
//UNUSED                        case 8:
//UNUSED                        case 9:
//UNUSED                        case 10:
//UNUSED                            m_propPartDistanceValue->setHidden( false );
//UNUSED                            break;
                        default:
                            break;
                    }
                    break;
                case 2:
                    m_propPartRelativeValue->setHidden( false );
                    break;
                default:
                    break;
            }
        }



        // if cluster selection has been triggered
        if( m_propDoPartition->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            clusterSelection();
        }


        // === VISUALIZATION OPTIONS ===


        // if coloring pattern has changed
        if( m_propColorSchemeSelector->changed( true ) )
        {
            updateSelection();
        }

        // if color actionhas been activated
        if( m_propColorActionTrigger->get( true )== WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            colorAction();
        }

        // if show label option has changed
        if( m_propShowLabels->changed( true ) )
        {
            m_labelsDirty = true;
            m_treeDirty = true;
            if( m_propShowLabels->get( true ) )
            {
                m_propLabelInfoSelector->setHidden( false );
            }
            else
            {
                m_propLabelInfoSelector->setHidden( true );
            }
        }

        // if labeling scheme has changed
        if( m_propLabelInfoSelector->changed( true ) )
        {
            m_labelsDirty = true;
        }

        // if showing discarded voxels has changed
        if( m_propShowDiscarded->changed( true ) )
        {
            if( m_propShowDiscarded->get( true ) )
            {
                m_propDiscardedColor->setHidden( false );
            }
            else
            {
                m_propDiscardedColor->setHidden( true );
            }
            updateColors();
            updateOutput2();
        }


        // if showing just boundaries has changed
        if( m_propBoundaries->changed( true ) )
        {
            if( m_propBoundaries->get( true ) )
            {
                m_propBoundaryColor->set( WColor( 1, 0, 0, 1 ) );
                m_propBoundaryColor->setHidden( false );
            }
            else
            {
                m_propBoundaryColor->setHidden( true );
            }
            updateColors();
        }


        // if discarded, inactive, or boundary color has changed
        if( m_propDiscardedColor->changed( true ) || m_propInactiveColor->changed( true ) || m_propBoundaryColor->changed( true ) )
        {
            updateColors();
        }

        // if showing rendered cube voxels has changed
        if( m_propShowCubeMesh->changed( true ) )
        {
            if( m_propShowCubeMesh->get( true ) )
            {
                m_propInactiveColor->setHidden( false );
            }
            else
            {
                m_propInactiveColor->setHidden( true );
            }
            renderCubeMesh();
        }





        // === TREE PROCESSING OPTIONS ===


        // if processing method has changed
        if( m_propProcessSelector->changed( true ) )
        {
            m_propSafeSize->setHidden( true );
            m_propPruneJoinSize->setHidden( true );
            m_propPruneSizeRatio->setHidden( true );
            m_propDistanceGap->setHidden( true );
            m_propPrunedNumber->setHidden( true );
            m_propPrunedSeed->setHidden( true );
            m_propFlatLimit->setHidden( true );
            m_propKeepBases->setHidden( true );


            switch( m_propProcessSelector->get( true ).getItemIndexOfSelected( 0 ) )
            {
            case 0:
                m_propSafeSize->setHidden( false );
                m_propPruneSizeRatio->setHidden( false );
                break;
            case 1:
                m_propSafeSize->setHidden( false );
                m_propPruneJoinSize->setHidden( false );
                break;
            case 2:
                m_propSafeSize->setHidden( false );
                m_propDistanceGap->setHidden( false );
                break;
            case 3:
                m_propPrunedNumber->setHidden( false );
                m_propPrunedSeed->setHidden( false );
                break;
            case 4:
                m_propDistanceGap->setHidden( false );
                m_propKeepBases->setHidden( false );
                break;
            case 5:
                m_propKeepBases->setHidden( false );
                break;
            case 6:
                break;
            default:
                break;
            }
        }

        // if processing has been triggered
        if( m_propDoProcessing->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            boost::shared_ptr< WProgress > progressProcessing = boost::shared_ptr< WProgress >( new WProgress( "Processing tree..." ) );
            m_progress->addSubProgress( progressProcessing );

            std::pair<size_t, size_t> prunedElements( 0, 0 );
            WHtreeProcesser processer( &m_tree );
            std::string message;

            switch( m_propProcessSelector->get( true ).getItemIndexOfSelected( 0 ) )
            {
            case 0:
                prunedElements = processer.pruneTree( m_propPruneSizeRatio->get( true ), m_propSafeSize->get( true ), HTPR_SIZERATIO );
                message = "Pruning by ratio: ";
                break;
            case 1:
                prunedElements = processer.pruneTree( m_propPruneJoinSize->get( true ), m_propSafeSize->get( true ), HTPR_JOINSIZE );
                message = "Pruning by join size: ";
                break;
            case 2:
                prunedElements = processer.pruneTree( m_propDistanceGap->get( true ), m_propSafeSize->get( true ), HTPR_JOINLEVEL );
                message = "Pruning by join level: ";
                break;
            case 3:
                prunedElements = processer.pruneRandom( m_propPrunedNumber->get( true ), m_propPrunedSeed->get( true ) );
                message = "Random Pruning: ";
                break;
            case 4:
                prunedElements.first = processer.collapseTreeLinear( m_propDistanceGap->get( true ), m_propKeepBases->get( true ) );
                message = "Node Collapsing, linear: ";
                break;
            case 5:
                prunedElements.first = processer.flattenSelection( m_selectionClusters, m_propKeepBases->get( true ) );
                message = "Selection Flattened: ";
                break;
            case 6:
                prunedElements = processer.pruneSelection( m_selectionClusters );
                message = "Selection Pruned: ";
                break;
            default:
                break;
            }

            progressProcessing->finish();

            initTreeData();
            initProperties();
            updateSelection();

            if( m_propProcessSelector->get( true ).getItemIndexOfSelected( 0 ) != 8 )
            {
                message+= ( string_utils::toString( prunedElements.first ) +
                          " leaves and "+ string_utils::toString( prunedElements.second )+ " nodes eliminated" );
            }
            m_infoPartMessage->set( message );

            m_propDoProcessing->set( WPVBaseTypes::PV_TRIGGER_READY, false );
        }

        // === PARTITION TREE OPTIONS ===

        // add partitions
        if( m_propAddPartTrigger->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            addPartitions();
        }

        // recolor partitions
        if( m_propRecolorPartTrigger->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            recolorPartitions();
        }

        if( m_propClearPartsTrigger->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            infoLog() << "Clearing partitions";
            m_tree.clearPartitions();
            m_propPreloadPartitionNr->removeConstraint( PC_MAX );
            m_propPreloadPartitionNr->setMax( 0 );
            m_propPreloadPartitionNr->removeConstraint( PC_MIN );
            m_propPreloadPartitionNr->setMin( 0 );
            m_propPreloadPartitionNr->set( 0 );
            m_propClearPartsTrigger->set( WPVBaseTypes::PV_TRIGGER_READY, false );
        }


        // === WRITE TREE OPTIONS ===

        // write tree file
        if( m_propWriteTreeTrigger->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            infoLog() << "Writing tree file";
            boost::filesystem::path fileName = m_propWriteTreeFilename->get();
            m_tree.writeTree( fileName.string() );
            m_propWriteTreeTrigger->set( WPVBaseTypes::PV_TRIGGER_READY, false );
        }

        // write partition file
        if( m_propPartitionsTrigger->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            infoLog() << "Writing partition files";
            writePartition();
            m_propPartitionsTrigger->set( WPVBaseTypes::PV_TRIGGER_READY, false );
        }
    }
// DEPRECATED    con.disconnect();

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_moduleNode );
} // end moduleMain()

/*
  This function aims to display and color a lego-like surface of the non-zero voxels in the grid, that has been previously calculated and stored in m_cubeMesh,
  and give the vertices corresponding to each "cube" of this surface a color defined by the value of the voxel
  (as it is just the outer surface the number of vertices corresponding to each cube will vary depending of thesurrounding geometry)
  */
void WMHierchClustDisplay::renderCubeMesh()
{
    // if mesh doesnt need to be rendered, finish
    if( !m_propShowCubeMesh->get( true ) )
    {
        m_meshNode->clear(); //this will make the m_meschCube surface not to be shown in screen
        return;
    }

    boost::shared_ptr< WProgress > progressRenderCubes = boost::shared_ptr< WProgress >( new WProgress( "Rendering seed voxels..." ) );
    m_progress->addSubProgress( progressRenderCubes );

    {
        osg::Geometry* surfaceGeometry = new osg::Geometry();
        osg::ref_ptr< osg::Geode >outputGeode = osg::ref_ptr< osg::Geode >( new osg::Geode );

        outputGeode->setName( std::string( "seed voxels" )  );
        surfaceGeometry->setVertexArray( m_cubeMesh->getVertexArray() );

        // ------------------------------------------------
        // normals
        // NOTE: this was done using bind_per_primitive. This is not available anymore.
        surfaceGeometry->setNormalArray( m_cubeMesh->getVertexNormalArray( true ) );
        surfaceGeometry->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );

        // ------------------------------------------------
        // colors


        std::vector<WHcoord> searchCoords; // here we simply a vector of coordinates  that constitutes the direct nehborhood in positive space,
                                           // 000 001 100 010 101 and so on... till 111
        searchCoords.reserve( 8 );
        for( int cz = 0; cz < 2; ++cz )
        {
            for( int cy = 0; cy < 2; ++cy )
            {
                for( int cx = 0; cx < 2; ++cx )
                {
                    searchCoords.push_back( WHcoord( cx, cy, cz ) );
                }
            }
        }

        // this is a table that maps a label value (the value ofa voxel in the grid)
        std::vector<std::map<size_t, WColor>::const_reverse_iterator> colorTable;
        colorTable.reserve( m_selectionColors.size() );
        for( std::map< size_t, WColor >::const_reverse_iterator colorIter( m_selectionColors.rbegin() );
             colorIter != m_selectionColors.rend();
             ++colorIter )
        {
            colorTable.push_back( colorIter );
        }

        osg::Vec4Array* cubeMeshColors = new osg::Vec4Array; //this will keep the colors for each vertex

        for( size_t i = 0; i < m_cubeMesh->getVertexArray()->getNumElements(); ++i ) // loop over all vertices of cube mesh
        {
            // decide to which voxel centre (cube centre) a vertex is going to be linked to in order to color it according to the value of that voxel
            unsigned int vX( m_cubeMesh->getVertex( i ).x() );
            if( vX >= m_grid->getNbCoordsX() )
            {
                vX = m_grid->getNbCoordsX()-1;
            }
            unsigned int vY( m_cubeMesh->getVertex( i ).y() );
            if( vY >= m_grid->getNbCoordsY() )
            {
                vY  = m_grid->getNbCoordsY()-1;
            }
            unsigned int vZ( m_cubeMesh->getVertex( i ).z() );
            if( vZ >=  m_grid->getNbCoordsZ() )
            {
                vZ = m_grid->getNbCoordsZ()-1;
            }

            size_t colorLabel( 0 );
            for( size_t j = 0; j < searchCoords.size(); ++j ) // assing color to the vertex
            {
                if( ( ( static_cast<int>( vX ) )-searchCoords[j].m_x ) < 0 ||
                    ( ( static_cast<int>( vY ) )-searchCoords[j].m_y ) < 0 ||
                    ( ( static_cast<int>( vZ ) )-searchCoords[j].m_z ) < 0 )
                {
                    continue;
                }
                // m_texture labels, keeps a value for each voxel of the grid, that will define the color
                colorLabel = m_textureLabels[m_grid->getVoxelNum( vX-searchCoords[j].m_x, vY-searchCoords[j].m_y, vZ-searchCoords[j].m_z )];
                // if the label is 0 or is equal to m_tree.getNumNodes()+1  we dont want to recolor it
                if( colorLabel != 0 && colorLabel != m_tree.getNumNodes() + 1 )
                {
                    break;
                }
            }

            if( colorLabel == m_tree.getNumNodes()+2 ) //depending on the colorLabel value, we give it a different color vector value
            {
                cubeMeshColors->push_back( m_propBoundaryColor->get( true ) );
            }
            else if( colorLabel == 0 || colorLabel == m_tree.getNumNodes()+1  )
            {
                cubeMeshColors->push_back( m_propInactiveColor->get( true ) );
            }
            else
            {
                cubeMeshColors->push_back( colorTable[colorLabel-1]->second );
            }
        }

        surfaceGeometry->setColorArray( cubeMeshColors ); // assing the array of colors per vertex to the geometry
        surfaceGeometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );


        osg::DrawElementsUInt* surfaceElement = new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES, 0 );

        std::vector< size_t >tris = m_cubeMesh->getTriangles();
        surfaceElement->reserve( tris.size() );

        for( unsigned int vertId = 0; vertId < tris.size(); ++vertId )
        {
            surfaceElement->push_back( tris[vertId] );
        }
        surfaceGeometry->addPrimitiveSet( surfaceElement );
        outputGeode->addDrawable( surfaceGeometry );

        osg::StateSet* state = outputGeode->getOrCreateStateSet();
        osg::ref_ptr<osg::LightModel> lightModel = new osg::LightModel();
        lightModel->setTwoSided( true );
        state->setAttributeAndModes( lightModel.get(), osg::StateAttribute::ON );
        state->setMode(  GL_BLEND, osg::StateAttribute::ON );

        m_meshNode->clear();
        m_meshNode->insert( outputGeode );
    }

    if( m_propShowDiscarded->get() )
    {
        osg::Geometry* surfaceGeometry = new osg::Geometry();
        osg::ref_ptr< osg::Geode >outputGeode = osg::ref_ptr< osg::Geode >( new osg::Geode );

        outputGeode->setName( ( std::string( "non active" ) ).c_str() );

        surfaceGeometry->setVertexArray( m_discardedMesh->getVertexArray() );

        // ------------------------------------------------
        // normals
        // NOTE: this was done using bind_per_primitive. This is not available anymore.
        surfaceGeometry->setNormalArray( m_discardedMesh->getVertexNormalArray( true ) );
        surfaceGeometry->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );

        // ------------------------------------------------
        // colors
        osg::Vec4Array* discardedColor = new osg::Vec4Array;
        discardedColor->push_back( m_propDiscardedColor->get( true ) );
        surfaceGeometry->setColorArray( discardedColor );
        surfaceGeometry->setColorBinding( osg::Geometry::BIND_OVERALL );

        osg::DrawElementsUInt* surfaceElement = new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES, 0 );

        std::vector< size_t >tris = m_discardedMesh->getTriangles();
        surfaceElement->reserve( tris.size() );

        for( unsigned int vertId = 0; vertId < tris.size(); ++vertId )
        {
            surfaceElement->push_back( tris[vertId] );
        }
        surfaceGeometry->addPrimitiveSet( surfaceElement );
        outputGeode->addDrawable( surfaceGeometry );

        osg::StateSet* state = outputGeode->getOrCreateStateSet();
        osg::ref_ptr<osg::LightModel> lightModel = new osg::LightModel();
        lightModel->setTwoSided( true );
        state->setAttributeAndModes( lightModel.get(), osg::StateAttribute::ON );
        state->setMode(  GL_BLEND, osg::StateAttribute::ON );

        m_meshNode->insert( outputGeode );
    }
    progressRenderCubes->finish();
    return;
} // end renderCubeMesh()



void WMHierchClustDisplay::updateWidgets()
{
    osg::ref_ptr<osgViewer::View> viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getView();
    int totalHeight = viewer->getCamera()->getViewport()->height();
    int totalWidth = viewer->getCamera()->getViewport()->width();

    const unsigned int labelHeight = 20;
    const unsigned int labelWidth = 100;
    int labelRows = ( totalHeight - 20 ) / labelHeight;

    if( ( totalHeight != m_oldViewHeight ) || totalWidth != m_oldViewWidth )
    {
        m_oldViewHeight = totalHeight;
        m_oldViewWidth = totalWidth;
        m_treeDirty = true;
        m_labelsDirty = true;
    }

    bool buttonClicked( false );

    for( size_t i = 0; i < m_selectionLabels.size(); ++i )
    {
        if( m_selectionLabels[i]->clicked() )
        {
            if( m_selectionLabels[i]->pushed() )
            {
                m_selectionColors[m_selectionLabels[i]->getId()] = WColor( 0.9, 0.9, 0.9, 1 );
            }
            else
            {
                m_selectionColors[m_selectionLabels[i]->getId()] = m_selectionColorsBackup[m_selectionLabels[i]->getId()];
            }
            m_selectionLabels[i]->setBackgroundColor( m_selectionColors[m_selectionLabels[i]->getId()] );
            m_propSubselectedCluster->set( m_selectionLabels[i]->getId() );
            m_propSubselectedcolor->set( m_selectionColors[m_selectionLabels[i]->getId()] );
            buttonClicked = true;
            updateColors();
        }
    }

    if( !buttonClicked && !m_treeDirty && !m_labelsDirty )
    {
        return;
    }

    boost::shared_ptr< WProgress > progressUpdateWidgets = boost::shared_ptr< WProgress >( new WProgress( "Updating widgets..." ) );
    m_progress->addSubProgress( progressUpdateWidgets );

    int usedXoffset( 0 );
    int usedWidth( totalWidth );
    int usedHeight( totalHeight/2 );
    int usedYoffset( 0 );

    switch( m_propDendroSideSelector->get( true ).getItemIndexOfSelected( 0 ) )
    {
        case 0:
                usedWidth -= 20;
                usedXoffset += 10;
                break;
        case 1:
                usedWidth = ( usedWidth-30 )/2;
                usedXoffset += 10;
                break;
        case 2:
                usedXoffset += ( usedWidth/2 ) + 5;
                usedWidth = ( usedWidth-30 )/2;
                break;
        case 3:
                usedWidth = m_propDendroSizeX->get( true );
                usedHeight = m_propDendroSizeY->get( true );
                usedXoffset = m_propDendroOffsetX->get( true );
                usedYoffset = m_propDendroOffsetY->get( true );
                break;
        default:
                break;
     }


    if( m_labelsDirty )
    {
        // redo labels
        for( size_t i = 0; i < m_selectionLabels.size(); ++i )
        {
            m_wm->removeChild( m_selectionLabels[i] );
        }
        m_selectionLabels.clear();

        if( m_propShowLabels->get( true ) )
        {
            std::vector<std::pair<float, size_t> > labelPairs;
            labelPairs.reserve( m_selectionColors.size() );
            for( std::map<size_t, WColor>::iterator cIter( m_selectionColors.begin() ); cIter != m_selectionColors.end(); ++cIter )
            {
                const WHnode& thisCluster( m_tree.getNode( cIter->first ) );
                float value( 0 );
                switch( m_propLabelInfoSelector->get( true ).getItemIndexOfSelected( 0 ) )
                {
                case 0:
                    value = thisCluster.getID();
                    break;
                case 1:
                    value = thisCluster.getSize();
                    break;
                case 2:
                    value = thisCluster.getDistLevel();
                    break;
                case 3:
                    value = thisCluster.getHLevel();
                    break;
                default:
                    break;
                }
                labelPairs.push_back( std::make_pair( value, thisCluster.getID() ) );
            }

            std::sort( labelPairs.begin(), labelPairs.end() );
            for( size_t i = 0; i < labelPairs.size(); ++i )
            {
                std::string labelString;
                if( labelPairs[i].first<1 && labelPairs[i].first>0 )
                {
                    labelString = ( str( boost::format( "%.05f" ) % labelPairs[i].first ) );
                }
                else
                {
                    labelString = ( str( boost::format( "%-6d" ) % labelPairs[i].first ) );
                }

                osg::ref_ptr<WOSGButton> newButton = osg::ref_ptr<WOSGButton>( new WOSGButton( labelString, osgWidget::Box::VERTICAL, true, true ) );
                newButton->setPosition( osg::Vec3( ( i / labelRows ) * labelWidth + usedXoffset,  ( i % labelRows ) * 20.f, 0 ) );
                newButton->setId( labelPairs[i].second );
                newButton->setBackgroundColor( m_selectionColors[labelPairs[i].second] );
                newButton->managed( m_wm );
                m_wm->addChild( newButton );
                m_selectionLabels.push_back( newButton );
            }
        }
        m_labelsDirty = false;
    }

    if( m_propShowLabels->get( true ) )
    {
        usedXoffset += ( ( m_selectionColors.size() / labelRows ) + 1 ) * labelWidth;
        usedWidth -= ( ( m_selectionColors.size() / labelRows ) + 1 ) * labelWidth;
    }

    if( m_treeDirty )
    {
        m_wm->resizeAllWindows();

        m_camera->removeChild( m_dendrogramGeode );

        if( m_propShowDendrogram->get( true ) )
        {
            m_dendrogramGeode = new WNonBinDendroGeode( m_tree, m_treeDisplayColors, m_treeZoomRoot, usedWidth , usedHeight , usedXoffset,
                            usedYoffset, m_propDendroPlotByLevel->get( true ), m_propTriangleLeaves->get( true ), m_horizontalLine );

            m_camera->addChild( m_dendrogramGeode );
        }
        m_treeDirty = false;
    }
    progressUpdateWidgets->finish();

    return;
} // end updateWidgets()



    //DEPRECATED void WMHierchClustDisplay::dendrogramClick( WPickInfo pickInfo )
bool WMHierchClustDisplay::dendrogramClick( const WVector2f& pos )
{
    //DEPRECATED    if( !m_propShowDendrogram->get() || !( pickInfo.getName() == "nothing" ) )
    if( m_tree.isLoaded() && m_propShowDendrogram->get( true ) )
    {
        int x = pos[0];
        int y = pos[1];
        if( m_dendrogramGeode->inDendrogramArea( x, y ) )
        {
            size_t cluster = m_dendrogramGeode->getClickedCluster( x, y );
            if( cluster != m_treeZoomRoot+1 )
            {
                m_propSourceCluster->set( cluster );
            }
            return true;
        }
    }

    //DEPRECATED    int x = pickInfo.getPickPixel().x();
    //DEPRECATED    int y = pickInfo.getPickPixel().y();
    //DEPRECATED    size_t cluster = m_dendrogramGeode->getClickedCluster( x, y );
    //DEPRECATED m_buttonExecuteSelection->set( WPVBaseTypes::PV_TRIGGER_TRIGGERED );} // end dendrogramClick()

    return false;
} // end dendrogramClick()


bool WMHierchClustDisplay::dendrogramClickRight( const WVector2f& pos )
{
    if( m_tree.isLoaded() && m_propShowDendrogram->get( true ) )
    {
        int x = pos[0];
        int y = pos[1];
        if( m_dendrogramGeode->inDendrogramArea( x, y ) )
        {
            size_t cluster = m_dendrogramGeode->getClickedCluster( x, y );

            if( cluster != m_treeZoomRoot+1 )
            {
                std::vector< size_t >::const_iterator findIter( std::find( m_selectionClusters.begin(), m_selectionClusters.end(), cluster ) );
                if( findIter != m_selectionClusters.end() )
                {
                    //size_t sel_pos=findIter-m_selectionClusters.begin();
                    //std::cout<<"cluster pos "<<sel_pos<<std::endl;
                    m_propSubselectedCluster->set( cluster );

                    if( m_selectionColors[cluster] == WColor( 0.9, 0.9, 0.9, 1 ) )
                    {
                        m_selectionColors[cluster]=m_selectionColorsBackup[cluster];
                    }
                    else
                    {
                        m_selectionColors[cluster] = WColor( 0.9, 0.9, 0.9, 1 );
                    }
                    m_propSubselectedcolor->set( m_selectionColorsBackup[cluster] );
                    m_labelsDirty = true;
                    updateColors();
                }
            }
            return true;
        }
    }
    return false;
}

void WMHierchClustDisplay::colorBranch( const size_t &thisNode, const WColor &color )
{
    const WHnode& current( m_tree.getNode( thisNode ) );
    m_treeDisplayColors[thisNode] = color;

    // if the children are anly leaves stop
    if( current.getHLevel() == 1 )
        return;

    std::vector<nodeID_t> kids( current.getChildren() );
    for( unsigned int i = 0; i < kids.size(); ++i )
    {
        if( kids[i].first )
        {
            //if its a node
            colorBranch( kids[i].second, color );
        }
    }
    return;
}

void WMHierchClustDisplay::colorBranchCustom( const size_t &thisNode, const WColor &color )
{
    const WHnode& current( m_tree.getNode( thisNode ) );
    m_nodeColorsCustom[thisNode] = color;

    // if the children are only leaves stop
    if( current.getHLevel() == 1 )
        return;

    std::vector<nodeID_t> kids( current.getChildren() );
    for( unsigned int i = 0; i < kids.size(); ++i )
    {
        if( kids[i].first )
        {
            //if its a node
            colorBranchCustom( kids[i].second, color );
        }
    }
    return;
}


void WMHierchClustDisplay::updateSelection( const std::vector< WColor > &preloadedColors )
{
    boost::shared_ptr< WProgress > progressUpdateSelection = boost::shared_ptr< WProgress >( new WProgress( "Updating selection..." ) );
    m_progress->addSubProgress( progressUpdateSelection );

    // get a color for each of the clusters currently selected
        m_selectionColors.clear();
        m_selectionColorsBackup.clear();

    if( preloadedColors.empty() || preloadedColors.size() != m_selectionClusters.size() )
    {
        // sorting before color assignment
        switch( m_propColorSchemeSelector->get( true ).getItemIndexOfSelected( 0 ) )
        {
            case 0:
                break;
            case 1:
//UNUSED            case 2:
                    m_tree.sortBySize( &m_selectionClusters );
                    std::reverse( m_selectionClusters.begin(), m_selectionClusters.end() );
                break;
            default:
                infoLog() << "Error at color assignment";
                break;
        }

//UNUSED         unsigned int seed( 0 );
        for( size_t k = 0; k < m_selectionClusters.size(); ++k )
        {
            switch( m_propColorSchemeSelector->get( true ).getItemIndexOfSelected( 0 ) )
            {
                case 0:
                        m_selectionColors.insert( std::make_pair( m_selectionClusters[k], m_nodeColorsCustom[m_selectionClusters[k]] ) );
                    break;
                case 1:
                        m_selectionColors.insert( std::make_pair( m_selectionClusters[k], wge::getNthHSVColor( k ) ) );
                    break;
//UNUSED                case 2:
//UNUSED                        m_selectionColors.insert( std::make_pair( m_selectionClusters[k], wge::getNthHSVColor( rand_r( &seed ) ) ) );
//UNUSED                    break;
                default:
                infoLog() << "Error at color assignment";
                    break;
            }
        }
    }
    else
    {
        infoLog() << "Preloaded colors";
        for( size_t k = 0; k < m_selectionClusters.size(); ++k )
        {
            m_selectionColors.insert( std::make_pair( m_selectionClusters[k], preloadedColors[k] ) );
        }
    }
    // after assigning colors we need to order in decreasing hierarchical order for cluster and dendrogram coloring to work
    std::sort( m_selectionClusters.begin(), m_selectionClusters.end() );
    std::reverse( m_selectionClusters.begin(), m_selectionClusters.end() );

    m_selectionColorsBackup = m_selectionColors;

    m_labelsDirty = true;

    progressUpdateSelection->finish();

    updateColors();

    return;
} // end updateSelection()


void WMHierchClustDisplay::updateColors()
{
    const size_t filteredLabel( m_tree.getNumNodes()+2 );
    const size_t discardedLabel( m_tree.getNumNodes()+1 );


    // recolor clusters for shown tree
    boost::shared_ptr< WProgress > progressUpdateColors = boost::shared_ptr< WProgress >( new WProgress( "Updating colors..." ) );
    m_progress->addSubProgress( progressUpdateColors );


    // set colors for cluster in the tree

    colorBranch( m_tree.getRoot().getID(), WColor( 0.3, 0.3, 0.3, 1 ) );

    for( std::map< size_t, WColor >::reverse_iterator revIter( m_selectionColors.rbegin() ); revIter != m_selectionColors.rend(); ++revIter )
    {
        colorBranch(  revIter->first, revIter->second );
    }


    // reassing texture labels
    m_textureLabels.clear();
    m_textureLabels.resize( m_grid->size(), 0 );
    if( m_propShowDiscarded->get( true ) )
    {
        for( std::vector<size_t>::iterator discardedIter( m_discardedVoxels.begin() ); discardedIter != m_discardedVoxels.end(); ++discardedIter )
        {
            m_textureLabels[*discardedIter] = discardedLabel;
        }
    }


    std::vector<std::map<size_t, WColor>::const_reverse_iterator> colorTable;
    colorTable.reserve( m_selectionColors.size() );
    {
        size_t thisLabel = 1;
        for( std::map< size_t, WColor >::const_reverse_iterator revIter( m_selectionColors.rbegin() );
             revIter != m_selectionColors.rend();
             ++revIter )
        {
            for( size_t k = 0; k < m_clusterVoxels[revIter->first].size(); ++k )
            {
                m_textureLabels[m_clusterVoxels[revIter->first][k]] = thisLabel;
            }
            ++thisLabel;
            colorTable.push_back( revIter );
        }
    }

    // reassing labels to show only boundaries if set
    if( m_propBoundaries->get( true ) )
    {
        const size_t inactiveLabel(  m_tree.getNumNodes()+3 );
        size_t rootID( m_tree.getRoot().getID() );
        for( size_t i = 0; i < m_clusterVoxels[rootID].size(); ++i )
        {
            if( m_textureLabels[m_clusterVoxels[rootID][i]] == 0 )
            {
                m_textureLabels[m_clusterVoxels[rootID][i]] = inactiveLabel;
            }
        }

        for( size_t i = 0; i < m_grid->size(); ++i )
        {
            if( m_textureLabels[i] == discardedLabel || m_textureLabels[i] == 0 )
            {
                continue;
            }

            size_t thisLabel( m_textureLabels[i] );
            std::vector<size_t> nbors( m_grid->getNeighbours27( i ) );


            for( size_t j = 0; j < nbors.size(); ++j )
            {
                size_t nbID( nbors[j] );
                size_t nbLabel( m_textureLabels[nbID] );
                if( nbLabel == discardedLabel || nbLabel == 0 || nbLabel == filteredLabel || nbLabel== thisLabel )
                {
                    continue;
                }
                else
                {
                    m_textureLabels[i] = filteredLabel;
                    break;
                }
            }
        }
        for( size_t i = 0; i < m_textureLabels.size(); ++i )
        {
            if( m_textureLabels[i] == discardedLabel || m_textureLabels[i] == filteredLabel || m_textureLabels[i] == 0 )
            {
                continue;
            }
            else
            {
                 m_textureLabels[i] = 0;
            }
        }
    }


    // redraw texture
    unsigned char* data = m_texture->getImage()->data();
    for( size_t i = 0; i < m_grid->size(); ++i )
    {
        WColor color( 0, 0, 0, 1 );

        if( m_textureLabels[i] == discardedLabel )
        {
            color = m_propDiscardedColor->get( true );
        }
        else if( m_textureLabels[i] == filteredLabel )
        {
            color = m_propBoundaryColor->get( true );
        }
        else if( m_textureLabels[i] != 0 )
        {
            color = colorTable[m_textureLabels[i]-1]->second;
        }

        data[i * 3    ] = color[0] * 255;
        data[i * 3 + 1] = color[1] * 255;
        data[i * 3 + 2] = color[2] * 255;
    }
    m_texture->dirtyTextureObject();


    // update output texture
    WAssert( m_anatomy, "" );
    WAssert( m_anatomy->getValueSet(), "" );
    WAssert( m_anatomy->getGrid(), "" );

    boost::shared_ptr< std::vector< unsigned char > >ptr( new std::vector< unsigned char >( m_textureLabels.size() * 3 ) );

    for( size_t i = 0; i < m_textureLabels.size()*3; ++i )
    {
        ptr->at( i ) = data[i];
    }

    boost::shared_ptr< WValueSet< unsigned char > > vs =
        boost::shared_ptr< WValueSet< unsigned char > >( new WValueSet< unsigned char >( 1, 3, ptr, W_DT_UINT8 ) );

    boost::shared_ptr< WGridRegular3D> grid =
                boost::shared_ptr< WGridRegular3D>( new WGridRegular3D(
                                                  m_grid->getNbCoordsX(), m_grid->getNbCoordsY(), m_grid->getNbCoordsZ(), m_grid->getTransform() ) );
    m_outData = boost::shared_ptr< WDataSetVector >( new WDataSetVector( vs, grid ) );
    m_output->updateData( m_outData );

    m_treeDirty = true;

    progressUpdateColors->finish();

    renderCubeMesh();

    return;
} // end updateColors()


void WMHierchClustDisplay::updateOutput2()
{
    //update coordinates output
    boost::shared_ptr< WProgress > progressUpdateOutput2 = boost::shared_ptr< WProgress >( new WProgress( "Updating Output 2 ( coordinates )..." ) );
    m_progress->addSubProgress( progressUpdateOutput2 );

    boost::shared_ptr< std::vector< int > >coordPtr( new std::vector< int >() );
    std::vector<WHcoord> coordinates( m_tree.getRoi() );
    for( std::vector<WHcoord>::iterator coordIter( coordinates.begin() ); coordIter != coordinates.end(); ++coordIter )
    {
        coordPtr->push_back( coordIter->m_x );
        coordPtr->push_back( coordIter->m_y );
        coordPtr->push_back( coordIter->m_z );
    }
    std::list<WHcoord> discarded( m_tree.getDiscarded() );
    if( m_propShowDiscarded->get( true ) )
    {
        for( std::list<WHcoord>::iterator discardedIter( discarded.begin() ); discardedIter != discarded.end(); ++discardedIter )
        {
            coordPtr->push_back( discardedIter->m_x );
            coordPtr->push_back( discardedIter->m_y );
            coordPtr->push_back( discardedIter->m_z );
        }
    }
    boost::shared_ptr< WValueSet< int > > vs = boost::shared_ptr< WValueSet< int > >( new WValueSet< int >( 0, 1, coordPtr, W_DT_INT16 ) );
    boost::shared_ptr< WGridRegular3D> grid = boost::shared_ptr< WGridRegular3D>(
                new WGridRegular3D( coordPtr->size(), 1, 1, m_grid->getTransform() ) );

    m_outData2 = boost::shared_ptr< WDataSetScalar >( new WDataSetScalar( vs, grid ) );
    m_output2->updateData( m_outData2 );

    progressUpdateOutput2->finish();

    return;
} // end updateOutput2()


void WMHierchClustDisplay::writePartition()
{
    if( m_selectionClusters.empty() )
    {
        infoLog() << "Partition empty, partition file not written";
        return;
    }
    boost::filesystem::path partitionFilename = m_propPartitionFile->get();

    std::ofstream streamFile( partitionFilename.string() );
    if( !streamFile )
    {
        errorLog() << "ERROR: unable to open partition file: \"" << partitionFilename << "\"";
    }
    else
    {
        for( size_t i = 0; i < m_selectionClusters.size(); ++i )
        {
            const WHnode &thisNode( m_tree.getNode( m_selectionClusters[i] ) );
            size_t thisID( thisNode.getID() );
            std::vector< WHcoord > thisCoords = m_tree.getCoordinates4node( thisID );
            for( size_t j = 0; j < thisCoords.size(); ++j )
            {
                streamFile << thisCoords[j] << " " << thisID << std::endl;
            }
        }
    }
} // end writePartition()

void WMHierchClustDisplay::clusterSelection()
{
    boost::shared_ptr< WProgress > progressPartition = boost::shared_ptr< WProgress >( new WProgress( "Finding partition..." ) );
    m_progress->addSubProgress( progressPartition );

    std::vector<nodeID_t> partition;
    std::vector< size_t > selectedPartition;
    std::vector< WHcoord > selectedColors;
    std::vector< WColor > partitionColors;

    float returnedCutValue( 0 );
    float conditionValue( 0 );
    float relativeCoefficient( 0 );
    HT_CONDITION conditionMode( HTC_CNUM );
    HT_PARTMODE partitionMode1( HTP_HOZ );
    HT_PARTMODE2 partitionMode2( HTP2_CSD );

    WHtreePartition partitioner( &m_tree );

    switch( m_propPartitionSelector->get( true ).getItemIndexOfSelected( 0 ) )
    {
    case 0:
        partitionMode1 = HTP_HOZ;
        conditionValue = m_propPartDistanceValue->get( true );
        relativeCoefficient = 0.01;
        break;
    case 1:
        partitionMode1 = HTP_HLEVEL;
        conditionValue = m_propPartHlevelValue->get( true );
        relativeCoefficient = m_tree.getNode( m_propSourceCluster->get( true ) ).getHLevel()/100.0;
        break;
    case 2:
        partitionMode2 = HTP2_CSD;
        conditionValue = m_propPartSizeValue->get( true );
        relativeCoefficient = m_tree.getNode( m_propSourceCluster->get( true ) ).getSize()/100.0;
        break;
    case 3:
        partitionMode2 = HTP2_OPT;
        break;
    case 4:
        conditionValue = m_propPartDistanceValue->get( true );
        break;
    case 5:
        break;
    default:
        break;
    }

    switch( m_propConditionSelector->get( true ).getItemIndexOfSelected( 0 ) )
    {
        case 0:
            conditionValue = m_propPartNumClusters->get( true );
            conditionMode = HTC_CNUM;
            break;
        case 1:
            conditionMode = HTC_VALUE;
            break;
        case 2:
            conditionValue = m_propPartRelativeValue->get( true )*relativeCoefficient;
            conditionMode = HTC_VALUE;
            break;
        default:
            break;
    }

    switch( m_propPartitionSelector->get( true ).getItemIndexOfSelected( 0 ) )
    {
    case 0:
    case 1:
        returnedCutValue = partitioner.partitionClassic( conditionValue,
                                                    &partition,
                                                    partitionMode1,
                                                    conditionMode,
                                                    m_propPartExcludeLeaves->get( true ),
                                                    m_propSourceCluster->get( true ) );
        break;
    case 2:
    case 3:
        returnedCutValue = partitioner.partitionOptimized( conditionValue,
                                                      &partition,
                                                      partitionMode2,
                                                      conditionMode,
                                                      m_propPartExcludeLeaves->get( true ),
                                                      m_propSourceCluster->get( true ),
                                                      m_propPartSearchDepthValue->get( true ) );
        break;
    case 4:
        returnedCutValue = partitioner.partitionSharp( conditionValue,
                                                  &partition,
                                                  m_propPartExcludeLeaves->get( true ),
                                                  m_propSourceCluster->get( true ), false );
        break;
    case 5:
        if(  m_tree.getSelectedValues().empty() )
        {
            infoLog() << "tree has no partitions";
            break;
        }
        else
        {
            int partIndex = m_propPreloadPartitionNr->get( true ) -1;
            if( partIndex < 0 )
            {
                infoLog() << " Partition index is negative";
                break;
            }

            returnedCutValue = m_tree.getSelectedValues( partIndex );
            selectedPartition = m_tree.getSelectedPartitions( partIndex );
            selectedColors = m_tree.getSelectedColors( partIndex );

            partition.clear();
            partition.reserve( selectedPartition.size() );

            for( size_t i = 0; i < selectedPartition.size(); ++i )
            {
                partition.push_back( std::make_pair( true, selectedPartition[i] ) );
            }
        }
        break;
    default:
        break;
    }

    if( m_propPartitionSelector->get( true ).getItemIndexOfSelected( 0 ) == 0 )
    {
        m_horizontalLine = returnedCutValue;
    }
    size_t totalLeafClusterNum( 0 ), totalNodeClusterNum( 0 );

    {
        std::vector<size_t> empty1, empty2, empty3;
        m_selectionClusters.swap( empty1 );
        m_selectionLeaves.swap( empty3 );
    }
    partitionColors.clear();
    size_t addIndex( partition.size() );

    for( size_t i = 0; i < partition.size(); ++i )
    {
        if( partition[i].first )
        {
            m_selectionClusters.push_back( partition[i].second );
            ++totalNodeClusterNum;

            if( partition.size() == selectedColors.size() )
            {
                float Rcolor( selectedColors[i].m_x/ 255.0 );
                float Gcolor( selectedColors[i].m_y/ 255.0 );
                float Bcolor( selectedColors[i].m_z/ 255.0 );
                WColor thisColor( Rcolor, Gcolor, Bcolor, 1.0 );

                // if encoded color is black, select a new color
                if( thisColor == WColor( 0, 0, 0, 1 ) )
                {
                    partitionColors.push_back( wge::getNthHSVColor( addIndex++ ) );
                }
                else
                {
                    partitionColors.push_back( thisColor );
                }
            }
        }
        else
        {
            m_selectionLeaves.push_back( partition[i].second );
            ++totalLeafClusterNum;
        }
    }
    std::string message( "Total of " + string_utils::toString( partition.size() ) + " clusters" );
    message += ( " (" + string_utils::toString( totalNodeClusterNum ) + " nodes, " );
    message += ( string_utils::toString( totalLeafClusterNum )+ " single leaves)" );
    message += ( " at a cut level: "+ string_utils::toString( returnedCutValue ) );
    m_infoPartMessage->set( message );

    m_infoPartNumber->set( partition.size() );
    m_infoPartActive->set( totalNodeClusterNum );
    m_infoPartLeaves->set( totalLeafClusterNum );
    m_infoCutValue->set( returnedCutValue );

    std::sort( m_selectionLeaves.begin(), m_selectionLeaves.end() );
    std::reverse( m_selectionLeaves.begin(), m_selectionLeaves.end() );

    progressPartition->finish();
    updateSelection( partitionColors );
    m_propDoPartition->set( WPVBaseTypes::PV_TRIGGER_READY, false );
}



void WMHierchClustDisplay::colorAction()
{
    m_labelsDirty = true;
    bool doUpdateSelection( true );
    std::map<size_t, WColor >::iterator clustIter( m_selectionColors.find( m_propSubselectedCluster->get( true ) ) );

    switch( m_propColorActionSelector->get( true ).getItemIndexOfSelected( 0 ) )
    {
    case 0:
        {
        std::vector<WColor> shuffledColors;
        shuffledColors.reserve( m_selectionColorsBackup.size() );
            for( std::map<size_t, WColor>::const_iterator cIter( m_selectionColorsBackup.begin() );
                 cIter != m_selectionColorsBackup.end();
                 ++cIter )
            {
                shuffledColors.push_back( cIter->second );
            }
            std::random_shuffle( shuffledColors.begin(), shuffledColors.end() );
            size_t i( 0 );
            for( std::map<size_t, WColor>::iterator cIter( m_selectionColorsBackup.begin() ); cIter != m_selectionColorsBackup.end(); ++cIter)
            {
                cIter->second = shuffledColors[i++];
            }
        }
        m_selectionColors = m_selectionColorsBackup;
        doUpdateSelection = false;
        break;
    case 1:
        initializeColorsSize();
        break;
    case 2:
        initializeColorsHdist();
        break;
    case 3:
        // change in color table
        m_nodeColorsCustom[ m_propSubselectedCluster->get( true ) ] = m_propSubselectedcolor->get( true );
        //change in current screen
        if( clustIter != m_selectionColors.end() )
        {
            clustIter->second = m_propSubselectedcolor->get( true );
            m_selectionColorsBackup[clustIter->first] = m_propSubselectedcolor->get( true );
        }
        doUpdateSelection = false;
        break;
    case 4:
        //change in current screen
        if( clustIter != m_selectionColors.end() )
        {
            clustIter->second = m_propSubselectedcolor->get( true );
            m_selectionColorsBackup[clustIter->first] = m_propSubselectedcolor->get( true );
        }
        // change in color table
        assignColorHierch( m_propSubselectedCluster->get( true ), m_propSubselectedcolor->get( true ) );
        doUpdateSelection = false;
        break;
    case 5:
        for( std::map<size_t, WColor>::iterator cIter( m_selectionColorsBackup.begin() ); cIter != m_selectionColorsBackup.end(); ++cIter)
        {
            m_nodeColorsCustom[ cIter->first ] = cIter->second;
        }
        break;
    case 6:
        for( std::map<size_t, WColor>::iterator cIter( m_selectionColorsBackup.begin() ); cIter != m_selectionColorsBackup.end(); ++cIter)
        {
            assignColorHierch( cIter->first, cIter->second );
        }
        break;
    default:
        break;
    }

    if( doUpdateSelection )
    {
        m_propColorSchemeSelector->set( m_propColorSchemeList->getSelectorFirst(), true );
        updateSelection();
    }
    else
    {
        updateColors();
    }
    m_propColorActionTrigger->set( WPVBaseTypes::PV_TRIGGER_READY, false );
}


void WMHierchClustDisplay::addPartitions()
{
    if( m_selectionClusters.empty() )
    {
        infoLog() << "Selection is empty. Partition was not added";
    }
    else if( m_selectionClusters.size() == 1 && m_selectionClusters.front() == m_tree.getRoot().getID() )
    {
        infoLog() << "Selection is trivial. Partition was not added";
    }
    else
    {
        infoLog() << "Adding current partition to tree file";
        std::vector< float > partValues( m_tree.getSelectedValues() );
        std::vector< std::vector< size_t > > partSet( m_tree.getSelectedPartitions() );
        std::vector< std::vector< WHcoord > > partColors( m_tree.getSelectedColors() );

        partValues.push_back( m_horizontalLine );
        partSet.push_back( m_selectionClusters );

        std::vector< WHcoord > addedPartColor;
        addedPartColor.reserve( m_selectionColorsBackup.size() );
        for( size_t i = 0; i < m_selectionClusters.size(); ++i )
        {
            WColor thisColor( m_selectionColorsBackup[m_selectionClusters[i]] );
            WHcoord thisColorCoord( thisColor.r()*255, thisColor.g()*255, thisColor.b()*255 );
            addedPartColor.push_back( thisColorCoord );
        }
        partColors.push_back( addedPartColor );
        m_tree.insertPartitions( partSet, partValues, partColors );

        m_propPreloadPartitionNr->removeConstraint( PC_MAX );
        m_propPreloadPartitionNr->setMax( partValues.size() );
        m_propPreloadPartitionNr->removeConstraint( PC_MIN );
        m_propPreloadPartitionNr->setMin( 1 );
        m_propPreloadPartitionNr->set( 1 );

        std::cout << "Tree now has " << partValues.size() << " partitions saved of sizes: " << std::flush;
        for( size_t i = 0; i < partSet.size(); ++i )
        {
            std::cout << partSet[i].size() << "," << std::flush;
        }
        std::cout << std::endl;
    }
    m_propAddPartTrigger->set( WPVBaseTypes::PV_TRIGGER_READY, false );
}

void WMHierchClustDisplay::recolorPartitions()
{
    if( m_tree.getSelectedValues().empty() )
    {
        infoLog() << "WARNING: Selection is empty. Nothing was done";
    }
    else
    {
        if( !m_tree.getSelectedColors().empty() )
        {
            infoLog() << "WARNING: Partitions already had assigned colors... they will be replaced";
        }

        std::vector< float > partValues( m_tree.getSelectedValues() );
        std::vector< std::vector< size_t > > partSet( m_tree.getSelectedPartitions() );
        std::vector< std::vector< WHcoord > > partColors;
        partColors.reserve( partValues.size() );

        for( size_t i = 0; i < partSet.size(); ++i )
        {
            std::vector< WHcoord > addedPartColor;
            addedPartColor.reserve( partSet[i].size() );
            for( size_t j = 0; j < partSet[i].size(); ++j )
            {
                WColor thisColor( m_nodeColorsCustom[partSet[i][j]] );
                WHcoord thisColorCoord( thisColor.r()*255, thisColor.g()*255, thisColor.b()*255 );
                addedPartColor.push_back( thisColorCoord );
            }
            partColors.push_back( addedPartColor );
        }
        m_tree.insertPartitions( partSet, partValues, partColors );
    }
    m_propRecolorPartTrigger->set( WPVBaseTypes::PV_TRIGGER_READY, false );
}


