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

#ifndef WELECTRODEPOSITIONCALLBACK_H
#define WELECTRODEPOSITIONCALLBACK_H

#include <cstddef>

#include <boost/shared_ptr.hpp>

#include <osg/Drawable>
#include <osg/ref_ptr>
#include <osgSim/ScalarsToColors>

#include "core/common/WFlag.h"
#include "core/common/WPropertyTypes.h"
#include "WEEGEvent.h"


/**
 * OSG Update Callback to change the color of an electrode position when the
 * event position changed.
 * \note Only add it to a ShapeDrawable!
 */
class WElectrodePositionCallback : public osg::Drawable::UpdateCallback
{
public:
    /**
     * Constructor
     *
     * \param channelID        the number of the channel
     * \param colorSensitivity The sensitivity of the color map as property. The
     *                             color map ranges from -colorSensitivity to
     *                             +colorSensitivity in microvolt.
     * \param event            event marking a special time position as WFlag
     * \param colorMap         the object mapping the electrode potentials to
     *                             colors
     */
    WElectrodePositionCallback( std::size_t channelID,
                                WPropDouble colorSensitivity,
                                boost::shared_ptr< WFlag< boost::shared_ptr< WEEGEvent > > > event,
                                osg::ref_ptr< const osgSim::ScalarsToColors > colorMap );

    /**
     * Callback method called by the NodeVisitor.
     * Changes the color of the electrode position according to the event.
     *
     * \param drawable The drawable this callback is connected to. Should be
     *                     a ShapeDrawable.
     */
    virtual void update( osg::NodeVisitor* /*nv*/, osg::Drawable* drawable );

protected:
private:
    /**
     * the number of the channel
     */
    const std::size_t m_channelID;

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
     * The sensitivity of the color map as property. The color map ranges
     * from -m_colorSensitivity to +m_colorSensitivity in microvolt.
     */
    WPropDouble m_colorSensitivity;

    /**
     * event marking a special time position as WFlag
     */
    boost::shared_ptr< WFlag< boost::shared_ptr< WEEGEvent > > > m_event;

    /**
     * the ScalarsToColors object mapping the potentials at the electrodes
     * to colors
     */
    const osg::ref_ptr< const osgSim::ScalarsToColors > m_colorMap;
};

#endif  // WELECTRODEPOSITIONCALLBACK_H
