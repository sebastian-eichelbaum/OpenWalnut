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

#include <string>

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
//        m_dataSet = m_input->getData();
//        if ( !( m_dataSet.get() ) ) // ok, the output has not yet sent data
//        {
//            m_moduleState.wait();
//            continue;
//        }

        update();

        m_moduleState.wait(); // waits for firing of m_moduleState ( dataChanged, shutdown, etc. )

        // May be called twice
        WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->removeChild( m_osgNode.get() );
    }
}

void WMVoxelizer::update()
{
    // remove nodes if they are any
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->removeChild( m_osgNode.get() );

    // create new node
    m_osgNode = osg::ref_ptr< osg::Group >( new osg::Group );
//    m_osgNode->addChild( generateGrid().get() );
    m_osgNode->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->addChild( m_osgNode.get() );
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
