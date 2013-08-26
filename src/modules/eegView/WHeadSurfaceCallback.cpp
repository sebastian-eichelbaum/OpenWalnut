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

#include <vector>

#include <boost/shared_ptr.hpp>

#include <osg/Array>
#include <osg/Drawable>
#include <osg/Geometry>

#include "core/common/WFlag.h"
#include "core/common/WPropertyTypes.h"
#include "core/common/WPropertyVariable.h"
#include "WEEGEvent.h"
#include "WHeadSurfaceCallback.h"

// Compatibility between OSG 3.2 and earlier versions
#include "core/graphicsEngine/WOSG.h"

WHeadSurfaceCallback::WHeadSurfaceCallback( const std::vector< std::size_t >& channelIDs,
                                            WPropDouble colorSensitivity,
                                            boost::shared_ptr< WFlag< boost::shared_ptr< WEEGEvent > > > event )
    : m_currentColorSensitivity( 1.0 ),
      m_currentTime( -1.0 ),
      m_channelIDs( channelIDs ),
      m_colorSensitivity( colorSensitivity ),
      m_event( event )
{
}

void WHeadSurfaceCallback::update( osg::NodeVisitor* /*nv*/, osg::Drawable* drawable )
{
    const double colorSensitivity = m_colorSensitivity->get();
    boost::shared_ptr< WEEGEvent > event = m_event->get();
    const double time = event->getTime();
    if( colorSensitivity != m_currentColorSensitivity || time != m_currentTime )
    {
        wosg::Geometry* geometry = static_cast< wosg::Geometry* >( drawable );
        if( geometry )
        {
            osg::FloatArray* texCoords = static_cast< osg::FloatArray* >( geometry->getTexCoordArray( 0 ) );
            if( 0.0 <= time )
            {
                const std::size_t size = 256u;
                const std::vector< double >& values = event->getValues();
                for( std::size_t vertexID = 0; vertexID < texCoords->size(); ++vertexID )
                {
                    (*texCoords)[vertexID] = values[m_channelIDs[vertexID]] * ( 0.5 - 0.5 / size ) / colorSensitivity + 0.5;
                }
            }
            else
            {
                for( std::size_t vertexID = 0; vertexID < texCoords->size(); ++vertexID )
                {
                    (*texCoords)[vertexID] = 0.5f;
                }
            }

            geometry->setTexCoordArray( 0, texCoords );

            osg::Vec4Array* colors = new osg::Vec4Array;
            colors->push_back( osg::Vec4( 1.0f, 1.0f, 1.0f, 1.0f ) );
            geometry->setColorArray( colors );
            geometry->setColorBinding( wosg::Geometry::BIND_OVERALL );
        }

        m_currentColorSensitivity = colorSensitivity;
        m_currentTime = time;
    }
}
