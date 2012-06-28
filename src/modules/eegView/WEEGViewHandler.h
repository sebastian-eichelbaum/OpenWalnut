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

#ifndef WEEGVIEWHANDLER_H
#define WEEGVIEWHANDLER_H

#include <cstddef>

#include <boost/shared_ptr.hpp>

#include <osg/ref_ptr>
#include <osgGA/GUIActionAdapter>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIEventHandler>

#include "core/common/WFlag.h"
#include "core/common/WPropertyTypes.h"
#include "core/dataHandler/WDataSetDipoles.h"
#include "core/dataHandler/WEEG2.h"
#include "core/graphicsEngine/WGEGroupNode.h"
#include "WEEGEvent.h"


/**
 * GUI event handler class used for the 2D EEG view.
 */
class WEEGViewHandler : public osgGA::GUIEventHandler
{
public:
    /**
     * Constructor
     *
     * \param labelsWidth      the width of the label display in pixel as
     *                             property
     * \param timePos          the time position in seconds where to start the
     *                             graph at the left edge as property
     * \param timeRange        the width of the graph in seconds as property
     * \param graphWidth       the width of the graph in pixel as property
     * \param yPos             the y position in pixel at the lower edge as
     *                             property
     * \param ySpacing         the distance between two curves of the graph in
     *                             pixel as property
     * \param ySensitivity     the sensitivity of the graph in microvolt per
     *                             pixel as property
     * \param colorSensitivity The sensitivity of the color map as property. The
     *                             color map ranges from -colorSensitivity to
     *                             +colorSensitivity in microvolt.
     * \param event            event marking a special time position as WFlag
     * \param eventParentNode  parent node, where an marked event position is
     *                             inserted and removed from
     * \param eeg              pointer to the loaded EEG dataset
     * \param segmentID        number of the segment
     * \param snapToDipole     Property determining whether the selected time
     *                             position should be snapped to an active
     *                             dipole
     * \param proofOfConcept   Property determining whether we only show the
     *                             proof of concept or the real dipoles
     * \param dipoles          pointer to the loaded dipoles dataset
     */
    WEEGViewHandler( WPropInt labelsWidth,
                     WPropDouble timePos,
                     WPropDouble timeRange,
                     WPropInt graphWidth,
                     WPropDouble yPos,
                     WPropDouble ySpacing,
                     WPropDouble ySensitivity,
                     WPropDouble colorSensitivity,
                     boost::shared_ptr< WFlag< boost::shared_ptr< WEEGEvent > > > event,
                     osg::ref_ptr< WGEGroupNode > eventParentNode,
                     boost::shared_ptr< WEEG2 > eeg,
                     std::size_t segmentID,
                     WPropBool snapToDipole,
                     WPropBool proofOfConcept,
                     boost::shared_ptr< WDataSetDipoles > dipoles );

    /**
     * Handle events.
     *
     * \param ea event class for storing keyboard, mouse and window events
     * \return true if handled, false otherwise
     */
    virtual bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& /*aa*/ );

protected:
private:
    /**
     * the width of the label display in pixel as property
     */
    WPropInt m_labelsWidth;

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
     * the width of the graph in pixel as property
     */
    WPropInt m_graphWidth;

    /**
     * the y position in pixel at the lower edge as property
     */
    WPropDouble m_yPos;

    /**
     * the distance between two curves of the graph in pixel as property
     */
    WPropDouble m_ySpacing;

    /**
     * the sensitivity of the graph in microvolt per pixel as property
     */
    WPropDouble m_ySensitivity;

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
     * parent node, where an marked event position is inserted and removed from
     */
    osg::ref_ptr< WGEGroupNode > m_eventParentNode;

    /**
     * pointer to the loaded EEG dataset
     */
    boost::shared_ptr< WEEG2 > m_eeg;

    /**
     * number of the segment
     */
    std::size_t m_segmentID;

    /**
     * Property determining whether the selected time position should be snapped to an active dipole
     */
    WPropBool m_snapToDipole;

    /**
     * Property determining whether we only show the proof of concept or the real dipoles
     */
    WPropBool m_proofOfConcept;

    /**
     * Pointer to the loaded dipoles dataset
     */
    boost::shared_ptr< WDataSetDipoles > m_dipoles;

    float m_oldX; //!< previous mouse x position
    float m_oldY; //!< previous mouse y position

    /**
     * Update the event position according to the clicked position
     *
     * \param x the x coordinate of the mouse pointer in pixel
     * \return true if handled, false otherwise
     */
    bool markEvent( float x );
};

#endif  // WEEGVIEWHANDLER_H
