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

#ifndef WLINESTRIPCALLBACK_H
#define WLINESTRIPCALLBACK_H

#include <cstddef>

#include <boost/shared_ptr.hpp>

#include <osg/Drawable>
#include <osg/NodeVisitor>

#include "core/common/WPropertyTypes.h"
#include "core/dataHandler/WEEG2Segment.h"


/**
 * OSG Update Callback to update the EEG graph of one channel by changing the
 * vertex array of the line strip.
 */
class WLineStripCallback : public osg::Drawable::UpdateCallback
{
public:
    /**
     * Constructor
     *
     * \param channelID    the number of the channel
     * \param timePos      the time position in seconds where to start the graph
     *                     at the left edge as property
     * \param timeRange    the width of the graph in seconds as property
     * \param segment      pointer to the EEG segment
     * \param samplingRate sampling rate used by the recording
     */
    WLineStripCallback( std::size_t channelID,
                        WPropDouble timePos,
                        WPropDouble timeRange,
                        boost::shared_ptr< WEEG2Segment > segment,
                        double samplingRate );

    /**
     * Callback method called by the NodeVisitor.
     * Changes the vertex array of the line strip according to the properties.
     *
     * \param drawable The drawable this callback is connected to. Should be
     *                 a Geometry.
     */
    virtual void update( osg::NodeVisitor* /*nv*/, osg::Drawable* drawable );

protected:
private:
    /**
     * the number of the channel
     */
    const std::size_t m_channelID;

    /**
     * the time position in seconds where to start the graph at the left edge
     * which is currently used
     */
    double m_currentTimePos;

    /**
     * the width of the graph in seconds which is currently used
     */
    double m_currentTimeRange;

    /**
     * the time position in seconds where to start the graph at the left edge as
     * property
     */
    WPropDouble m_timePos;

    /**
     * the width of the graph in seconds as property
     */
    WPropDouble m_timeRange;

    /**
     * pointer to the EEG segment
     */
    boost::shared_ptr< WEEG2Segment > m_segment;

    /**
     * sampling rate used by the recording
     */
    double m_samplingRate;

    /**
     * Convert the given double value to std::size_t and clamp it into the given
     * range
     *
     * \param value value to convert
     * \param min   minimum of the valid range
     * \param max   maximum of the valid range
     * \return converted value
     */
    std::size_t clampToRange( double value, std::size_t min, std::size_t max ) const;
};

#endif  // WLINESTRIPCALLBACK_H
