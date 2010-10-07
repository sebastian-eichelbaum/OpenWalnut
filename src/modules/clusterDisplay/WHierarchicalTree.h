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

#ifndef WHIERARCHICALTREE_H
#define WHIERARCHICALTREE_H

#include <utility>
#include <vector>
#include <queue>
#include <list>

#include <boost/shared_ptr.hpp>

#include "../../common/WColor.h"

/**
 * Class implements a hierarchical tree and provides helper functions for selection and navigation
 */
class WHierarchicalTree
{
public:
    /**
     * standard constructor
     */
    WHierarchicalTree();

    /**
     * destructor
     */
    ~WHierarchicalTree();

    /**
     * A leaf is at the very bottom of the tree, it represents a single fiber or voxel, for several purposes
     * a leaf also counts as a cluster
     */
    void addLeaf();

    /**
     * adds a cluster to the set, it combines 2 already existing clusters
     *
     * \param cluster1 first cluster to add
     * \param cluster2 second cluster to add
     * \param level level of the new cluster
     * \param leafes vector of leafes the new cluster contains
     * \param customData some arbitrary data stored with the cluster
     */
    void addCluster( size_t cluster1, size_t cluster2, size_t level, std::vector<size_t> leafes, float customData );

    /**
     * generates a bitfield where for every leaf in the selected cluster the value is true, false otherwise
     *
     * \param cluster
     * \return shared pointer to the bitfield
     */
    boost::shared_ptr< std::vector<bool> >getOutputBitfield( size_t cluster );

    /**
     * generates a bitfield where for every leaf in the selected cluster the value is true, false otherwise
     *
     * \param clusters
     * \return shared pointer to the bitfield
     */
    boost::shared_ptr< std::vector<bool> >getOutputBitfield( std::vector<size_t>clusters );

    /**
     * getter
     * \return the number of leafes
     */
    size_t getLeafCount();

    /**
     * getter
     * \return the number of clusters
     */
    size_t getClusterCount();

    /**
     * getter
     * \return maxlevel, i.e. the level of the root cluster
     */
    size_t getMaxLevel();

    /**
     * getter
     * \param cluster
     * \return the level of the selected cluster
     */
    size_t getLevel( size_t cluster );

    /**
     * getter
     * \param cluster the cluster in question
     * \return the parent for the selected cluster
     */
    size_t getParent( size_t cluster );

    /**
     * getter
     * \param cluster
     * \return the custom data for the selected cluster
     */
    float getCustomData( size_t cluster );

    /**
     * setter sets the color for a single cluster
     * \param color
     * \param cluster
     */
    void setColor( WColor color, size_t cluster );

    /**
     * getter
     * \param cluster
     * \return the color for the selected cluster
     */
    WColor getColor( size_t cluster );

    /**
     * sets the color for a selected cluster and all sub clusters
     * \param cluster
     * \param color
     */
    void colorCluster( size_t cluster, WColor color );

    /**
     * getter
     * \param cluster
     * \return children for the selected cluster
     */
    std::pair<size_t, size_t>getChildren( size_t cluster );

    /**
     * getter
     * \param cluster
     * \return the leafes contained in the selected cluster
     */
    std::vector<size_t>getLeafesForCluster( size_t cluster );

    /**
     * finds the X biggest clusters for a given cluster
     * \param cluster
     * \param number of sub clusters
     */
    std::vector< size_t >findXBiggestClusters( size_t cluster, size_t number = 10 );

    /**
     * finds clusters that match a given ROI up to a certain percentage
     *
     * \param ratio value of [0,1] of how many leafes have to be in the roi to activate the cluster
     * \param number number of cluster to select, if more than give number matches the ratio criteria only the
     * biggest clusters are returned
     */
    std::vector<size_t> getBestClustersFittingRoi( float ratio = 0.9, size_t number = 1 );

    /**
     * calculates the ratio of fibers in the roi for a given cluster
     * \param cluster
     * \return ratio
     */
    float getRatio( size_t cluster );

    /**
     * getter
     * \param cluster
     * \return number of leafes for the selected cluster
     */
    size_t size( size_t cluster );

    /**
     * setter
     * \param bitfield
     */
    void setRoiBitField( boost::shared_ptr< std::vector<bool> > bitfield );

protected:
private:
    /**
     * helper function to sort a list of clusters depending on the number of leafes in them
     * \param input reference to the list to be sorted
     */
    void sortList( std::list<size_t> &input ); //NOLINT

    /**
     * overall number of cluster, counts both leafes ( clusters of size one ) and real clusters
     */
    size_t m_clusterCount;

    /**
     * number of leaf nodes
     */
    size_t m_leafCount;

    /**
     * the maximum level, naturally the level of the root node
     */
    size_t m_maxLevel;

    /**
     * to enforce valid datastructures there will be no leaf with an id higher than a cluster, thus when
     * the first cluster is inserted, no leafes may be added
     */
    bool m_leafesLocked;

    /**
     * vector that stores the level of each cluster, the level is the maximum of the levels of the child clusters +1
     */
    std::vector<size_t>m_level;

    /**
     * vector that stores the parent cluster for each cluster
     */
    std::vector<size_t>m_parents;

    /**
     * vector that stores the 2 children of each cluster, contains an empty pair for leafes
     */
    std::vector< std::pair< size_t, size_t> >m_children;

    /**
     * vector that stores the leaf id's for each cluster, this is quite memory intensive but speeds up selection
     * of leafes for nodes at higher levels
     */
    std::vector< std::vector<size_t> >m_containsLeafes;

    /**
     * custom data for each cluster, this may some energy or similarity level generated by the clustering algorithm
     * or something completely different
     */
    std::vector<float>m_customData;

    /**
     * a color value for each cluster
     */
    std::vector<WColor>m_colors;

    /**
     * stores a pointer to the bitfield by the current roi setting
     */
    boost::shared_ptr< std::vector<bool> > m_roiSelection;
};

inline size_t WHierarchicalTree::getLeafCount()
{
    return m_leafCount;
}

inline size_t WHierarchicalTree::getClusterCount()
{
    return m_clusterCount;
}

inline size_t WHierarchicalTree::getMaxLevel()
{
    return m_maxLevel;
}

inline size_t WHierarchicalTree::getLevel( size_t cluster )
{
    return m_level[cluster];
}

inline size_t WHierarchicalTree::getParent( size_t cluster )
{
    if ( m_level[cluster] < m_maxLevel )
    {
        return m_parents[cluster];
    }
    else
    {
        // this is just to quiet the compiler, this branch should never be reached
        return cluster;
    }
}

inline std::pair<size_t, size_t>WHierarchicalTree::getChildren( size_t cluster )
{
    if ( m_level[cluster] > 0 )
    {
        return m_children[cluster];
    }
    else
    {
        // this is just to quiet the compiler, this branch should never be reached
        return std::pair<size_t, size_t>( cluster, cluster );
    }
}

inline std::vector<size_t>WHierarchicalTree::getLeafesForCluster( size_t cluster )
{
    return m_containsLeafes[cluster];
}

inline float WHierarchicalTree::getCustomData( size_t cluster )
{
    return m_customData[cluster];
}

inline size_t WHierarchicalTree::size( size_t cluster )
{
    return m_containsLeafes[cluster].size();
}

inline void WHierarchicalTree::setColor( WColor color, size_t cluster )
{
    m_colors[cluster] = color;
}

inline WColor WHierarchicalTree::getColor( size_t cluster )
{
    return m_colors[cluster];
}

inline void WHierarchicalTree::setRoiBitField( boost::shared_ptr< std::vector<bool> > bitfield )
{
    m_roiSelection = bitfield;
}

#endif  // WHIERARCHICALTREE_H
