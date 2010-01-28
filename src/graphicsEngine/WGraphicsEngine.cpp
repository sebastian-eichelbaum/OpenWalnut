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
#include "../math/WPosition.h"
#include "WGEViewer.h"
#include "WGraphicsEngine.h"
#include "exceptions/WGEInitFailed.h"
#include "exceptions/WGESignalSubscriptionFailed.h"
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
    m_fontPath = "";

    // initialize OSG render window
    m_viewer = osg::ref_ptr<osgViewer::CompositeViewer>( new osgViewer::CompositeViewer() );

    // ThreadingModel: enum with the following possibilities
    //
    //  SingleThreadet
    //  CullDrawThreadPerContext
    //  ThreadPerContext
    //  DrawThreadPerContext
    //  CullThreadPerCameraDrawThreadPerContext
    //  ThreadPerCamera
    //  AutomaticSelection
    m_viewer->setThreadingModel( osgViewer::Viewer::CullThreadPerCameraDrawThreadPerContext );

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

std::string WGraphicsEngine::getFontPath() const
{
    return m_fontPath;
}

void WGraphicsEngine::setFontPath( std::string path )
{
    m_fontPath = path;

    // we need to propagate the change to the resource manager
    WGEResourceManager::getResourceManager()->setFontPath( path );
}

boost::shared_ptr<WGEViewer> WGraphicsEngine::createViewer( std::string name, osg::ref_ptr<WindowData> wdata, int x, int y,
                                                            int width, int height, WGECamera::ProjectionMode projectionMode,
                                                            WColor bgColor )
{
    // init the composite viewer if not already done
    if ( m_viewer == osg::ref_ptr< osgViewer::CompositeViewer >() )
    {
    }

    boost::shared_ptr<WGEViewer> viewer = boost::shared_ptr<WGEViewer>(
        new WGEViewer( name, wdata, x, y, width, height, projectionMode ) );
    viewer->setBgColor( bgColor );
    viewer->setScene( getScene() );

    // finally add view
    m_viewer->addView( viewer->getView() );

    // store it in viewer list
    boost::mutex::scoped_lock lock( m_viewersLock );
    assert( m_viewers.insert( make_pair( name, viewer ) ).second == true );
    //m_viewersLock.unlock();

    return viewer;
}

void WGraphicsEngine::closeViewer( const std::string name )
{
    boost::mutex::scoped_lock lock( m_viewersLock );
    if( m_viewers.count( name ) > 0 )
    {
        m_viewers[name]->close();

        m_viewers.erase( name );
    }
    //m_viewersLock.unlock();
}

boost::shared_ptr< WGEViewer > WGraphicsEngine::getViewerByName( std::string name )
{
    boost::mutex::scoped_lock lock( m_viewersLock );
    boost::shared_ptr< WGEViewer > out = m_viewers.count( name ) > 0 ?
        m_viewers[name] :
        boost::shared_ptr< WGEViewer >();
    //m_viewersLock.unlock();
    return out;
}

void WGraphicsEngine::threadMain()
{
    WLogger::getLogger()->addLogMessage( "Starting Graphics Engine", "GE", LL_INFO );

    m_viewer->startThreading();
    m_viewer->run();
    m_viewer->stopThreading();
}

void WGraphicsEngine::notifyStop()
{
    WLogger::getLogger()->addLogMessage( "Stopping Graphics Engine", "GE", LL_INFO );

    m_viewer->setDone( true );
}

void WGraphicsEngine::requestShaderReload()
{
    m_reloadShadersSignal();
}

boost::signals2::connection WGraphicsEngine::subscribeSignal( GE_SIGNAL signal, t_GEGenericSignalHandlerType notifier )
{
    switch ( signal )
    {
        case GE_RELOADSHADERS:
            return m_reloadShadersSignal.connect( notifier );
        default:
            std::ostringstream s;
            s << "Could not subscribe to unknown signal.";
            throw WGESignalSubscriptionFailed( s.str() );
            break;
    }
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

osg::Vec3 wge::getQuadNormal( const wmath::WPosition& a,
                              const wmath::WPosition& b,
                              const wmath::WPosition& c )
{
    wmath::WPosition vec1 = a - b;
    wmath::WPosition vec2 = c - b;
    wmath::WPosition normal = vec2.crossProduct( vec1 );
    normal.normalize();
    return osgVec3( normal );
}

osg::ref_ptr< osg::Vec3Array > wge::generateCuboidQuadNormals( const std::vector< wmath::WPosition >& corners )
{
    osg::ref_ptr< osg::Vec3Array > vertices = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array );

    vertices->push_back( getQuadNormal( corners[0], corners[1], corners[2] ) );
    vertices->push_back( getQuadNormal( corners[1], corners[5], corners[6] ) );
    vertices->push_back( getQuadNormal( corners[5], corners[4], corners[7] ) );
    vertices->push_back( getQuadNormal( corners[4], corners[0], corners[3] ) );
    vertices->push_back( getQuadNormal( corners[3], corners[2], corners[6] ) );
    vertices->push_back( getQuadNormal( corners[0], corners[1], corners[5] ) );
    return vertices;
}

osg::ref_ptr< osg::Geode > wge::generateBoundingBoxGeode( const wmath::WPosition& pos1, const wmath::WPosition& pos2, const WColor& color )
{
    assert( pos1[0] <= pos2[0] && pos1[1] <= pos2[1] && pos1[2] <= pos2[2] && "pos1 doesn't seem to be the frontLowerLeft corner of the BB!" );
    using osg::ref_ptr;
    ref_ptr< osg::Vec3Array > vertices = ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    ref_ptr< osg::Vec4Array > colors = ref_ptr< osg::Vec4Array >( new osg::Vec4Array );
    ref_ptr< osg::Geometry > geometry = ref_ptr< osg::Geometry >( new osg::Geometry );

    vertices->push_back( osg::Vec3( pos1[0], pos1[1], pos1[2] ) );
    vertices->push_back( osg::Vec3( pos2[0], pos1[1], pos1[2] ) );
    vertices->push_back( osg::Vec3( pos2[0], pos2[1], pos1[2] ) );
    vertices->push_back( osg::Vec3( pos1[0], pos2[1], pos1[2] ) );
    vertices->push_back( osg::Vec3( pos1[0], pos1[1], pos1[2] ) );
    vertices->push_back( osg::Vec3( pos1[0], pos1[1], pos2[2] ) );
    vertices->push_back( osg::Vec3( pos2[0], pos1[1], pos2[2] ) );
    vertices->push_back( osg::Vec3( pos2[0], pos2[1], pos2[2] ) );
    vertices->push_back( osg::Vec3( pos1[0], pos2[1], pos2[2] ) );
    vertices->push_back( osg::Vec3( pos1[0], pos1[1], pos2[2] ) );

    geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, 0, vertices->size() ) );

    vertices->push_back( osg::Vec3( pos1[0], pos2[1], pos1[2] ) );
    vertices->push_back( osg::Vec3( pos1[0], pos2[1], pos2[2] ) );
    vertices->push_back( osg::Vec3( pos2[0], pos2[1], pos1[2] ) );
    vertices->push_back( osg::Vec3( pos2[0], pos2[1], pos2[2] ) );
    vertices->push_back( osg::Vec3( pos2[0], pos1[1], pos1[2] ) );
    vertices->push_back( osg::Vec3( pos2[0], pos1[1], pos2[2] ) );

    geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINES, vertices->size() - 6, 6 ) );

    geometry->setVertexArray( vertices );
    colors->push_back( osgColor( color ) );
    geometry->setColorArray( colors );
    geometry->setColorBinding( osg::Geometry::BIND_OVERALL );
    osg::ref_ptr< osg::Geode > geode = osg::ref_ptr< osg::Geode >( new osg::Geode );
    geode->addDrawable( geometry );

    // disable light for this geode as lines can't be lit properly
    osg::StateSet* state = geode->getOrCreateStateSet();
    state->setMode( GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );

    return geode;
}
