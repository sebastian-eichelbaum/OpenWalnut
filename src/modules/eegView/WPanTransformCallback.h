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

#ifndef WPANTRANSFORMCALLBACK_H
#define WPANTRANSFORMCALLBACK_H

#include <osg/Node>
#include <osg/NodeCallback>
#include <osg/NodeVisitor>

#include "core/common/WPropertyTypes.h"


/**
 * OSG Node Callback to update the MatrixTransform Node used for the panning.
 */
class WPanTransformCallback : public osg::NodeCallback
{
public:
    /**
     * Constructor
     *
     * \param labelsWidth the width of the label display in pixel as property
     * \param timePos     the time position in seconds where to start the graph at
     *                        the left edge as property
     * \param timeRange   the width of the graph in seconds as property
     * \param graphWidth  the width of the graph in pixel as property
     * \param yPos        the y position in pixel at the lower edge as property
     */
    WPanTransformCallback( WPropInt labelsWidth,
                           WPropDouble timePos,
                           WPropDouble timeRange,
                           WPropInt graphWidth,
                           WPropDouble yPos );

    /**
     * Callback method called by the NodeVisitor when visiting a node.
     * Change the matrix according to the properties.
     *
     * \param node The node this callback is connected to. Should be a
     *             MatrixTransform node.
     * \param nv   The node visitor which performs the traversal. Should be an
     *             update visitor.
     */
    virtual void operator()( osg::Node* node, osg::NodeVisitor* nv );

protected:
private:
    /**
     * the width of the label display in pixel which is currently used
     */
    int m_currentLabelsWidth;

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
     * the width of the graph in pixel which is currently used
     */
    int m_currentGraphWidth;

    /**
     * the y position in pixel at the lower edge which is currently used
     */
    double m_currentYPos;

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
};

#endif  // WPANTRANSFORMCALLBACK_H
