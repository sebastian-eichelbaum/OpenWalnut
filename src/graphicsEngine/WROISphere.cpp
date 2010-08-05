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
#include <utility>

#include <osg/LightModel>

#include "WROISphere.h"
#include "WGraphicsEngine.h"
#include "WGEUtils.h"

size_t WROISphere::maxSphereId = 0;


WROISphere::WROISphere( wmath::WPosition position, float radius ) :
    WROI(),
    sphereId( maxSphereId++ ),
    m_position( position ),
    m_originalPosition( position ),
    m_radius( radius ),
    m_lockX( false ),
    m_lockY( false ),
    m_lockZ( false ),
    m_pickNormal( wmath::WVector3D() ),
    m_oldPixelPosition( std::make_pair( 0, 0 ) ),
    m_color( osg::Vec4( 0.f, 1.f, 1.f, 0.4f ) ),
    m_notColor( osg::Vec4( 1.0f, 0.0f, 0.0f, 0.4f ) )
{
    boost::shared_ptr< WGraphicsEngine > ge = WGraphicsEngine::getGraphicsEngine();
    assert( ge );
    boost::shared_ptr< WGEViewer > viewer = ge->getViewerByName( "main" );
    assert( viewer );
    m_viewer = viewer;
    m_pickHandler = m_viewer->getPickHandler();
    m_pickHandler->getPickSignal()->connect( boost::bind( &WROISphere::registerRedrawRequest, this, _1 ) );

    redrawSphere();
    //**********************************************************
    m_isModified = true;
    m_isNot = false;

    assert( WGraphicsEngine::getGraphicsEngine() );
    WGraphicsEngine::getGraphicsEngine()->getScene()->addChild( this );

    setUserData( this );
    setUpdateCallback( osg::ref_ptr<ROISphereNodeCallback>( new ROISphereNodeCallback ) );
}

void WROISphere::redrawSphere()
{
    removeDrawables( 0 );

    osg::ShapeDrawable* shape = new osg::ShapeDrawable( new osg::Sphere( osg::Vec3( m_position[0], m_position[1], m_position[2] ), m_radius ) );

    std::stringstream ss;
    ss << "ROISphere" << sphereId;

    setName( ss.str() );
    shape->setName( ss.str() );

    addDrawable( shape );
}

WROISphere::~WROISphere()
{
//    std::cout << "destructor called" << std::endl;
//    std::cout << "ref count geode: " << m_geode->referenceCount() << std::endl;
//
//    WGraphicsEngine::getGraphicsEngine()->getScene()->remove( m_geode );
}

wmath::WPosition WROISphere::getPosition() const
{
    return m_position;
}

void WROISphere::setPosition( wmath::WPosition position )
{
    m_position = position;
    m_originalPosition = position;
    m_isModified = true;
}

void WROISphere::registerRedrawRequest( WPickInfo pickInfo )
{
    m_pickInfo = pickInfo;
}

void WROISphere::updateGFX()
{
    std::stringstream ss;
    ss << "ROISphere" << sphereId << "";
    if ( m_pickInfo.getName() == ss.str() )
    {
        std::pair< float, float > newPixelPos( m_pickInfo.getPickPixelPosition() );
        if ( m_isPicked )
        {
            osg::Vec3 in( newPixelPos.first, newPixelPos.second, 0.0 );
            osg::Vec3 world = wge::unprojectFromScreen( in, m_viewer->getCamera() );

            wmath::WPosition newPixelWorldPos( world[0], world[1], world[2] );
            wmath::WPosition oldPixelWorldPos;
            if(  m_oldPixelPosition.first == 0 && m_oldPixelPosition.second == 0 )
            {
                oldPixelWorldPos = newPixelWorldPos;
            }
            else
            {
                osg::Vec3 in( m_oldPixelPosition.first, m_oldPixelPosition.second, 0.0 );
                osg::Vec3 world = wge::unprojectFromScreen( in, m_viewer->getCamera() );
                oldPixelWorldPos = wmath::WPosition( world[0], world[1], world[2] );
            }

            wmath::WVector3D moveVec = newPixelWorldPos - oldPixelWorldPos;

            osg::ref_ptr<osg::Vec3Array> vertices = osg::ref_ptr<osg::Vec3Array>( new osg::Vec3Array );

            // move sphere
            if( m_pickInfo.getModifierKey() == WPickInfo::NONE )
            {
                moveSphere( moveVec );
            }
        }

        m_oldPixelPosition = newPixelPos;
        m_isModified = true;
        m_isPicked = true;

        m_signalIsModified();
    }
    if ( m_isPicked && m_pickInfo.getName() == "unpick" )
    {
        // Perform all actions necessary for finishing a pick
        m_pickNormal = wmath::WVector3D();
        m_isPicked = false;
    }

    if ( isModified() )
    {
        redrawSphere();
    }
}

void WROISphere::moveSphere( wmath::WVector3D offset )
{
    m_position += offset;

    if ( m_lockX )
    {
        m_position[0] = m_originalPosition[0];
    }
    if ( m_lockY )
    {
        m_position[1] = m_originalPosition[1];
    }

    if ( m_lockZ )
    {
        m_position[2] = m_originalPosition[2];
    }
}

void WROISphere::setLockX( bool value )
{
    m_lockX = value;
}

void WROISphere::setLockY( bool value )
{
    m_lockY = value;
}

void WROISphere::setLockZ( bool value )
{
    m_lockZ = value;
}


void WROISphere::setColor( osg::Vec4 color )
{
    m_color = color;
}

void WROISphere::setNotColor( osg::Vec4 color )
{
    m_notColor = color;
}
