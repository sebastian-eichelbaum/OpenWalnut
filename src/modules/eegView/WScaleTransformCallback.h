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

#ifndef WSCALETRANSFORMCALLBACK_H
#define WSCALETRANSFORMCALLBACK_H

#include <cstddef>

#include <osg/Node>
#include <osg/NodeCallback>
#include <osg/NodeVisitor>

#include "core/common/WPropertyTypes.h"


/**
 * OSG Node Callback to update the MatrixTransform Nodes of one channel used for
 * the y-scaling.
 */
class WScaleTransformCallback : public osg::NodeCallback
{
public:
    /**
     * Constructor
     *
     * \param channelID    the number of the channel
     * \param ySpacing     the distance between two curves of the graph in pixel
     *                     as property
     * \param ySensitivity the sensitivity of the graph in microvolt per pixel
     *                     as property
     */
    WScaleTransformCallback( std::size_t channelID, WPropDouble ySpacing, WPropDouble ySensitivity );

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
     * the number of the channel
     */
    const std::size_t m_channelID;

    /**
     * the distance between two curves of the graph in pixel which is currently
     * used
     */
    double m_currentYSpacing;

    /**
     * the sensitivity of the graph in microvolt per pixel which is currently
     * used
     */
    double m_currentYSensitivity;

    /**
     * the distance between two curves of the graph in pixel as property
     */
    WPropDouble m_ySpacing;

    /**
     * the sensitivity of the graph in microvolt per pixel as property
     */
    WPropDouble m_ySensitivity;
};

#endif  // WSCALETRANSFORMCALLBACK_H
