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
#include "../../dataHandler/WDataSetFibers.h"
#include "../../dataHandler/WSubject.h"
#include "../../graphicsEngine/WGraphicsEngine.h"
#include "../../kernel/WKernel.h"
#include "../../math/WFiber.h"
#include "../../utils/WColorUtils.h"
#include "WBresenhamDBL.h"
#include "WMVoxelizer.h"
#include "WRasterAlgorithm.h"

WMVoxelizer::WMVoxelizer()
    : WModule()
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
        WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->removeChild( m_osgNode.get() );
    }
}

osg::ref_ptr< osg::Geode > WMVoxelizer::genFiberGeode() const
{
    using osg::ref_ptr;
    ref_ptr< osg::Vec3Array > vertices = ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    ref_ptr< osg::Vec4Array > colors = ref_ptr< osg::Vec4Array >( new osg::Vec4Array );
    ref_ptr< osg::Geometry > geometry = ref_ptr< osg::Geometry >( new osg::Geometry );

    const WDataSetFibers& fibs = *m_clusters->getDataSetReference();

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
            colors->push_back( wge::osgColor( color_utils::getRGBAColorFromDirection( fib[i], fib[i-1] ) ) );
        }
        colors->push_back( colors->back() );
        geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, vertices->size() - fib.size(), fib.size() ) );
    }
    // TODO(math): This is just a line for testing purposes
    // vertices->push_back( osg::Vec3( 73, 38, 29 ) );
    // vertices->push_back( osg::Vec3( 120, 150, 130 ) );
    // colors->push_back( colors->back() );
    // colors->push_back( colors->back() );
    // geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, vertices->size() - 2, 2 ) );

    geometry->setVertexArray( vertices );
    geometry->setColorArray( colors );
    geometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );
    osg::ref_ptr< osg::Geode > geode = osg::ref_ptr< osg::Geode >( new osg::Geode );
    geode->addDrawable( geometry.get() );
    return geode;
}

void WMVoxelizer::update()
{
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->removeChild( m_osgNode.get() );

    m_osgNode = osg::ref_ptr< osg::Group >( new osg::Group );
    m_osgNode->addChild( genFiberGeode() );
    std::pair< wmath::WPosition, wmath::WPosition > bb = createBoundingBox( *m_clusters );

    boost::shared_ptr< WGridRegular3D > grid;

    // TODO(math): implement the snap-to-grid (of the T1 image) feature for fll and bur.
    // TODO(math): remove hardcoded meta grid here.
    size_t nbPosX = std::ceil( bb.second[0] - bb.first[0] ) + 1;
    size_t nbPosY = std::ceil( bb.second[1] - bb.first[1] ) + 1;
    size_t nbPosZ = std::ceil( bb.second[2] - bb.first[2] ) + 1;
    debugLog() << "Bounding Box: " << bb.first << " " << bb.second;

    grid = boost::shared_ptr< WGridRegular3D >( new WGridRegular3D( nbPosX, nbPosY, nbPosZ, bb.first, 1, 1, 1 ) );
    debugLog() << "Created grid of size: " << grid->size();
    boost::shared_ptr< WBresenhamDBL > bresenham = boost::shared_ptr< WBresenhamDBL >( new WBresenhamDBL( grid ) );
    raster( bresenham );
    boost::shared_ptr< WDataSetSingle > outputDataSet = bresenham->generateDataSet();
    m_output->updateData( outputDataSet );
    m_osgNode->addChild( genDataSetGeode( outputDataSet ) );
    m_osgNode->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    m_osgNode->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->addChild( m_osgNode );
}

void WMVoxelizer::raster( boost::shared_ptr< WBresenhamDBL > algo ) const
{
    const WDataSetFibers& fibs = *m_clusters->getDataSetReference();
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
    // std::vector< wmath::WPosition > lineData;
    // lineData.push_back( wmath::WPosition( 73, 38, 29 ) );
    // lineData.push_back( wmath::WPosition( 120, 150, 130 ) );
    // wmath::WLine l( lineData );
    // algo->raster( l );
}

void WMVoxelizer::connectors()
{
    using boost::shared_ptr;
    typedef WModuleInputData< const WFiberCluster > InputData;  // just an alias

    m_input = shared_ptr< InputData >( new InputData( shared_from_this(), "voxelInput", "A loaded dataset with grid." ) );

    addConnector( m_input );

    m_output = boost::shared_ptr< WModuleOutputData< WDataSetSingle > >( new WModuleOutputData< WDataSetSingle >(
                shared_from_this(), "out", "The voxelized data set." )
            );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_output );

    WModule::connectors();  // call WModules initialization
}

void WMVoxelizer::properties()
{
    m_properties->addString( getName(), getDescription() );
    // this bool is hidden
    m_properties->addBool( "active", true, true )->connect( boost::bind( &WMVoxelizer::slotPropertyChanged, this, _1 ) );
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
    else
    {
        // instead of WLogger we must use std::cerr since WLogger needs to much time!
        std::cerr << propertyName << std::endl;
        assert( 0 && "This property name is not supported by this function yet." );
    }
}

std::pair< wmath::WPosition, wmath::WPosition > WMVoxelizer::createBoundingBox( const WFiberCluster& cluster ) const
{
    const WDataSetFibers& fibs = *cluster.getDataSetReference();

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
                fll[x] = std::min( fiber[i][x], fll[x] );
                bur[x] = std::max( fiber[i][x], bur[x] );
            }
        }
    }
    m_osgNode->addChild( genBBGeode( fll, bur ) );
    return std::make_pair( fll, bur );
}

osg::ref_ptr< osg::Geode > WMVoxelizer::genBBGeode( const wmath::WPosition& fll,
                                                    const wmath::WPosition& bur ) const
{
    using osg::ref_ptr;
    ref_ptr< osg::Vec3Array > vertices = ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    ref_ptr< osg::Vec4Array > colors = ref_ptr< osg::Vec4Array >( new osg::Vec4Array );
    ref_ptr< osg::Geometry > geometry = ref_ptr< osg::Geometry >( new osg::Geometry );

    vertices->push_back( osg::Vec3( fll[0], fll[1], fll[2] ) );
    vertices->push_back( osg::Vec3( bur[0], fll[1], fll[2] ) );
    vertices->push_back( osg::Vec3( bur[0], bur[1], fll[2] ) );
    vertices->push_back( osg::Vec3( fll[0], bur[1], fll[2] ) );
    vertices->push_back( osg::Vec3( fll[0], fll[1], fll[2] ) );
    vertices->push_back( osg::Vec3( fll[0], fll[1], bur[2] ) );
    vertices->push_back( osg::Vec3( bur[0], fll[1], bur[2] ) );
    vertices->push_back( osg::Vec3( bur[0], bur[1], bur[2] ) );
    vertices->push_back( osg::Vec3( fll[0], bur[1], bur[2] ) );
    vertices->push_back( osg::Vec3( fll[0], fll[1], bur[2] ) );

    geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, 0, vertices->size() ) );

    vertices->push_back( osg::Vec3( fll[0], bur[1], fll[2] ) );
    vertices->push_back( osg::Vec3( fll[0], bur[1], bur[2] ) );
    vertices->push_back( osg::Vec3( bur[0], bur[1], fll[2] ) );
    vertices->push_back( osg::Vec3( bur[0], bur[1], bur[2] ) );
    vertices->push_back( osg::Vec3( bur[0], fll[1], fll[2] ) );
    vertices->push_back( osg::Vec3( bur[0], fll[1], bur[2] ) );

    geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINES, vertices->size() - 6, 6 ) );

    geometry->setVertexArray( vertices );
    colors->push_back( wge::osgColor( WColor( 0.3, 0.3, 0.3, 1 ) ) );
    geometry->setColorArray( colors );
    geometry->setColorBinding( osg::Geometry::BIND_OVERALL );
    osg::ref_ptr< osg::Geode > geode = osg::ref_ptr< osg::Geode >( new osg::Geode );
    geode->addDrawable( geometry );
    return geode;
}

osg::ref_ptr< osg::Geode > WMVoxelizer::genDataSetGeode( boost::shared_ptr< WDataSetSingle > dataset ) const
{
    using osg::ref_ptr;
    ref_ptr< osg::Vec3Array > vertices = ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    ref_ptr< osg::Vec4Array > colors = ref_ptr< osg::Vec4Array >( new osg::Vec4Array );
    ref_ptr< osg::Geometry > geometry = ref_ptr< osg::Geometry >( new osg::Geometry );

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
            boost::shared_ptr< std::vector< wmath::WPosition > > voxelCornerVertices = grid->getVoxelVertices( pos );
            osg::ref_ptr< osg::Vec3Array > ver = wge::generateCuboidQuads( *voxelCornerVertices );
            vertices->insert( vertices->end(), ver->begin(), ver->end() );
            geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::QUADS, vertices->size() - ver->size(), ver->size() ) );
            for( size_t j = 0; j < ver->size(); ++j )
            {
                colors->push_back( wge::osgColor( WColor( 0.8 * values[i], 0, 0, 0.8 * values[i] ) ) );
            }
        }
    }

    geometry->setVertexArray( vertices );
    colors->push_back( wge::osgColor( WColor( 1, 0, 0, 0.1 ) ) );
    geometry->setColorArray( colors );
    geometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );
    osg::ref_ptr< osg::Geode > geode = osg::ref_ptr< osg::Geode >( new osg::Geode );
    geode->addDrawable( geometry );
    return geode;
}
