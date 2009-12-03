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

#include "WMFiberDisplay.h"
#include "../../math/WFiber.h"
#include "../../common/WLogger.h"
#include "../../common/WColor.h"
#include "../../dataHandler/WDataHandler.h"
#include "../../dataHandler/WSubject.h"
#include "../../dataHandler/WDataSetFibers.h"
#include "../../kernel/WKernel.h"
#include "../../utils/WColorUtils.h"

WMFiberDisplay::WMFiberDisplay()
    : WModule()
{
}

WMFiberDisplay::~WMFiberDisplay()
{
}

boost::shared_ptr< WModule > WMFiberDisplay::factory() const
{
    return boost::shared_ptr< WModule >( new WMFiberDisplay() );
}

osg::ref_ptr< osg::Geode > WMFiberDisplay::genFiberGeode(
        boost::shared_ptr< const WDataSetFibers > fibers,
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
            colors->push_back( c.getOSGColor() );
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

void WMFiberDisplay::connectToGui()
{
    WKernel::getRunningKernel()->getGui()->connectProperties( m_properties );
    WKernel::getRunningKernel()->getGui()->addModuleToBrowser( shared_from_this() );
}

void WMFiberDisplay::moduleMain()
{
//    ready();
//    while( !m_FinishRequested )
//    {
//        if( m_fiberInput->getData() != boost::shared_ptr< const WDataSetFibers >() )
//        {
//            boost::shared_ptr< const WDataSetFibers > fiberDS = m_fiberInput->getData();
//
//            osg::ref_ptr< osg::Group > group = osg::ref_ptr< osg::Group >( new osg::Group );
//            group->addChild( genFiberGeode( fiberDS, false ).get() );
//            group->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
//
//            WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->addChild( group.get() );
//        }
//        // TODO(math): implement some redraw if data has changed
//        //  - replacing the OSG group node?
//        //  - using notifyDataChange() method
//        while( !m_FinishRequested )
//        {
//            sleep( 1 );
//        }
//    }
    boost::shared_ptr< WDataHandler > dataHandler;
    // TODO(math): fix this hack when possible by using an input connector.
    while( !WKernel::getRunningKernel() )
    {
        sleep( 1 );
    }
    while( !( dataHandler = WKernel::getRunningKernel()->getDataHandler() ) )
    {
        sleep( 1 );
    }
    while( !dataHandler->getNumberOfSubjects() )
    {
        sleep( 1 );
    }

    boost::shared_ptr< const WDataSetFibers > fiberDS;
    assert( fiberDS = boost::shared_dynamic_cast< const WDataSetFibers >( dataHandler->getSubject( 0 )->getDataSet( 0 ) ) );
    osg::ref_ptr< osg::Group > group = osg::ref_ptr< osg::Group >( new osg::Group );
    group->addChild( genFiberGeode( fiberDS, false ).get() );
    group->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->addChild( group.get() );

    // Since the modules run in a separate thread: such loops are possible
    while ( !m_FinishRequested )
    {
        // do fancy stuff
        sleep( 1 );
    }
}

void WMFiberDisplay::connectors()
{
    using boost::shared_ptr;
    typedef WModuleInputData< const WDataSetFibers > FiberInputData;  // just an alias

    m_fiberInput = shared_ptr< FiberInputData >( new FiberInputData(
                shared_from_this(), "fiberInput", "A loaded fiber dataset." )
            );

    addConnector( m_fiberInput );
    WModule::connectors();  // call WModules initialization
}

void WMFiberDisplay::properties()
{
    m_properties->addString( "Fibers Display Module", "Display fibers" );
}
