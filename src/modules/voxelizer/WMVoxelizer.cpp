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

#include <algorithm>
#include <iomanip>
#include <list>
#include <string>
#include <utility>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <osg/Geode>
#include <osg/Geometry>

#include "../../common/WColor.h"
#include "../../common/WLogger.h"
#include "../../common/datastructures/WFiber.h"
#include "../../common/WPropertyHelper.h"
#include "../../dataHandler/WDataSetFiberVector.h"
#include "../../dataHandler/WSubject.h"
#include "../../graphicsEngine/WGEGeodeUtils.h"
#include "../../graphicsEngine/WGEGeometryUtils.h"
#include "../../graphicsEngine/WGEUtils.h"
#include "../../kernel/WKernel.h"
#include "WBresenham.h"
#include "WBresenhamDBL.h"
#include "WMVoxelizer.h"
#include "WRasterAlgorithm.h"
#include "WIntegrationParameterization.h"
#include "WCenterlineParameterization.h"
#include "WMVoxelizer.xpm"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMVoxelizer )

WMVoxelizer::WMVoxelizer()
    : WModule(),
      m_osgNode( new WGEGroupNode ),
      m_fiberGeode( new osg::Geode ),
      m_centerLineGeode( new osg::Geode ),
      m_boundingBoxGeode( new osg::Geode ),
      m_fullUpdate( new WCondition() )
{
}

WMVoxelizer::~WMVoxelizer()
{
}

boost::shared_ptr< WModule > WMVoxelizer::factory() const
{
    return boost::shared_ptr< WModule >( new WMVoxelizer() );
}

const char** WMVoxelizer::getXPMIcon() const
{
    return voxelizer_xpm;
}

void WMVoxelizer::moduleMain()
{
    m_moduleState.setResetable();
    m_moduleState.add( m_input->getDataChangedCondition() );  // additional fire-condition: "data changed" flag
    m_moduleState.add( m_fullUpdate );
    m_moduleState.add( m_drawCenterLine->getCondition() );

    ready();

    while ( !m_shutdownFlag() ) // loop until the module container requests the module to quit
    {
        if ( !m_input->getData() ) // ok, the output has not yet sent data
        {
            // since there is no data yet we will eat property changes
            m_drawVoxels->get( true );
            m_rasterAlgo->get( true );
            m_antialiased->get( true );
            m_drawfibers->get( true );
            continue;
        }
        if ( m_input->getData()->size() == 0 )
        {
            infoLog() << "Got empty fiber dataset. Ignoring.";
            m_moduleState.wait();
            continue;
        }

        boost::shared_ptr< WProgress > progress = boost::shared_ptr< WProgress >( new WProgress( "Marching Cubes", 4 ) );
        m_progress->addSubProgress( progress );

        ++*progress;
        // full update
        if( m_antialiased->changed() || m_drawVoxels->changed() || m_rasterAlgo->changed() || m_voxelsPerUnit->changed() ||
                m_clusters != m_input->getData() || m_parameterAlgo->changed() )
        {
            m_drawVoxels->get( true );
            m_rasterAlgo->get( true );
            m_antialiased->get( true );
            m_clusters = m_input->getData();
            update();
        }

        ++*progress;

        if( m_drawfibers->changed() || m_explicitFiberColor->changed() || m_fiberTransparency->changed() )
        {
            updateFibers();
        }

        ++*progress;

        if( m_drawCenterLine->changed() )
        {
            updateCenterLine();
        }

        ++*progress;
        progress->finish();

        m_moduleState.wait(); // waits for firing of m_moduleState ( dataChanged, shutdown, etc. )
    }
}

void WMVoxelizer::properties()
{
    m_paramAlgoSelections = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_paramAlgoSelections->addItem( "No Parameterization", "Disable parameterization." );          // NOTE: you can add XPM images here.
    m_paramAlgoSelections->addItem( "By Longest Line", "Use the longest line and parameterize the bundle along it." );
    m_paramAlgoSelections->addItem( "By Centerline", "Use the centerline and parameterize the bundle along it." );
    m_paramAlgoSelections->addItem( "By Integration", "Integrate along the voxelized line." );

    m_antialiased     = m_properties->addProperty( "Antialiasing", "Enable/Disable antialiased drawing of voxels.", true, m_fullUpdate );
    m_drawfibers      = m_properties->addProperty( "Fiber Tracts", "Enable/Disable drawing of the fibers of a cluster.", true, m_fullUpdate );
    m_drawBoundingBox = m_properties->addProperty( "Bounding Box Enable Feature", "Enable/Disable drawing of a clusters BoundingBox.", true );
    m_drawCenterLine  = m_properties->addProperty( "Center Line", "Enable/Disable display of the center line", true );
    m_lighting        = m_properties->addProperty( "Lighting", "Enable/Disable lighting.", true );
    m_drawVoxels      = m_properties->addProperty( "Display Voxels", "Enable/Disable drawing of marked voxels.", true, m_fullUpdate );
    m_rasterAlgo      = m_properties->addProperty( "Raster Algo", "Specifies the algorithm you may want to use for voxelization.",
                                                    std::string( "WBresenham" ), m_fullUpdate );
    m_voxelsPerUnit   = m_properties->addProperty( "Voxels per Unit", "Specified the number of voxels per unit in the coordinate system. This "
                                                                       "is useful to increase the resolution of the grid", 1, m_fullUpdate );
    m_parameterAlgo   = m_properties->addProperty( "Parameterization", "Select the parameterization algorithm.",
                                                   m_paramAlgoSelections->getSelectorFirst(), m_fullUpdate );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_parameterAlgo );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_parameterAlgo );
    m_fiberTransparency = m_properties->addProperty( "Fiber Transparency", "", 1.0, m_fullUpdate );
    m_fiberTransparency->setMin( 0.0 );
    m_fiberTransparency->setMax( 1.0 );
    m_explicitFiberColor = m_properties->addProperty( "Explicit Fiber Color", "", WColor( 0.2, 0.2, 0.2, 1.0 ), m_fullUpdate );

    WModule::properties();
}

void WMVoxelizer::activate()
{
    if( m_osgNode )
    {
        if( m_active->get() )
        {
            m_osgNode->setNodeMask( 0xFFFFFFFF );
        }
        else
        {
            m_osgNode->setNodeMask( 0x0 );
        }
    }

    WModule::activate();
}

osg::ref_ptr< osg::Geode > WMVoxelizer::genFiberGeode() const
{
    using osg::ref_ptr;
    ref_ptr< osg::Vec3Array > vertices = ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    ref_ptr< osg::Vec4Array > colors = ref_ptr< osg::Vec4Array >( new osg::Vec4Array );
    ref_ptr< osg::Geometry > geometry = ref_ptr< osg::Geometry >( new osg::Geometry );

    const WDataSetFiberVector& fibs = *m_clusters->getDataSetReference();

    const std::list< size_t >& fiberIDs = m_clusters->getIndices();
    std::list< size_t >::const_iterator cit = fiberIDs.begin();

    assert( fibs.size() > 0 && "no empty fiber dataset for clusters allowed in WMVoxelizer::createBoundingBox" );
    assert( fibs[0].size() > 0 && "no empty fibers in a cluster allowed in WMVoxelizer::createBoundingBox" );
    assert( fiberIDs.size() > 0 && "no empty clusters allowed in WMVoxelizer::createBoundingBox" );

    for( cit = fiberIDs.begin(); cit != fiberIDs.end(); ++cit )
    {
        const WFiber& fib = fibs[*cit];
        vertices->push_back( osg::Vec3( fib[0][0], fib[0][1], fib[0][2] ) );
        for( size_t i = 1; i < fib.size(); ++i )
        {
            vertices->push_back( osg::Vec3( fib[i][0], fib[i][1], fib[i][2] ) );
            WColor col = m_explicitFiberColor->get( true );
            if( m_explicitFiberColor->get() == WColor( 0.2, 0.2, 0.2, 1.0 ) )
            {
                col = wge::getRGBAColorFromDirection( fib[i], fib[i-1] );
            }
            col[3] = m_fiberTransparency->get( true );
            colors->push_back( col );
        }
        colors->push_back( colors->back() );
        geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, vertices->size() - fib.size(), fib.size() ) );
    }

    geometry->setVertexArray( vertices );
    geometry->setColorArray( colors );
    geometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );
    osg::ref_ptr< osg::Geode > geode = osg::ref_ptr< osg::Geode >( new osg::Geode );
    geode->addDrawable( geometry.get() );
    return geode;
}

boost::shared_ptr< WGridRegular3D > WMVoxelizer::constructGrid( const WBoundingBox& bb ) const
{
    int32_t nbVoxelsPerUnit = m_voxelsPerUnit->get( true );

    // TODO(math): implement the snap-to-grid (of the T1 image) feature for fll and bur.
    // TODO(math): remove hardcoded meta grid here.
    // the "+1" in the following three statements is because there are may be some more voxels
    // The first and last voxel are only half sized! hence one more position is needed
    size_t nbPosX = std::ceil( bb.xMax() - bb.xMin() ) + 1;
    size_t nbPosY = std::ceil( bb.yMax() - bb.yMin() ) + 1;
    size_t nbPosZ = std::ceil( bb.zMax() - bb.zMin() ) + 1;

    boost::shared_ptr< WGridRegular3D > grid( new WGridRegular3D( nbVoxelsPerUnit * nbPosX, nbVoxelsPerUnit * nbPosY, nbVoxelsPerUnit * nbPosZ,
                bb.getMin(), 1.0 / nbVoxelsPerUnit, 1.0 / nbVoxelsPerUnit, 1.0 / nbVoxelsPerUnit ) );
    return grid;
}

void WMVoxelizer::updateFibers()
{
    debugLog() << "Fiber Update";
    assert( m_osgNode );
    if( m_drawfibers->get( true ) )
    {
        m_osgNode->remove( m_fiberGeode );
        m_fiberGeode = genFiberGeode();
        m_osgNode->insert( m_fiberGeode );
    }
    else
    {
        m_osgNode->remove( m_fiberGeode );
    }
}

void WMVoxelizer::updateCenterLine()
{
    assert( m_osgNode );
    if( m_drawCenterLine->get( true ) )
    {
        boost::shared_ptr< WFiber > centerLine = m_clusters->getCenterLine();
        if( centerLine )
        {
            m_centerLineGeode = wge::generateLineStripGeode( *centerLine, 3.f );
        }
        else
        {
            warnLog() << "CenterLine update on non existing CenterLine (null)";
            m_centerLineGeode = osg::ref_ptr< osg::Geode >( new osg::Geode() );
        }
        m_osgNode->insert( m_centerLineGeode );
    }
    else
    {
        m_osgNode->remove( m_centerLineGeode );
    }
}

void WMVoxelizer::update()
{
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_osgNode );
    m_osgNode = osg::ref_ptr< WGEGroupNode >( new WGEGroupNode );
    m_osgNode->addUpdateCallback( new OSGCB_HideUnhideBB( this ) );
    m_osgNode->addUpdateCallback( new OSGCB_ChangeLighting( this ) );

    updateFibers();

    updateCenterLine();

    WBoundingBox bb = createBoundingBox( *m_clusters );

    m_boundingBoxGeode = wge::generateBoundingBoxGeode( bb, WColor( 0.3, 0.3, 0.3, 1 ) );
    m_osgNode->insert( m_boundingBoxGeode );

    boost::shared_ptr< WGridRegular3D > grid = constructGrid( bb );

    debugLog() << "Created grid of size: " << grid->size();

    boost::shared_ptr< WRasterAlgorithm > rasterAlgo;
    if( m_rasterAlgo->get() == std::string( "WBresenham" ) )
    {
        rasterAlgo = boost::shared_ptr< WBresenham >( new WBresenham( grid, m_antialiased->get() ) );
    }
    else if( m_rasterAlgo->get() == std::string( "WBresenhamDBL" ) )
    {
        rasterAlgo =  boost::shared_ptr< WBresenhamDBL >( new WBresenhamDBL( grid, m_antialiased->get() ) );
    }
    else
    {
        errorLog() << "Invalid rasterization algorithm: " << m_rasterAlgo->get();
        m_rasterAlgo->set( std::string( "WBresenham" ) );
        rasterAlgo = boost::shared_ptr< WBresenham >( new WBresenham( grid, m_antialiased->get() ) );
    }
    debugLog() << "Using: " << m_rasterAlgo->get() << " as rasterization Algo.";

    // decide which param algo to use:
    size_t algo = m_parameterAlgo->get( true ).getItemIndexOfSelected( 0 );
    boost::shared_ptr< WRasterParameterization > paramAlgo;
    if ( algo == 0 )
    {
        debugLog() << "No parameterization algorithm selected.";
    }
    else if ( algo == 1 )
    {
        debugLog() << "Parameterization algorithm: by longest line.";
        paramAlgo = boost::shared_ptr< WRasterParameterization >(
            new WCenterlineParameterization( grid, m_clusters->getLongestLine() )
        );
    }
    else if ( algo == 2 )
    {
        debugLog() << "Parameterization algorithm: by centerline.";
        paramAlgo = boost::shared_ptr< WRasterParameterization >(
            new WCenterlineParameterization( grid, m_clusters->getCenterLine() )
        );
    }
    else if ( algo == 3 )
    {
        debugLog() << "Parameterization algorithm: by integration.";
        paramAlgo = boost::shared_ptr< WRasterParameterization >(
            new WIntegrationParameterization( grid )
        );
    }
    if ( paramAlgo )
    {
        rasterAlgo->addParameterizationAlgorithm( paramAlgo );
    }

    raster( rasterAlgo );

    // update both outputs
    boost::shared_ptr< WDataSetScalar > outputDataSet = rasterAlgo->generateDataSet();
    m_output->updateData( outputDataSet );
    if ( paramAlgo )
    {
        boost::shared_ptr< WDataSetScalar > outputDataSetIntegration = paramAlgo->getDataSet();
        m_parameterizationOutput->updateData( outputDataSetIntegration );
    }

    if( m_drawVoxels->get() )
    {
        m_voxelGeode = genDataSetGeode( outputDataSet );
        m_osgNode->insert( m_voxelGeode );
    }

    m_osgNode->setNodeMask( m_active->get() ? 0xFFFFFFFF : 0x0 );
    m_osgNode->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_osgNode );
}

void WMVoxelizer::raster( boost::shared_ptr< WRasterAlgorithm > algo ) const
{
    const WDataSetFiberVector& fibs = *m_clusters->getDataSetReference();
    const std::list< size_t >& fiberIDs = m_clusters->getIndices();
    std::list< size_t >::const_iterator cit = fiberIDs.begin();

    debugLog() << "Cluster indices to voxelize: " << fiberIDs;
    debugLog() << "Using: " << m_clusters->getDataSetReference() << " as fiber dataset";

    assert( fibs.size() > 0 && "no empty fiber dataset for clusters allowed in WMVoxelizer::createBoundingBox" );
    assert( fibs[0].size() > 0 && "no empty fibers in a cluster allowed in WMVoxelizer::createBoundingBox" );
    assert( fiberIDs.size() > 0 && "no empty clusters allowed in WMVoxelizer::createBoundingBox" );

    for( cit = fiberIDs.begin(); cit != fiberIDs.end(); ++cit )
    {
        algo->raster( fibs.at( *cit ) );
    }
    algo->finished();

    // TODO(math): This is just a line for testing purposes
//    wmath::WLine l;
//    l.push_back( wmath::WPosition( 73, 38, 29 ) );
//    l.push_back( wmath::WPosition( 120, 150, 130 ) );
//    algo->raster( l );
}

void WMVoxelizer::connectors()
{
    m_input = WModuleInputData< const WFiberCluster >::createAndAdd( shared_from_this(), "tractInput", "A cluster of tracts" );
    m_output = WModuleOutputData< WDataSetScalar >::createAndAdd( shared_from_this(), "voxelOutput", "The voxelized data set" );
    m_parameterizationOutput = WModuleOutputData< WDataSetScalar >::createAndAdd( shared_from_this(),
                                                                                  "parameterizationOutput",
                                                                                  "The parameter field for the voxelized fibers." );
    WModule::connectors();  // call WModules initialization
}

WBoundingBox WMVoxelizer::createBoundingBox( const WFiberCluster& cluster ) const
{
    const WDataSetFiberVector& fibs = *cluster.getDataSetReference();

    const std::list< size_t >& fiberIDs = cluster.getIndices();
    std::list< size_t >::const_iterator cit = fiberIDs.begin();

    WAssert( fibs.size() > 0, "no empty fiber dataset for clusters allowed in WMVoxelizer::createBoundingBox" );
    WAssert( fibs[0].size() > 0, "no empty fibers in a cluster allowed in WMVoxelizer::createBoundingBox" );
    WAssert( fiberIDs.size() > 0, "no empty clusters allowed in WMVoxelizer::createBoundingBox" );

    WBoundingBox result;

    for( cit = fiberIDs.begin(); cit != fiberIDs.end(); ++cit )
    {
        const WFiber& fiber = fibs[ *cit ];
        for( size_t i = 0; i < fiber.size(); ++i )
        {
            result.expandBy( fiber[i] );
        }
    }

    return result;
}

osg::ref_ptr< osg::Geode > WMVoxelizer::genDataSetGeode( boost::shared_ptr< WDataSetScalar > dataset ) const
{
    using osg::ref_ptr;
    ref_ptr< osg::Vec3Array > vertices = ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    ref_ptr< osg::Vec4Array > colors = ref_ptr< osg::Vec4Array >( new osg::Vec4Array );
    ref_ptr< osg::Geometry > geometry = ref_ptr< osg::Geometry >( new osg::Geometry );
    ref_ptr< osg::Vec3Array > normals = ref_ptr< osg::Vec3Array >( new osg::Vec3Array );

    // cycle through all positions in the dataSet
    boost::shared_ptr< WValueSet< double > > valueset = boost::shared_dynamic_cast< WValueSet< double > >( dataset->getValueSet() );
    WAssert( valueset != 0, "No scalar double valueset was given while generating the dataset geode" );
    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( dataset->getGrid() );
    WAssert( grid != 0, "No WGridRegular3D was given while generating the dataset geode"  );
    const std::vector< double >& values = *valueset->rawDataVectorPointer();
    for( size_t i = 0; i < values.size(); ++i )
    {
        if( values[i] != 0.0 )
        {
            wmath::WPosition pos = grid->getPosition( i );
            boost::shared_ptr< std::vector< wmath::WPosition > > voxelCornerVertices = grid->getVoxelVertices( pos, 0.01 );
            osg::ref_ptr< osg::Vec3Array > ver = wge::generateCuboidQuads( *voxelCornerVertices );
            vertices->insert( vertices->end(), ver->begin(), ver->end() );
            osg::ref_ptr< osg::Vec3Array > nor = wge::generateCuboidQuadNormals( *voxelCornerVertices );
            normals->insert( normals->end(), nor->begin(), nor->end() );
            geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::QUADS, vertices->size() - ver->size(), ver->size() ) );
            for( size_t j = 0; j < ver->size(); ++j )
            {
                double transparency = ( values[i] <= 1.0 ? values[i] : 1.0 );
                colors->push_back( WColor( 1.0, 0.0, 0.0, transparency ) );
            }
        }
    }

    geometry->setVertexArray( vertices );
    colors->push_back( WColor( 1.0, 0.0, 0.0, 0.1 ) );
    geometry->setColorArray( colors );
    geometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );
    geometry->setNormalArray( normals );
    geometry->setNormalBinding( osg::Geometry::BIND_PER_PRIMITIVE );
    osg::ref_ptr< osg::Geode > geode = osg::ref_ptr< osg::Geode >( new osg::Geode );
    geode->addDrawable( geometry );

    osg::StateSet* state = geode->getOrCreateStateSet();

    // Enable blending, select transparent bin.
    state->setMode( GL_BLEND, osg::StateAttribute::ON );
    state->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

    // Enable depth test so that an opaque polygon will occlude a transparent one behind it.
    state->setMode( GL_DEPTH_TEST, osg::StateAttribute::ON );

    // Conversely, disable writing to depth buffer so that a transparent polygon will allow polygons behind it to shine through.
    // OSG renders transparent polygons after opaque ones.
    osg::Depth* depth = new osg::Depth;
    depth->setWriteMask( false );
    state->setAttributeAndModes( depth, osg::StateAttribute::ON );

    // disable light for this geode as lines can't be lit properly
    state->setMode( GL_LIGHTING, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );

    return geode;
}

void WMVoxelizer::OSGCB_HideUnhideBB::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    if( m_module->m_boundingBoxGeode )
    {
        if ( m_module->m_drawBoundingBox->get() )
        {
            m_module->m_boundingBoxGeode->setNodeMask( 0xFFFFFFFF );
        }
        else
        {
            m_module->m_boundingBoxGeode->setNodeMask( 0x0 );
        }
    }
    traverse( node, nv );
}

void WMVoxelizer::OSGCB_ChangeLighting::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    if ( m_module->m_lighting->get() )
    {
        m_module->m_osgNode->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::ON );
    }
    else
    {
        m_module->m_osgNode->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    }
    traverse( node, nv );
}
