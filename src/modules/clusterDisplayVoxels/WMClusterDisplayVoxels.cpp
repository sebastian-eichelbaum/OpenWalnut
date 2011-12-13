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

#include <queue>
#include <string>
#include <vector>

#include <osgGA/TrackballManipulator>
#include <osgGA/StateSetManipulator>
#include <osgViewer/ViewerEventHandlers>
#include <osgWidget/Util> //NOLINT
#include <osgWidget/ViewerEventHandlers> //NOLINT
#include <osgWidget/WindowManager> //NOLINT
#include <osg/LightModel>

#include "core/common/WPathHelper.h"
#include "core/common/WPropertyHelper.h"
#include "core/graphicsEngine/algorithms/WMarchingLegoAlgorithm.h"
#include "core/graphicsEngine/WGEColormapping.h"
#include "core/graphicsEngine/WGEUtils.h"
#include "core/kernel/WKernel.h"
#include "core/kernel/WSelectionManager.h"
#include "WFileParser.h"
#include "WMClusterDisplayVoxels.h"
#include "WMClusterDisplayVoxels.xpm"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMClusterDisplayVoxels )

WMClusterDisplayVoxels::WMClusterDisplayVoxels():
    WModule(),
    m_currentDisplayMode( CDV_SINGLE ),
    m_currentDisplayModeString( "" ),
    m_moduleNode( new WGEGroupNode() ),
    m_dendrogramNode( new WGEGroupNode() ),
    m_meshNode( new WGEGroupNode() ),
    m_selectionChanged( false ),
    m_dendrogramDirty( false )
{
}

WMClusterDisplayVoxels::~WMClusterDisplayVoxels()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMClusterDisplayVoxels::factory() const
{
    // See "src/modules/template/" for an extensively documented example.
    return boost::shared_ptr< WModule >( new WMClusterDisplayVoxels() );
}

const char** WMClusterDisplayVoxels::getXPMIcon() const
{
    return clusterDisplayVoxels_xpm;
}
const std::string WMClusterDisplayVoxels::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "ClusterDisplayVoxels";
}

const std::string WMClusterDisplayVoxels::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    // See "src/modules/template/" for an extensively documented example.
    return "Visualizes voxel clusterings in texture and 3D view";
}

void WMClusterDisplayVoxels::connectors()
{
    // the input dataset is just used as source for resolurtion and transformation matrix
    m_input = boost::shared_ptr< WModuleInputData < WDataSetSingle  > >(
        new WModuleInputData< WDataSetSingle >( shared_from_this(), "in", "The input dataset." ) );
    addConnector( m_input );

    m_output = boost::shared_ptr< WModuleOutputData < WDataSetScalar  > >(
        new WModuleOutputData< WDataSetScalar >( shared_from_this(), "out", "The extracted image." ) );
    addConnector( m_output );

    WModule::connectors();
}

void WMClusterDisplayVoxels::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_propClusterFile = m_properties->addProperty( "Cluster file", "", boost::filesystem::path( "/SCR/schurade/data/david/whole/avg.txt" ) );
    m_propReadTrigger = m_properties->addProperty( "Do read",  "Press!", WPVBaseTypes::PV_TRIGGER_READY, m_propCondition );


    // Group selection
    m_groupSelection = m_properties->addPropertyGroup( "Cluster selections",  "Groups the different cluster selection methods" ); //NOLINT

    m_clusterSelectionsList = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_clusterSelectionsList->addItem( "Single", "" );
    m_clusterSelectionsList->addItem( "Biggest", "" );
    m_clusterSelectionsList->addItem( "X Clusters", "" );
    m_clusterSelectionsList->addItem( "Similarity", "" );
    m_clusterSelectionsList->addItem( "Levels from top", "" );
    m_clusterSelectionsList->addItem( "Minimum branch length", "" );
    m_clusterSelectionsList->addItem( "Loaded partition", "" );

    m_clusterSelection = m_groupSelection->addProperty( "Selection method",  "selection",
            m_clusterSelectionsList->getSelectorFirst(), m_propCondition );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_clusterSelection );



    m_propSelectedCluster = m_groupSelection->addProperty( "Selected Cluster", "Selects a single cluster by number.", 0, m_propCondition );
    m_propSelectedCluster->setMin( 0 );
    m_propSelectedCluster->setMax( 0 );

    m_propXBiggestClusters = m_groupSelection->addProperty( "Biggest Clusters", "Selects a number of biggest clusters.", 5, m_propCondition );
    m_propXBiggestClusters->setMin( 1 );
    m_propXBiggestClusters->setMax( 1000 );

    m_propXClusters = m_groupSelection->addProperty( "X Clusters", "Selects a number of clusters by dividing clusters.", 5, m_propCondition );
    m_propXClusters->setMin( 1 );
    m_propXClusters->setMax( 1000 );

    m_propValue = m_groupSelection->addProperty( "Similarity Value", "Selects clusters below a given similarity value", 1.0, m_propCondition );
    m_propValue->setMin( 0.0 );
    m_propValue->setMax( 1.0 );

    m_propLevelsFromTop = m_groupSelection->addProperty( "Levels from top", "", 0, m_propCondition );
    m_propHideOutliers = m_groupSelection->addProperty( "Hide outliers", "", false, m_propCondition );

    m_propSelectedLoadedPartion = m_groupSelection->addProperty( "Loaded Partition", "Activates a predetermined partition, loaded from file", 0, m_propCondition ); // NOLINT
    m_propSelectedLoadedPartion->setMin( 1 );
    m_propSelectedLoadedPartion->setMax( 1 );

    m_propMinBranchLength = m_groupSelection->addProperty( "Minimum branch length", "", 0.1, m_propCondition );
    m_propMinBranchLength->setMin( 0.0 );
    m_propMinBranchLength->setMax( 1.0 );

    m_propMinBranchSize = m_groupSelection->addProperty( "Minimum branch size", "", 50, m_propCondition );
    m_propMinBranchSize->setMin( 1 );
    m_propMinBranchSize->setMax( 500 );

    m_buttonExecuteSelection = m_groupSelection->addProperty( "Update",  "Press!", WPVBaseTypes::PV_TRIGGER_READY, m_propCondition );

    m_propShowSelectedButtons = m_groupSelection->addProperty( "Show Buttons", "Shows/Hides the buttons for selected cluster on the left side", true, m_propCondition ); //NOLINT

    m_buttonLabelList = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_buttonLabelList->addItem( "Number", "" );
    m_buttonLabelList->addItem( "Size", "" );
    m_buttonLabelList->addItem( "Level", "" );
    m_buttonLabelList->addItem( "Similarity", "" );

    m_buttonLabelSelection = m_groupSelection->addProperty( "Button label",  "selection",
                m_buttonLabelList->getSelectorFirst(), m_propCondition );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_buttonLabelSelection );


    //  Group Triangulation
    m_groupTriangulation = m_properties->addPropertyGroup( "Triangulation",  "Groups the triangulation properties" ); //NOLINT
    m_propShowVoxelTriangulation = m_groupTriangulation->addProperty( "Triangulate", "", false, m_propCondition );
    m_showNotInClusters = m_groupTriangulation->addProperty( "Show non active", "", false, m_propCondition );

    // Group Dendrogram
    m_groupDendrogram = m_properties->addPropertyGroup( "Dendrogram",  "Properties only related to the dendrogram." );

    m_propShowDendrogram = m_groupDendrogram->addProperty( "Show dendrogram", "", true, m_propCondition );
    m_propPlotHeightByLevel = m_groupDendrogram->addProperty( "Height by Level or Value", "", false, m_propCondition );

    m_propMinSizeToColor = m_groupDendrogram->addProperty( "Min size to show", "Specifies a minimum size for a cluster to be drawn", 1, m_propCondition ); // NOLINT
    m_propMinSizeToColor->setMin( 1 );
    m_propMinSizeToColor->setMax( 200 );

    m_propZoomIn = m_groupDendrogram->addProperty( "Zoom in", "Press!", WPVBaseTypes::PV_TRIGGER_READY, m_propCondition );
    m_propZoomOut = m_groupDendrogram->addProperty( "Zoom out", "Press!", WPVBaseTypes::PV_TRIGGER_READY, m_propCondition );

    m_propResizeWithWindow = m_groupDendrogram->addProperty( "Resize with window", "", true, m_propCondition );

    m_propDendrogramSizeX = m_groupDendrogram->addProperty( "Width", "", 100, m_propCondition );
    m_propDendrogramSizeX->setMin( 0 );
    m_propDendrogramSizeX->setMax( 10000 );
    m_propDendrogramSizeY = m_groupDendrogram->addProperty( "Height", "", 100, m_propCondition );
    m_propDendrogramSizeY->setMin( 0 );
    m_propDendrogramSizeY->setMax( 10000 );
    m_propDendrogramOffsetX = m_groupDendrogram->addProperty( "Horizontal position", "", 100, m_propCondition );
    m_propDendrogramOffsetX->setMin( -9000 );
    m_propDendrogramOffsetX->setMax( 1000 );
    m_propDendrogramOffsetY = m_groupDendrogram->addProperty( "Verctical position", "", 100, m_propCondition );
    m_propDendrogramOffsetY->setMin( -9000 );
    m_propDendrogramOffsetY->setMax( 1000 );

    m_groupSelection->setHidden( true );
    m_groupTriangulation->setHidden( true );
    m_groupDendrogram->setHidden( true );

    m_buttonUpdateOutput = m_properties->addProperty( "Update output", "Updates the output connector",
            WPVBaseTypes::PV_TRIGGER_READY, m_propCondition  );


    // Info properties
    m_infoCountLeafes = m_infoProperties->addProperty( "Count voxels", "", 0 );
    m_infoCountClusters  = m_infoProperties->addProperty( "Count clusters", "", 0 );
    m_infoMaxLevel = m_infoProperties->addProperty( "Max Level", "", 0 );

    WModule::properties();
}

void WMClusterDisplayVoxels::setPropertyBoundaries()
{
    m_propSelectedCluster->setMin( m_tree.getLeafCount() );
    m_propSelectedCluster->setMax( m_tree.getClusterCount() - 1 );

    m_propSelectedCluster->setMin( 0 );
    m_propSelectedCluster->setMax( m_tree.getClusterCount() - 1 );
    m_propSelectedCluster->set( m_tree.getClusterCount() - 1 );

    m_infoCountLeafes->set( m_tree.getLeafCount() );
    m_infoCountClusters->set( m_tree.getClusterCount() );
    m_infoMaxLevel->set( m_tree.getMaxLevel() );

    m_propMinSizeToColor->setMax( 1000 );
    m_rootCluster = m_propSelectedCluster->get();

    m_propLevelsFromTop->setMin( 0 );
    m_propLevelsFromTop->setMax( m_tree.getMaxLevel() );

    if( m_loadedPartitions.empty() )
    {
        m_propSelectedLoadedPartion->setHidden( true );
    }
    else
    {
        m_propSelectedLoadedPartion->setMin( 1 );
        m_propSelectedLoadedPartion->setMax( m_loadedPartitions.size() );
    }

    m_zoom = false;
    m_zoomRoot = m_propSelectedCluster->get();
}

void WMClusterDisplayVoxels::moduleMain()
{
    boost::signals2::connection con = WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getPickHandler()->getPickSignal()->
            connect( boost::bind( &WMClusterDisplayVoxels::dendrogramClick, this, _1 ) );


    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );
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
        bool dataChanged = ( m_dataSet != newDataSet );
        bool dataValid   = ( newDataSet );

        if( dataValid )
        {
            if( dataChanged )
            {
                m_dataSet = newDataSet;
                m_grid = boost::shared_dynamic_cast< WGridRegular3D >( m_dataSet->getGrid() );
                break;
            }
        }
    }

    // wait for a cluster file to be loaded
    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        if( m_shutdownFlag() )
        {
            break;
        }

        if( m_propReadTrigger->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            boost::filesystem::path fileName = m_propClusterFile->get();
            m_propReadTrigger->set( WPVBaseTypes::PV_TRIGGER_READY, true );

            if( loadClustering( fileName ) )
            {
                m_propReadTrigger->setHidden( true );
                m_propClusterFile->setHidden( true );
                m_groupSelection->setHidden( false );
                m_groupTriangulation->setHidden( false );
                m_groupDendrogram->setHidden( false );
                break;
            }
        }
    }

    // initialize
    if( !m_shutdownFlag() )
    {
        createTexture();
        setPropertyBoundaries();
        initWidgets();

        m_propSelectedCluster->get( true );
        m_propSelectedLoadedPartion->get( true );
        m_propMinSizeToColor->get( true );
        m_propXClusters->get( true );
        m_propXBiggestClusters->get( true );
        m_propValue->get( true );
        m_propMinBranchLength->get( true );
        m_propMinBranchSize->get( true );
        m_propLevelsFromTop->get( true );
        m_propHideOutliers->get( true );
        m_showNotInClusters->get( true );

        updateAll();
    }

    // main loop, respond to controls
    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        if( m_shutdownFlag() )
        {
            break;
        }

        if( m_clusterSelection->changed( true ) )
        {
            m_propSelectedCluster->setHidden( true );
            m_propXBiggestClusters->setHidden( true );
            m_propXClusters->setHidden( true );
            m_propValue->setHidden( true );
            m_propLevelsFromTop->setHidden( true );
            m_propHideOutliers->setHidden( true );
            m_propMinBranchLength->setHidden( true );
            m_propMinBranchSize->setHidden( true );
            m_propSelectedLoadedPartion->setHidden( true );

            switch( m_clusterSelection->get( true ).getItemIndexOfSelected( 0 ) )
            {
                case 0:
                    m_propSelectedCluster->setHidden( false );
                    break;
                case 1:
                    m_propSelectedCluster->setHidden( false );
                    m_propXBiggestClusters->setHidden( false );
                    break;
                case 2:
                    m_propSelectedCluster->setHidden( false );
                    m_propXClusters->setHidden( false );
                    break;
                case 3:
                    m_propValue->setHidden( false );
                    break;
                case 4:
                    m_propLevelsFromTop->setHidden( false );
                    m_propHideOutliers->setHidden( false );
                    break;
                case 5:
                    m_propMinBranchLength->setHidden( false );
                    m_propMinBranchSize->setHidden( false );
                    break;
                case 6:
                    m_propSelectedLoadedPartion->setHidden( false );
                    break;
                default:
                    m_propSelectedCluster->setHidden( false );
                    break;
            }
        }

        if( m_buttonExecuteSelection->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            m_buttonExecuteSelection->set( WPVBaseTypes::PV_TRIGGER_READY, false );
            switch( m_clusterSelection->get( true ).getItemIndexOfSelected( 0 ) )
            {
                case 0:
                    m_currentDisplayMode = CDV_SINGLE;
                    break;
                case 1:
                    m_currentDisplayMode = CDV_BIGGEST;
                    break;
                case 2:
                    m_currentDisplayMode = CDV_X;
                    break;
                case 3:
                    m_currentDisplayMode = CDV_SIMILARITY;
                    break;
                case 4:
                    m_currentDisplayMode = CDV_LEVELSFROMTOP;
                    break;
                case 5:
                    m_currentDisplayMode = CDV_MINBRANCHLENGTH;
                    break;
                case 6:
                    m_currentDisplayMode = CDV_LOADED;
                    break;
                default:
                    m_currentDisplayMode = CDV_SINGLE;
                    break;
            }
            updateAll();
        }

        if( m_propShowVoxelTriangulation->changed( true ) )
        {
            createMesh();
            renderMesh();
        }

        if( m_showNotInClusters->changed( true ) )
        {
            updateAll();
        }

        if( m_propMinSizeToColor->changed( true ) )
        {
            handleMinSizeChanged();
        }

        if( m_buttonLabelSelection->changed() )
        {
            setButtonLabels();
        }

        if( m_propShowDendrogram->changed( true ) || m_propResizeWithWindow->changed( true ) || m_propDendrogramSizeX->changed( true ) ||
                m_propDendrogramSizeY->changed( true ) || m_propDendrogramOffsetX->changed( true ) || m_propDendrogramOffsetY->changed( true ) ||
                m_propPlotHeightByLevel->changed( true ) || m_propShowSelectedButtons->changed() )
        {
            m_dendrogramDirty = true;
        }

        if( m_buttonUpdateOutput->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            updateOutDataset();
            m_buttonUpdateOutput->set( WPVBaseTypes::PV_TRIGGER_READY, false );
        }

        if( m_propZoomIn->changed( true ) )
        {
            m_zoom = true;
            m_zoomRoot = m_propSelectedCluster->get( true );
            m_dendrogramDirty = true;
            m_propZoomIn->set( WPVBaseTypes::PV_TRIGGER_READY, true );
            m_propZoomIn->get( true );
        }

        if( m_propZoomOut->changed( true ) )
        {
            m_zoom = false;
            m_dendrogramDirty = true;
            m_propZoomOut->set( WPVBaseTypes::PV_TRIGGER_READY, true );
        }
    }

    con.disconnect();

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_moduleNode );
}

void WMClusterDisplayVoxels::updateAll()
{
    switch( m_currentDisplayMode )
    {
        case CDV_SINGLE:
            m_activatedClusters.clear();
            m_activatedClusters.push_back( m_propSelectedCluster->get( true ) );
            m_currentDisplayModeString = std::string( "Single selection" );
            break;
        case CDV_BIGGEST:
            m_activatedClusters = m_tree.findXBiggestClusters( m_propSelectedCluster->get(), m_propXBiggestClusters->get( true ) );
            m_currentDisplayModeString = std::string( "Biggest clusters" );
            break;
        case CDV_X:
            m_activatedClusters = m_tree.findXClusters( m_propSelectedCluster->get(), m_propXClusters->get( true ) );
            m_currentDisplayModeString = std::string( "X clusters" );
            break;
        case CDV_SIMILARITY:
            m_activatedClusters = m_tree.findClustersForValue( m_propValue->get( true ) );
            m_currentDisplayModeString = std::string( "Similarity value selection" );
            break;
        case CDV_LEVELSFROMTOP:
            m_activatedClusters = m_tree.downXLevelsFromTop( m_propLevelsFromTop->get( true ), m_propHideOutliers->get( true ) );
            m_currentDisplayModeString = std::string( "Levels from top" );
            break;
        case CDV_MINBRANCHLENGTH:
            m_activatedClusters = m_tree.findClustersForBranchLength( m_propMinBranchLength->get( true ), m_propMinBranchSize->get( true ) );
            m_currentDisplayModeString = std::string( "Minimum branch length" );
            break;
        case CDV_LOADED:
            m_activatedClusters = m_loadedPartitions[ m_propSelectedLoadedPartion->get( true ) - 1 ];
            m_currentDisplayModeString = std::string( "Loaded Partition" );
            break;
        default:
            break;
    };


    // set colors for cluster in the tree
    m_tree.colorCluster( m_tree.getClusterCount() - 1, WColor( 0.3, 0.3, 0.3, 1.0 ) );
    for( size_t k = 0; k < m_activatedClusters.size(); ++k )
    {
        size_t current = m_activatedClusters[k];
        //m_tree.colorCluster( current, wge::getNthHSVColor( k ) );
        m_tree.colorCluster( current, m_clusterColors[current] );
    }

    // redraw the texture
    m_data.clear();
    m_data.resize( m_grid->size(), 0 );
    std::vector<size_t> vox = m_tree.getVoxelsForCluster( m_tree.getClusterCount() - 1 );
    if( m_showNotInClusters->get() )
    {
        for( size_t k = 0; k < vox.size(); ++k )
        {
            m_data[vox[k]] = 999999;
        }
    }
    for( size_t i = 0; i < m_activatedClusters.size(); ++i )
    {
        std::vector<size_t> voxels = m_tree.getVoxelsForCluster( m_activatedClusters[i] );

        for( size_t k = 0; k < voxels.size(); ++k )
        {
            m_data[voxels[k]] = i + 1;
        }
    }

    unsigned char* data = m_texture->getImage()->data();
    for( size_t i = 0; i < m_grid->size(); ++i )
    {
        if( m_data[i] == 999999 )
        {
            data[i * 3    ] = 75;
            data[i * 3 + 1] = 75;
            data[i * 3 + 2] = 75;
        }
        else if( m_data[i] != 0 )
        {
            WColor color = m_clusterColors[m_activatedClusters[m_data[i]-1]];
            data[i * 3    ] = color[0] * 255;
            data[i * 3 + 1] = color[1] * 255;
            data[i * 3 + 2] = color[2] * 255;
        }
        else
        {
            data[i * 3    ] = 0.0;
            data[i * 3 + 1] = 0.0;
            data[i * 3 + 2] = 0.0;
        }
    }
    m_texture->dirtyTextureObject();

    createMesh();
    renderMesh();

    m_selectionChanged = true;
    m_dendrogramDirty = true;
}

void WMClusterDisplayVoxels::handleMinSizeChanged()
{
    m_dendrogramDirty = true;
}

bool WMClusterDisplayVoxels::loadClustering( boost::filesystem::path clusterFile )
{
    debugLog() << "start parsing tree file...";

    WFileParser parser( clusterFile.string() );
    if( !parser.readFile() )
    {
        debugLog() << "parser error";
        return false;
    }

    std::vector<std::string>lines = parser.getRawLines();

    if( lines.size() == 0 )
    {
        debugLog() << "file is empty";
        return false;
    }

    std::vector< std::vector< std::string> >coords = parser.getLinesForTagSeparated( "coordinates" );

    for( size_t i = 0; i < coords.size(); ++i )
    {
        std::vector< std::string > svec = coords[i];

        m_tree.addLeaf( m_grid->getVoxelNum( boost::lexical_cast< size_t >( svec[0] ),
                                             boost::lexical_cast< size_t >( svec[1] ),
                                             boost::lexical_cast< size_t >( svec[2] ) ) );
    }

    std::vector< std::vector< std::string> >clusters = parser.getLinesForTagSeparated( "clusters" );

    for( size_t i = 0; i < clusters.size(); ++i )
    {
        std::vector< std::string > svec = clusters[i];

        m_tree.addCluster( boost::lexical_cast< size_t >( svec[0] ),
                           boost::lexical_cast< size_t >( svec[1] ),
                           boost::lexical_cast< float >( svec[2] ) );
    }

    std::vector< std::vector< std::string> >partitions = parser.getLinesForTagSeparated( "partitions" );

    for( size_t i = 0; i < partitions.size(); ++i )
    {
        std::vector< std::string > svec = partitions[i];

        std::vector<size_t>partition;

        for( size_t k = 0; k < svec.size(); ++k )
        {
            partition.push_back( boost::lexical_cast< size_t >( svec[k] ) );
        }
        m_loadedPartitions.push_back( partition );
    }

    m_colorIndex = 0;
    m_clusterColors.resize( m_tree.getClusterCount() );
    initColors( m_tree.getClusterCount() - 1, 0 );
    return true;
}

void WMClusterDisplayVoxels::initColors( size_t root, size_t index )
{
    if( m_tree.size( root ) == 1 )
    {
        m_clusterColors[root] = wge::getNthHSVColor( index );
        return;
    }
    m_clusterColors[root] = wge::getNthHSVColor( index );
    size_t left = m_tree.getChildren( root ).first;
    size_t right = m_tree.getChildren( root ).second;
    if( m_tree.size( left ) >= m_tree.size( right ) )
    {
        initColors( left, index );
        initColors( right, ++m_colorIndex );
    }
    else
    {
        initColors( left, ++m_colorIndex );
        initColors( right, index );
    }
}

void WMClusterDisplayVoxels::createTexture()
{
     osg::ref_ptr< osg::Image > ima = new osg::Image;
     ima->allocateImage( m_grid->getNbCoordsX(), m_grid->getNbCoordsY(), m_grid->getNbCoordsZ(), GL_RGB, GL_UNSIGNED_BYTE );
     unsigned char* data = ima->data();
     m_data.resize( m_grid->getNbCoordsX() * m_grid->getNbCoordsY() * m_grid->getNbCoordsZ(), 0 );

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


//     osg::ref_ptr< osg::Image > ima = new osg::Image;
//     ima->allocateImage( m_grid->getNbCoordsX(), m_grid->getNbCoordsY(), m_grid->getNbCoordsZ(), GL_RGB, GL_UNSIGNED_BYTE );
//
//     unsigned char* data = ima->data();
//     m_data.resize( m_grid->getNbCoordsX() * m_grid->getNbCoordsY() * m_grid->getNbCoordsZ(), 0 );
//
//     for( unsigned int i = 0; i < m_grid->size() * 3; ++i )
//     {
//         data[i] = 0.0;
//     }
//
//     m_texture = osg::ref_ptr< WGETexture3D >( new WGETexture3D( ima ) );
//     m_texture->setFilterMinMag( osg::Texture3D::LINEAR );
//     m_texture->setWrapSTR( osg::Texture::CLAMP_TO_BORDER );
//     m_texture->colormap()->set( m_texture->colormap()->get().newSelector( WItemSelector::IndexList( 1, 4 ) ) );
//     m_properties->addProperty( m_texture->alpha() );
//
//     WGEColormapping::registerTexture( m_texture, "Cluster Texture" );
}

void WMClusterDisplayVoxels::createMesh()
{
    m_triMeshes.clear();

    for( size_t k = 1; k <= m_activatedClusters.size(); ++k )
    {
        WMarchingLegoAlgorithm mlAlgo;
        m_triMeshes.push_back( mlAlgo.genSurfaceOneValue( m_grid->getNbCoordsX(), m_grid->getNbCoordsY(), m_grid->getNbCoordsZ(),
                                            m_grid->getTransformationMatrix(),
                                            &m_data,
                                            k ) );
    }

    if( m_showNotInClusters->get() )
    {
        WMarchingLegoAlgorithm mlAlgo;
        m_nonActiveMesh = mlAlgo.genSurfaceOneValue( m_grid->getNbCoordsX(), m_grid->getNbCoordsY(), m_grid->getNbCoordsZ(),
                                            m_grid->getTransformationMatrix(),
                                            &m_data,
                                            999999 );
    }
}

void WMClusterDisplayVoxels::renderMesh()
{
    if( !m_outputGeodes.empty() )
    {
        for( size_t i = 0; i < m_outputGeodes.size(); ++i )
        {
            m_meshNode->remove( m_outputGeodes[i] );
        }
        m_outputGeodes.clear();
    }

    if( m_propShowVoxelTriangulation->get( true ) )
    {
        for( size_t i = 0; i < m_triMeshes.size(); ++i )
        {
            osg::Geometry* surfaceGeometry = new osg::Geometry();
            osg::ref_ptr< osg::Geode >outputGeode = osg::ref_ptr< osg::Geode >( new osg::Geode );

            outputGeode->setName( ( std::string( "cluster" ) + boost::lexical_cast<std::string>( m_activatedClusters[i] ) ).c_str() );

            surfaceGeometry->setVertexArray( m_triMeshes[i]->getVertexArray() );

            // ------------------------------------------------
            // normals
            surfaceGeometry->setNormalArray( m_triMeshes[i]->getTriangleNormalArray() );
            surfaceGeometry->setNormalBinding( osg::Geometry::BIND_PER_PRIMITIVE );

            // ------------------------------------------------
            // colors
            osg::Vec4Array* colors = new osg::Vec4Array;
            colors->push_back( m_clusterColors[m_activatedClusters[i]] );
            surfaceGeometry->setColorArray( colors );
            surfaceGeometry->setColorBinding( osg::Geometry::BIND_OVERALL );

            osg::DrawElementsUInt* surfaceElement = new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES, 0 );

            std::vector< size_t >tris = m_triMeshes[i]->getTriangles();
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
            m_outputGeodes.push_back( outputGeode );
        }
    }
    if( m_propShowVoxelTriangulation->get( true ) && m_showNotInClusters->get() )
    {
        osg::Geometry* surfaceGeometry = new osg::Geometry();
        osg::ref_ptr< osg::Geode >outputGeode = osg::ref_ptr< osg::Geode >( new osg::Geode );

        outputGeode->setName( ( std::string( "non active" ) ).c_str() );

        surfaceGeometry->setVertexArray( m_nonActiveMesh->getVertexArray() );

        // ------------------------------------------------
        // normals
        surfaceGeometry->setNormalArray( m_nonActiveMesh->getTriangleNormalArray() );
        surfaceGeometry->setNormalBinding( osg::Geometry::BIND_PER_PRIMITIVE );

        // ------------------------------------------------
        // colors
        osg::Vec4Array* colors = new osg::Vec4Array;

        colors->push_back( osg::Vec4( 0.3, 0.3, 0.3, 1.0f ) );
        surfaceGeometry->setColorArray( colors );
        surfaceGeometry->setColorBinding( osg::Geometry::BIND_OVERALL );

        osg::DrawElementsUInt* surfaceElement = new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES, 0 );

        std::vector< size_t >tris = m_nonActiveMesh->getTriangles();
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
        m_outputGeodes.push_back( outputGeode );
    }
}

void WMClusterDisplayVoxels::initWidgets()
{
    osg::ref_ptr<osgViewer::View> viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getView();

    int height = viewer->getCamera()->getViewport()->height();
    int width = viewer->getCamera()->getViewport()->width();

    m_oldViewHeight = height;
    m_oldViewWidth = width;

    m_moduleNode = osg::ref_ptr< WGEManagedGroupNode >( new WGEManagedGroupNode( m_active ) );
    m_dendrogramNode = osg::ref_ptr< WGEManagedGroupNode >( new WGEManagedGroupNode( m_active ) );
    m_meshNode = osg::ref_ptr< WGEManagedGroupNode >( new WGEManagedGroupNode( m_active ) );
    m_moduleNode->insert( m_dendrogramNode );
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

    m_moduleNode->addUpdateCallback( new WGEFunctorCallback< osg::Node >( boost::bind( &WMClusterDisplayVoxels::updateWidgets, this ) ) );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_moduleNode );
}

void WMClusterDisplayVoxels::updateWidgets()
{
    osg::ref_ptr<osgViewer::View> viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getView();

    int height = viewer->getCamera()->getViewport()->height();
    int width = viewer->getCamera()->getViewport()->width();

    int rows = ( height - 20 ) / 20;
    int buttonWidth = 70;

    if( ( height != m_oldViewHeight ) || width != m_oldViewWidth )
    {
        m_oldViewHeight = height;
        m_oldViewWidth = width;

        m_dendrogramDirty = true;
    }

    bool buttonClicked = widgetClicked();

    if( !buttonClicked && !m_dendrogramDirty )
    {
        return;
    }

    if( m_selectionChanged || m_propShowSelectedButtons->changed() || m_dendrogramDirty )
    {
        for( size_t i = 0; i < m_activeClustersButtonList.size(); ++i )
        {
            m_wm->removeChild( m_activeClustersButtonList[i] );
        }

        m_activeClustersButtonList.clear();

        if( m_propShowSelectedButtons->get( true ) )
        {
            for( size_t i = 0; i < m_activatedClusters.size(); ++i )
            {
                osg::ref_ptr<WOSGButton> newButton = osg::ref_ptr<WOSGButton>( new WOSGButton( std::string( "" ),
                        osgWidget::Box::VERTICAL, true, true ) );
                newButton->setPosition( osg::Vec3( 5.f + ( i / rows ) * buttonWidth,  ( i % rows ) * 20.f, 0 ) );
                newButton->setId( m_activatedClusters[i] );
                newButton->managed( m_wm );
                m_wm->addChild( newButton );
                m_activeClustersButtonList.push_back( newButton );
                newButton->setBackgroundColor( m_clusterColors[m_activatedClusters[i]] );
            }
        }

        setButtonLabels();

        osg::ref_ptr<WOSGButton> newButton = osg::ref_ptr<WOSGButton>( new WOSGButton( std::string( "" ),
                                osgWidget::Box::VERTICAL, true, true ) );
        newButton->setPosition( osg::Vec3( 5.f, height - 20.f, 0 ) );
        newButton->setId( 0 );
        newButton->setLabel( m_currentDisplayModeString );
        newButton->managed( m_wm );
        m_wm->addChild( newButton );
        m_activeClustersButtonList.push_back( newButton );
        newButton->setBackgroundColor( wge::getNthHSVColor( m_currentDisplayMode ) );

        m_selectionChanged = false;
    }
    m_wm->resizeAllWindows();

    if( m_propShowSelectedButtons && buttonClicked )
    {
        m_tree.colorCluster( m_tree.getClusterCount() - 1, WColor( 0.3, 0.3, 0.3, 1.0 ) );
        for( size_t k = 0; k < m_activatedClusters.size(); ++k )
        {
            if( m_activeClustersButtonList[k]->pushed() )
            {
                size_t current = m_activatedClusters[k];
                //m_tree.colorCluster( current, wge::getNthHSVColor( k ) );
                m_tree.colorCluster( current, m_clusterColors[current] );
            }
        }
        m_dendrogramDirty = true;
    }



    if( m_dendrogramDirty )
    {
        m_camera->removeChild( m_dendrogramGeode );
        //m_camera->removeChild( 1, 1 );

        int dwidth = m_propDendrogramSizeX->get( true );
        int dheight = m_propDendrogramSizeY->get( true );
        int dxOff = m_propDendrogramOffsetX->get( true );
        int dyOff = m_propDendrogramOffsetY->get( true );

        if( m_propShowDendrogram->get( true ) )
        {
            size_t rootCluster = m_tree.getClusterCount() - 1;

            if( m_zoom )
            {
                rootCluster = m_zoomRoot;
            }


            if( m_propResizeWithWindow->get( true ) )
            {
                if( m_propShowSelectedButtons->get() )
                {
                    m_dendrogramGeode = new WDendrogramGeode( &m_tree, rootCluster, m_propPlotHeightByLevel->get( true ),
                                        m_propMinSizeToColor->get(), width - ( ( m_activatedClusters.size() / rows ) + 1 ) * buttonWidth, height / 2 ,
                                        ( ( m_activatedClusters.size() / rows ) + 1 ) * buttonWidth );
                }
                else
                {
                    m_dendrogramGeode = new WDendrogramGeode( &m_tree, rootCluster, m_propPlotHeightByLevel->get( true ),
                                            m_propMinSizeToColor->get(), width - 20, height / 2 , 10 );
                }
            }
            else
            {
                m_dendrogramGeode = new WDendrogramGeode( &m_tree, rootCluster, m_propPlotHeightByLevel->get( true ),
                        m_propMinSizeToColor->get(), dwidth, dheight, dxOff, dyOff );
            }
            m_camera->addChild( m_dendrogramGeode );
        }
        m_dendrogramDirty = false;
    }
}

bool WMClusterDisplayVoxels::widgetClicked()
{
    bool clicked = false;
    bool selectionChanged = false;

    for( size_t i = 0; i < m_activeClustersButtonList.size(); ++i )
    {
        if( m_activeClustersButtonList[i]->clicked() )
        {
            if( m_activeClustersButtonList[i]->getId() < 10000000 )
            {
                selectionChanged = true;
                clicked = true;
            }
        }
    }

    if( selectionChanged )
    {
        std::vector<size_t>activeClusters;
        for( size_t i = 0; i < m_activeClustersButtonList.size(); ++i )
        {
            if( m_activeClustersButtonList[i]->pushed() )
            {
                if( m_outputGeodes.size() > i )
                {
                    m_outputGeodes[i]->setNodeMask( 0xFFFFFFFF );
                }
            }
            else
            {
                if( m_outputGeodes.size() > i )
                {
                    m_outputGeodes[i]->setNodeMask( 0x00000000 );
                }
            }
        }
    }
    return clicked;
}

void WMClusterDisplayVoxels::dendrogramClick( WPickInfo pickInfo )
{
    if( !m_propShowDendrogram->get() || !( pickInfo.getName() == "nothing" ) )
    {
        return;
    }
    int x = pickInfo.getPickPixel().x();
    int y = pickInfo.getPickPixel().y();

    size_t cluster = m_dendrogramGeode->getClickedCluster( x, y );
    //std::cout << cluster << std::endl;
    m_propSelectedCluster->set( cluster );
    m_buttonExecuteSelection->set( WPVBaseTypes::PV_TRIGGER_TRIGGERED );
}

void WMClusterDisplayVoxels::updateOutDataset()
{
    WAssert( m_dataSet, "" );
    WAssert( m_dataSet->getValueSet(), "" );
    WAssert( m_dataSet->getGrid(), "" );

    boost::shared_ptr< std::vector< float > >ptr( new std::vector< float >( m_data.size() ) );

    for( size_t i = 0; i < m_data.size(); ++i )
    {
        ptr->at( i ) = static_cast<float>( m_data[i] );
    }

    boost::shared_ptr< WValueSet< float > > vs =
        boost::shared_ptr< WValueSet< float > >( new WValueSet< float >( 0, 1, ptr, W_DT_FLOAT ) );

    m_outData = boost::shared_ptr< WDataSetScalar >( new WDataSetScalar( vs, m_grid ) );
    m_output->updateData( m_outData );
}

void WMClusterDisplayVoxels::setButtonLabels()
{
    if( m_propShowSelectedButtons->get() )
    {
        std::string ns;
        for( size_t i = 0; i < m_activatedClusters.size(); ++i )
        {
            if( m_buttonLabelSelection->get( true ).getItemIndexOfSelected( 0 ) == 0 )
            {
                ns = boost::lexical_cast<std::string>( m_activatedClusters[i] );
            }
            else if( m_buttonLabelSelection->get( true ).getItemIndexOfSelected( 0 ) == 1 )
            {
                ns = boost::lexical_cast<std::string>( m_tree.size( m_activatedClusters[i] ) );
            }
            else if( m_buttonLabelSelection->get( true ).getItemIndexOfSelected( 0 ) == 2 )
            {
                ns = boost::lexical_cast<std::string>( m_tree.getLevel( m_activatedClusters[i] ) );
            }
            else
            {
                ns = boost::lexical_cast<std::string>( m_tree.getCustomData( m_activatedClusters[i] ) );
                if( ns.size() > 8 )
                {
                    ns = ns.substr( 0, 8 );
                }
            }


            for( size_t k = 8 - ns.size(); k > 0; --k )
            {
                if( k %2 == 1 )
                {
                    ns = std::string( " " ) + ns;
                }
                else
                {
                    ns = ns + std::string( " " );
                }
            }
            if( m_activeClustersButtonList.size() > i )
            {
                m_activeClustersButtonList[i]->setLabel( ns );
            }
        }
    }
}
