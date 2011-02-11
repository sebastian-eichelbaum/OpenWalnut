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

#ifndef WGEGRIDNODE_H
#define WGEGRIDNODE_H

#include <osg/Geode>
#include <osg/MatrixTransform>

#include "../../dataHandler/WGridRegular3D.h"
#include "../../common/WSharedObject.h"
#include "../../common/WProperties_Fwd.h"

#include "../widgets/labeling/WGELabel.h"

#include "../WExportWGE.h"

/**
 * This node is able to represent a grid in certain ways. It can show its boundary or the whole grid if desired.
 */
class WGE_EXPORT WGEGridNode: public osg::MatrixTransform
{
public:

    /**
     * Convenience typedef for a osg::ref_ptr< WGEGridNode >.
     */
    typedef osg::ref_ptr< WGEGridNode > SPtr;

    /**
     * Convenience typedef for a osg::ref_ptr< const WGEGridNode >.
     */
    typedef osg::ref_ptr< const WGEGridNode > ConstSPtr;

    /**
     * Creates a node representing the specified grid.
     *
     * \param grid the grid to represent.
     */
    explicit WGEGridNode( WGridRegular3D::ConstSPtr grid );

    /**
     * Destructor.
     */
    virtual ~WGEGridNode();

    /**
     * Updates the node to use the new specified grid
     *
     * \param grid the new grid to use
     */
    void setGrid( WGridRegular3D::ConstSPtr grid );

    /**
     * Returns the currently set grid.
     *
     * \return the current grid.
     */
    WGridRegular3D::ConstSPtr getGrid() const;

    // TODO(all): add setter and getter for boundary colors and so on.

    /**
     * Returns whether labels on the corners are enabled or not.
     *
     * \return true if labels are enabled
     */
    bool getEnableLabels() const;

    /**
     * En- or disable labels on the boundary corners.
     *
     * \param enable true to enbable
     */
    void setEnableLabels( bool enable = true );

protected:

private:

    /**
     * The actual grid which should be represented by this node.
     */
    WSharedObject< WGridRegular3D::ConstSPtr > m_grid;

    /**
     * The geometry for the boundary.
     */
    osg::ref_ptr< osg::Geode > m_boundaryGeode;

    /**
     * The geometry for the whole grid.
     */
    osg::ref_ptr< osg::Geode > m_innerGridGeode;

    /**
     * The labels at the corner.
     */
    WGELabel::SPtr m_borderLabels[8];

    /**
     * The geode keeping the labels
     */
    osg::ref_ptr< osg::Geode > m_labelGeode;

    /**
     * The actual callback handling changes in the grid
     *
     * \param node the node. This will be the this pointer.
     */
    void callback( osg::Node* node );

    /**
     * If true, the labels and geometry gets adapted properly.
     */
    bool m_gridUpdate;

    /**
     * If true, labels get used.
     */
    bool m_showLabels;
};

#endif  // WGEGRIDNODE_H

