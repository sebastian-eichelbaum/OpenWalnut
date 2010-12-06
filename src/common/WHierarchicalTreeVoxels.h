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

#ifndef WHIERARCHICALTREEVOXELS_H
#define WHIERARCHICALTREEVOXELS_H

#include <utility>
#include <vector>
#include <queue>
#include <list>

#include <boost/shared_ptr.hpp>

#include "WColor.h"

#include "WHierarchicalTree.h"

/**
 * Class implements a hierarchical tree and provides helper functions for selection and navigation
 */
class WHierarchicalTreeVoxels : public WHierarchicalTree
{
public:
    /**
     * standard constructor
     */
    WHierarchicalTreeVoxels();

    /**
     * destructor
     */
    ~WHierarchicalTreeVoxels();

    /**
     * getter
     * \param cluster the cluster to work on
     * \return vector of voxels contained by this cluster
     */
    std::vector<size_t>getVoxelsForCluster( size_t cluster );

    /**
     * A leaf is at the very bottom of the tree, it represents a single fiber or voxel, for several purposes
     * a leaf also counts as a cluster
     * \param voxelnum the voxel id for this leaf
     */
    void addLeaf( size_t voxelnum );

    /**
     * adds a cluster to the set, it combines 2 already existing clusters
     *
     * \param cluster1 first cluster to add
     * \param cluster2 second cluster to add
     * \param customData some arbitrary data stored with the cluster
     */
    void addCluster( size_t cluster1, size_t cluster2, float customData );

    /**
     * getter
     * \param leaf
     * \return the voxel num of a leaf node in the tree
     */
    size_t getVoxelNum( size_t leaf );

protected:
private:
    /**
     * A leaf is at the very bottom of the tree, it represents a single fiber or voxel, for several purposes
     * a leaf also counts as a cluster
     */
    void addLeaf();

    std::vector<size_t>m_voxelnums; //!< stores the voxel id of each leaf node
};

inline size_t WHierarchicalTreeVoxels::getVoxelNum( size_t leaf )
{
    return m_voxelnums[leaf];
}

#endif  // WHIERARCHICALTREEVOXELS_H
