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
#include "WMFiberDisplay.h"

WMFiberDisplay::WMFiberDisplay()
    : WModule(),
      m_globalColoring( true )
{
}

WMFiberDisplay::~WMFiberDisplay()
{
}

boost::shared_ptr< WModule > WMFiberDisplay::factory() const
{
    return boost::shared_ptr< WModule >( new WMFiberDisplay() );
}

osg::ref_ptr< osg::Geode > WMFiberDisplay::genFiberGeode( boost::shared_ptr< const WDataSetFibers > fibers,
        bool globalColoring ) const
{
    using osg::ref_ptr;
    ref_ptr< osg::Vec3Array > vertices = ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    ref_ptr< osg::Vec4Array > colors = ref_ptr< osg::Vec4Array >( new osg::Vec4Array );
    ref_ptr< osg::Geometry > geometry = ref_ptr< osg::Geometry >( new osg::Geometry );

    size_t vertexNum = 0;
    for( size_t j = 0; j < fibers->size(); ++j )
    {
        const wmath::WFiber &fib = ( *fibers )[j];
        vertices->push_back( osg::Vec3( fib[0][0], fib[0][1], fib[0][2] ) );
        ++vertexNum;
        for( size_t i = 1; i < fib.size(); ++i )
        {
            vertices->push_back( osg::Vec3( fib[i][0], fib[i][1], fib[i][2] ) );
            ++vertexNum;
            WColor c;
            if( !globalColoring )
            {
                c = color_utils::getRGBAColorFromDirection( fib[i], fib[i-1] );
            }
            else
            {
                c = color_utils::getRGBAColorFromDirection( fib[0], fib[ fib.size() -1 ] );
            }
            colors->push_back( wge::osgColor( c ) );
        }
        colors->push_back( colors->back() );
        geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, vertexNum - fib.size(), fib.size() ) );
    }
    geometry->setVertexArray( vertices );
    geometry->setColorArray( colors );
    geometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );
    osg::ref_ptr< osg::Geode > geode = osg::ref_ptr< osg::Geode >( new osg::Geode );
    geode->addDrawable( geometry.get() );
    return geode;
}

void WMFiberDisplay::moduleMain()
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

        update();

        m_moduleState.wait(); // waits for firing of m_moduleState ( dataChanged, shutdown, etc. )

        // May be called twice
        WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_osgNode );
    }
}

void WMFiberDisplay::update()
{
    // remove nodes if they are any
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_osgNode );

    // create new node
    m_osgNode = osg::ref_ptr< WGEGroupNode >( new WGEGroupNode );
    m_osgNode->insert( genFiberGeode( m_dataset, m_globalColoring ) );
    m_osgNode->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_osgNode );
}

void WMFiberDisplay::connectors()
{
    using boost::shared_ptr;
    typedef WModuleInputData< const WDataSetFibers > FiberInputData;  // just an alias

    m_fiberInput = shared_ptr< FiberInputData >( new FiberInputData( shared_from_this(), "fiberInput", "A loaded fiber dataset." ) );

    addConnector( m_fiberInput );
    WModule::connectors();  // call WModules initialization
}

void WMFiberDisplay::properties()
{
    m_properties->addString( "Fibers Display Module", "Display fibers" );
    // this bool is hidden
    m_properties->addBool( "active", true, true )->connect( boost::bind( &WMFiberDisplay::slotPropertyChanged, this, _1 ) );
    m_properties->addBool( "Local Color", true )->connect( boost::bind( &WMFiberDisplay::slotPropertyChanged, this, _1 ) );
}

void WMFiberDisplay::slotPropertyChanged( std::string propertyName )
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
        if( m_properties->getValue< bool >( propertyName ) != m_globalColoring )
        {
            m_globalColoring = m_properties->getValue< bool >( propertyName );
            update();
        }
    }
    else
    {
        // instead of WLogger we must use std::cerr since WLogger needs to much time!
        std::cerr << propertyName << std::endl;
        assert( 0 && "This property name is not supported by this function yet." );
    }
}
