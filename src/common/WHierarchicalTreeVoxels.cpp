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

#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>
#include <queue>
#include <list>

#include "WHierarchicalTreeVoxels.h"

WHierarchicalTreeVoxels::WHierarchicalTreeVoxels() :
    WHierarchicalTree()
{
}

WHierarchicalTreeVoxels::~WHierarchicalTreeVoxels()
{
}

void WHierarchicalTreeVoxels::addLeaf()
{
}

void WHierarchicalTreeVoxels::addLeaf( size_t voxelnum )
{
    // after a cluster was added no more leafes may be inserted
    if ( m_leafesLocked )
    {
        return;
    }
    m_level.push_back( 0 );
    m_parents.push_back( m_clusterCount );
    std::vector<size_t> tmp( 1, m_clusterCount );
    m_containsLeafes.push_back( tmp );
    std::pair<size_t, size_t>tmp2;
    m_children.push_back( tmp2 );
    m_customData.push_back( 0.0 );
    m_colors.push_back( WColor( 0.3, 0.3, 0.3, 1.0 ) );
    m_voxelnums.push_back( voxelnum );

    ++m_leafCount;
    ++m_clusterCount;
}

void WHierarchicalTreeVoxels::addCluster( size_t cluster1, size_t cluster2, float customData )
{
    m_leafesLocked = true;

    size_t level = std::max( m_level[cluster1], m_level[cluster2] ) + 1;
    m_level.push_back( level );

    m_maxLevel = std::max( m_maxLevel, level );

    m_parents.push_back( m_clusterCount );
    m_customData.push_back( customData );
    m_colors.push_back( WColor( 0.3, 0.3, 0.3, 1.0 ) );

    std::pair<size_t, size_t>childs( cluster1, cluster2 );
    m_children.push_back( childs );

    std::vector<size_t>leafes;
    leafes.reserve( m_containsLeafes[cluster1].size() + m_containsLeafes[cluster2].size() );
    leafes.insert( leafes.end(), m_containsLeafes[cluster1].begin(), m_containsLeafes[cluster1].end() );
    leafes.insert( leafes.end(), m_containsLeafes[cluster2].begin(), m_containsLeafes[cluster2].end() );
    m_containsLeafes.push_back( leafes );

    m_parents[cluster1] = m_clusterCount;
    m_parents[cluster2] = m_clusterCount;

    ++m_clusterCount;
}

std::vector<size_t>WHierarchicalTreeVoxels::getVoxelsForCluster( size_t cluster )
{
    std::vector<size_t>returnVec = getLeafesForCluster( cluster );

    for ( size_t i = 0; i < returnVec.size(); ++i )
    {
        returnVec[i] = m_voxelnums[returnVec[i]];
    }
    return returnVec;
}
