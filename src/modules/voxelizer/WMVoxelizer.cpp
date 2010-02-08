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
#include "../../dataHandler/WDataSetFiberVector.h"
#include "../../dataHandler/WSubject.h"
#include "../../graphicsEngine/WGEUtils.h"
#include "../../graphicsEngine/WGEGeodeUtils.h"
#include "../../graphicsEngine/WGEGeometryUtils.h"
#include "../../kernel/WKernel.h"
#include "../../math/WFiber.h"
#include "../../modules/fiberDisplay/WMFiberDisplay.h"
#include "WBresenham.h"
#include "WBresenhamDBL.h"
#include "WMVoxelizer.h"
#include "WRasterAlgorithm.h"

WMVoxelizer::WMVoxelizer()
    : WModule(),
      m_antialiased( true ),
      m_drawfibers( true ),
      m_drawBoundingBox( true ),
      m_lighting( true ),
      m_drawVoxels( true ),
      m_rasterAlgo( "WBresenham" )
{
}

WMVoxelizer::~WMVoxelizer()
{
}

boost::shared_ptr< WModule > WMVoxelizer::factory() const
{
    return boost::shared_ptr< WModule >( new WMVoxelizer() );
}

void WMVoxelizer::moduleMain()
{
    m_moduleState.add( m_input->getDataChangedCondition() );  // additional fire-condition: "data changed" flag

    ready();

    while ( !m_shutdownFlag() ) // loop until the module container requests the module to quit
    {
        m_clusters = m_input->getData();
        if ( !( m_clusters.get() ) ) // ok, the output has not yet sent data
        {
            m_moduleState.wait();
            continue;
        }

        update();

        m_moduleState.wait(); // waits for firing of m_moduleState ( dataChanged, shutdown, etc. )

        // May be called twice
        WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_osgNode.get() );
    }
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
        const wmath::WFiber& fib = fibs[*cit];
        vertices->push_back( osg::Vec3( fib[0][0], fib[0][1], fib[0][2] ) );
        for( size_t i = 1; i < fib.size(); ++i )
        {
            vertices->push_back( osg::Vec3( fib[i][0], fib[i][1], fib[i][2] ) );
            colors->push_back( wge::osgColor( wge::getRGBAColorFromDirection( fib[i], fib[i-1] ) ) );
        }
        colors->push_back( colors->back() );
        geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, vertices->size() - fib.size(), fib.size() ) );
    }
    // TODO(math): This is just a line for testing purposes
//    vertices->push_back( osg::Vec3( 73, 38, 29 ) );
//    vertices->push_back( osg::Vec3( 120, 150, 130 ) );
//    colors->push_back( colors->back() );
//    colors->push_back( colors->back() );
//    geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, vertices->size() - 2, 2 ) );
//    vertices->push_back( osg::Vec3( 7.2766304016113281e+01, 3.7974670410156250e+01, 2.9449142456054688e+01 ) );
//    vertices->push_back( osg::Vec3( 1.1976630401611328e+02, 1.4997467041015625e+02, 1.3044914245605469e+02 ) );
//    colors->push_back( osg::Vec4( 1, 1, 1, 1 ) );
//    colors->push_back( osg::Vec4( 1, 1, 1, 1 ) );
//    geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, vertices->size() - 2, 2 ) );

    geometry->setVertexArray( vertices );
    geometry->setColorArray( colors );
    geometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );
    osg::ref_ptr< osg::Geode > geode = osg::ref_ptr< osg::Geode >( new osg::Geode );
    geode->addDrawable( geometry.get() );
    return geode;
}

boost::shared_ptr< WGridRegular3D > WMVoxelizer::constructGrid( const std::pair< wmath::WPosition, wmath::WPosition >& bb ) const
{
    boost::shared_ptr< WGridRegular3D > grid;

    // TODO(math): implement the snap-to-grid (of the T1 image) feature for fll and bur.
    // TODO(math): remove hardcoded meta grid here.
    // the "+1" in the following three statements is because there are may be some more voxels
    // The first and last voxel are only half sized! hence one more position is needed
    size_t nbPosX = std::ceil( bb.second[0] - bb.first[0] ) + 1;
    size_t nbPosY = std::ceil( bb.second[1] - bb.first[1] ) + 1;
    size_t nbPosZ = std::ceil( bb.second[2] - bb.first[2] ) + 1;

    // TODO(math): implement: enlarge grid so antializing is possible. This depends on how
    // many voxels you use for antialiasing
    grid = boost::shared_ptr< WGridRegular3D >( new WGridRegular3D( nbPosX, nbPosY, nbPosZ, bb.first, 1, 1, 1 ) );
    return grid;
}

void WMVoxelizer::update()
{
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_osgNode );
    m_osgNode = osg::ref_ptr< WGEGroupNode >( new WGEGroupNode );

    if( m_drawfibers )
    {
        m_osgNode->insert( genFiberGeode() );
    }
    std::pair< wmath::WPosition, wmath::WPosition > bb = createBoundingBox( *m_clusters );
    if( m_drawBoundingBox )
    {
        m_osgNode->insert( wge::generateBoundingBoxGeode( bb.first, bb.second, WColor( 0.3, 0.3, 0.3, 1 ) ) );
    }

    boost::shared_ptr< WGridRegular3D > grid = constructGrid( bb );

    debugLog() << "Created grid of size: " << grid->size();

    boost::shared_ptr< WRasterAlgorithm > rasterAlgo;
    if( m_rasterAlgo == "WBresenham" )
    {
        rasterAlgo = boost::shared_ptr< WBresenham >( new WBresenham( grid, m_antialiased ) );
    }
    else if( m_rasterAlgo == "WBresenhamDBL" )
    {
        rasterAlgo =  boost::shared_ptr< WBresenhamDBL >( new WBresenhamDBL( grid, m_antialiased ) );
    }
    else
    {
        errorLog() << "Invalid rasterization algorithm: " << m_rasterAlgo;
        m_rasterAlgo = "WBresenham";
        rasterAlgo = boost::shared_ptr< WBresenham >( new WBresenham( grid, m_antialiased ) );
    }
    debugLog() << "Using: " << m_rasterAlgo << " as rasterization Algo.";
    raster( rasterAlgo );

    boost::shared_ptr< WDataSetSingle > outputDataSet = rasterAlgo->generateDataSet();
    m_output->updateData( outputDataSet );
    if( m_drawVoxels )
    {
        m_osgNode->insert( genDataSetGeode( outputDataSet ) );
    }
    if( m_lighting )
    {
        m_osgNode->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::ON );
    }
    else
    {
        m_osgNode->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    }
    m_osgNode->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_osgNode );
}

void WMVoxelizer::raster( boost::shared_ptr< WRasterAlgorithm > algo ) const
{
    const WDataSetFiberVector& fibs = *m_clusters->getDataSetReference();
    const std::list< size_t >& fiberIDs = m_clusters->getIndices();
    std::list< size_t >::const_iterator cit = fiberIDs.begin();

    assert( fibs.size() > 0 && "no empty fiber dataset for clusters allowed in WMVoxelizer::createBoundingBox" );
    assert( fibs[0].size() > 0 && "no empty fibers in a cluster allowed in WMVoxelizer::createBoundingBox" );
    assert( fiberIDs.size() > 0 && "no empty clusters allowed in WMVoxelizer::createBoundingBox" );

    for( cit = fiberIDs.begin(); cit != fiberIDs.end(); ++cit )
    {
        algo->raster( fibs[*cit] );
    }
    // TODO(math): This is just a line for testing purposes
//    wmath::WLine l;
//    l.push_back( wmath::WPosition( 73, 38, 29 ) );
//    l.push_back( wmath::WPosition( 120, 150, 130 ) );
//    algo->raster( l );
}

void WMVoxelizer::connectors()
{
    using boost::shared_ptr;

    typedef WModuleInputData< const WFiberCluster > InputConnectorType; // just an alias
    m_input = shared_ptr< InputConnectorType >( new InputConnectorType( shared_from_this(), "voxelInput", "A loaded dataset with grid." ) );
    addConnector( m_input );

    typedef WModuleOutputData< WDataSetSingle > OutputConnectorType; // just an alias
    m_output = shared_ptr< OutputConnectorType >( new OutputConnectorType( shared_from_this(), "out", "The voxelized data set." ) );
    addConnector( m_output );

    WModule::connectors();  // call WModules initialization
}

void WMVoxelizer::properties()
{
    m_properties->addBool( "active", true, true )->connect( boost::bind( &WMVoxelizer::slotPropertyChanged, this, _1 ) );
    m_properties->addBool( "antialiased", m_antialiased, false )->connect( boost::bind( &WMVoxelizer::slotPropertyChanged, this, _1 ) );
    m_properties->addBool( "drawfibers", m_drawfibers, false )->connect( boost::bind( &WMVoxelizer::slotPropertyChanged, this, _1 ) );
    m_properties->addBool( "drawBoundingBox", m_drawBoundingBox, false )->connect( boost::bind( &WMVoxelizer::slotPropertyChanged, this, _1 ) );
    m_properties->addBool( "lighting", m_lighting, false )->connect( boost::bind( &WMVoxelizer::slotPropertyChanged, this, _1 ) );
    m_properties->addString( "rasterAlgo", m_rasterAlgo, false )->connect( boost::bind( &WMVoxelizer::slotPropertyChanged, this, _1 ) );
    m_properties->addBool( "drawVoxels", m_drawVoxels, false )->connect( boost::bind( &WMVoxelizer::slotPropertyChanged, this, _1 ) );
}

void WMVoxelizer::slotPropertyChanged( std::string propertyName )
{
    if( propertyName == "active" )
    {
        if ( m_properties->getValue< bool >( propertyName ) )
        {
            m_osgNode->setNodeMask( 0xFFFFFFFF );
        }
        else
        {
            m_osgNode->setNodeMask( 0x0 );
        }
    }
    else if( propertyName == "antialiased" )
    {
        m_antialiased = m_properties->getValue< bool >( propertyName );
        update();
    }
    else if( propertyName == "drawfibers" )
    {
        m_drawfibers = m_properties->getValue< bool >( propertyName );
        update();
    }
    else if( propertyName == "drawBoundingBox" )
    {
        m_drawBoundingBox = m_properties->getValue< bool >( propertyName );
        update();
    }
    else if( propertyName == "lighting" )
    {
        m_lighting = m_properties->getValue< bool >( propertyName );
        if( m_lighting )
        {
            m_osgNode->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::ON );
        }
        else
        {
            m_osgNode->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
        }
    }
    else if( propertyName == "rasterAlgo" )
    {
        m_rasterAlgo = m_properties->getValue< std::string >( propertyName );
        update();
    }
    else if( propertyName == "drawVoxels" )
    {
        m_drawVoxels = m_properties->getValue< bool >( propertyName );
        update();
    }
    else
    {
        // instead of WLogger we must use std::cerr since WLogger needs to much time!
        std::cerr << propertyName << std::endl;
        assert( 0 && "This property name is not supported by this function yet." );
    }
}

std::pair< wmath::WPosition, wmath::WPosition > WMVoxelizer::createBoundingBox( const WFiberCluster& cluster ) const
{
    const WDataSetFiberVector& fibs = *cluster.getDataSetReference();

    const std::list< size_t >& fiberIDs = cluster.getIndices();
    std::list< size_t >::const_iterator cit = fiberIDs.begin();

    assert( fibs.size() > 0 && "no empty fiber dataset for clusters allowed in WMVoxelizer::createBoundingBox" );
    assert( fibs[0].size() > 0 && "no empty fibers in a cluster allowed in WMVoxelizer::createBoundingBox" );
    assert( fiberIDs.size() > 0 && "no empty clusters allowed in WMVoxelizer::createBoundingBox" );

    wmath::WPosition fll = fibs[0][0]; // front lower left corner ( initialize with first WPosition of first fiber )
    wmath::WPosition bur = fibs[0][0]; // back upper right corner ( initialize with first WPosition of first fiber )
    for( cit = fiberIDs.begin(); cit != fiberIDs.end(); ++cit )
    {
        const wmath::WFiber& fiber = fibs[*cit];
        for( size_t i = 0; i < fiber.size(); ++i )
        {
            for( int x = 0; x < 3; ++x )
            {
#ifdef _WIN32
                fll[x] = min( fiber[i][x], fll[x] );
                bur[x] = max( fiber[i][x], bur[x] );
#else
                fll[x] = std::min( fiber[i][x], fll[x] );
                bur[x] = std::max( fiber[i][x], bur[x] );
#endif
            }
        }
    }
    return std::make_pair( fll, bur );
}

osg::ref_ptr< osg::Geode > WMVoxelizer::genDataSetGeode( boost::shared_ptr< WDataSetSingle > dataset ) const
{
    using osg::ref_ptr;
    ref_ptr< osg::Vec3Array > vertices = ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    ref_ptr< osg::Vec4Array > colors = ref_ptr< osg::Vec4Array >( new osg::Vec4Array );
    ref_ptr< osg::Geometry > geometry = ref_ptr< osg::Geometry >( new osg::Geometry );
    ref_ptr< osg::Vec3Array > normals = ref_ptr< osg::Vec3Array >( new osg::Vec3Array );

    // cycle through all positions in the dataSet
    boost::shared_ptr< WValueSet< double > > valueset = boost::shared_dynamic_cast< WValueSet< double > >( dataset->getValueSet() );
    assert( valueset != 0 );
    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( dataset->getGrid() );
    assert( grid != 0 );
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
                colors->push_back( wge::osgColor( WColor( 1, 0, 0, values[i] ) ) );
            }
        }
    }

    geometry->setVertexArray( vertices );
    colors->push_back( wge::osgColor( WColor( 1, 0, 0, 0.1 ) ) );
    geometry->setColorArray( colors );
    geometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );
    geometry->setNormalArray( normals );
    geometry->setNormalBinding( osg::Geometry::BIND_PER_PRIMITIVE );
    osg::ref_ptr< osg::Geode > geode = osg::ref_ptr< osg::Geode >( new osg::Geode );
    geode->addDrawable( geometry );
    return geode;
}
