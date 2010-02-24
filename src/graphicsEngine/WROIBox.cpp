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

#include <osg/LineWidth>
#include <osg/LightModel>

#include "WROIBox.h"
#include "WGraphicsEngine.h"
#include "WGEUtils.h"

size_t WROIBox::maxBoxId = 0;

void buildFacesFromPoints( osg::DrawElementsUInt* surfaceElements )
{
    surfaceElements->push_back( 0 );
    surfaceElements->push_back( 2 );
    surfaceElements->push_back( 3 );
    surfaceElements->push_back( 1 );

    surfaceElements->push_back( 2 );
    surfaceElements->push_back( 6 );
    surfaceElements->push_back( 7 );
    surfaceElements->push_back( 3 );

    surfaceElements->push_back( 6 );
    surfaceElements->push_back( 4 );
    surfaceElements->push_back( 5 );
    surfaceElements->push_back( 7 );

    surfaceElements->push_back( 4 );
    surfaceElements->push_back( 0 );
    surfaceElements->push_back( 1 );
    surfaceElements->push_back( 5 );

    surfaceElements->push_back( 1 );
    surfaceElements->push_back( 3 );
    surfaceElements->push_back( 7 );
    surfaceElements->push_back( 5 );

    surfaceElements->push_back( 0 );
    surfaceElements->push_back( 4 );
    surfaceElements->push_back( 6 );
    surfaceElements->push_back( 2 );
}

void buildLinesFromPoints( osg::DrawElementsUInt* surfaceElements )
{
    surfaceElements->push_back( 0 );
    surfaceElements->push_back( 2 );
    surfaceElements->push_back( 2 );
    surfaceElements->push_back( 3 );
    surfaceElements->push_back( 3 );
    surfaceElements->push_back( 1 );
    surfaceElements->push_back( 1 );
    surfaceElements->push_back( 0 );

    surfaceElements->push_back( 6 );
    surfaceElements->push_back( 4 );
    surfaceElements->push_back( 4 );
    surfaceElements->push_back( 5 );
    surfaceElements->push_back( 5 );
    surfaceElements->push_back( 7 );
    surfaceElements->push_back( 7 );
    surfaceElements->push_back( 6 );

    surfaceElements->push_back( 2 );
    surfaceElements->push_back( 6 );
    surfaceElements->push_back( 7 );
    surfaceElements->push_back( 3 );

    surfaceElements->push_back( 4 );
    surfaceElements->push_back( 0 );
    surfaceElements->push_back( 1 );
    surfaceElements->push_back( 5 );
}

void setVertices( osg::Vec3Array* vertices, wmath::WPosition minPos, wmath::WPosition maxPos )
{
    vertices->push_back( osg::Vec3( minPos[0], minPos[1], minPos[2] ) );
    vertices->push_back( osg::Vec3( minPos[0], minPos[1], maxPos[2] ) );
    vertices->push_back( osg::Vec3( minPos[0], maxPos[1], minPos[2] ) );
    vertices->push_back( osg::Vec3( minPos[0], maxPos[1], maxPos[2] ) );
    vertices->push_back( osg::Vec3( maxPos[0], minPos[1], minPos[2] ) );
    vertices->push_back( osg::Vec3( maxPos[0], minPos[1], maxPos[2] ) );
    vertices->push_back( osg::Vec3( maxPos[0], maxPos[1], minPos[2] ) );
    vertices->push_back( osg::Vec3( maxPos[0], maxPos[1], maxPos[2] ) );
}

WROIBox::WROIBox( wmath::WPosition minPos, wmath::WPosition maxPos ) :
    WROI(),
    boxId( maxBoxId++ ),
    m_oldPixelPosition( std::make_pair( 0, 0 ) )
{
    m_minPos = minPos;
    m_maxPos = maxPos;

    boost::shared_ptr< WGraphicsEngine > ge = WGraphicsEngine::getGraphicsEngine();
    assert( ge );
    boost::shared_ptr< WGEViewer > viewer = ge->getViewerByName( "main" );
    assert( viewer );
    m_viewer = viewer;
    m_pickHandler = m_viewer->getPickHandler();
    m_pickHandler->getPickSignal()->connect( boost::bind( &WROIBox::registerRedrawRequest, this, _1 ) );

    m_surfaceGeometry = osg::ref_ptr<osg::Geometry>( new osg::Geometry() );
    m_surfaceGeometry->setDataVariance( osg::Object::DYNAMIC );

    m_geode = osg::ref_ptr<osg::Geode>( new osg::Geode );
    std::stringstream ss;
    ss << "ROIBox" << boxId;

    m_geode->setName( ss.str() );

    osg::ref_ptr<osg::Vec3Array> vertices = osg::ref_ptr<osg::Vec3Array>( new osg::Vec3Array );
    setVertices( vertices, minPos, maxPos );
    m_surfaceGeometry->setVertexArray( vertices );

    osg::DrawElementsUInt* surfaceElements;
    surfaceElements = new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS, 0 );
    buildFacesFromPoints( surfaceElements );

    osg::DrawElementsUInt* lineElements;
    lineElements = new osg::DrawElementsUInt( osg::PrimitiveSet::LINES, 0 );
    buildLinesFromPoints( lineElements );

    m_surfaceGeometry->addPrimitiveSet( surfaceElements );
    m_surfaceGeometry->addPrimitiveSet( lineElements );
    m_geode->addDrawable( m_surfaceGeometry );
    osg::StateSet* state = m_geode->getOrCreateStateSet();
    state->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

    osg::LineWidth* linewidth = new osg::LineWidth();
    linewidth->setWidth( 2.f );
    state->setAttributeAndModes( linewidth, osg::StateAttribute::ON );

    // ------------------------------------------------
    // colors
    osg::ref_ptr<osg::Vec4Array> colors = osg::ref_ptr<osg::Vec4Array>( new osg::Vec4Array );

    colors->push_back( osg::Vec4( .0f, .0f, 1.f, 0.5f ) );
    m_surfaceGeometry->setColorArray( colors );
    m_surfaceGeometry->setColorBinding( osg::Geometry::BIND_OVERALL );

    osg::ref_ptr< osg::LightModel > lightModel = new osg::LightModel();
    lightModel->setTwoSided( true );
    state->setAttributeAndModes( lightModel.get(), osg::StateAttribute::ON );
    state->setMode( GL_BLEND, osg::StateAttribute::ON );

    m_isModified = true;
    m_isNot = false;

    assert( WGraphicsEngine::getGraphicsEngine() );
    WGraphicsEngine::getGraphicsEngine()->getScene()->addChild( m_geode );

    m_geode->setUserData( this );
    m_geode->setUpdateCallback( new ROIBoxNodeCallback );
}

wmath::WPosition WROIBox::getMinPos() const
{
    return m_minPos;
}

wmath::WPosition WROIBox::getMaxPos() const
{
    return m_maxPos;
}

bool WROIBox::isModified()
{
    bool tmp = m_isModified;
    m_isModified = false;
    return tmp;
}

void WROIBox::registerRedrawRequest( WPickInfo pickInfo )
{
    boost::unique_lock< boost::shared_mutex > lock;
    lock = boost::unique_lock< boost::shared_mutex >( m_updateLock );

    m_pickInfo = pickInfo;

    lock.unlock();
}

void WROIBox::updateGFX()
{
    boost::unique_lock< boost::shared_mutex > lock;
    lock = boost::unique_lock< boost::shared_mutex >( m_updateLock );

    std::stringstream ss;
    ss << "ROIBox" << boxId << "";
    if ( m_pickInfo.getName() == ss.str() )
    {
        wmath::WPosition newPos( m_pickInfo.getPickPosition() );
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

            // resize Box
            if( m_pickInfo.getModifierKey() == WPickInfo::SHIFT && m_pickInfo.getPickNormal() != wmath::WVector3D() )
            {
                wmath::WVector3D normal = m_pickInfo.getPickNormal();
                if( normal[0] <= 0 && normal[1] <= 0 && normal[2] <= 0 )
                {
                    m_maxPos += normal * ( moveVec * normal );
                }
                if( normal[0] >= 0 && normal[1] >= 0 && normal[2] >= 0 )
                {
                    m_minPos += normal * ( moveVec * normal );
                }

                setVertices( vertices, m_minPos, m_maxPos );
                m_surfaceGeometry->setVertexArray( vertices );
            }

            // move Box
            if( m_pickInfo.getModifierKey() == WPickInfo::NONE )
            {
                m_minPos += moveVec;
                m_maxPos += moveVec;
                setVertices( vertices, m_minPos, m_maxPos );
                m_surfaceGeometry->setVertexArray( vertices );
            }
        }
        else
        {
            osg::ref_ptr<osg::Vec4Array> colors = osg::ref_ptr<osg::Vec4Array>( new osg::Vec4Array );
            colors->push_back( osg::Vec4( 1.f, .0f, .0f, 0.5f ) );
            m_surfaceGeometry->setColorArray( colors );
        }
        m_pickedPosition = newPos;
        m_oldPixelPosition = newPixelPos;
        m_isModified = true;
        m_isPicked = true;

        m_signalIsModified();
    }
    if ( m_isPicked && m_pickInfo.getName() == "unpick" )
    {
        osg::ref_ptr<osg::Vec4Array> colors = osg::ref_ptr<osg::Vec4Array>( new osg::Vec4Array );
        colors->push_back( osg::Vec4( 0.f, 0.f, 1.f, 0.5f ) );
        m_surfaceGeometry->setColorArray( colors );
        m_isPicked = false;
    }

    lock.unlock();
}
