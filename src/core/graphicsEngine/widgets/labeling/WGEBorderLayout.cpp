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

#include <osgText/Text>
#include <osg/LineWidth>
#include <osg/Geometry>

#include "../../WGraphicsEngine.h"

#include "WGEBorderLayout.h"

WGEBorderLayout::WGEBorderLayout():
    WGEGroupNode(),
    m_geode( new osg::Geode() ),
    m_lineGeode( new osg::Geode() ),
    m_screen( new osg::Projection() )

{
    // initialize members
    osg::ref_ptr< osg::MatrixTransform > matrix = new osg::MatrixTransform();
    setDataVariance( osg::Object::DYNAMIC );
    matrix->setMatrix( osg::Matrix::identity() );
    matrix->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
    matrix->addChild( m_geode );
    matrix->addChild( m_lineGeode );
    m_screen->addChild( matrix );
    insert( m_screen );

    m_geode->setDataVariance( osg::Object::DYNAMIC );
    m_lineGeode->setDataVariance( osg::Object::DYNAMIC );
    m_screen->setDataVariance( osg::Object::DYNAMIC );

    addUpdateCallback( new SafeUpdateCallback( this ) );

    // ensure it is drawn the last
    getOrCreateStateSet()->setRenderBinDetails( 11, "RenderBin" );
    getOrCreateStateSet()->setDataVariance( osg::Object::DYNAMIC );
    getOrCreateStateSet()->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF );
    getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
}

WGEBorderLayout::~WGEBorderLayout()
{
    // cleanup
}

void WGEBorderLayout::addLayoutable( osg::ref_ptr< WGELabel > obj )
{
    m_geode->addDrawable( obj );
}

void WGEBorderLayout::SafeUpdateCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    osg::ref_ptr<osg::Camera> cam = WGraphicsEngine::getGraphicsEngine()->getViewer()->getCamera();

    // set up projection
    unsigned int  vwidth = cam->getViewport()->width();
    unsigned int  vheight = cam->getViewport()->height();
    m_layouter->m_screen->setMatrix( osg::Matrix::ortho2D( 0, vwidth, 0, vheight ) );

    // the geometry for the lead lines
    osg::ref_ptr< osg::Vec3Array > v = new osg::Vec3Array;

    for( unsigned int i = 0; i < m_layouter->m_geode->getNumDrawables(); ++i )
    {
        // each drawable is a WGELabel:
        osg::ref_ptr< WGELabel > t = static_cast< WGELabel* >( m_layouter->m_geode->getDrawable( i ) );

        // get anchor position in screen space for this item
        osg::Vec4 anchor = osg::Vec4( t->getAnchor(), 1.0 );
        osg::Matrixd projection = cam->getProjectionMatrix();
        osg::Matrixd view = cam->getViewMatrix();
        osg::Matrixd window = cam->getViewport()->computeWindowMatrix();
        osg::Vec4 anchorScreen = anchor * view * projection * window;

        // is the anchor on this or the other side of the screen?
        //int b = static_cast< int >( anchorScreen.y() / vheight * 10.0  ) % 10;

        // draw a line
        osg::Vec3 leadPos;

        if( anchorScreen.x() >= vwidth / 2 )
        {
            leadPos = osg::Vec3( vwidth - 10.0, anchorScreen.y(), 0.0 );

            t->setPosition( osg::Vec3( vwidth - 10.0, anchorScreen.y() + 5, 0.0 ) );
            t->setAlignment( osgText::Text::RIGHT_BOTTOM );
        }
        else
        {
            leadPos = osg::Vec3( 10.0, anchorScreen.y(), 0.0 );

            t->setPosition( osg::Vec3( 10.0, anchorScreen.y() + 5, 0.0 ) );
            t->setAlignment( osgText::Text::LEFT_BOTTOM );
        }

        v->push_back( leadPos );
        v->push_back( osg::Vec3( anchorScreen.x(), anchorScreen.y(), anchorScreen.z() ) );
    }

    // create geometry for the lines calculated above
    osg::ref_ptr< osg::Geometry > g = new osg::Geometry;
    g->setDataVariance( osg::Object::DYNAMIC );
    osg::ref_ptr< osg::DrawArrays > da = new osg::DrawArrays( osg::PrimitiveSet::LINES, 0, v->size() );
    g->setVertexArray( v );
    osg::ref_ptr< osg::Vec4Array > colors = new osg::Vec4Array;
    colors->push_back( osg::Vec4( 0.0f, 0.0f, 0.0f, 1.0f ) );
    g->setColorArray( colors );
    g->setColorBinding( osg::Geometry::BIND_OVERALL );
    g->addPrimitiveSet( da );

    osg::LineWidth* linewidth = new osg::LineWidth();
    linewidth->setWidth( 2.0f );
    g->getOrCreateStateSet()->setAttributeAndModes( linewidth, osg::StateAttribute::ON );

    // remove all previous drawables and insert new
    m_layouter->m_lineGeode->removeDrawables( 0, m_layouter->m_lineGeode->getNumDrawables() );
    m_layouter->m_lineGeode->addDrawable( g );

    traverse( node, nv );
}

