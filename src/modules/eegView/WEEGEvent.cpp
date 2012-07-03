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

#include <cstddef>

#include <limits>
#include <sstream>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <osg/Array>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/PrimitiveSet>
#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/ref_ptr>
#include <osgText/Text>

#include "core/common/exceptions/WOutOfBounds.h"
#include "core/common/WStringUtils.h"
#include "core/dataHandler/WDataSetDipoles.h"
#include "core/dataHandler/WEEG2.h"
#include "core/dataHandler/WEEG2Segment.h"
#include "core/dataHandler/WEEGValueMatrix.h"
#include "core/graphicsEngine/WGEGroupNode.h"
#include "WEEGEvent.h"


WEEGEvent::WEEGEvent( double time,
                      double yPos,
                      boost::shared_ptr< WEEG2 > eeg,
                      std::size_t segmentID,
                      osg::ref_ptr< WGEGroupNode > parentNode,
                      bool snapToDipole,
                      bool proofOfConcept,
                      boost::shared_ptr< WDataSetDipoles > dipoles ) throw( WOutOfBounds )
    : m_time( time ),
      m_parentNode( parentNode )
{
    if( segmentID < eeg->getNumberOfSegments() )
    {
        // snap to dipole
        if( snapToDipole && !proofOfConcept && dipoles.get() )
        {
            const double epsilon = 1e-4;
            double error = std::numeric_limits<double>::infinity();
            for( size_t dipoleId = 0u; dipoleId < dipoles->getNumberOfDipoles(); ++dipoleId )
            {
                if( time < dipoles->getStartTime( dipoleId ) )
                {
                    if( dipoles->getStartTime( dipoleId ) - time < error )
                    {
                        m_time = dipoles->getStartTime( dipoleId ) + epsilon;
                        error = dipoles->getStartTime( dipoleId ) - time;
                    }
                }
                else if( time > dipoles->getEndTime( dipoleId ) )
                {
                    if( time - dipoles->getEndTime( dipoleId ) < error )
                    {
                        m_time = dipoles->getEndTime( dipoleId ) - epsilon;
                        error = time - dipoles->getEndTime( dipoleId );
                    }
                }
                else
                {
                    m_time = time;
                    // error = 0.0;
                    break;
                }
            }
        }

        const double sampleIDAsDouble = m_time * eeg->getSamplingRate();

        boost::shared_ptr< WEEG2Segment > segment = eeg->getSegment( segmentID );
        if( 0.0 <= sampleIDAsDouble && sampleIDAsDouble < segment->getNumberOfSamples() - 1 )
        {
            // calculate value of each channel at the given time position using
            // linear interpolation
            const std::size_t sampleID = sampleIDAsDouble;
            const double fpart = sampleIDAsDouble - sampleID;
            boost::shared_ptr< WEEGValueMatrix > rawValues = segment->getValues( sampleID, 2 );
            m_values.reserve( eeg->getNumberOfChannels() );
            for( std::size_t channelID = 0; channelID < eeg->getNumberOfChannels(); ++channelID )
            {
                m_values.push_back( ( 1.0 - fpart ) * (*rawValues)[channelID][0] + fpart * (*rawValues)[channelID][1] );
            }

            if( m_parentNode.valid() )
            {
                const osg::Vec4 color( 1.0f, 0.5f, 0.5f, 1.0f );

                // create geode to draw the event as line
                osg::Geometry* geometry = new osg::Geometry;

                osg::Vec3Array* vertices = new osg::Vec3Array();
                vertices->reserve( 2 );
                vertices->push_back( osg::Vec3( m_time, -1048576.0f, 0.0f ) );
                vertices->push_back( osg::Vec3( m_time, 1024.0f, 0.0f ) );
                geometry->setVertexArray( vertices );

                osg::Vec4Array* colors = new osg::Vec4Array;
                colors->push_back( color );
                geometry->setColorArray( colors );
                geometry->setColorBinding( osg::Geometry::BIND_OVERALL );

                geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINES, 0, 2 ) );

                // create text for the time label
                osgText::Text* text = new osgText::Text;
                text->setText( string_utils::toString( m_time ).c_str() );
                text->setPosition( osg::Vec3( m_time, yPos + 10.0, 0.0 ) );
                text->setAlignment( osgText::Text::LEFT_CENTER );
                text->setAxisAlignment( osgText::Text::SCREEN );
                text->setCharacterSize( 12 );
                text->setCharacterSizeMode( osgText::Text::SCREEN_COORDS );
                text->setColor( osg::Vec4( 0.0, 0.0, 1.0, 1.0 ) );
                text->setBackdropType( osgText::Text::OUTLINE );
                text->setBackdropColor( osg::Vec4( 1.0, 1.0, 1.0, 1.0 ) );
                text->setBackdropOffset( 0.1 );

                osg::Geode* geode = new osg::Geode;
                geode->addDrawable( geometry );
                geode->addDrawable( text );


                m_node = geode;

                m_parentNode->insert( m_node );
            }
        }
        else
        {
            std::ostringstream stream;
            stream << "Invalid time for the EEG: " << m_time << "s";
            throw WOutOfBounds( stream.str() );
        }
    }
    else
    {
        std::ostringstream stream;
        stream << "The EEG has no segment number " << segmentID;
        throw WOutOfBounds( stream.str() );
    }
}

WEEGEvent::WEEGEvent()
    : m_time( -1.0 )
{
}

WEEGEvent::~WEEGEvent()
{
    if( m_parentNode.valid() && m_node.valid() )
    {
        m_parentNode->remove( m_node );
    }
}

double WEEGEvent::getTime() const
{
    return m_time;
}

const std::vector< double >& WEEGEvent::getValues() const
{
    return m_values;
}
