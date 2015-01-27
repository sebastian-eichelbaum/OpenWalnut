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

#ifndef WDENDROGRAMGEODE_H
#define WDENDROGRAMGEODE_H

#include <osg/Geode>
#include <osg/Vec3>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>

#include "../../common/WHierarchicalTreeFibers.h"


/**
 * Class creates a dendrogram from a hierarchical clustering
 */
class WDendrogramGeode : public osg::Geode // NOLINT
{
public:
    /**
     * constructor
     *
     * \param tree reference to the tree object to work on
     * \param cluster root cluster for the dendrogram
     * \param useLevel if true the height of a node is determined by the level of the cluster
     * \param minClusterSize minimum for cluster to be drawn, when i the whole tree is drawn
     * \param xSize number of pixel to scale the tree on along the x axis
     * \param ySize number of pixel to scale the tree on along the y axis
     * \param xOffset translation alogn the x axis
     * \param yOffset translation alogn the y axis
     *
     */
    WDendrogramGeode( WHierarchicalTree* tree, size_t cluster, bool useLevel = true, size_t minClusterSize = 1, float xSize = 1000.f,
            float ySize = 500.f, float xOffset = 0.0f, float yOffset = 0.0f );

    /**
     * destructor
     */
    ~WDendrogramGeode();

    /**
     * calculate which cluster was clicked from given pixel coordinates
     * \param xClick the x coordinate
     * \param yClick the y coordinate
     * \return the cluster id, will return the root cluster if no cluster can be determinded
     */
    size_t getClickedCluster( int xClick, int yClick );

    /**
     * calculates if the given position is within the view area of the dendrogram
     * \param pos the position within the view
     * \return true if pos is within the dendrogram area, otherwise false
     */
    bool inDendrogramArea( const WVector2f& pos ) const;

protected:
private:
    /**
     * helper function the starts the layout process from the input data in the constructor
     */
    void create();

    /**
     * recursive funtion that lays out the tree from top to bottom,
     * height of the joins is determined by the level of the cluster
     * \param cluster the current cluster to work on
     * \param left left border of the current subcluster
     * \param right right border of the current subcluster
     */
    void layoutLevel( size_t cluster, float left, float right );

    /**
     * recursive funtion that lays out the tree from top to bottom,
     * height of the joins is determined by the similarity value of the cluster
     * \param cluster the current cluster to work on
     * \param left left border of the current subcluster
     * \param right right border of the current subcluster
     */
    void layoutValue( size_t cluster, float left, float right );


    /**
     * recurse function that follows the layout to determine the cluster from pixel coordinates, used when the level of the cluster
     * is used for height
     *
     * \param cluster cluster to check against coordinates
     * \param left left boundary of cluster
     * \param right right boundary of cluster
     */
    void getClickClusterRecursive( size_t cluster, float left, float right );

    /**
     * recurse function that follows the layout to determine the cluster from pixel coordinates, used when the customData value is used
     * for height
     *
     * \param cluster cluster to check against coordinates
     * \param left left boundary of cluster
     * \param right right boundary of cluster
     */
    void getClickClusterRecursive2( size_t cluster, float left, float right );


    WHierarchicalTree* m_tree; //!< the tree to work on

    size_t m_rootCluster; //!< top cluster to draw the tree from

    osg::ref_ptr<osg::Vec4Array> m_colors; //!< color array

    osg::Vec3Array* m_vertexArray; //!< vertex array

    osg::DrawElementsUInt* m_lineArray; //!< line array

    size_t m_minClusterSize; //!< minimum cluster size to be considered while laying out the dendrogram

    float m_xSize; //!< x size in pixel of the final dendrogram
    float m_ySize; //!< y size in pixel of the final dendrogram
    float m_xOff; //!< x offset
    float m_yOff; //!< y offset
    float m_xMult; //!< helper variable for the recursive function
    float m_yMult; //!< helper variable for the recursive function

    int m_xClicked; //!< stores the click position for use int he recursive function
    int m_yClicked; //!< stores the click position for use int he recursive function

    bool m_useLevel; //!< flag indicating if the level or the value of a cluster will be used for the height of join

    size_t m_clickedCluster; //!< the clicked cluster
};

#endif  // WDENDROGRAMGEODE_H
