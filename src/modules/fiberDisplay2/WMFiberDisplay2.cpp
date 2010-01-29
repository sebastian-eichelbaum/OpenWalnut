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
#include "../../graphicsEngine/WGraphicsEngine.h"
#include "../../graphicsEngine/WROIBox.h"
#include "../../kernel/WKernel.h"

#include "WMFiberDisplay2.h"

WMFiberDisplay2::WMFiberDisplay2()
    : WModule()
{
}

WMFiberDisplay2::~WMFiberDisplay2()
{
}

boost::shared_ptr< WModule > WMFiberDisplay2::factory() const
{
    return boost::shared_ptr< WModule >( new WMFiberDisplay2() );
}

osg::ref_ptr< osg::Geode > WMFiberDisplay2::genFiberGeode()
{
    using osg::ref_ptr;
    ref_ptr< osg::Vec3Array > vertices = ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    m_globalColors = ref_ptr< osg::Vec4Array >( new osg::Vec4Array );
    m_localColors = ref_ptr< osg::Vec4Array >( new osg::Vec4Array );
    m_geometry = ref_ptr< osg::Geometry >( new osg::Geometry );

    vertices->reserve( m_dataset->getVertices()->size() );
    m_globalColors->reserve( m_dataset->getVertices()->size() );
    m_localColors->reserve( m_dataset->getVertices()->size() );

    boost::shared_ptr< std::vector< unsigned int > > startIndexes = m_dataset->getLineStartIndexes();
    boost::shared_ptr< std::vector< unsigned int > > pointsPerLine = m_dataset->getLineLengths();
    boost::shared_ptr< std::vector< float > > verts = m_dataset->getVertices();

    size_t vertexNum = 0;
    WColor gc;
    WColor lc;

    boost::shared_ptr< WProgress > progress = boost::shared_ptr< WProgress >( new WProgress( "Fiber Display", m_dataset->size() ) );
    m_progress->addSubProgress( progress );

    for( size_t j = 0; j < m_dataset->size(); ++j )
    {
        vertices->push_back( osg::Vec3( verts->at( 3 * vertexNum ), verts->at( 3 * vertexNum + 1 ), verts->at( 3 * vertexNum + 2 ) ) );
        ++vertexNum;

        gc = getRGBAColorFromDirection( m_dataset->getPosition( j, 0 ), m_dataset->getPosition( j,  pointsPerLine->at( j ) - 1 ) );

        for( size_t i = 1; i < pointsPerLine->at( j ); ++i )
        {
            vertices->push_back( osg::Vec3( verts->at( 3 * vertexNum ), verts->at( 3 * vertexNum + 1 ), verts->at( 3 * vertexNum + 2 ) ) );
            ++vertexNum;

            lc = getRGBAColorFromDirection( m_dataset->getPosition( j, i ), m_dataset->getPosition( j, i - 1 ) );

            m_localColors->push_back( wge::osgColor( lc ) );
            m_globalColors->push_back( wge::osgColor( gc ) );
        }
        m_localColors->push_back( wge::osgColor( lc ) );
        m_globalColors->push_back( wge::osgColor( gc ) );

        m_geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, startIndexes->at( j ), pointsPerLine->at( j ) ) );

        ++*progress;
    }

    progress->finish();

    m_geometry->setVertexArray( vertices );
    m_geometry->setColorArray( m_globalColors );
    m_geometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );

    m_geometry->setUseDisplayList( false );
    m_geometry->setUseVertexBufferObjects( true );

    osg::ref_ptr< osg::Geode > geode = osg::ref_ptr< osg::Geode >( new osg::Geode );
    geode->addDrawable( m_geometry.get() );

    return geode;
}

void WMFiberDisplay2::updateColoring()
{
    if ( m_properties->getValue< bool >( "Local Color" ) )
    {
        m_geometry->setColorArray( m_localColors );
    }
    else
    {
        m_geometry->setColorArray( m_globalColors );
    }
}

void WMFiberDisplay2::updateLinesShown()
{
    boost::shared_ptr< std::vector< unsigned int > > startIndexes = m_dataset->getLineStartIndexes();
    boost::shared_ptr< std::vector< unsigned int > > pointsPerLine = m_dataset->getLineLengths();

    boost::shared_ptr< std::vector< bool > >active = WKernel::getRunningKernel()->getRoiManager()->getBitField( m_dataset );

    m_geometry->removePrimitiveSet( 0, m_geometry->getNumPrimitiveSets() );
    for( size_t i = 0; i < active->size(); ++i )
    {
        if ( active->at( i ) )
        {
            m_geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, startIndexes->at( i ), pointsPerLine->at( i ) ) );
        }
    }
}

void WMFiberDisplay2::moduleMain()
{
    // additional fire-condition: "data changed" flag
    m_moduleState.add( m_fiberInput->getDataChangedCondition() );

    ready();

    while ( !m_shutdownFlag() ) // loop until the module container requests the module to quit
    {
        m_dataset = m_fiberInput->getData();
        if ( !m_dataset.get() ) // ok, the output has not yet sent data
        {
            m_moduleState.wait();
            continue;
        }
        WKernel::getRunningKernel()->getRoiManager()->addFiberDataset( m_dataset );

        create();

        m_moduleState.wait(); // waits for firing of m_moduleState ( dataChanged, shutdown, etc. )

        // May be called twice
        WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->removeChild( m_osgNode.get() );
    }
}

void WMFiberDisplay2::update()
{
    boost::shared_lock<boost::shared_mutex> slock;
    slock = boost::shared_lock<boost::shared_mutex>( m_updateLock );

    if ( WKernel::getRunningKernel()->getRoiManager()->isDirty() )
    {
        //std::cout << "fd update" << std::endl;
        updateLinesShown();
    }
    slock.unlock();
}

void WMFiberDisplay2::create()
{
    // remove nodes if they are any
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->removeChild( m_osgNode.get() );

    // create new node
    m_osgNode = osg::ref_ptr< osg::Group >( new osg::Group );
    m_osgNode->addChild( genFiberGeode().get() );
    m_osgNode->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->addChild( m_osgNode.get() );

    m_osgNode->setUserData( this );
    m_osgNode->setUpdateCallback( new fdNodeCallback );
}

void WMFiberDisplay2::connectors()
{
    using boost::shared_ptr;
    typedef WModuleInputData< const WDataSetFibers2 > FiberInputData;  // just an alias

    m_fiberInput = shared_ptr< FiberInputData >( new FiberInputData( shared_from_this(), "fiberInput", "A loaded fiber dataset." ) );

    addConnector( m_fiberInput );
    WModule::connectors();  // call WModules initialization
}

void WMFiberDisplay2::properties()
{
    m_properties->addString( "Fibers Display Module", "Display fibers" );
    // this bool is hidden
    m_properties->addBool( "active", true, true )->connect( boost::bind( &WMFiberDisplay2::slotPropertyChanged, this, _1 ) );
    m_properties->addBool( "Local Color", false )->connect( boost::bind( &WMFiberDisplay2::slotPropertyChanged, this, _1 ) );
}

void WMFiberDisplay2::slotPropertyChanged( std::string propertyName )
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
    else if ( propertyName == "Local Color" )
    {
        updateColoring();
    }
    else
    {
        // instead of WLogger we must use std::cerr since WLogger needs to much time!
        std::cerr << propertyName << std::endl;
        assert( 0 && "This property name is not supported by this function yet." );
    }
}

WColor WMFiberDisplay2::getRGBAColorFromDirection( const wmath::WPosition &pos1, const wmath::WPosition &pos2 )
{
    wmath::WPosition direction( ( pos2 - pos1 ) );
    direction.normalize();
    return WColor( std::abs( direction[0] ), std::abs( direction[1] ), std::abs( direction[2] ) );
}
