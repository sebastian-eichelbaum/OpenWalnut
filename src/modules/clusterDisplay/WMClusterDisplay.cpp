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

#include <queue>
#include <string>
#include <vector>

#include <boost/regex.hpp>

#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIEventHandler>
#include <osgGA/StateSetManipulator>
#include <osgGA/TrackballManipulator>
#include <osgViewer/ViewerEventHandlers>
#include <osgWidget/Util> //NOLINT
#include <osgWidget/ViewerEventHandlers> //NOLINT
#include <osgWidget/WindowManager> //NOLINT

#include "core/common/WStringUtils.h"
#include "core/common/WPathHelper.h"
#include "core/common/WPropertyHelper.h"
#include "core/graphicsEngine/WGEUtils.h"
#include "core/kernel/WKernel.h"
#include "core/kernel/WROIManager.h"
#include "core/ui/WUIViewWidget.h"

#include "WMClusterDisplay.h"
#include "WMClusterDisplay.xpm"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMClusterDisplay )


bool WMClusterDisplay::MainViewEventHandler::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& /* aa */ )
{
//    wlog::debug( "WMClusterDisplay::MainViewEventHandler" ) << "handle";
    if( ea.getEventType() == GUIEvents::PUSH && ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON )
    {
        return ( true == m_signalLeftButtonPush( WVector2f( ea.getX(), ea.getY() ) ) );
    }
    return false;
}

void WMClusterDisplay::MainViewEventHandler::subscribeLeftButtonPush( LeftButtonPushSignalType::slot_type slot )
{
    m_signalLeftButtonPush.connect( slot );
}

WMClusterDisplay::WMClusterDisplay():
    WModule(),
    m_widgetDirty( false ),
    m_biggestClusterButtonsChanged( false ),
    m_dendrogramDirty( false ),
    m_labelMode( 0 )
{
}

WMClusterDisplay::~WMClusterDisplay()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMClusterDisplay::factory() const
{
    // See "src/modules/template/" for an extensively documented example.
    return boost::shared_ptr< WModule >( new WMClusterDisplay() );
}

const char** WMClusterDisplay::getXPMIcon() const
{
    return clusterDisplay_xpm; // Please put a real icon here.
}
const std::string WMClusterDisplay::getName() const
{
    return "Cluster Display";
}

const std::string WMClusterDisplay::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    // See "src/modules/template/" for an extensively documented example.
    return "This module loads a text file containing the hierarchical tree representation of a fiber clustering"
            " and provides selection appropriate tools";
}

void WMClusterDisplay::connectors()
{
    using boost::shared_ptr;
    typedef WModuleInputData< const WDataSetFibers > FiberInputData;  // just an alias

    m_fiberInput = shared_ptr< FiberInputData >( new FiberInputData( shared_from_this(), "fiberInput", "A loaded fiber dataset." ) );

    addConnector( m_fiberInput );
    // call WModules initialization
    WModule::connectors();
}

std::vector< std::string > WMClusterDisplay::readFile( const std::string fileName )
{
    std::ifstream ifs( fileName.c_str(), std::ifstream::in );

    std::vector< std::string > lines;

    std::string line;

    if( ifs.is_open() )
    {
        debugLog() << "trying to load " << fileName;
        debugLog() << "file exists";
    }
    else
    {
        debugLog() << "trying to load " << fileName;
        debugLog() << "file doesn\'t exist";
        ifs.close();
        return lines;
    }

    while( !ifs.eof() )
    {
        getline( ifs, line );

        lines.push_back( std::string( line ) );
    }

    ifs.close();

    return lines;
}

bool WMClusterDisplay::loadTreeAscii( std::string fileName )
{
    std::vector<std::string> lines;

    debugLog() << "start parsing tree file...";

    lines = readFile( fileName );

    if( lines.size() == 0 )
    {
        return false;
    }

    for( size_t i = 0; i < lines.size() - 1; ++i )
    {
        std::string &ls = lines[i];

        boost::regex reg1( "\\(" );
        ls = boost::regex_replace( ls, reg1, "(," );

        boost::regex reg2( "\\)" );
        ls = boost::regex_replace( ls, reg2, ",)" );

        boost::regex reg3( "\\ " );
        ls = boost::regex_replace( ls, reg3, "" );
    }

    std::vector<std::string>svec;

    for( size_t i = 0; i < lines.size()-1; ++i )
    {
        svec.clear();
        boost::regex reg( "," );
        boost::sregex_token_iterator it( lines[i].begin(), lines[i].end(), reg, -1 );
        boost::sregex_token_iterator end;
        while( it != end )
        {
            svec.push_back( *it++ );
        }

        size_t level = string_utils::fromString< size_t >( svec[1] );
        if( level == 0 )
        {
            m_tree.addLeaf();
        }
        else
        {
            // start after ( level (
            size_t k = 3;
            std::vector<size_t>leafes;
            while( svec[k] != ")" )
            {
                leafes.push_back( string_utils::fromString< size_t >( svec[k] ) );
                ++k;
            }
            //skip ) (
            k += 2;

            size_t cluster1 = string_utils::fromString< size_t >( svec[k++] );
            size_t cluster2 = string_utils::fromString< size_t >( svec[k++] );
            ++k;
            float data = string_utils::fromString< float >( svec[k++] );

            m_tree.addCluster( cluster1, cluster2, level, leafes, data );
            //m_tree.addCluster( cluster1, cluster2, data );

            if( svec[k] != ")" )
            {
                std::cout << "parse error" << std::endl;
                return false;
            }
        }
    }
    debugLog() << m_tree.getClusterCount() << " clusters created.";

    debugLog() << "finished parsing tree file...";

    if( m_tree.getLeafCount() == m_fiberInput->getData()->size() )
    {
        return true;
    }
    debugLog() << "something is wrong with the tree file";
    return false;
}

void WMClusterDisplay::initWidgets()
{
    osg::ref_ptr<osgViewer::View> viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getView();

    int height = viewer->getCamera()->getViewport()->height();
    int width = viewer->getCamera()->getViewport()->width();

    m_oldViewHeight = height;
    m_oldViewWidth = width;

    m_rootNode = osg::ref_ptr< WGEManagedGroupNode >( new WGEManagedGroupNode( m_active ) );

    m_wm = new osgWidget::WindowManager( viewer, 0.0f, 0.0f, MASK_2D );

    float xorig = 100.f;
    float yorig = 300.f;

    WOSGButton* current = new WOSGButton( std::string( "current" ), osgWidget::Box::VERTICAL, true, false );
    WOSGButton* up1 =     new WOSGButton( std::string( "+1" ),      osgWidget::Box::VERTICAL, true, false );
    WOSGButton* down11 =  new WOSGButton( std::string( "-1.1" ),    osgWidget::Box::VERTICAL, true, false );
    WOSGButton* down12 =  new WOSGButton( std::string( "-1.2" ),    osgWidget::Box::VERTICAL, true, false );

    WOSGButton* showN =  new WOSGButton( std::string( " N " ),    osgWidget::Box::VERTICAL, true, false );
    WOSGButton* showS =  new WOSGButton( std::string( " S " ),    osgWidget::Box::VERTICAL, true, false );
    WOSGButton* showE =  new WOSGButton( std::string( " E " ),    osgWidget::Box::VERTICAL, true, false );

    up1->setPosition(     osg::Vec3( xorig + 85.f,  yorig + 300.f, 0 ) );
    current->setPosition( osg::Vec3( xorig + 85.f,  yorig + 200.f, 0 ) );
    down11->setPosition(  osg::Vec3( xorig + 30.f,  yorig + 100.f, 0 ) );
    down12->setPosition(  osg::Vec3( xorig + 140.f, yorig + 100.f, 0 ) );

    showN->setPosition( osg::Vec3( xorig + 85.f,  yorig + 220.f, 0 ) );
    showS->setPosition( osg::Vec3( xorig + 110.f,  yorig + 220.f, 0 ) );
    showE->setPosition( osg::Vec3( xorig + 135.f,  yorig + 220.f, 0 ) );

    up1->managed( m_wm );
    current->managed( m_wm );
    down11->managed( m_wm );
    down12->managed( m_wm );

    showN->managed( m_wm );
    showS->managed( m_wm );
    showE->managed( m_wm );

    m_wm->addChild( current );
    m_wm->addChild( up1 );
    m_wm->addChild( down11 );
    m_wm->addChild( down12 );

    m_wm->addChild( showN );
    m_wm->addChild( showS );
    m_wm->addChild( showE );

    m_treeButtonList.push_back( current );
    m_treeButtonList.push_back( up1 );
    m_treeButtonList.push_back( down11 );
    m_treeButtonList.push_back( down12 );

    m_treeButtonList.push_back( showN );
    m_treeButtonList.push_back( showS );
    m_treeButtonList.push_back( showE );

    m_camera = new WGECamera();
    m_camera->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );

    m_camera->setProjectionMatrix( osg::Matrix::ortho2D( 0.0, width, 0.0f, height ) );
    m_camera->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
    m_camera->setViewMatrix( osg::Matrix::identity() );
    m_camera->setClearMask( GL_DEPTH_BUFFER_BIT );
    m_camera->setRenderOrder( WGECamera::POST_RENDER );

    m_rootNode->addChild( m_camera );
    m_camera->addChild( m_wm );

    viewer->addEventHandler( new osgWidget::MouseHandler( m_wm ) );
    viewer->addEventHandler( new osgWidget::KeyboardHandler( m_wm ) );
    viewer->addEventHandler( new osgWidget::ResizeHandler( m_wm, m_camera ) );
    viewer->addEventHandler( new osgWidget::CameraSwitchHandler( m_wm, m_camera ) );
    viewer->addEventHandler( new osgViewer::StatsHandler() );
    viewer->addEventHandler( new osgViewer::WindowSizeHandler() );
    viewer->addEventHandler( new osgGA::StateSetManipulator( viewer->getCamera()->getOrCreateStateSet() ) );

    m_wm->resizeAllWindows();

    m_rootNode->addUpdateCallback( new WGEFunctorCallback< osg::Node >( boost::bind( &WMClusterDisplay::updateWidgets, this ) ) );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootNode );
}


void WMClusterDisplay::properties()
{
    // Initialize the properties
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_propSelectedCluster = m_properties->addProperty( "Selected Cluster", "", 0, m_propCondition );
    m_propSelectedCluster->setMin( 0 );
    m_propSelectedCluster->setMax( 0 );

    m_propSelectedClusterOffset = m_properties->addProperty( "Offset", "", 0, m_propCondition );
    m_propSelectedClusterOffset->setMin( -10 );
    m_propSelectedClusterOffset->setMax( 10 );


    m_propSubClusters = m_properties->addProperty( "Subclusters", "How many sub clusters should be selected", 10, m_propCondition );
    m_propSubClusters->setMin( 1 );
    m_propSubClusters->setMax( 50 );

    m_propSubLevelsToColor = m_properties->addProperty( "Sublevels to Color", "Number of subclusters to color differently", 0, m_propCondition );
    m_propSubLevelsToColor->setMin( 0 );
    m_propSubLevelsToColor->setMax( 0 );

    m_propMinSizeToColor = m_properties->addProperty( "Min size to show", "Specifies a minimum size for a cluster to be drawn", 1, m_propCondition ); // NOLINT
    m_propMinSizeToColor->setMin( 1 );
    m_propMinSizeToColor->setMax( 200 );

    m_propMaxSubClusters = m_properties->addProperty( "Max clusters in box ", "", 1, m_propCondition );
    m_propMaxSubClusters->setMin( 1 );
    m_propMaxSubClusters->setMax( 100 );

    m_propBoxClusterRatio = m_properties->addProperty( "Box-Cluster Ratio", "", 0.9, m_propCondition );
    m_propBoxClusterRatio->setMin( 0.0 );
    m_propBoxClusterRatio->setMax( 1.0 );

    m_propShowTree = m_properties->addProperty( "Show widget", "", false, m_propCondition );

    m_groupDendrogram = m_properties->addPropertyGroup( "Dendrogram",  "Properties only related to the dendrogram." );

    m_propShowDendrogram = m_groupDendrogram->addProperty( "Show dendrogram", "", true, m_propCondition );

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

    m_propTreeFile = m_properties->addProperty( "Tree file", "", WPathHelper::getAppPath() );
    m_readTriggerProp = m_properties->addProperty( "Do read",  "Press!", WPVBaseTypes::PV_TRIGGER_READY, m_propCondition );
    WPropertyHelper::PC_PATHEXISTS::addTo( m_propTreeFile );

    WModule::properties();
}

void WMClusterDisplay::moduleMain()
{
    osg::ref_ptr< MainViewEventHandler > eh( new MainViewEventHandler );
    eh->subscribeLeftButtonPush( boost::bind( &WMClusterDisplay::dendrogramClick, this, _1 ) );

    WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getView()->addEventHandler( eh );

    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_propCondition );
    m_moduleState.add( m_active->getUpdateCondition() );
    m_moduleState.add( m_fiberInput->getDataChangedCondition() );

    ready();

    bool treeLoaded = false;

    // no text file was found, wait for the user to manually load on
    while( !m_shutdownFlag() )
    {
        if( m_shutdownFlag() )
        {
            break;
        }

        m_moduleState.wait();

        if( m_dataSet != m_fiberInput->getData() )
        {
            m_dataSet = m_fiberInput->getData();
            std::string fn = m_dataSet->getFilename();
            std::string ext( ".fib" );
            std::string csvExt( "_hie.txt" );
            fn.replace( fn.find( ext ), ext.size(), csvExt );
            treeLoaded = loadTreeAscii( fn );
            if( treeLoaded )
            {
                break;
            }
        }


        if( m_readTriggerProp->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            std::string fileName = m_propTreeFile->get().string().c_str();
            treeLoaded = loadTreeAscii( fileName );
            m_readTriggerProp->set( WPVBaseTypes::PV_TRIGGER_READY, true );
            if( treeLoaded )
            {
                break;
            }
        }
    }

    m_fiberSelector = boost::shared_ptr<WFiberSelector>( new WFiberSelector( m_dataSet ) );
    m_tree.setRoiBitField( m_fiberSelector->getBitfield() );

    m_propTreeFile->setHidden( true );
    m_readTriggerProp->setHidden( true );

    m_propSelectedCluster->setMin( m_tree.getLeafCount() );
    m_propSelectedCluster->setMax( m_tree.getClusterCount() - 1 );
    m_propSelectedCluster->set( m_tree.getClusterCount() - 1 );
    m_propSubLevelsToColor->setMax( m_tree.getLevel( m_propSelectedCluster->get() ) );
    m_propMinSizeToColor->setMax( 100 );
    m_rootCluster = m_propSelectedCluster->get();

    m_propSelectedClusterOffset->setMax( m_tree.getMaxLevel() - m_tree.getLevel( m_propSelectedCluster->get() ) );
    m_propSelectedClusterOffset->setMin( 0 - m_tree.getLevel( m_propSelectedCluster->get() ) );

    initWidgets();
    createFiberGeode();

    m_widgetDirty = true;
    updateWidgets();

    WKernel::getRunningKernel()->getRoiManager()->getProperties()->getProperty( "dirty" )->getUpdateCondition()->subscribeSignal(
            boost::bind( &WMClusterDisplay::handleRoiChanged, this ) );

    m_dendrogramGeode = new WDendrogramGeode( &m_tree, m_rootCluster, 1000, 500 );
    m_camera->addChild( m_dendrogramGeode );

    m_propSelectedCluster->get( true );
    m_propSubClusters->get( true );
    m_propSubLevelsToColor->get( true );
    m_propMinSizeToColor->get( true );

    // main loop
    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        if( m_shutdownFlag() )
        {
           break;
        }

        if( m_propSelectedCluster->changed() )
        {
            handleSelectedClusterChanged();
        }

        if( m_propSelectedClusterOffset->changed() )
        {
            handleOffsetChanged();
        }

        if( m_propSubClusters->changed() )
        {
            handleBiggestClustersChanged();
        }

        if( m_propSubLevelsToColor->changed() )
        {
            handleColoringChanged();
        }

        if( m_propMinSizeToColor->changed() )
        {
            handleMinSizeChanged();
        }

        if( m_propBoxClusterRatio->changed() || m_propMaxSubClusters->changed() )
        {
            handleRoiChanged();
        }

        if( m_propShowTree->changed() )
        {
            m_widgetDirty = true;
        }

        if( m_propShowDendrogram->changed() || m_propResizeWithWindow->changed() || m_propDendrogramSizeX->changed() ||
                m_propDendrogramSizeY->changed() || m_propDendrogramOffsetX->changed() || m_propDendrogramOffsetY->changed() )
        {
            m_dendrogramDirty = true;
        }

        if( m_active->changed() )
        {
            //WKernel::getRunningKernel()->getRoiManager()->setUseExternalBitfield( m_active->get( true ) );
        }
    }

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );
}

void WMClusterDisplay::handleSelectedClusterChanged()
{
    m_rootCluster = m_propSelectedCluster->get( true );
    m_propSelectedClusterOffset->setMax( m_tree.getMaxLevel() - m_tree.getLevel( m_propSelectedCluster->get() ) );
    m_propSelectedClusterOffset->setMin( 0 - m_tree.getLevel( m_propSelectedCluster->get() ) );
    m_propSelectedClusterOffset->set( 0 );
    m_propSelectedClusterOffset->get( true );
    m_propMinSizeToColor->setMax( m_tree.size( m_biggestClusters.back() ) );

    m_fiberDrawable->setBitfield( m_tree.getOutputBitfield( m_rootCluster ) );

    m_propSubLevelsToColor->setMax( m_tree.getLevel( m_rootCluster ) );
    colorClusters( m_propSelectedCluster->get( true ) );

    m_dendrogramDirty = true;
}

void WMClusterDisplay::handleOffsetChanged()
{
    if( m_propSelectedClusterOffset->get( true ) == 0 )
    {
        m_rootCluster = m_propSelectedCluster->get( true );
    }
    else if( m_propSelectedClusterOffset->get( true ) > 0 )
    {
        int plus = m_propSelectedClusterOffset->get( true );
        m_rootCluster = m_propSelectedCluster->get( true );
        while( plus > 0 )
        {
            m_rootCluster = m_tree.getParent( m_rootCluster );
            --plus;
        }
    }
    else
    {
        int minusOff = m_propSelectedClusterOffset->get( true );
        m_rootCluster = m_propSelectedCluster->get( true );

        while( minusOff < 0 )
        {
            if( m_tree.getLevel( m_rootCluster ) > 0 )
            {
                size_t left =  m_tree.getChildren( m_rootCluster ).first;
                size_t right = m_tree.getChildren( m_rootCluster ).second;

                size_t leftSize =  m_tree.size( left );
                size_t rightSize = m_tree.size( right );

                if( leftSize >= rightSize )
                {
                    m_rootCluster = left;
                }
                else
                {
                    m_rootCluster = right;
                }
            }
            ++minusOff;
        }
    }
    m_propSubLevelsToColor->setMax( m_tree.getLevel( m_rootCluster ) );

    m_dendrogramDirty = true;
}

void WMClusterDisplay::handleBiggestClustersChanged()
{
    m_biggestClusters = m_tree.findXBiggestClusters( m_propSelectedCluster->get(), m_propSubClusters->get( true ) );

    m_propMinSizeToColor->setMax( m_tree.size( m_biggestClusters.back() ) );

    m_tree.colorCluster( m_tree.getClusterCount() - 1, WColor( 0.3, 0.3, 0.3, 1.0 ) );
    setColor( m_tree.getLeafesForCluster( m_rootCluster ), WColor( 0.3, 0.3, 0.3, 1.0 ) );

    for( size_t k = 0; k < m_biggestClusters.size(); ++k )
    {
        size_t current = m_biggestClusters[k];
        setColor( m_tree.getLeafesForCluster( current ), wge::getNthHSVColor( k ) );
        m_tree.colorCluster( current, wge::getNthHSVColor( k ) );
    }

    m_dendrogramDirty = true;
    m_widgetDirty = true;
    m_biggestClusterButtonsChanged = true;

    m_fiberDrawable->setBitfield( m_tree.getOutputBitfield( m_biggestClusters ) );
}

void WMClusterDisplay::handleColoringChanged()
{
    m_propSubLevelsToColor->get( true );
    m_propMinSizeToColor->get( true );
    colorClusters( m_propSelectedCluster->get( true ) );
    m_dendrogramDirty = true;
}

void WMClusterDisplay::handleMinSizeChanged()
{
    m_dendrogramDirty = true;
}

void WMClusterDisplay::handleRoiChanged()
{
    WKernel::getRunningKernel()->getRunningKernel()->getRoiManager()->dirty( true );

    if( m_active->get() )
    {
        m_biggestClusters = m_tree.getBestClustersFittingRoi( m_propBoxClusterRatio->get( true ), m_propMaxSubClusters->get( true ) );

        m_tree.colorCluster( m_tree.getClusterCount() - 1, WColor( 0.3, 0.3, 0.3, 1.0 ) );
        setColor( m_tree.getLeafesForCluster( m_rootCluster ), WColor( 0.3, 0.3, 0.3, 1.0 ) );

        for( size_t k = 0; k < m_biggestClusters.size(); ++k )
        {
            size_t current = m_biggestClusters[k];
            setColor( m_tree.getLeafesForCluster( current ), wge::getNthHSVColor( k ) );
            m_tree.colorCluster( current, wge::getNthHSVColor( k ) );
        }

        m_widgetDirty = true;
        m_biggestClusterButtonsChanged = true;
        m_dendrogramDirty = true;
    }

    m_fiberDrawable->setBitfield( m_tree.getOutputBitfield( m_biggestClusters ) );
}

void WMClusterDisplay::updateWidgets()
{
    osg::ref_ptr<osgViewer::View> viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getView();

    int height = viewer->getCamera()->getViewport()->height();
    int width = viewer->getCamera()->getViewport()->width();

    if( ( height != m_oldViewHeight ) || width != m_oldViewWidth )
    {
        m_oldViewHeight = height;
        m_oldViewWidth = width;

        m_dendrogramDirty = true;
    }


    if( !widgetClicked() && !m_widgetDirty && !m_dendrogramDirty )
    {
        return;
    }
    // store current cluster id for ease of access and better readability
    size_t current = m_propSelectedCluster->get();
    size_t up1, down11, down12; // up2, down21, down22, down23, down24;

    m_treeButtonList[0]->setId( current );
    m_treeButtonList[0]->setLabel( createLabel( current ) );

    for( size_t i = 0; i < m_treeButtonList.size(); ++i )
    {
        m_treeButtonList[i]->hide();
    }

    if( m_propShowTree->get( true ) )
    {
        m_treeButtonList[0]->show();

        // are we on top?
        if( m_tree.getLevel( current ) < m_tree.getMaxLevel() )
        {
            // not yet
            up1 = m_tree.getParent( current );
            m_treeButtonList[1]->setId( up1 );
            m_treeButtonList[1]->setLabel( createLabel( up1 ) );
            m_treeButtonList[1]->show();
        }
        // are we all the way down
        if( m_tree.getLevel( current ) > 0 )
        {
            down11 = m_tree.getChildren( current ).first;
            down12 = m_tree.getChildren( current ).second;

            m_treeButtonList[2]->setId( down11 );
            m_treeButtonList[2]->setLabel( createLabel( down11 ) );
            m_treeButtonList[2]->show();

            m_treeButtonList[3]->setId( down12 );
            m_treeButtonList[3]->setLabel( createLabel( down12 ) );
            m_treeButtonList[3]->show();
        }

        m_treeButtonList[4]->show();
        m_treeButtonList[5]->show();
        m_treeButtonList[6]->show();
    }

    if( m_biggestClusterButtonsChanged )
    {
        for( size_t i = 0; i < m_biggestClustersButtonList.size(); ++i )
        {
            m_wm->removeChild( m_biggestClustersButtonList[i] );
        }

        m_biggestClustersButtonList.clear();
        for( size_t i = 0; i < m_biggestClusters.size(); ++i )
        {
            osg::ref_ptr<WOSGButton> newButton1 = osg::ref_ptr<WOSGButton>( new WOSGButton( std::string( "" ),
                    osgWidget::Box::VERTICAL, true, false ) );
            newButton1->setPosition( osg::Vec3( 10.f,  i * 20.f, 0 ) );
            newButton1->setId( 10000000 + m_biggestClusters[i] );
            newButton1->setLabel( std::string( " S " ) );
            newButton1->managed( m_wm );
            m_wm->addChild( newButton1 );
            m_biggestClustersButtonList.push_back( newButton1 );
            newButton1->setBackgroundColor( wge::getNthHSVColor( i ) );

            osg::ref_ptr<WOSGButton> newButton = osg::ref_ptr<WOSGButton>( new WOSGButton( std::string( "" ),
                    osgWidget::Box::VERTICAL, true, true ) );
            newButton->setPosition( osg::Vec3( 35.f,  i * 20.f, 0 ) );
            newButton->setId( m_biggestClusters[i] );
            newButton->setLabel( createLabel( m_biggestClusters[i] ) );
            newButton->managed( m_wm );
            m_wm->addChild( newButton );
            m_biggestClustersButtonList.push_back( newButton );
            newButton->setBackgroundColor( wge::getNthHSVColor( i ) );
        }
        osg::ref_ptr<WOSGButton> newButton1 = osg::ref_ptr<WOSGButton>( new WOSGButton( std::string( "" ),
                osgWidget::Box::VERTICAL, true, false ) );
        newButton1->setPosition( osg::Vec3( 10.f,  m_biggestClusters.size() * 20.f, 0 ) );
        newButton1->setId( 10000000 + m_propSelectedCluster->get() );
        newButton1->setLabel( std::string( " S " ) );
        newButton1->managed( m_wm );
        m_wm->addChild( newButton1 );
        m_biggestClustersButtonList.push_back( newButton1 );
        newButton1->setBackgroundColor( WColor( 0.4, 0.4, 0.4, 1.0 ) );

        osg::ref_ptr<WOSGButton> newButton = osg::ref_ptr<WOSGButton>( new WOSGButton( std::string( "" ),
                        osgWidget::Box::VERTICAL, true, true ) );
        newButton->setPosition( osg::Vec3( 35.f,  m_biggestClusters.size() * 20.f, 0 ) );
        newButton->setId( m_propSelectedCluster->get() );
        newButton->setLabel( createLabel( m_propSelectedCluster->get() ) );
        newButton->managed( m_wm );
        m_wm->addChild( newButton );

        m_biggestClustersButtonList.push_back( newButton );
        newButton->setBackgroundColor( WColor( 0.4, 0.4, 0.4, 1.0 ) );
        m_biggestClusterButtonsChanged = false;
    }
    m_wm->resizeAllWindows();

    if( m_dendrogramDirty )
    {
        //m_camera->removeChild( m_dendrogramGeode );
        m_camera->removeChild( 1, 1 );

        int dwidth = m_propDendrogramSizeX->get( true );
        int dheight = m_propDendrogramSizeY->get( true );
        int dxOff = m_propDendrogramOffsetX->get( true );
        int dyOff = m_propDendrogramOffsetY->get( true );

        if( m_propShowDendrogram->get( true ) )
        {
            if( m_propResizeWithWindow->get( true ) )
            {
                m_dendrogramGeode = new WDendrogramGeode( &m_tree, m_tree.getClusterCount() - 1, true,
                        m_propMinSizeToColor->get(), width - 120, height / 2 , 100 );
            }
            else
            {
                m_dendrogramGeode = new WDendrogramGeode( &m_tree, m_tree.getClusterCount() - 1, true,
                        m_propMinSizeToColor->get(), dwidth, dheight, dxOff, dyOff );
            }
            m_camera->addChild( m_dendrogramGeode );
        }
        m_dendrogramDirty = false;
    }
}

std::string WMClusterDisplay::createLabel( size_t id )
{
    switch( m_labelMode )
    {
        case 1:
            return string_utils::toString( m_tree.size( id ) );
            break;
        case 2:
            return string_utils::toString( m_tree.getCustomData( id ) );
            break;
        default:
            return string_utils::toString( id );
    }
}

bool WMClusterDisplay::widgetClicked()
{
    bool clicked = false;

    if( m_treeButtonList[0]->clicked() )
    {
        clicked = true;
        m_propSelectedCluster->set( m_rootCluster );
        m_propSelectedClusterOffset->set( 0 );
        m_propSelectedClusterOffset->setMax( m_tree.getMaxLevel() - m_tree.getLevel( m_propSelectedCluster->get() ) );
        m_propSelectedClusterOffset->setMin( 0 - m_tree.getLevel( m_propSelectedCluster->get() ) );
    }

    for( size_t i = 1; i < m_treeButtonList.size() - 3; ++i )
    {
        if( m_treeButtonList[i]->clicked() )
        {
            clicked = true;
            m_propSelectedCluster->set( m_treeButtonList[i]->getId() );
        }
    }

    for( size_t i = 0; i < 3; ++i )
    {
        if( m_treeButtonList[4 + i]->clicked() )
        {
            clicked = true;
            m_labelMode = i;
        }
    }

    bool biggestClusterSelectionChanged = false;
    for( size_t i = 0; i < m_biggestClustersButtonList.size(); ++i )
    {
        if( m_biggestClustersButtonList[i]->clicked() )
        {
            if( m_biggestClustersButtonList[i]->getId() < 10000000 )
            {
                biggestClusterSelectionChanged = true;
                clicked = true;
            }
            else
            {
                clicked = true;
                m_propSelectedCluster->set( m_biggestClustersButtonList[i]->getId() - 10000000 );
            }
        }
    }

    if( biggestClusterSelectionChanged )
    {
        std::vector<size_t>activeClusters;
        for( size_t i = 0; i < m_biggestClustersButtonList.size(); ++i )
        {
            if( m_biggestClustersButtonList[i]->pushed() )
            {
                activeClusters.push_back( m_biggestClustersButtonList[i]->getId() );
            }
        }

        m_fiberDrawable->setBitfield( m_tree.getOutputBitfield( activeClusters ) );
        //WKernel::getRunningKernel()->getRoiManager()->setExternalBitfield( m_tree.getOutputBitfield( activeClusters ) );
    }
    return clicked;
}

void WMClusterDisplay::colorClusters( size_t current )
{
    size_t num = m_propSubLevelsToColor->get();
    size_t size = m_propMinSizeToColor->get();

    m_tree.colorCluster( m_tree.getClusterCount() - 1, WColor( 0.3, 0.3, 0.3, 1.0 ) );

    std::vector<size_t>finalList;
    std::queue<size_t>currentLevelList;

    currentLevelList.push( current );

    std::queue<size_t>nextLevelList;
    while( num > 0 )
    {
        while( !currentLevelList.empty() )
        {
            size_t cluster = currentLevelList.front();
            currentLevelList.pop();

            if( m_tree.getLevel( cluster ) > 0 )
            {
                size_t left = m_tree.getChildren( cluster ).first;
                size_t right = m_tree.getChildren( cluster ).second;

                if(  m_tree.size( left ) >= size )
                {
                    nextLevelList.push( left );
                }
                else
                {
                    //finalList.push( left );
                }
                if( m_tree.size( right ) >= size )
                {
                    nextLevelList.push( right );
                }
                else
                {
                    //finalList.push( right );
                }
            }
            else
            {
                finalList.push_back( cluster );
            }
        }

        while( !nextLevelList.empty() )
        {
            size_t cluster = nextLevelList.front();
            nextLevelList.pop();
            currentLevelList.push( cluster );
        }
        --num;
    }


    while( !currentLevelList.empty() )
    {
        size_t cluster = currentLevelList.front();
        currentLevelList.pop();
        finalList.push_back( cluster );
    }

    int n = 0;

    for( size_t i = 0; i < finalList.size(); ++i )
    {
        size_t cluster = finalList[i];
        m_tree.colorCluster( cluster, wge::getNthHSVColor( n ) );
        setColor( m_tree.getLeafesForCluster( cluster ), wge::getNthHSVColor( n++ ) );
    }

    m_fiberDrawable->setBitfield( m_tree.getOutputBitfield( finalList ) );
    //WKernel::getRunningKernel()->getRoiManager()->setExternalBitfield( m_tree.getOutputBitfield( finalList ) );

    m_biggestClusters.clear();
    m_biggestClusterButtonsChanged = true;
}

void WMClusterDisplay::setColor( std::vector<size_t> clusters, WColor color )
{
    boost::shared_ptr< std::vector< float > >colorField = m_dataSet->getColorScheme( "Custom Color" )->getColor();
    boost::shared_ptr< std::vector< size_t > > starts   = m_fiberSelector->getStarts();
    boost::shared_ptr< std::vector< size_t > > lengths  = m_fiberSelector->getLengths();

    for( size_t i = 0; i < clusters.size(); ++i )
    {
        size_t current = clusters[i];

        for( size_t k = (*starts)[current]; k < (*starts)[current] + (*lengths)[current]; ++k)
        {
            (*colorField)[k*3] =   color[0];
            (*colorField)[k*3+1] = color[1];
            (*colorField)[k*3+2] = color[2];
        }
    }
}

bool WMClusterDisplay::dendrogramClick( const WVector2f& pos )
{
    if( m_dendrogramGeode->inDendrogramArea( pos ) )
    {
        int x = pos[0];
        int y = pos[1];
        m_propSelectedCluster->set( m_dendrogramGeode->getClickedCluster( x, y ) );
        return true;
    }
    return false;
}

void WMClusterDisplay::createFiberGeode()
{
    m_fiberDrawable = osg::ref_ptr< WFiberDrawable >( new WFiberDrawable );
    m_fiberDrawable->setBound( m_dataSet->getBoundingBox().toOSGBB() );
    m_fiberDrawable->setStartIndexes( m_dataSet->getLineStartIndexes() );
    m_fiberDrawable->setPointsPerLine( m_dataSet->getLineLengths() );
    m_fiberDrawable->setVerts( m_dataSet->getVertices() );
    m_fiberDrawable->setTangents( m_dataSet->getTangents() );
    m_fiberDrawable->setColor( m_dataSet->getColorScheme( "Custom Color" )->getColor() );
    m_fiberDrawable->setBitfield( m_fiberSelector->getBitfield() );

    m_fiberDrawable->setUseDisplayList( false );
    m_fiberDrawable->setDataVariance( osg::Object::DYNAMIC );

    osg::ref_ptr< osg::Geode > geode = osg::ref_ptr< osg::Geode >( new osg::Geode );
    geode->addDrawable( m_fiberDrawable );

    m_rootNode->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    m_rootNode->addChild( geode );
}
