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

#include <osgGA/GUIActionAdapter>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIEventHandler>

#include "../../common/WPropertyTypes.h"


/**
 * GUI event handler class used for the 2D EEG view.
 */
class WEEGViewHandler : public osgGA::GUIEventHandler
{
public:
    /**
     * Constructor
     *
     * \param labelsWidth  the width of the label display in pixel as property
     * \param timePos      the time position in seconds where to start the graph
     *                         at the left edge as property
     * \param timeRange    the width of the graph in seconds as property
     * \param graphWidth   the width of the graph in pixel as property
     * \param yPos         the y position in pixel at the lower edge as property
     * \param ySpacing     the distance between two curves of the graph in pixel
     *                         as property
     * \param ySensitivity the sensitivity of the graph in microvolt per pixel
     *                         as property
     */
    WEEGViewHandler( WPropInt labelsWidth,
                     WPropDouble timePos,
                     WPropDouble timeRange,
                     WPropInt graphWidth,
                     WPropDouble yPos,
                     WPropDouble ySpacing,
                     WPropDouble ySensitivity );

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

    float m_oldX; //!< previous mouse x position
    float m_oldY; //!< previous mouse y position
};

#endif  // WEEGVIEWHANDLER_H
