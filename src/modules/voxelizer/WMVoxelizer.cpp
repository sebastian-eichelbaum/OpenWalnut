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
#include "WMVoxelizer.h"

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

//osg::ref_ptr< osg::Geode > WMVoxelizer::generateGrid() const
//{
//    using osg::ref_ptr;
//    ref_ptr< osg::Vec3Array > vertices = ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
//    ref_ptr< osg::Vec4Array > colors = ref_ptr< osg::Vec4Array >( new osg::Vec4Array );
//    ref_ptr< osg::Geometry > geometry = ref_ptr< osg::Geometry >( new osg::Geometry );
//
//    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( ( *m_dataSet ).getGrid() );
//    assert( grid.get() );
//    const wmath::WPosition& origin = grid->getOrigin();
//    double dx = grid->getOffsetX();
//    double dy = grid->getOffsetY();
//    double dz = grid->getOffsetZ();
//    double nbCoordsX = 8;
//    double nbCoordsY = 8;
//    double nbCoordsZ = 10;
//
//
//    for( size_t x = 0; x < nbCoordsX; ++x )
//    {
//        vertices->push_back( osg::Vec3( x * dx, 0, 0 ) );
//        vertices->push_back( osg::Vec3( x * dx, 0, nbCoordsZ * dz ) );
//        vertices->push_back( osg::Vec3( x * dx, nbCoordsY * dy, nbCoordsZ * dz ) );
//        vertices->push_back( osg::Vec3( x * dx, nbCoordsY * dy, 0 ) );
//        vertices->push_back( osg::Vec3( x * dx, 0, 0 ) );
//        geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, vertices->size() - 5, 5 ) );
//        for( size_t i = 0; i < nbCoordsZ; ++i )
//        {
//            vertices->push_back( osg::Vec3( x * dx, 0, i * dz ) );
//            vertices->push_back( osg::Vec3( x * dx, nbCoordsY * dy, i * dz ) );
//            geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINES, vertices->size() - 2, 2 ) );
//        }
//    }
//    for( size_t y = 0; y < nbCoordsY; ++y )
//    {
//        vertices->push_back( osg::Vec3(  0, y * dy, 0 ) );
//        vertices->push_back( osg::Vec3(  0, y * dy, nbCoordsZ * dz ) );
//        vertices->push_back( osg::Vec3(  nbCoordsX * dx, y * dy, nbCoordsZ * dz ) );
//        vertices->push_back( osg::Vec3(  nbCoordsX * dx, y * dy, 0 ) );
//        vertices->push_back( osg::Vec3(  0, y * dy, 0 ) );
//        geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, vertices->size() -5 , 5 ) );
//        for( size_t i = 0; i < nbCoordsZ; ++i )
//        {
//            vertices->push_back( osg::Vec3( 0 , y * dy, i * dz ) );
//            vertices->push_back( osg::Vec3( nbCoordsX * dx, y * dy, i * dz ) );
//            geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINES, vertices->size() - 2, 2 ) );
//        }
//    }
//    for( size_t z = 0; z < nbCoordsZ; ++z )
//    {
//        vertices->push_back( osg::Vec3(  0, 0, z * dz ) );
//        vertices->push_back( osg::Vec3(  nbCoordsX * dx, 0, z * dz ) );
//        vertices->push_back( osg::Vec3(  nbCoordsX * dx, nbCoordsY * dy, z * dz ) );
//        vertices->push_back( osg::Vec3(  0, nbCoordsY * dy, z * dz ) );
//        vertices->push_back( osg::Vec3(  0, 0, z * dz ) );
//        geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, vertices->size() - 5, 5 ) );
//    }
//    for( size_t x = 0; x < nbCoordsX; ++x )
//    {
//        for( size_t y = 0; y < nbCoordsY; ++y )
//        {
//            vertices->push_back( osg::Vec3( x * dx, y * dy, 0 ) );
//            vertices->push_back( osg::Vec3( x * dx, y * dy, nbCoordsZ * dz ) );
//            geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINES, vertices->size() - 2, 2 ) );
//        }
//    }
//    // draw quad
//    vertices->push_back( osg::Vec3( , , ) );
//    geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINES, vertices->size() - 2, 2 ) );
//
//  geometry->setVertexArray( vertices );
//  colors->push_back( osg::Vec4( 0.3, 0.3, 0.3, 0 ) );
//  geometry->setColorArray( colors );
//  geometry->setColorBinding( osg::Geometry::BIND_OVERALL );
//  osg::ref_ptr< osg::Geode > geode = osg::ref_ptr< osg::Geode >( new osg::Geode );
//  geode->addDrawable( geometry.get() );
//  return geode;
//}
////void WMVoxelizer::addCuboid( ref_ptr< osg::Geometry > geometry,
////                             const WPosition& lowerLeft,
////                             const WPosition& upperRight,
////                             const WColor& color,
////                             double transparency
////                           ) const
////{
////    using osg::ref_ptr;
////    ref_ptr< osg::Vec3Array > vertices = geometry->getVertexArray();
////    ref_ptr< osg::Vec4Array > colors = geometry->getColorArray();
////    geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::QUAD_STRIP, vertices->size() - 2, 2 ) );
////}

void WMVoxelizer::moduleMain()
{
    // additional fire-condition: "data changed" flag
    m_moduleState.add( m_input->getDataChangedCondition() );

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

osg::ref_ptr< osg::Geode > WMVoxelizer::genFiberGeode( boost::shared_ptr< const WDataSetFibers > fibers ) const
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

    geometry->setVertexArray( vertices );
    geometry->setColorArray( colors );
    geometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );
    osg::ref_ptr< osg::Geode > geode = osg::ref_ptr< osg::Geode >( new osg::Geode );
    geode->addDrawable( geometry.get() );
    return geode;
}

void WMVoxelizer::update()
{
    // remove nodes if they are any
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->removeChild( m_osgNode.get() );

    // create new node
    m_osgNode = osg::ref_ptr< osg::Group >( new osg::Group );
    m_osgNode->addChild( genFiberGeode( m_clusters->getDataSetReference() ) );
    std::pair< wmath::WPosition, wmath::WPosition > x = createBoundingBox( *m_clusters );
    boost::shared_ptr< WDataSetSingle > ds = createDataSet( x.first, x.second );
    m_osgNode->addChild( genDataSetGeode( ds ) );
    m_osgNode->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    m_osgNode->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->addChild( m_osgNode );
}

void WMVoxelizer::connectors()
{
    using boost::shared_ptr;
    typedef WModuleInputData< const WFiberCluster > InputData;  // just an alias

    m_input = shared_ptr< InputData >( new InputData( shared_from_this(), "voxelInput", "A loaded dataset with grid." ) );

    addConnector( m_input );
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

boost::shared_ptr< WDataSetSingle > WMVoxelizer::createDataSet( const wmath::WPosition& fll,
                                                                const wmath::WPosition& bur ) const
{
    boost::shared_ptr< WDataSetSingle > result;
    boost::shared_ptr< WGridRegular3D > grid;

    // TODO(math): implement the snap-to-grid (of the T1 image) feature for fll and bur.

    // TODO(math): remove hardcoded meta grid here.
    size_t nbPosX = bur[0] - fll[0] + 1; // round to next integer
    size_t nbPosY = bur[1] - fll[1] + 1; // -"-
    size_t nbPosZ = bur[2] - fll[2] + 1; // -"-
    const wmath::WPosition& origin = fll;

    // TODO(math): code a convinient constructor for WGridRegular3D allowing alos WPositions
    // please take care of tests.
    grid = boost::shared_ptr< WGridRegular3D >( new WGridRegular3D( nbPosX, nbPosY, nbPosZ, origin[0], origin[1], origin[2], 1, 1, 1 ) );

    boost::shared_ptr< WValueSet< double > > values;
    std::vector< double > valueData( nbPosX * nbPosY * nbPosZ, 0.0 );
    values = boost::shared_ptr< WValueSet< double > >( new WValueSet< double >( 0, 1, valueData, W_DT_DOUBLE ) );
    result = boost::shared_ptr< WDataSetSingle >( new WDataSetSingle( values, grid ) );
    return result;
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
            // TODO(math): enahnce WGridRegular3D so, it may return the vertices the
            //             voxel which holds the position of the i'th value
            //             This is because not every grid has cubes as voxels but cuboids
            ref_ptr< osg::Vec3Array > ver = generateCuboidVertices( grid->getPosition( i ), 1 );
            vertices->insert( vertices->end(), ver->begin(), ver->end() );
            geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::QUADS,
                                                            vertices->size() - ver->size(),
                                                            ver->size() ) );
            for( size_t i = 0; i < ver->size(); ++i )
            {
                colors->push_back( wge::osgColor( WColor( 1, 0, 0, values[i] ) ) );
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

osg::ref_ptr< osg::Vec3Array > WMVoxelizer::generateCuboidVertices( const wmath::WPosition& center, double margin ) const
{
    using osg::ref_ptr;
    ref_ptr< osg::Vec3Array > vertices = ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    double halfMargin = margin / 2.0;

    // Vertices
    //     h_____g
    //    /:    /|
    //   d_:___c |
    //   | :...|.|
    //   |.e   | f
    //   |_____|/
    //  a      b
    //
    osg::Vec3 a( center[0] - halfMargin, center[1] - halfMargin, center[2] - halfMargin );
    osg::Vec3 b( center[0] + halfMargin, center[1] - halfMargin, center[2] - halfMargin );
    osg::Vec3 c( center[0] + halfMargin, center[1] - halfMargin, center[2] + halfMargin );
    osg::Vec3 d( center[0] - halfMargin, center[1] - halfMargin, center[2] + halfMargin );
    osg::Vec3 e( center[0] - halfMargin, center[1] + halfMargin, center[2] - halfMargin );
    osg::Vec3 f( center[0] + halfMargin, center[1] + halfMargin, center[2] - halfMargin );
    osg::Vec3 g( center[0] + halfMargin, center[1] + halfMargin, center[2] + halfMargin );
    osg::Vec3 h( center[0] - halfMargin, center[1] + halfMargin, center[2] + halfMargin );

    // Surfaces
    vertices->push_back( a );
    vertices->push_back( b );
    vertices->push_back( c );
    vertices->push_back( d );

    vertices->push_back( b );
    vertices->push_back( f );
    vertices->push_back( g );
    vertices->push_back( c );

    vertices->push_back( f );
    vertices->push_back( e );
    vertices->push_back( h );
    vertices->push_back( g );

    vertices->push_back( e );
    vertices->push_back( a );
    vertices->push_back( d );
    vertices->push_back( h );

    vertices->push_back( d );
    vertices->push_back( c );
    vertices->push_back( g );
    vertices->push_back( h );

    vertices->push_back( a );
    vertices->push_back( b );
    vertices->push_back( f );
    vertices->push_back( e );
    return vertices;
}
