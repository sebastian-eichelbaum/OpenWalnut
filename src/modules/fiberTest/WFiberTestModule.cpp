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

#include <osg/Geode>
#include <osg/Geometry>

#include "WFiberTestModule.h"
#include "../../math/WFiber.h"
#include "../../common/WLogger.h"
#include "../../common/WColor.hpp"
#include "../../dataHandler/WDataHandler.h"
#include "../../dataHandler/WSubject.h"
#include "../../dataHandler/WDataSetFibers.h"
#include "../../dataHandler/WLoaderManager.h"
#include "../../kernel/WKernel.h"
#include "../../utils/WColorUtils.h"

WFiberTestModule::WFiberTestModule()
    : WModule()
{
}

WFiberTestModule::~WFiberTestModule()
{
}

const std::string WFiberTestModule::getName() const
{
    return std::string( "FiberTestModule" );
}

const std::string WFiberTestModule::getDescription() const
{
    return std::string( "Draws fibers out of a WDataSetFibers" );
}

void WFiberTestModule::drawFiber( const wmath::WFiber &fib, osg::Geode *geode ) const
{
    osg::Vec3Array* vertices = new osg::Vec3Array;
    osg::Vec4Array* colors = new osg::Vec4Array;

    vertices->push_back( osg::Vec3( fib[0][0], fib[0][1], fib[0][2] ) );
    for( size_t i = 1; i < fib.size(); ++i )
    {
        vertices->push_back( osg::Vec3( fib[i][0], fib[i][1], fib[i][2] ) );
        WColor c = color_utils::getRGBAColorFromDirection( wmath::WPosition( fib[i][0], fib[i][1], fib[i][2] ),
                                                           wmath::WPosition( fib[i-1][0], fib[i-1][1], fib[i-1][2] ) );
        colors->push_back( c.getOSGColor() );
    }
    colors->push_back( colors->back() );

    osg::Geometry* geometry = new osg::Geometry();
    geometry->setVertexArray( vertices );
    geometry->setColorArray( colors );
    geometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );
    geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, 0, fib.size() ) );
    geode->addDrawable( geometry );
}

void WFiberTestModule::threadMain()
{
    boost::shared_ptr< WDataHandler > dataHandler;
    // TODO(wiebel): fix this hack when possible by using an input connector.
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
    const WDataSetFibers &fibers = *fiberDS;  // just an alias
    osg::Geode *geode = new osg::Geode;

    for( size_t i = 0; i < fibers.size(); ++i )
    {
        drawFiber( fibers[i], geode );
    }
    geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->addChild( geode );

    // Since the modules run in a separate thread: such loops are possible
    while ( !m_FinishRequested )
    {
        // do fancy stuff
        sleep( 1 );
    }
}
