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

#include <boost/regex.hpp>

#include <osgGA/TrackballManipulator>
#include <osgGA/StateSetManipulator>
#include <osgViewer/ViewerEventHandlers>
#include <osgWidget/Util> //NOLINT
#include <osgWidget/ViewerEventHandlers> //NOLINT
#include <osgWidget/WindowManager> //NOLINT
#include <osg/LightModel>

#include "../../common/WPathHelper.h"
#include "../../common/WPropertyHelper.h"

#include "../../graphicsEngine/WGEUtils.h"
#include "../../graphicsEngine/algorithms/WMarchingLegoAlgorithm.h"

#include "../../kernel/WKernel.h"

#include "WMClusterDisplayVoxels.xpm"

#include "WMClusterDisplayVoxels.h"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMClusterDisplayVoxels )

WMClusterDisplayVoxels::WMClusterDisplayVoxels():
    WModule(),
    m_moduleNode( new WGEGroupNode() ),
    m_dendrogramNode( new WGEGroupNode() ),
    m_meshNode( new WGEGroupNode() ),
    m_widgetDirty( false ),
    m_biggestClusterButtonsChanged( false ),
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

std::vector< std::string > WMClusterDisplayVoxels::readFile( const std::string fileName )
{
    std::ifstream ifs( fileName.c_str(), std::ifstream::in );

    std::vector< std::string > lines;

    std::string line;

    if ( ifs.is_open() )
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

    while ( !ifs.eof() )
    {
        getline( ifs, line );

        lines.push_back( std::string( line ) );
    }

    ifs.close();

    return lines;
}

void WMClusterDisplayVoxels::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_propClusterFile = m_properties->addProperty( "Cluster file", "", boost::filesystem::path( "/SCR/schurade/data/david/whole/avg.txt" ) );
    m_propReadTrigger = m_properties->addProperty( "Do read",  "Press!", WPVBaseTypes::PV_TRIGGER_READY, m_propCondition );

    m_propLevelsFromTop = m_properties->addProperty( "Levels from top", "", 0, m_propCondition );
    m_propHideOutliers = m_properties->addProperty( "Hide outliers", "", false, m_propCondition );

    m_propSelectedCluster = m_properties->addProperty( "Selected Cluster", "", 0, m_propCondition );
    m_propSelectedCluster->setMin( 0 );
    m_propSelectedCluster->setMax( 0 );

    m_propXBiggestClusters = m_properties->addProperty( "Biggest Clusters", "", 5, m_propCondition );
    m_propXBiggestClusters->setMin( 1 );
    m_propXBiggestClusters->setMax( 1000 );

    m_propShowVoxelTriangulation = m_properties->addProperty( "Triangulate", "", false, m_propCondition );

    m_propMinSizeToColor = m_properties->addProperty( "Min size to show", "Specifies a minimum size for a cluster to be drawn", 1, m_propCondition ); // NOLINT
    m_propMinSizeToColor->setMin( 1 );
    m_propMinSizeToColor->setMax( 200 );

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

    m_infoCountLeafes->set( static_cast<int>( m_tree.getLeafCount() ) );
    m_infoCountClusters->set( static_cast<int>( m_tree.getClusterCount() ) );
    m_infoMaxLevel->set( m_tree.getMaxLevel() );

    m_propMinSizeToColor->setMax( 100 );
    m_rootCluster = m_propSelectedCluster->get();

    m_propLevelsFromTop->setMin( 0 );
    m_propLevelsFromTop->setMax( m_tree.getMaxLevel() );
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

    while ( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        if ( m_shutdownFlag() )
        {
            break;
        }

        boost::shared_ptr< WDataSetSingle > newDataSet = m_input->getData();
        bool dataChanged = ( m_dataSet != newDataSet );
        bool dataValid   = ( newDataSet );

        if( dataValid )
        {
            if ( dataChanged )
            {
                m_dataSet = newDataSet;
                m_grid = boost::shared_dynamic_cast< WGridRegular3D >( m_dataSet->getGrid() );
                break;
            }
        }
    }

    while ( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        if ( m_shutdownFlag() )
        {
            break;
        }

        if ( m_propReadTrigger->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            boost::filesystem::path fileName = m_propClusterFile->get();
            m_propReadTrigger->set( WPVBaseTypes::PV_TRIGGER_READY, true );

            if ( loadClustering( fileName ) )
            {
                m_propReadTrigger->setHidden( true );
                m_propClusterFile->setHidden( true );
                break;
            }
        }
    }

    createTexture();
    setPropertyBoundaries();

    initWidgets();
    m_widgetDirty = true;
    updateWidgets();

    m_dendrogramGeode = new WDendrogramGeode( &m_tree, m_rootCluster, 1000, 500 );
    m_camera->addChild( m_dendrogramGeode );

    m_propSelectedCluster->get( true );
    m_propMinSizeToColor->get( true );

    while ( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        if ( m_shutdownFlag() )
        {
            break;
        }

        if( m_propSelectedCluster->changed() )
        {
            m_activatedClusters.clear();
            m_activatedClusters.push_back( m_propSelectedCluster->get( true ) );
            updateAll();
        }

        if( m_propXBiggestClusters->changed() )
        {
            m_activatedClusters = m_tree.findXBiggestClusters2( m_propSelectedCluster->get(), m_propXBiggestClusters->get( true ) );
            updateAll();
        }

        if ( m_propLevelsFromTop->changed() || m_propHideOutliers->changed() )
        {
            m_activatedClusters = m_tree.downXLevelsFromTop( m_propLevelsFromTop->get( true ), m_propHideOutliers->get( true ) );
            updateAll();
        }

        if( m_propShowVoxelTriangulation->changed() )
        {
            createMesh();
            renderMesh();
        }

        if ( m_propMinSizeToColor->changed() )
        {
            handleMinSizeChanged();
        }

        if ( m_propShowDendrogram->changed() || m_propResizeWithWindow->changed() || m_propDendrogramSizeX->changed() ||
                m_propDendrogramSizeY->changed() || m_propDendrogramOffsetX->changed() || m_propDendrogramOffsetY->changed() )
        {
            m_dendrogramDirty = true;
        }
    }

    con.disconnect();

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_moduleNode );
}

void WMClusterDisplayVoxels::updateAll()
{
    debugLog() << "start erasing clusters";
    m_tree.colorCluster( m_tree.getClusterCount() - 1, WColor( 0.3, 0.3, 0.3, 1.0 ) );
    debugLog() << "end erasing clusters";

    debugLog() << "start color clusters";
    for ( size_t k = 0; k < m_activatedClusters.size(); ++k )
    {
        size_t current = m_activatedClusters[k];
        m_tree.colorCluster( current, wge::getNthHSVColor( k, m_activatedClusters.size() ) );
    }
    debugLog() << "end color clusters";

    debugLog() << "start label voxels";
    m_data.resize( m_data.size(), 0 );
    for ( size_t i = 0; i < m_activatedClusters.size(); ++i )
    {
        std::vector<size_t> voxels = m_tree.getVoxelsForCluster( m_activatedClusters[i] );

        for ( size_t k = 0; k < voxels.size(); ++k )
        {
            m_data[voxels[k]] = i + 1;
        }
    }
    debugLog() << "end label voxels";

    debugLog() << "start fill texture";
    unsigned char* data = m_texture->getImage()->data();
    for ( size_t i = 0; i < m_grid->size(); ++i )
    {
        if ( m_data[i] )
        {
            WColor color = wge::getNthHSVColor( m_data[i] - 1, m_activatedClusters.size() );
            data[i * 3    ] = color.getRed() * 255;
            data[i * 3 + 1] = color.getGreen() * 255;
            data[i * 3 + 2] = color.getBlue() * 255;
        }
        else
        {
            data[i * 3    ] = 0.0;
            data[i * 3 + 1] = 0.0;
            data[i * 3 + 2] = 0.0;
        }
    }

    m_texture->dirtyTextureObject();
    debugLog() << "end fill texture";

    createMesh();
    renderMesh();

    m_dendrogramDirty = true;
    m_widgetDirty = true;
    m_biggestClusterButtonsChanged = true;
}

void WMClusterDisplayVoxels::handleMinSizeChanged()
{
    m_dendrogramDirty = true;
}

bool WMClusterDisplayVoxels::loadClustering( boost::filesystem::path clusterFile )
{
    debugLog() << "start parsing tree file...";
    using namespace boost::filesystem; //NOLINT

    if ( !exists( clusterFile ) )
    {
        debugLog() << "file doesn't exist";
        return false;
    }
    std::vector<std::string> lines;
    lines = readFile( clusterFile.string() );

    if ( lines.size() == 0 )
    {
        debugLog() << "file is empty";
        return false;
    }

    size_t i = 0;
    while ( i < lines.size() )
    {
        if ( lines[i] == "#coordinates" )
        {
            debugLog() << "coordinates tag at line " << i;
            ++i;
            break;
        }
        else
        {
            ++i;
        }
    }
    while ( i < lines.size() )
    {
        if ( lines[i] == "#endcoordinates" )
        {
            debugLog() << "endcoordinates tag at line " << i;
            ++i;
            break;
        }
        else
        {
            std::vector<std::string>svec;
            boost::regex reg( "," );
            boost::sregex_token_iterator it( lines[i].begin(), lines[i].end(), reg, -1 );
            boost::sregex_token_iterator end;
            while ( it != end )
            {
                svec.push_back( *it++ );
            }
            m_tree.addLeaf( m_grid->getVoxelNum( boost::lexical_cast< size_t >( svec[0] ),
                                                  boost::lexical_cast< size_t >( svec[1] ),
                                                  boost::lexical_cast< size_t >( svec[2] ) ) );
            ++i;
        }
    }

    while ( i < lines.size() )
    {
        if ( lines[i] == "#clusters" )
        {
            debugLog() << "clusters tag at line " << i;
            ++i;
            break;
        }
        else
        {
            ++i;
        }
    }
    while ( i < lines.size() )
    {
        if ( lines[i] == "#endclusters" )
        {
            debugLog() << "endclusters tag at line " << i;
            ++i;
            break;
        }
        else
        {
            std::vector<std::string>svec;
            boost::regex reg( "," );
            boost::sregex_token_iterator it( lines[i].begin(), lines[i].end(), reg, -1 );
            boost::sregex_token_iterator end;
            while ( it != end )
            {
                svec.push_back( *it++ );
            }
            m_tree.addCluster( boost::lexical_cast< size_t >( svec[0] ) - 1,
                                boost::lexical_cast< size_t >( svec[1] ) - 1,
                                boost::lexical_cast< float >( svec[2] ) );
            ++i;
        }
    }
    return true;
}

void WMClusterDisplayVoxels::createTexture()
{
     osg::ref_ptr< osg::Image > ima = new osg::Image;
     ima->allocateImage( m_grid->getNbCoordsX(), m_grid->getNbCoordsY(), m_grid->getNbCoordsZ(), GL_RGB, GL_UNSIGNED_BYTE );
     unsigned char* data = ima->data();
     m_data.resize( m_grid->getNbCoordsX() * m_grid->getNbCoordsY() * m_grid->getNbCoordsZ(), 0 );

     for ( unsigned int i = 0; i < m_grid->size() * 3; ++i )
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
}

void WMClusterDisplayVoxels::createMesh()
{
    m_triMeshes.clear();

    size_t order = ( *m_dataSet ).getValueSet()->order();
    size_t vDim = ( *m_dataSet ).getValueSet()->dimension();

    for ( size_t k = 1; k <= m_activatedClusters.size(); ++k )
    {
        std::vector< float > newVals( m_grid->size(), 0 );

        for ( size_t i = 0; i < m_grid->size(); ++i )
        {
            if ( m_data[i] == k )
            {
                newVals[i] = 1.0;
            }
        }

        boost::shared_ptr< WValueSet< float > >newValueSet = boost::shared_ptr< WValueSet< float > >(
                                                                new WValueSet< float >( order, vDim, newVals, W_DT_FLOAT ) );
        WMarchingLegoAlgorithm mlAlgo;
        m_triMeshes.push_back( mlAlgo.generateSurface( m_grid->getNbCoordsX(), m_grid->getNbCoordsY(), m_grid->getNbCoordsZ(),
                                            m_grid->getTransformationMatrix(),
                                            newValueSet->rawDataVectorPointer(),
                                            0.9 ) );
    }
}

void WMClusterDisplayVoxels::renderMesh()
{
    if ( !m_outputGeodes.empty() )
    {
        for ( size_t i = 0; i < m_outputGeodes.size(); ++i )
        {
            m_meshNode->remove( m_outputGeodes[i] );
        }
        m_outputGeodes.clear();
    }

    if ( m_propShowVoxelTriangulation->get( true ) )
    {
        for ( size_t i = 0; i < m_triMeshes.size(); ++i )
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

            WColor color = wge::getNthHSVColor( i, m_triMeshes.size() );
            colors->push_back( osg::Vec4( color.getRed(), color.getGreen(), color.getBlue(), 1.0f ) );
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

    if ( ( height != m_oldViewHeight ) || width != m_oldViewWidth )
    {
        m_oldViewHeight = height;
        m_oldViewWidth = width;

        m_dendrogramDirty = true;
    }

    if ( !widgetClicked() && !m_widgetDirty && !m_dendrogramDirty )
    {
        return;
    }

    if ( m_biggestClusterButtonsChanged )
    {
        for ( size_t i = 0; i < m_activeClustersButtonList.size(); ++i )
        {
            m_wm->removeChild( m_activeClustersButtonList[i] );
        }

        m_activeClustersButtonList.clear();
        for ( size_t i = 0; i < m_activatedClusters.size(); ++i )
        {
            osg::ref_ptr<WOSGButton> newButton1 = osg::ref_ptr<WOSGButton>( new WOSGButton( std::string( "" ),
                    osgWidget::Box::VERTICAL, true, false ) );
            newButton1->setPosition( osg::Vec3( 10.f,  i * 20.f, 0 ) );
            newButton1->setId( 10000000 + m_activatedClusters[i] );
            newButton1->setLabel( std::string( " S " ) );
            newButton1->managed( m_wm );
            m_wm->addChild( newButton1 );
            m_activeClustersButtonList.push_back( newButton1 );
            newButton1->setBackgroundColor( wge::getNthHSVColor( i, m_activatedClusters.size() ) );

            osg::ref_ptr<WOSGButton> newButton = osg::ref_ptr<WOSGButton>( new WOSGButton( std::string( "" ),
                    osgWidget::Box::VERTICAL, true, true ) );
            newButton->setPosition( osg::Vec3( 35.f,  i * 20.f, 0 ) );
            newButton->setId( m_activatedClusters[i] );
            newButton->setLabel( boost::lexical_cast<std::string>( m_tree.size( m_activatedClusters[i] ) ) );
            newButton->managed( m_wm );
            m_wm->addChild( newButton );
            m_activeClustersButtonList.push_back( newButton );
            newButton->setBackgroundColor( wge::getNthHSVColor( i, m_activatedClusters.size() ) );
        }
        osg::ref_ptr<WOSGButton> newButton1 = osg::ref_ptr<WOSGButton>( new WOSGButton( std::string( "" ),
                osgWidget::Box::VERTICAL, true, false ) );
        newButton1->setPosition( osg::Vec3( 10.f,  m_activatedClusters.size() * 20.f, 0 ) );
        newButton1->setId( 10000000 + m_propSelectedCluster->get() );
        newButton1->setLabel( std::string( " S " ) );
        newButton1->managed( m_wm );
        m_wm->addChild( newButton1 );
        m_activeClustersButtonList.push_back( newButton1 );
        newButton1->setBackgroundColor( WColor( 0.4, 0.4, 0.4, 1.0 ) );

        osg::ref_ptr<WOSGButton> newButton = osg::ref_ptr<WOSGButton>( new WOSGButton( std::string( "" ),
                        osgWidget::Box::VERTICAL, true, true ) );
        newButton->setPosition( osg::Vec3( 35.f,  m_activatedClusters.size() * 20.f, 0 ) );
        newButton->setId( m_propSelectedCluster->get() );
        newButton->setLabel( boost::lexical_cast<std::string>( m_tree.size( m_propSelectedCluster->get() ) ) );
        newButton->managed( m_wm );
        m_wm->addChild( newButton );

        m_activeClustersButtonList.push_back( newButton );
        newButton->setBackgroundColor( WColor( 0.4, 0.4, 0.4, 1.0 ) );
        m_biggestClusterButtonsChanged = false;
    }
    m_wm->resizeAllWindows();

    if ( m_dendrogramDirty )
    {
        m_camera->removeChild( m_dendrogramGeode );
        //m_camera->removeChild( 1, 1 );

        int dwidth = m_propDendrogramSizeX->get( true );
        int dheight = m_propDendrogramSizeY->get( true );
        int dxOff = m_propDendrogramOffsetX->get( true );
        int dyOff = m_propDendrogramOffsetY->get( true );

        if ( m_propShowDendrogram->get( true ) )
        {
            if ( m_propResizeWithWindow->get( true ) )
            {
                m_dendrogramGeode = new WDendrogramGeode( &m_tree, m_tree.getClusterCount() - 1,
                        m_propMinSizeToColor->get(), width - 120, height / 2 , 100 );
            }
            else
            {
                m_dendrogramGeode = new WDendrogramGeode( &m_tree, m_tree.getClusterCount() - 1,
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
    bool biggestClusterSelectionChanged = false;

    for ( size_t i = 0; i < m_activeClustersButtonList.size(); ++i )
    {
        if ( m_activeClustersButtonList[i]->clicked() )
        {
            if ( m_activeClustersButtonList[i]->getId() < 10000000 )
            {
                biggestClusterSelectionChanged = true;
                clicked = true;
            }
            else
            {
                clicked = true;
                m_propSelectedCluster->set( m_activeClustersButtonList[i]->getId() - 10000000 );
            }
        }
    }

    if ( biggestClusterSelectionChanged )
    {
        std::vector<size_t>activeClusters;
        for ( size_t i = 0; i < m_activeClustersButtonList.size(); ++i )
        {
            if ( m_activeClustersButtonList[i]->pushed() )
            {
                activeClusters.push_back( m_activeClustersButtonList[i]->getId() );
            }
        }
    }
    return clicked;
}

void WMClusterDisplayVoxels::dendrogramClick( WPickInfo pickInfo )
{
    if ( !m_propShowDendrogram->get() || !( pickInfo.getName() == "nothing" ) )
    {
        return;
    }
    int x = pickInfo.getPickPixelPosition().first;
    int y = pickInfo.getPickPixelPosition().second;

    m_propSelectedCluster->set( m_dendrogramGeode->getClickedCluster( x, y ) );
}
