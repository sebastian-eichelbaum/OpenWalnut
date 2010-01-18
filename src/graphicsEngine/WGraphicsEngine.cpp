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

#include <iostream>
#include <list>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/thread/locks.hpp>

#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/ref_ptr>
#include <osgViewer/CompositeViewer>
#include <osgViewer/View>
#include <osgViewer/Viewer>

#include "../common/WColor.h"
#include "../common/WLogger.h"
#include "../kernel/WKernel.h"
#include "../math/WPosition.h"
#include "WGEViewer.h"
#include "WGraphicsEngine.h"
#include "exceptions/WGEInitFailed.h"
#include "WGEResourceManager.h"

// graphics engine instance as singleton
boost::shared_ptr< WGraphicsEngine > WGraphicsEngine::m_instance = boost::shared_ptr< WGraphicsEngine >();

WGraphicsEngine::WGraphicsEngine():
    WThreadedRunner()
{
    WLogger::getLogger()->addLogMessage( "Initializing Graphics Engine", "GE", LL_INFO );

    // initialize members
    m_rootNode = new WGEScene();

    m_shaderPath = "";

    // initialize OSG render window
    m_Viewer = osg::ref_ptr<osgViewer::CompositeViewer>( new osgViewer::CompositeViewer() );

    // ThreadingModel: enum with the following possibilities
    //
    //  SingleThreadet
    //  CullDrawThreadPerContext
    //  ThreadPerContext
    //  DrawThreadPerContext
    //  CullThreadPerCameraDrawThreadPerContext
    //  ThreadPerCamera
    //  AutomaticSelection
    m_Viewer->setThreadingModel( osgViewer::Viewer::CullThreadPerCameraDrawThreadPerContext );

    // init resource manager ( it is a singleton and gets created during first "getResourceManager" request.
    WGEResourceManager::getResourceManager();
}

WGraphicsEngine::~WGraphicsEngine()
{
    // cleanup
    WLogger::getLogger()->addLogMessage( "Shutting down Graphics Engine", "GE", LL_INFO );
}


boost::shared_ptr< WGraphicsEngine > WGraphicsEngine::getGraphicsEngine()
{
    if ( !m_instance )
    {
        m_instance = boost::shared_ptr< WGraphicsEngine >( new WGraphicsEngine() );
    }

    return m_instance;
}

osg::ref_ptr<WGEScene> WGraphicsEngine::getScene()
{
    return m_rootNode;
}

std::string WGraphicsEngine::getShaderPath() const
{
    return m_shaderPath;
}

void WGraphicsEngine::setShaderPath( std::string path )
{
    m_shaderPath = path;
}

boost::shared_ptr<WGEViewer> WGraphicsEngine::createViewer( std::string name, osg::ref_ptr<WindowData> wdata, int x, int y,
                                                            int width, int height, WGECamera::ProjectionMode projectionMode,
                                                            WColor bgColor )
{
    // init the composite viewer if not already done
    if ( m_Viewer == osg::ref_ptr< osgViewer::CompositeViewer >() )
    {
    }

    boost::shared_ptr<WGEViewer> viewer = boost::shared_ptr<WGEViewer>(
        new WGEViewer( name, wdata, x, y, width, height, projectionMode ) );
    viewer->setBgColor( bgColor );
    viewer->setScene( getScene() );

    // finally add view
    m_Viewer->addView( viewer->getViewer().get() );

    // store it in viewer list
    boost::mutex::scoped_lock lock( m_ViewersLock );
    assert( m_Viewers.insert( make_pair( name, viewer ) ).second == true );
    m_ViewersLock.unlock();

    return viewer;
}

void WGraphicsEngine::closeViewer( const std::string name )
{
    boost::mutex::scoped_lock lock( m_ViewersLock );
    if( m_Viewers.count( name ) > 0 )
    {
        m_Viewers[name]->close();

        m_Viewers.erase( name );
    }
    m_ViewersLock.unlock();
}

boost::shared_ptr< WGEViewer > WGraphicsEngine::getViewerByName( std::string name )
{
    boost::mutex::scoped_lock lock( m_ViewersLock );
    boost::shared_ptr< WGEViewer > out = m_Viewers.count( name ) > 0 ?
        m_Viewers[name] :
        boost::shared_ptr< WGEViewer >();
    m_ViewersLock.unlock();
    return out;
}

void WGraphicsEngine::threadMain()
{
    WLogger::getLogger()->addLogMessage( "Starting Graphics Engine", "GE", LL_INFO );

    m_Viewer->startThreading();
    m_Viewer->run();
    m_Viewer->stopThreading();
}

void WGraphicsEngine::notifyStop()
{
    WLogger::getLogger()->addLogMessage( "Stopping Graphics Engine", "GE", LL_INFO );

    m_Viewer->setDone( true );
}

osg::Vec4 wge::osgColor( const WColor& color )
{
    return osg::Vec4( color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha() );
}

osg::Vec3 wge::osgVec3( const wmath::WPosition& pos )
{
    return osg::Vec3( pos[0], pos[1], pos[2] );
}

osg::ref_ptr< osg::Vec3Array > wge::osgVec3Array( const std::vector< wmath::WPosition >& posArray )
{
    osg::ref_ptr< osg::Vec3Array > result = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    result->reserve( posArray.size() );
    std::vector< wmath::WPosition >::const_iterator cit;
    for( cit = posArray.begin(); cit != posArray.end(); ++cit )
    {
        result->push_back( osgVec3( *cit ) );
    }
    return result;
}

osg::ref_ptr< osg::Vec3Array > wge::generateCuboidQuads( const std::vector< wmath::WPosition >& corners )
{
    osg::ref_ptr< osg::Vec3Array > vertices = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array );

    // Surfaces
    vertices->push_back( wge::osgVec3( corners[0] ) );
    vertices->push_back( wge::osgVec3( corners[1] ) );
    vertices->push_back( wge::osgVec3( corners[2] ) );
    vertices->push_back( wge::osgVec3( corners[3] ) );

    vertices->push_back( wge::osgVec3( corners[1] ) );
    vertices->push_back( wge::osgVec3( corners[5] ) );
    vertices->push_back( wge::osgVec3( corners[6] ) );
    vertices->push_back( wge::osgVec3( corners[2] ) );

    vertices->push_back( wge::osgVec3( corners[5] ) );
    vertices->push_back( wge::osgVec3( corners[4] ) );
    vertices->push_back( wge::osgVec3( corners[7] ) );
    vertices->push_back( wge::osgVec3( corners[6] ) );

    vertices->push_back( wge::osgVec3( corners[4] ) );
    vertices->push_back( wge::osgVec3( corners[0] ) );
    vertices->push_back( wge::osgVec3( corners[3] ) );
    vertices->push_back( wge::osgVec3( corners[7] ) );

    vertices->push_back( wge::osgVec3( corners[3] ) );
    vertices->push_back( wge::osgVec3( corners[2] ) );
    vertices->push_back( wge::osgVec3( corners[6] ) );
    vertices->push_back( wge::osgVec3( corners[7] ) );

    vertices->push_back( wge::osgVec3( corners[0] ) );
    vertices->push_back( wge::osgVec3( corners[1] ) );
    vertices->push_back( wge::osgVec3( corners[5] ) );
    vertices->push_back( wge::osgVec3( corners[4] ) );
    return vertices;
}
