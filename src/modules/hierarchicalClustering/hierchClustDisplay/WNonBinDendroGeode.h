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

//---------------------------------------------------------------------------
//
// Project: hClustering
//
// Whole-Brain Connectivity-Based Hierarchical Parcellation Project
// David Moreno-Dominguez
// d.mor.dom@gmail.com
// moreno@cbs.mpg.de
// www.cbs.mpg.de/~moreno//
// This file is also part of OpenWalnut ( http://www.openwalnut.org ).
//
// hClustering is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// http://creativecommons.org/licenses/by-nc/3.0
//
// hClustering is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
//---------------------------------------------------------------------------

#ifndef WNONBINDENDROGEODE_H
#define WNONBINDENDROGEODE_H

#include <vector>

#include <osg/Geode>
#include <osg/Vec3>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>

#include "WHtree.h"

/**
 * Class creates a dendrogram from a hierarchical clustering, and allows to click and select specific nodes
 */
class WNonBinDendroGeode: public osg::Geode // NOLINT
{
public:
    /**
     * constructor
     * \param tree tree object to work on
     * \param displayColors vector of cluster selection colors
     * \param cluster root cluster for the dendrogram
     * \param xSize number of pixel to scale the tree on along the x axis
     * \param ySize number of pixel to scale the tree on along the y axis
     * \param xOffset translation alogn the x axis
     * \param yOffset translation alogn the y axis
     * \param useHLevel if true the height of a node is determined by the hierarchical level of the cluster
     * \param triangleLeaves if set leaves will be joined by a straight line to the parent node
     * \param hozLine if != 0 a horizontal cuttting line will be drawn across the tree
     */
    WNonBinDendroGeode( const WHtree& tree, const std::vector< WColor > &displayColors, size_t cluster, float xSize, float ySize,
                    float xOffset, float yOffset, bool useHLevel = false, bool triangleLeaves = false, float hozLine = 0 );

    /**
     * destructor
     */
    ~WNonBinDendroGeode();

    /**
     * calculate which cluster was clicked from given pixel coordinates
     * \param xClick the x coordinate
     * \param yClick the y coordinate
     * \return the cluster id, will return the root cluster if no cluster can be determinded
     */
    size_t getClickedCluster( int xClick, int yClick );

    /**
     * calculates if the given position is within the view area of the dendrogram
     * \param xClick the x coordinate
     * \param yClick the y coordinate
     * \return true if pos is within the dendrogram area, otherwise false
     */
    bool inDendrogramArea( int xClick, int yClick )const;

protected:
private:
    /**
     * helper function the starts the layout process from the input data in the constructor
     */
    void create();

    /**
     * recursive function that lays out the tree from top to bottom,
     * height of the joins is determined by the hierarchical level of the cluster or distance value
     * \param cluster the current node cluster to work on
     * \param leftLimit left border of the current subcluster
     * \param rightLimit right border of the current subcluster
     */
    void layout( const WHnode& cluster, const float leftLimit, const float rightLimit );

    /**
     * recurse function that follows the layout to determine the cluster from pixel coordinates
     * \param cluster cluster to check against coordinates
     * \param leftLimit left boundary of cluster
     * \param rightLimit right boundary of cluster
     */
    void findClickedCluster( size_t cluster, float leftLimit, float rightLimit );





    const WHtree m_tree; //!< the tree to work on

    size_t m_rootCluster; //!< top cluster to draw the tree from

    const std::vector< WColor > &m_displayColors; //!< stores a the current colors each node should be displayed, given the current selection

    osg::ref_ptr< osg::Vec4Array > m_lineColors; //!< stores a the colors of each line that should be drawn

    osg::Vec3Array* m_vertexArray; //!< vertex array

    osg::DrawElementsUInt* m_lineArray; //!< line array

    float m_xSize; //!< x size in pixel of the final dendrogram
    float m_ySize; //!< y size in pixel of the final dendrogram
    float m_xOff; //!< x offset for dendrogram drawing in the screen
    float m_yOff; //!< y offset for dendrogram drawing in the screen
    float m_xUnit; //!< helper variable for x position in the node selection recursive function
    float m_yUnit; //!< helper variable for y position in the node selection recursive function

    float m_xClicked; //!< stores the click x position for use in the node selection recursive function
    float m_yClicked; //!< stores the click y position for use in the node selection recursive function

    bool m_useHLevel; //!< flag indicating if the level or the value of a cluster will be used for the height of join
    bool m_triangleLeaves; //!< option to join leaves with straight lines, giving a triangle appearance to the bottom level

    float m_hozLine; //!< determines if a horizontal line will be drawn to indicate aprtition level

    size_t m_clickedCluster; //!< ID of the clicked cluster
};

#endif  // WNONBINDENDROGEODE_H
