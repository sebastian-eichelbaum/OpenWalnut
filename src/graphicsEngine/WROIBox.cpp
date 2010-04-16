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
    m_pickNormal( wmath::WVector3D() ),
    m_oldPixelPosition( std::make_pair( 0, 0 ) ),
    m_color( osg::Vec4( 0.f, 1.f, 1.f, 0.4f ) ),
    m_notColor( osg::Vec4( 1.0f, 0.0f, 0.0f, 0.4f ) )
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

    //m_geode = osg::ref_ptr<osg::Geode>( new osg::Geode );
    std::stringstream ss;
    ss << "ROIBox" << boxId;

    setName( ss.str() );
    m_surfaceGeometry->setName( ss.str() );

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
    addDrawable( m_surfaceGeometry );
    osg::StateSet* state = getOrCreateStateSet();
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
    WGraphicsEngine::getGraphicsEngine()->getScene()->addChild( this );

    setUserData( this );
    setUpdateCallback( osg::ref_ptr<ROIBoxNodeCallback>( new ROIBoxNodeCallback ) );
}

WROIBox::~WROIBox()
{
//    std::cout << "destructor called" << std::endl;
//    std::cout << "ref count geode: " << m_geode->referenceCount() << std::endl;
//
//    WGraphicsEngine::getGraphicsEngine()->getScene()->remove( m_geode );
}

wmath::WPosition WROIBox::getMinPos() const
{
    return m_minPos;
}

wmath::WPosition WROIBox::getMaxPos() const
{
    return m_maxPos;
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
            if( m_pickInfo.getModifierKey() == WPickInfo::SHIFT )
            {
                if( m_pickNormal[0] <= 0 && m_pickNormal[1] <= 0 && m_pickNormal[2] <= 0 )
                {
                    m_maxPos += m_pickNormal * ( moveVec * m_pickNormal );
                }
                if( m_pickNormal[0] >= 0 && m_pickNormal[1] >= 0 && m_pickNormal[2] >= 0 )
                {
                    m_minPos += m_pickNormal * ( moveVec * m_pickNormal );
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
            m_pickNormal = m_pickInfo.getPickNormal();
            // color for moving box
            if( m_pickInfo.getModifierKey() == WPickInfo::NONE )
            {
                osg::ref_ptr<osg::Vec4Array> colors = osg::ref_ptr<osg::Vec4Array>( new osg::Vec4Array );
                if ( m_isNot )
                {
                    colors->push_back( m_notColor );
                }
                else
                {
                    colors->push_back( m_color );
                }
                m_surfaceGeometry->setColorArray( colors );
            }
            if( m_pickInfo.getModifierKey() == WPickInfo::SHIFT )
            {
                osg::ref_ptr<osg::Vec4Array> colors = osg::ref_ptr<osg::Vec4Array>( new osg::Vec4Array );
                colors->push_back( osg::Vec4( .0f, 1.0f, .0f, 0.4f ) );
                m_surfaceGeometry->setColorArray( colors );
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

        osg::ref_ptr<osg::Vec4Array> colors = osg::ref_ptr<osg::Vec4Array>( new osg::Vec4Array );
        if ( m_isNot )
        {
            colors->push_back( m_notColor );
        }
        else
        {
            colors->push_back( m_color );
        }
        m_surfaceGeometry->setColorArray( colors );
        m_pickNormal = wmath::WVector3D();
        m_isPicked = false;
    }

    if ( isModified() )
    {
        osg::ref_ptr<osg::Vec4Array> colors = osg::ref_ptr<osg::Vec4Array>( new osg::Vec4Array );
        if ( m_isNot )
        {
            colors->push_back( m_notColor );
        }
        else
        {
            colors->push_back( m_color );
        }
        m_surfaceGeometry->setColorArray( colors );
    }

    lock.unlock();
}

void WROIBox::setColor( osg::Vec4 color )
{
    m_color = color;
}

void WROIBox::setNotColor( osg::Vec4 color )
{
    m_notColor = color;
}
