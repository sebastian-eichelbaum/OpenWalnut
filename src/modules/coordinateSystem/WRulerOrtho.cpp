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

#include <osg/Geometry>

#include "WRulerOrtho.h"

WRulerOrtho::WRulerOrtho() :
    WRuler()
{
}

WRulerOrtho::~WRulerOrtho()
{
}

void WRulerOrtho::create( osg::Vec3 start, float length, scaleMode mode )
{
    osg::ref_ptr< osg::Geometry > geometry = osg::ref_ptr< osg::Geometry >( new osg::Geometry() );

    osg::Vec3Array* vertices = new osg::Vec3Array;

    vertices->push_back( start );

    int startX = static_cast< int > ( start.x() + 0.5 );
    int startY = static_cast< int > ( start.y() + 0.5 );
    int startZ = static_cast< int > ( start.z() + 0.5 );

    switch ( mode )
    {
        case RULER_ALONG_X_AXIS_SCALE_Y:
        {
            vertices->push_back( osg::Vec3( start.x() + length, start.y(), start.z() ) );

            for ( int i = startX; i <= static_cast< int > ( start.x() + length ); ++i )
            {
                if ( i % 10 == 0 )
                {
                    vertices->push_back( osg::Vec3( i, start.y() - 3.0, start.z() ) );
                    vertices->push_back( osg::Vec3( i, start.y() + 3.0, start.z() ) );
                }
                else if ( i % 5 == 0 )
                {
                    vertices->push_back( osg::Vec3( i, start.y() - 2.0, start.z() ) );
                    vertices->push_back( osg::Vec3( i, start.y() + 2.0, start.z() ) );
                }
                else
                {
                    vertices->push_back( osg::Vec3( i, start.y() - 1.0, start.z() ) );
                    vertices->push_back( osg::Vec3( i, start.y() + 1.0, start.z() ) );
                }
            }

            geometry->setVertexArray( vertices );

            osg::DrawElementsUInt* lines = new osg::DrawElementsUInt( osg::PrimitiveSet::LINES, 0 );

            lines->push_back( 0 );
            lines->push_back( 1 );

            for ( size_t i = 2; i < vertices->size(); ++i )
            {
                lines->push_back( i );
            }

            geometry->addPrimitiveSet( lines );

            this->addDrawable( geometry );
        }
            break;

        case RULER_ALONG_X_AXIS_SCALE_Z:
        {
            vertices->push_back( osg::Vec3( start.x() + length, start.y(), start.z() ) );

            for ( int i = startX; i <= static_cast< int > ( start.x() + length ); ++i )
            {
                if ( i % 10 == 0 )
                {
                    vertices->push_back( osg::Vec3( i, start.y(), start.z() - 3.0 ) );
                    vertices->push_back( osg::Vec3( i, start.y(), start.z() + 3.0 ) );
                }
                else if ( i % 5 == 0 )
                {
                    vertices->push_back( osg::Vec3( i, start.y(), start.z() - 2.0 ) );
                    vertices->push_back( osg::Vec3( i, start.y(), start.z() + 2.0 ) );
                }
                else
                {
                    vertices->push_back( osg::Vec3( i, start.y(), start.z() - 1.0 ) );
                    vertices->push_back( osg::Vec3( i, start.y(), start.z() + 1.0 ) );
                }
            }

            geometry->setVertexArray( vertices );

            osg::DrawElementsUInt* lines = new osg::DrawElementsUInt( osg::PrimitiveSet::LINES, 0 );

            lines->push_back( 0 );
            lines->push_back( 1 );

            for ( size_t i = 2; i < vertices->size(); ++i )
            {
                lines->push_back( i );
            }

            geometry->addPrimitiveSet( lines );

            this->addDrawable( geometry );
        }
            break;

        case RULER_ALONG_Y_AXIS_SCALE_X:
        {
            vertices->push_back( osg::Vec3( start.x(), start.y() + length, start.z() ) );

            for ( int i = startY; i <= static_cast< int > ( start.y() + length ); ++i )
            {
                if ( i % 10 == 0 )
                {
                    vertices->push_back( osg::Vec3( start.x() - 3.0, i, start.z() ) );
                    vertices->push_back( osg::Vec3( start.x() + 3.0, i, start.z() ) );
                }
                else if ( i % 5 == 0 )
                {
                    vertices->push_back( osg::Vec3( start.x() - 2.0, i, start.z() ) );
                    vertices->push_back( osg::Vec3( start.x() + 2.0, i, start.z() ) );
                }
                else
                {
                    vertices->push_back( osg::Vec3( start.x() - 1.0, i, start.z() ) );
                    vertices->push_back( osg::Vec3( start.x() + 1.0, i, start.z() ) );
                }
            }

            geometry->setVertexArray( vertices );

            osg::DrawElementsUInt* lines = new osg::DrawElementsUInt( osg::PrimitiveSet::LINES, 0 );

            lines->push_back( 0 );
            lines->push_back( 1 );

            for ( size_t i = 2; i < vertices->size(); ++i )
            {
                lines->push_back( i );
            }

            geometry->addPrimitiveSet( lines );

            this->addDrawable( geometry );
        }
            break;

        case RULER_ALONG_Y_AXIS_SCALE_Z:
        {
            vertices->push_back( osg::Vec3( start.x(), start.y() + length, start.z() ) );

            for ( int i = startY; i <= static_cast< int > ( start.y() + length ); ++i )
            {
                if ( i % 10 == 0 )
                {
                    vertices->push_back( osg::Vec3( start.x(), i, start.z() - 3.0 ) );
                    vertices->push_back( osg::Vec3( start.x(), i, start.z() + 3.0 ) );
                }
                else if ( i % 5 == 0 )
                {
                    vertices->push_back( osg::Vec3( start.x(), i, start.z() - 2.0 ) );
                    vertices->push_back( osg::Vec3( start.x(), i, start.z() + 2.0 ) );
                }
                else
                {
                    vertices->push_back( osg::Vec3( start.x(), i, start.z() - 1.0 ) );
                    vertices->push_back( osg::Vec3( start.x(), i, start.z() + 1.0 ) );
                }
            }

            geometry->setVertexArray( vertices );

            osg::DrawElementsUInt* lines = new osg::DrawElementsUInt( osg::PrimitiveSet::LINES, 0 );

            lines->push_back( 0 );
            lines->push_back( 1 );

            for ( size_t i = 2; i < vertices->size(); ++i )
            {
                lines->push_back( i );
            }

            geometry->addPrimitiveSet( lines );

            this->addDrawable( geometry );
        }
            break;

        case RULER_ALONG_Z_AXIS_SCALE_X:
        {
            vertices->push_back( osg::Vec3( start.x(), start.y(), start.z() + length ) );

            for ( int i = startZ; i <= static_cast< int > ( start.z() + length ); ++i )
            {
                if ( i % 10 == 0 )
                {
                    vertices->push_back( osg::Vec3( start.x() - 3.0, start.y(), i ) );
                    vertices->push_back( osg::Vec3( start.x() + 3.0, start.y(), i ) );
                }
                else if ( i % 5 == 0 )
                {
                    vertices->push_back( osg::Vec3( start.x() - 2.0, start.y(), i ) );
                    vertices->push_back( osg::Vec3( start.x() + 2.0, start.y(), i ) );
                }
                else
                {
                    vertices->push_back( osg::Vec3( start.x() - 1.0, start.y(), i ) );
                    vertices->push_back( osg::Vec3( start.x() + 1.0, start.y(), i ) );
                }
            }

            geometry->setVertexArray( vertices );

            osg::DrawElementsUInt* lines = new osg::DrawElementsUInt( osg::PrimitiveSet::LINES, 0 );

            lines->push_back( 0 );
            lines->push_back( 1 );

            for ( size_t i = 2; i < vertices->size(); ++i )
            {
                lines->push_back( i );
            }

            geometry->addPrimitiveSet( lines );

            this->addDrawable( geometry );
        }
            break;

        case RULER_ALONG_Z_AXIS_SCALE_Y:
        {
            vertices->push_back( osg::Vec3( start.x(), start.y(), start.z() + length ) );

            for ( int i = startZ; i <= static_cast< int > ( start.z() + length ); ++i )
            {
                if ( i % 10 == 0 )
                {
                    vertices->push_back( osg::Vec3( start.x(), start.y() - 3.0, i ) );
                    vertices->push_back( osg::Vec3( start.x(), start.y() + 3.0, i ) );
                }
                else if ( i % 5 == 0 )
                {
                    vertices->push_back( osg::Vec3( start.x(), start.y() - 2.0, i ) );
                    vertices->push_back( osg::Vec3( start.x(), start.y() + 2.0, i ) );
                }
                else
                {
                    vertices->push_back( osg::Vec3( start.x(), start.y() - 1.0, i ) );
                    vertices->push_back( osg::Vec3( start.x(), start.y() + 1.0, i ) );
                }
            }

            geometry->setVertexArray( vertices );

            osg::DrawElementsUInt* lines = new osg::DrawElementsUInt( osg::PrimitiveSet::LINES, 0 );

            lines->push_back( 0 );
            lines->push_back( 1 );

            for ( size_t i = 2; i < vertices->size(); ++i )
            {
                lines->push_back( i );
            }

            geometry->addPrimitiveSet( lines );

            this->addDrawable( geometry );
        }
            break;

        default:
            break;
    }
}

