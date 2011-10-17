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

#include <boost/shared_ptr.hpp>

#include <osg/Drawable>
#include <osg/ShapeDrawable>
#include <osg/ref_ptr>
#include <osgSim/ScalarsToColors>

#include "core/common/WFlag.h"
#include "core/common/WPropertyTypes.h"
#include "core/common/WPropertyVariable.h"
#include "WEEGEvent.h"
#include "WElectrodePositionCallback.h"


WElectrodePositionCallback::WElectrodePositionCallback( std::size_t channelID,
        WPropDouble colorSensitivity,
        boost::shared_ptr< WFlag< boost::shared_ptr< WEEGEvent > > > event,
        osg::ref_ptr< const osgSim::ScalarsToColors > colorMap )
    : m_channelID( channelID ),
      m_currentColorSensitivity( 0.0 ),
      m_currentTime( -2.0 ),
      m_colorSensitivity( colorSensitivity ),
      m_event( event ),
      m_colorMap( colorMap )
{
}

void WElectrodePositionCallback::update( osg::NodeVisitor* /*nv*/, osg::Drawable* drawable )
{
    const double colorSensitivity = m_colorSensitivity->get();
    boost::shared_ptr< WEEGEvent > event = m_event->get();
    const double time = event->getTime();
    if( colorSensitivity != m_currentColorSensitivity || time != m_currentTime )
    {
        osg::ShapeDrawable* shape = static_cast< osg::ShapeDrawable* >( drawable );
        if( shape )
        {
            // calculate color value between -1 and 1
            float color;
            if( 0.0 <= time )
            {
                color = event->getValues()[m_channelID] / colorSensitivity;
            }
            else
            {
                color = 0.0f;
            }

            shape->setColor( m_colorMap->getColor( color ) );
        }

        m_currentColorSensitivity = colorSensitivity;
        m_currentTime = time;
    }
}
