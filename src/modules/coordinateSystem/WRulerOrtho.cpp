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

#include <boost/lexical_cast.hpp>

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osgText/Text>

#include "core/common/WPathHelper.h"

#include "WRulerOrtho.h"

WRulerOrtho::WRulerOrtho( boost::shared_ptr<WCoordConverter>coordConverter, osg::Vec3 origin, scaleMode mode, bool showNumbers ) :
    WRuler(),
    m_coordConverter( coordConverter ),
    m_origin( origin ),
    m_scaleMode( mode ),
    m_showNumbers( showNumbers )
{
    m_lb = m_coordConverter->getBoundingBox().getMin();
    m_ub = m_coordConverter->getBoundingBox().getMax();

    create();
}

WRulerOrtho::~WRulerOrtho()
{
}

void WRulerOrtho::create()
{
    osg::ref_ptr< osg::Geode > rulerGeode = osg::ref_ptr< osg::Geode >( new osg::Geode() );
    osg::ref_ptr< osg::Geometry > geometry;

    switch( m_scaleMode )
    {
        case RULER_ALONG_X_AXIS_SCALE_Y:
            geometry = createXY();
            break;
        case RULER_ALONG_X_AXIS_SCALE_Z:
            geometry = createXZ();
            break;
        case RULER_ALONG_Y_AXIS_SCALE_X:
            geometry = createYX();
            break;
        case RULER_ALONG_Y_AXIS_SCALE_Z:
            geometry = createYZ();
            break;
        case RULER_ALONG_Z_AXIS_SCALE_X:
            geometry = createZX();
            break;
        case RULER_ALONG_Z_AXIS_SCALE_Y:
            geometry = createZY();
            break;
        default:
            break;
    }

    osg::DrawElementsUInt* lines = new osg::DrawElementsUInt( osg::PrimitiveSet::LINES, 0 );
    for( size_t i = 0; i < geometry->getVertexArray()->getNumElements(); ++i )
    {
        lines->push_back( i );
    }
    geometry->addPrimitiveSet( lines );

    osg::StateSet* state = geometry->getOrCreateStateSet();
    state->setMode( GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );
    rulerGeode->addDrawable( geometry );
    this->addChild( rulerGeode );

    osg::StateSet* stateGroup = this->getOrCreateStateSet();
    stateGroup->setMode( GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );
}

void WRulerOrtho::addLabel( osg::Vec3 position, std::string text )
{
    osg::ref_ptr< osgText::Text > label = osg::ref_ptr< osgText::Text >( new osgText::Text() );
    osg::ref_ptr< osg::Geode > labelGeode = osg::ref_ptr< osg::Geode >( new osg::Geode() );

    labelGeode->addDrawable( label );

    // setup font
    label->setFont( WPathHelper::getAllFonts().Default.string() );
    label->setBackdropType( osgText::Text::OUTLINE );
    label->setCharacterSize( 6 );

    label->setText( text );
    label->setAxisAlignment( osgText::Text::SCREEN );
    label->setDrawMode( osgText::Text::TEXT );
    label->setAlignment( osgText::Text::CENTER_TOP );
    label->setPosition( osg::Vec3( 0.0, 0.0, 0.0 ) );
    label->setColor( osg::Vec4( 1.0f, 1.0f, 1.0f, 1.0f ) );

    osg::PositionAttitudeTransform* labelXform = new osg::PositionAttitudeTransform();
    labelXform->setPosition( position );

    this->addChild( labelXform );
    labelXform->addChild( labelGeode );
}

osg::ref_ptr< osg::Geometry > WRulerOrtho::createXY()
{
    osg::ref_ptr< osg::Geometry > geometry = osg::ref_ptr< osg::Geometry >( new osg::Geometry() );
    osg::Vec3Array* vertices = new osg::Vec3Array;

    int startX = static_cast< int > ( m_lb[0] + 0.5 );

    switch( m_coordConverter->getCoordinateSystemMode() )
    {
        case CS_WORLD:
        case CS_CANONICAL:
        {
            vertices->push_back( osg::Vec3( m_lb[0], m_origin.y(), m_origin.z() ) );
            vertices->push_back( osg::Vec3( m_ub[0], m_origin.y(), m_origin.z() ) );

            for( int i = startX; i <= static_cast< int > ( m_ub[0] ); ++i )
            {
                float rlength = 1.0;
                if( m_coordConverter->numberToCsX( i ) % 10 == 0 )
                {
                    rlength += 1.0;
                    if( m_showNumbers )
                    {
                        addLabel( osg::Vec3( i, m_origin.y(), m_origin.z() ), numberToString( i ) );
                    }
                }
                if( m_coordConverter->numberToCsX( i ) % 5 == 0 )
                {
                    rlength += 1.0;
                }
                vertices->push_back( osg::Vec3( i, m_origin.y() - rlength, m_origin.z() ) );
                vertices->push_back( osg::Vec3( i, m_origin.y() + rlength, m_origin.z() ) );
            }
        }
            break;
        case CS_TALAIRACH:
        {
            WVector3d origin_t = m_coordConverter->w2t( WVector3d( m_origin[0], m_origin[1], m_origin[2] ) );

            WVector3d pA = m_coordConverter->t2w( WVector3d( origin_t[0], -70, origin_t[2] ) );
            WVector3d pO = m_coordConverter->t2w( WVector3d( origin_t[0], +70, origin_t[2] ) );

            vertices->push_back( osg::Vec3( pA[0], pA[1], m_origin.z() ) );
            vertices->push_back( osg::Vec3( pO[0], pO[1], m_origin.z() ) );

            for( int i = -70; i <= 71; ++i )
            {
                WVector3d tmpPoint = m_coordConverter->t2w( WVector3d( origin_t[0], i, origin_t[2] ) );
                float rlength = 1.0;
                if( i % 10 == 0 )
                {
                    rlength += 1.0;
                    if( m_showNumbers )
                    {
                        addLabel( osg::Vec3( tmpPoint[0], tmpPoint[1], m_origin.z() ), boost::lexical_cast< std::string >( i ) );
                    }
                }
                if( i % 5 == 0 )
                {
                    rlength += 1.0;
                }
                WVector3d p1 = m_coordConverter->t2w( WVector3d( origin_t[0] - rlength, i, origin_t[2] ) );
                WVector3d p2 = m_coordConverter->t2w( WVector3d( origin_t[0] + rlength, i, origin_t[2] ) );

                vertices->push_back( osg::Vec3( p1[0], p1[1], m_origin.z() ) );
                vertices->push_back( osg::Vec3( p2[0], p2[1], m_origin.z() ) );
            }
        }
    }
    geometry->setVertexArray( vertices );

    return geometry;
}

osg::ref_ptr< osg::Geometry > WRulerOrtho::createXZ()
{
    osg::ref_ptr< osg::Geometry > geometry = osg::ref_ptr< osg::Geometry >( new osg::Geometry() );
    osg::Vec3Array* vertices = new osg::Vec3Array;

    int startX = static_cast< int > ( m_lb[0] + 0.5 );

    switch( m_coordConverter->getCoordinateSystemMode() )
    {
        case CS_WORLD:
        case CS_CANONICAL:
        {
            vertices->push_back( osg::Vec3( m_lb[0], m_origin.y(), m_origin.z() ) );
            vertices->push_back( osg::Vec3( m_ub[0], m_origin.y(), m_origin.z() ) );

            for( int i = startX; i <= static_cast< int > ( m_ub[0] ); ++i )
            {
                float rlength = 1.0;
                if( m_coordConverter->numberToCsX( i ) % 10 == 0 )
                {
                    rlength += 1.0;
                    if( m_showNumbers )
                    {
                        addLabel( osg::Vec3( i, m_origin.y(), m_origin.z() ), numberToString( i ) );
                    }
                }
                if( m_coordConverter->numberToCsX( i ) % 5 == 0 )
                {
                    rlength += 1.0;
                }
                vertices->push_back( osg::Vec3( i, m_origin.y(), m_origin.z() - rlength ) );
                vertices->push_back( osg::Vec3( i, m_origin.y(), m_origin.z() + rlength ) );
            }
        }
            break;
        case CS_TALAIRACH:
        {
            WVector3d origin_t = m_coordConverter->w2t( WVector3d( m_origin[0], m_origin[1], m_origin[2] ) );

            WVector3d pA = m_coordConverter->t2w( WVector3d( origin_t[0], -70, origin_t[2] ) );
            WVector3d pO = m_coordConverter->t2w( WVector3d( origin_t[0], +70, origin_t[2] ) );

            vertices->push_back( osg::Vec3( pA[0], m_origin.y(), pA[2] ) );
            vertices->push_back( osg::Vec3( pO[0], m_origin.y(), pO[2] ) );

            for( int i = -70; i <= 71; ++i )
            {
                WVector3d tmpPoint = m_coordConverter->t2w( WVector3d( origin_t[0], i, origin_t[2] ) );
                float rlength = 1.0;
                if( i % 10 == 0 )
                {
                    rlength += 1.0;
                    if( m_showNumbers )
                    {
                        addLabel( osg::Vec3( tmpPoint[0], m_origin.y(), tmpPoint[2] ), boost::lexical_cast< std::string >( i ) );
                    }
                }
                if( i % 5 == 0 )
                {
                    rlength += 1.0;
                }
                WVector3d p1 = m_coordConverter->t2w( WVector3d( origin_t[0], i, origin_t[2] - rlength ) );
                WVector3d p2 = m_coordConverter->t2w( WVector3d( origin_t[0], i, origin_t[2] + rlength ) );

                vertices->push_back( osg::Vec3( p1[0], m_origin.y(), p1[2] ) );
                vertices->push_back( osg::Vec3( p2[0], m_origin.y(), p2[2] ) );
            }
        }
    }
    geometry->setVertexArray( vertices );
    return geometry;
}

osg::ref_ptr< osg::Geometry > WRulerOrtho::createYX()
{
    osg::ref_ptr< osg::Geometry > geometry = osg::ref_ptr< osg::Geometry >( new osg::Geometry() );
    osg::Vec3Array* vertices = new osg::Vec3Array;

    int startY = static_cast< int > ( m_lb[1] + 0.5 );

    switch( m_coordConverter->getCoordinateSystemMode() )
    {
        case CS_WORLD:
        case CS_CANONICAL:
        {
            vertices->push_back( osg::Vec3( m_origin.x(), m_lb[1], m_origin.z() ) );
            vertices->push_back( osg::Vec3( m_origin.x(), m_ub[1], m_origin.z() ) );

            for( int i = startY; i <= static_cast< int > ( m_ub[1] ); ++i )
            {
                float rlength = 1.0;
                if( m_coordConverter->numberToCsY( i ) % 10 == 0 )
                {
                    rlength += 1.0;
                    if( m_showNumbers )
                    {
                        addLabel( osg::Vec3( m_origin.x(), i, m_origin.z() ), numberToString( i ) );
                    }
                }
                if( m_coordConverter->numberToCsY( i ) % 5 == 0 )
                {
                    rlength += 1.0;
                }
                vertices->push_back( osg::Vec3( m_origin.x() - rlength, i, m_origin.z() ) );
                vertices->push_back( osg::Vec3( m_origin.x() + rlength, i, m_origin.z() ) );
            }
        }
            break;
        case CS_TALAIRACH:
        {
            WVector3d origin_t = m_coordConverter->w2t( WVector3d( m_origin[0], m_origin[1], m_origin[2] ) );

            WVector3d pA = m_coordConverter->t2w( WVector3d( -100, origin_t[1], origin_t[2] ) );
            WVector3d pO = m_coordConverter->t2w( WVector3d( 80, origin_t[1], origin_t[2] ) );

            vertices->push_back( osg::Vec3( pA[0], pA[1], m_origin.z() ) );
            vertices->push_back( osg::Vec3( pO[0], pO[1], m_origin.z() ) );

            for( int i = -100; i <= 81; ++i )
            {
                WVector3d tmpPoint = m_coordConverter->t2w( WVector3d( i, origin_t[1], origin_t[2] ) );
                float rlength = 1.0;
                if( i % 10 == 0 )
                {
                    rlength += 1.0;
                    if( m_showNumbers )
                    {
                        addLabel( osg::Vec3( tmpPoint[0], tmpPoint[1], m_origin.z() ), boost::lexical_cast< std::string >( i ) );
                    }
                }
                if( i % 5 == 0 )
                {
                    rlength += 1.0;
                }
                WVector3d p1 = m_coordConverter->t2w( WVector3d( i, origin_t[1] - rlength, origin_t[2] ) );
                WVector3d p2 = m_coordConverter->t2w( WVector3d( i, origin_t[1] + rlength, origin_t[2] ) );

                vertices->push_back( osg::Vec3( p1[0], p1[1], m_origin.z() ) );
                vertices->push_back( osg::Vec3( p2[0], p2[1], m_origin.z() ) );
            }
        }
            break;
    }
    geometry->setVertexArray( vertices );

    return geometry;
}

osg::ref_ptr< osg::Geometry > WRulerOrtho::createYZ()
{
    osg::ref_ptr< osg::Geometry > geometry = osg::ref_ptr< osg::Geometry >( new osg::Geometry() );
    osg::Vec3Array* vertices = new osg::Vec3Array;

    int startY = static_cast< int > ( m_lb[1] + 0.5 );

    switch( m_coordConverter->getCoordinateSystemMode() )
    {
        case CS_WORLD:
        case CS_CANONICAL:
        {
            vertices->push_back( osg::Vec3( m_origin.x(), m_lb[1], m_origin.z() ) );
            vertices->push_back( osg::Vec3( m_origin.x(), m_ub[1], m_origin.z() ) );

            for( int i = startY; i <= static_cast< int > ( m_ub[1] ); ++i )
            {
                float rlength = 1.0;
                if( m_coordConverter->numberToCsY( i ) % 10 == 0 )
                {
                    rlength += 1.0;
                    if( m_showNumbers )
                    {
                        addLabel( osg::Vec3( m_origin.x(), i, m_origin.z() ), numberToString( i ) );
                    }
                }
                if( m_coordConverter->numberToCsY( i ) % 5 == 0 )
                {
                    rlength += 1.0;
                }
                vertices->push_back( osg::Vec3( m_origin.x(), i, m_origin.z() - rlength ) );
                vertices->push_back( osg::Vec3( m_origin.x(), i, m_origin.z() + rlength ) );
            }
        }
            break;
        case CS_TALAIRACH:
        {
            WVector3d origin_t = m_coordConverter->w2t( WVector3d( m_origin[0], m_origin[1], m_origin[2] ) );

            WVector3d pA = m_coordConverter->t2w( WVector3d( -100, origin_t[1], origin_t[2] ) );
            WVector3d pO = m_coordConverter->t2w( WVector3d( 80, origin_t[1], origin_t[2] ) );

            vertices->push_back( osg::Vec3( m_origin.x(), pA[1], pA[2] ) );
            vertices->push_back( osg::Vec3( m_origin.x(), pO[1], pO[2] ) );

            for( int i = -100; i <= 81; ++i )
            {
                WVector3d tmpPoint = m_coordConverter->t2w( WVector3d( i, 0, origin_t[2] ) );
                float rlength = 1.0;
                if( i % 10 == 0 )
                {
                    rlength += 1.0;
                    if( m_showNumbers )
                    {
                        addLabel( osg::Vec3( m_origin.x(), tmpPoint[1], tmpPoint[2] ), boost::lexical_cast< std::string >( i ) );
                    }
                }
                if( i % 5 == 0 )
                {
                    rlength += 1.0;
                }
                WVector3d p1 = m_coordConverter->t2w( WVector3d( i, origin_t[1], origin_t[2] - rlength ) );
                WVector3d p2 = m_coordConverter->t2w( WVector3d( i, origin_t[1], origin_t[2] + rlength ) );

                vertices->push_back( osg::Vec3( m_origin.x(), p1[1], p1[2] ) );
                vertices->push_back( osg::Vec3( m_origin.x(), p2[1], p2[2] ) );
            }
        }
            break;
    }
    geometry->setVertexArray( vertices );

    return geometry;
}

osg::ref_ptr< osg::Geometry > WRulerOrtho::createZX()
{
    osg::ref_ptr< osg::Geometry > geometry = osg::ref_ptr< osg::Geometry >( new osg::Geometry() );
    osg::Vec3Array* vertices = new osg::Vec3Array;

    int startZ = static_cast< int > ( m_lb[2] + 0.5 );

    switch( m_coordConverter->getCoordinateSystemMode() )
    {
        case CS_WORLD:
        case CS_CANONICAL:
        {
            vertices->push_back( osg::Vec3( m_origin.x(), m_origin.y(), m_lb[2] ) );
            vertices->push_back( osg::Vec3( m_origin.x(), m_origin.y(), m_ub[2] ) );

            for( int i = startZ; i <= static_cast< int > ( m_ub[2] ); ++i )
            {
                float rlength = 1.0;
                if( m_coordConverter->numberToCsZ( i ) % 10 == 0 )
                {
                    rlength += 1.0;
                    if( m_showNumbers )
                    {
                        addLabel( osg::Vec3( m_origin.x(), m_origin.y(), i ), numberToString( i ) );
                    }
                }
                if( m_coordConverter->numberToCsZ( i ) % 5 == 0 )
                {
                    rlength += 1.0;
                }
                vertices->push_back( osg::Vec3( m_origin.x() - rlength, m_origin.y(), i ) );
                vertices->push_back( osg::Vec3( m_origin.x() + rlength, m_origin.y(), i ) );
            }
        }
            break;
        case CS_TALAIRACH:
        {
            WVector3d origin_t = m_coordConverter->w2t( WVector3d( m_origin[0], m_origin[1], m_origin[2] ) );

            WVector3d pA = m_coordConverter->t2w( WVector3d( origin_t[0], origin_t[1], -50 ) );
            WVector3d pO = m_coordConverter->t2w( WVector3d( origin_t[0], origin_t[1], 80 ) );

            vertices->push_back( osg::Vec3( pA[0], m_origin.y(), pA[2] ) );
            vertices->push_back( osg::Vec3( pO[0], m_origin.y(), pO[2] ) );

            for( int i = -50; i <= 81; ++i )
            {
                WVector3d tmpPoint = m_coordConverter->t2w( WVector3d( origin_t[0], origin_t[1], i ) );
                float rlength = 1.0;
                if( i % 10 == 0 )
                {
                    rlength += 1.0;
                    if( m_showNumbers )
                    {
                        addLabel( osg::Vec3( tmpPoint[0], m_origin.y(), tmpPoint[2] ), boost::lexical_cast< std::string >( i ) );
                    }
                }
                if( i % 5 == 0 )
                {
                    rlength += 1.0;
                }
                WVector3d p1 = m_coordConverter->t2w( WVector3d( origin_t[0], origin_t[1] - rlength, i ) );
                WVector3d p2 = m_coordConverter->t2w( WVector3d( origin_t[0], origin_t[1] + rlength, i ) );

                vertices->push_back( osg::Vec3( p1[0], m_origin.y(), p1[2] ) );
                vertices->push_back( osg::Vec3( p2[0], m_origin.y(), p2[2] ) );
            }
        }
            break;
    }
    geometry->setVertexArray( vertices );

    return geometry;
}

osg::ref_ptr< osg::Geometry > WRulerOrtho::createZY()
{
    osg::ref_ptr< osg::Geometry > geometry = osg::ref_ptr< osg::Geometry >( new osg::Geometry() );
    osg::Vec3Array* vertices = new osg::Vec3Array;

    int startZ = static_cast< int > ( m_lb[2] + 0.5 );

    switch( m_coordConverter->getCoordinateSystemMode() )
    {
        case CS_WORLD:
        case CS_CANONICAL:
        {
            vertices->push_back( osg::Vec3( m_origin.x(), m_origin.y(), m_lb[2] ) );
            vertices->push_back( osg::Vec3( m_origin.x(), m_origin.y(), m_ub[2] ) );

            for( int i = startZ; i <= static_cast< int > ( m_ub[2] ); ++i )
            {
                float rlength = 1.0;
                if( m_coordConverter->numberToCsZ( i ) % 10 == 0 )
                {
                    rlength += 1.0;
                    if( m_showNumbers )
                    {
                        addLabel( osg::Vec3( m_origin.x(), m_origin.y(), i ), numberToString( i ) );
                    }
                }
                if( m_coordConverter->numberToCsZ( i ) % 5 == 0 )
                {
                    rlength += 1.0;
                }
                vertices->push_back( osg::Vec3( m_origin.x(), m_origin.y() - rlength, i ) );
                vertices->push_back( osg::Vec3( m_origin.x(), m_origin.y() + rlength, i ) );
            }
        }
            break;
        case CS_TALAIRACH:
        {
            WVector3d origin_t = m_coordConverter->w2t( WVector3d( m_origin[0], m_origin[1], m_origin[2] ) );

            WVector3d pA = m_coordConverter->t2w( WVector3d( origin_t[0], origin_t[1], -50 ) );
            WVector3d pO = m_coordConverter->t2w( WVector3d( origin_t[0], origin_t[1], 80 ) );

            vertices->push_back( osg::Vec3( m_origin.x(), pA[1], pA[2] ) );
            vertices->push_back( osg::Vec3( m_origin.x(), pO[1], pO[2] ) );

            for( int i = -50; i <= 81; ++i )
            {
                WVector3d tmpPoint = m_coordConverter->t2w( WVector3d( origin_t[0], 0, i ) );
                float rlength = 1.0;
                if( i % 10 == 0 )
                {
                    rlength += 1.0;
                    if( m_showNumbers )
                    {
                        addLabel( osg::Vec3( m_origin.x(), tmpPoint[1], tmpPoint[2] ), boost::lexical_cast< std::string >( i ) );
                    }
                }
                if( i % 5 == 0 )
                {
                    rlength += 1.0;
                }
                WVector3d p1 = m_coordConverter->t2w( WVector3d( origin_t[0] - rlength, origin_t[1], i ) );
                WVector3d p2 = m_coordConverter->t2w( WVector3d( origin_t[0] + rlength, origin_t[1], i ) );

                vertices->push_back( osg::Vec3( m_origin.x(), p1[1], p1[2] ) );
                vertices->push_back( osg::Vec3( m_origin.x(), p2[1], p2[2] ) );
            }
        }
            break;
    }
    geometry->setVertexArray( vertices );

    return geometry;
}

std::string WRulerOrtho::numberToString( int number )
{
    switch( m_scaleMode )
    {
        case RULER_ALONG_X_AXIS_SCALE_Y:
            return boost::lexical_cast< std::string >( m_coordConverter->numberToCsX( number ) );
            break;
        case RULER_ALONG_X_AXIS_SCALE_Z:
            return boost::lexical_cast< std::string >( m_coordConverter->numberToCsX( number ) );
            break;
        case RULER_ALONG_Y_AXIS_SCALE_X:
            return boost::lexical_cast< std::string >( m_coordConverter->numberToCsY( number ) );
            break;
        case RULER_ALONG_Y_AXIS_SCALE_Z:
            return boost::lexical_cast< std::string >( m_coordConverter->numberToCsY( number ) );
            break;
        case RULER_ALONG_Z_AXIS_SCALE_X:
            return boost::lexical_cast< std::string >( m_coordConverter->numberToCsZ( number ) );
            break;
        case RULER_ALONG_Z_AXIS_SCALE_Y:
            return boost::lexical_cast< std::string >( m_coordConverter->numberToCsZ( number ) );
            break;
        default:
            break;
    }
    return std::string( "" );
}
