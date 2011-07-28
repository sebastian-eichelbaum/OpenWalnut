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

#ifndef WHEADSURFACECALLBACK_H
#define WHEADSURFACECALLBACK_H

#include <cstddef>

#include <vector>

#include <boost/shared_ptr.hpp>

#include <osg/Drawable>

#include "core/common/WFlag.h"
#include "core/common/WPropertyTypes.h"
#include "WEEGEvent.h"


/**
 * OSG Update Callback to change the color of a head surface by changing its 1D
 * texture coordinates when an event position changed.
 * \note Only add it to a Geometry with a 1D texture!
 */
class WHeadSurfaceCallback : public osg::Drawable::UpdateCallback
{
public:
    /**
     * Constructor
     *
     * \param channelIDs       the mapping from vertex indices to channel IDs
     * \param colorSensitivity The sensitivity of the color map as property. The
     *                             color map ranges from -colorSensitivity to
     *                             +colorSensitivity in microvolt.
     * \param event            event marking a special time position as WFlag
     */
    WHeadSurfaceCallback( const std::vector< std::size_t >& channelIDs,
                          WPropDouble colorSensitivity,
                          boost::shared_ptr< WFlag< boost::shared_ptr< WEEGEvent > > > event );

    /**
     * Callback method called by the NodeVisitor.
     * Changes the color of the head surface according to the event.
     *
     * \param drawable The drawable this callback is connected to. Should be
     *                     a Geometry with a 1D texture.
     */
    virtual void update( osg::NodeVisitor* /*nv*/, osg::Drawable* drawable );

protected:
private:
    /**
     * the sensitivity of the color map which is currently used
     */
    double m_currentColorSensitivity;

    /**
     * The time position which is currently used.
     * The color is updated if the new time from the m_event is different to
     * this.
     */
    double m_currentTime;

    /**
     * the mapping from vertex indices to channel IDs
     */
    const std::vector< std::size_t > m_channelIDs;

    /**
     * The sensitivity of the color map as property. The color map ranges
     * from -m_colorSensitivity to +m_colorSensitivity in microvolt.
     */
    WPropDouble m_colorSensitivity;

    /**
     * event marking a special time position as WFlag
     */
    boost::shared_ptr< WFlag< boost::shared_ptr< WEEGEvent > > > m_event;
};

#endif  // WHEADSURFACECALLBACK_H
