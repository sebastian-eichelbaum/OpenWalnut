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

#include <algorithm>
#include <string>
#include <utility>

#include <osg/LineWidth>
#include <osg/LightModel>
#include <osg/Geometry>

#include "../common/WLogger.h"
#include "shaders/WGEShader.h"

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

void setVertices( osg::Vec3Array* vertices, WPosition minPos, WPosition maxPos )
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

void setNormals( osg::Vec3Array* normals )
{
    normals->push_back( osg::Vec3( -1.0, 0.0, 0.0 ) );
    normals->push_back( osg::Vec3( 0.0, 1.0, 0.0 ) );
    normals->push_back( osg::Vec3( 1.0, 0.0, 0.0 ) );
    normals->push_back( osg::Vec3( 0.0, -1.0, 1.0 ) );
    normals->push_back( osg::Vec3( 0.0, 0.0, -1.0 ) );
    normals->push_back( osg::Vec3( 0.0, 0.0, 1.0 ) );
}

WROIBox::WROIBox( WPosition minPos, WPosition maxPos ) :
    WROI(),
    boxId( maxBoxId++ ),
    m_pickNormal( WVector3d() ),
    m_oldPixelPosition( WVector2d::zero() ),
    m_oldScrollWheel( 0 ),
    m_color( osg::Vec4( 0.391f, 0.594f, 0.828f, 0.5f ) ),
    m_notColor( osg::Vec4( 0.828f, 0.391f, 0.391f, 0.5f ) )
{
    m_propGrp = m_properties->addPropertyGroup( "ROI Box", "Properties of this ROI Box" );
    m_minPos = m_propGrp->addProperty( "Min Position", "When a box is described by its diagonal, this is the lower, left, front corner of it.",
                                          minPos, boost::bind( &WROIBox::boxPropertiesChanged, this, _1 ) );
    m_maxPos = m_propGrp->addProperty( "Max Position", "When a box is described by its diagonal, this is the upper, right, back corner of it.",
                                          maxPos, boost::bind( &WROIBox::boxPropertiesChanged, this, _1 ) );

    boost::shared_ptr< WGraphicsEngine > ge = WGraphicsEngine::getGraphicsEngine();
    assert( ge );
    boost::shared_ptr< WGEViewer > viewer = ge->getViewerByName( "Main View" );
    assert( viewer );
    m_viewer = viewer;
    m_pickHandler = m_viewer->getPickHandler();
    m_pickHandler->getPickSignal()->connect( boost::bind( &WROIBox::registerRedrawRequest, this, _1 ) );

    m_surfaceGeometry = osg::ref_ptr<osg::Geometry>( new osg::Geometry() );
    m_surfaceGeometry->setDataVariance( osg::Object::DYNAMIC );

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

    colors->push_back( osg::Vec4( 0.0f, 0.0f, 1.0f, 0.5f ) );
    m_surfaceGeometry->setColorArray( colors );
    m_surfaceGeometry->setColorBinding( osg::Geometry::BIND_OVERALL );

    osg::ref_ptr< osg::LightModel > lightModel = new osg::LightModel();
    lightModel->setTwoSided( true );
    //state->setAttributeAndModes( lightModel.get(), osg::StateAttribute::ON );
    state->setMode( GL_BLEND, osg::StateAttribute::ON );

    // add a simple default lighting shader
    m_lightShader = new WGEShader( "WGELighting" );

    // normals
    osg::ref_ptr<osg::Vec3Array> normals = osg::ref_ptr<osg::Vec3Array>( new osg::Vec3Array );
    setNormals( normals );
    m_surfaceGeometry->setNormalArray( normals );
    m_surfaceGeometry->setNormalBinding( osg::Geometry::BIND_PER_PRIMITIVE );

    m_not->set( false );

    assert( WGraphicsEngine::getGraphicsEngine() );
    WGraphicsEngine::getGraphicsEngine()->getScene()->addChild( this );

    setUserData( this );
    setUpdateCallback( osg::ref_ptr<ROIBoxNodeCallback>( new ROIBoxNodeCallback ) );

    setDirty();
}

WROIBox::~WROIBox()
{
}

WPosition WROIBox::getMinPos() const
{
    return m_minPos->get();
}

WPosition WROIBox::getMaxPos() const
{
    return m_maxPos->get();
}

WPropPosition WROIBox::getMinPosProperty()
{
    return m_minPos;
}

WPropPosition WROIBox::getMaxPosProperty()
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

void WROIBox::boxPropertiesChanged( boost::shared_ptr< WPropertyBase > /* property */ )
{
    m_needVertexUpdate = true;
    // NOTE: this is probably not needed as the above flag will trigger a dirty
    // setDirty();
}

void WROIBox::updateGFX()
{
    boost::unique_lock< boost::shared_mutex > lock;
    lock = boost::unique_lock< boost::shared_mutex >( m_updateLock );

    std::stringstream ss;
    ss << "ROIBox" << boxId << "";
    if( m_pickInfo.getName() == ss.str() )
    {
        WVector2d newPixelPos( m_pickInfo.getPickPixel() );
        if( m_isPicked )
        {
            osg::Vec3 in( newPixelPos.x(), newPixelPos.y(), 0.0 );
            osg::Vec3 world = wge::unprojectFromScreen( in, m_viewer->getCamera() );

            // we want the vector pointing into the screen in world coordinates
            // NOTE: set w = 0 to use it as vector and ignore translation
            osg::Vec4 toDepth = wge::unprojectFromScreen( osg::Vec4( 0.0, 0.0, 1.0, 0.0 ), m_viewer->getCamera() );
            toDepth.normalize();
            WPosition toDepthWorld( toDepth[0], toDepth[1], toDepth[2] );

            float depthMove = m_pickInfo.getScrollWheel() - m_oldScrollWheel;

            WPosition newPixelWorldPos( world[0], world[1], world[2] );
            WPosition oldPixelWorldPos;
            if(  m_oldPixelPosition.x() == 0 && m_oldPixelPosition.y() == 0 )
            {
                oldPixelWorldPos = newPixelWorldPos;
            }
            else
            {
                osg::Vec3 in( m_oldPixelPosition.x(), m_oldPixelPosition.y(), 0.0 );
                osg::Vec3 world = wge::unprojectFromScreen( in, m_viewer->getCamera() );
                oldPixelWorldPos = WPosition( world[0], world[1], world[2] );
            }

            WVector3d moveVec = newPixelWorldPos - oldPixelWorldPos;

            // resize Box
            if( m_pickInfo.getModifierKey() == WPickInfo::SHIFT )
            {
                if( m_pickNormal[0] <= 0 && m_pickNormal[1] <= 0 && m_pickNormal[2] <= 0 )
                {
                    m_maxPos->set( m_maxPos->get() + ( m_pickNormal * dot( moveVec, m_pickNormal ) ) );
                }
                if( m_pickNormal[0] >= 0 && m_pickNormal[1] >= 0 && m_pickNormal[2] >= 0 )
                {
                    m_minPos->set( m_minPos->get() + ( m_pickNormal * dot( moveVec, m_pickNormal ) ) );
                }
                // NOTE: this sets m_needVertexUpdate
            }

            // move Box
            if( m_pickInfo.getModifierKey() == WPickInfo::NONE )
            {
                m_minPos->set( m_minPos->get() + moveVec + ( 2.0 * toDepthWorld * depthMove ) );
                m_maxPos->set( m_maxPos->get() + moveVec + ( 2.0 * toDepthWorld * depthMove ) );
                // NOTE: this sets m_needVertexUpdate
            }
        }
        else
        {
            m_pickNormal = m_pickInfo.getPickNormal();
            // color for moving box
            if( m_pickInfo.getModifierKey() == WPickInfo::NONE )
            {
                osg::ref_ptr<osg::Vec4Array> colors = osg::ref_ptr<osg::Vec4Array>( new osg::Vec4Array );
                if( m_not->get() )
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
                colors->push_back( osg::Vec4( 0.0f, 1.0f, 0.0f, 0.4f ) );
                m_surfaceGeometry->setColorArray( colors );
            }

            m_oldScrollWheel = m_pickInfo.getScrollWheel();
        }
        m_oldPixelPosition = newPixelPos;
        setDirty();
        m_isPicked = true;
        m_oldScrollWheel = m_pickInfo.getScrollWheel();
    }
    if( m_isPicked && m_pickInfo.getName() == "unpick" )
    {
        // Perform all actions necessary for finishing a pick

        osg::ref_ptr<osg::Vec4Array> colors = osg::ref_ptr<osg::Vec4Array>( new osg::Vec4Array );
        if( m_not->get() )
        {
            colors->push_back( m_notColor );
        }
        else
        {
            colors->push_back( m_color );
        }
        m_surfaceGeometry->setColorArray( colors );
        m_pickNormal = WVector3d();
        m_isPicked = false;
    }

    if( m_needVertexUpdate )
    {
        osg::ref_ptr<osg::Vec3Array> vertices = osg::ref_ptr<osg::Vec3Array>( new osg::Vec3Array );
        setVertices( vertices, m_minPos->get(), m_maxPos->get() );
        m_surfaceGeometry->setVertexArray( vertices );

        m_lightShader->apply( this );

        setDirty();
        m_needVertexUpdate = false;
    }

    if( m_dirty->get() )
    {
        osg::ref_ptr<osg::Vec4Array> colors = osg::ref_ptr<osg::Vec4Array>( new osg::Vec4Array );
        if( m_not->get() )
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
