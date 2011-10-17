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

#ifndef WLABELSTRANSFORMCALLBACK_H
#define WLABELSTRANSFORMCALLBACK_H

#include <osg/Node>
#include <osg/NodeCallback>
#include <osg/NodeVisitor>

#include "core/common/WPropertyTypes.h"


/**
 * OSG Node Callback to update the MatrixTransform Node of the labels used for
 * panning and zooming.
 */
class WLabelsTransformCallback : public osg::NodeCallback
{
public:
    /**
     * Constructor
     *
     * \param yPos     the y position in pixel at the lower edge as property
     * \param ySpacing the distance between two curves of the graph in pixel as
     *                 property
     */
    WLabelsTransformCallback( WPropDouble yPos, WPropDouble ySpacing );

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
     * the y position in pixel at the lower edge which is currently used
     */
    double m_currentYPos;

    /**
     * the distance between two curves of the graph in pixel which is currently
     * used
     */
    double m_currentYSpacing;

    /**
     * the y position in pixel at the lower edge as property
     */
    WPropDouble m_yPos;

    /**
     * the distance between two curves of the graph in pixel as property
     */
    WPropDouble m_ySpacing;
};

#endif  // WLABELSTRANSFORMCALLBACK_H
