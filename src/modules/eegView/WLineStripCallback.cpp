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

#include <cmath>
#include <cstddef>

#include <algorithm>

#include <boost/shared_ptr.hpp>

#include <osg/Array>
#include <osg/Drawable>
#include <osg/Geometry>
#include <osg/NodeVisitor>
#include <osg/PrimitiveSet>
#include <osg/Vec3>

#include "core/common/WPropertyTypes.h"
#include "core/common/WPropertyVariable.h"
#include "core/dataHandler/WEEG2Segment.h"
#include "core/dataHandler/WEEGValueMatrix.h"
#include "WLineStripCallback.h"


WLineStripCallback::WLineStripCallback( std::size_t channelID,
                                        WPropDouble timePos,
                                        WPropDouble timeRange,
                                        boost::shared_ptr< WEEG2Segment > segment,
                                        double samplingRate )
    : m_channelID( channelID ),
      m_currentTimePos( 0.0 ),
      m_currentTimeRange( -1.0 ),
      m_timePos( timePos ),
      m_timeRange( timeRange ),
      m_segment( segment ),
      m_samplingRate( samplingRate )
{
}

void WLineStripCallback::update( osg::NodeVisitor* /*nv*/, osg::Drawable* drawable )
{
    const double timePos = m_timePos->get();
    const double timeRange = m_timeRange->get();

    if( timePos != m_currentTimePos || timeRange != m_currentTimeRange )
    {
        osg::Geometry* geometry = static_cast< osg::Geometry* >( drawable );
        if( geometry )
        {
            const std::size_t nbSamples = m_segment->getNumberOfSamples();
            const std::size_t startSample = clampToRange( timePos * m_samplingRate, 0u, nbSamples - 1u );
            const std::size_t endSample = clampToRange( std::ceil( ( timePos + timeRange ) * m_samplingRate ) + 1.0,
                                                        startSample,
                                                        nbSamples );
            const std::size_t currentStartSample = clampToRange( m_currentTimePos * m_samplingRate, 0u, nbSamples - 1u );
            const std::size_t currentEndSample = ( 0.0 <= m_currentTimeRange ) ?
                    clampToRange( std::ceil( ( m_currentTimePos + m_currentTimeRange ) * m_samplingRate ) + 1.0,
                                  currentStartSample,
                                  nbSamples ) :
                    currentStartSample;

            osg::Vec3Array* vertices = new osg::Vec3Array();
            vertices->reserve( endSample - startSample );

            // load the values before the current vertices from WEEG
            std::size_t start = startSample;
            std::size_t end = std::min( endSample, currentStartSample );
            if( start < end )
            {
                const std::size_t length = end - start;
                boost::shared_ptr< const WEEGValueMatrix > values = m_segment->getValues( start, length );
                for( std::size_t i = 0; i < length; ++i )
                {
                    vertices->push_back( osg::Vec3( ( start + i ) / m_samplingRate, (*values)[m_channelID][i], -0.001 ) );
                }
            }

            // copy the values from the current vertices
            start = std::max( startSample, currentStartSample );
            end = std::min( endSample, currentEndSample );
            if( start < end )
            {
                const osg::Vec3Array* currentVertices = static_cast< osg::Vec3Array* >( geometry->getVertexArray() );
                vertices->insert( vertices->end(),
                                  currentVertices->begin() + ( start - currentStartSample ),
                                  currentVertices->begin() + ( end - currentStartSample ) );
            }

            // load the values after the current vertices from WEEG
            start = std::max( startSample, currentEndSample );
            end = endSample;
            if( start < end )
            {
                const std::size_t length = end - start;
                boost::shared_ptr< const WEEGValueMatrix > values = m_segment->getValues( start, length );
                for( std::size_t i = 0; i < length; ++i )
                {
                    vertices->push_back( osg::Vec3( ( start + i ) / m_samplingRate, (*values)[m_channelID][i], -0.001 ) );
                }
            }

            geometry->setVertexArray( vertices );

            osg::DrawArrays* primitiveSet = static_cast< osg::DrawArrays* >( geometry->getPrimitiveSet( 0 ) );
            primitiveSet->setCount( endSample - startSample );
        }

        m_currentTimePos = timePos;
        m_currentTimeRange = timeRange;
    }
}

std::size_t WLineStripCallback::clampToRange( double value, std::size_t min, std::size_t max ) const
{
    std::size_t out = ( value > 0.0 ) ? value : 0u;
    if( out < min )
    {
        out = min;
    }
    else if( max < out )
    {
        out = max;
    }

    return out;
}
