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

#include "WHierarchicalTree.h"

WHierarchicalTree::WHierarchicalTree() :
    m_clusterCount( 0 ),
    m_leafCount( 0 ),
    m_maxLevel( 0 ),
    m_leafesLocked( false )
{
}

WHierarchicalTree::~WHierarchicalTree()
{
}

void WHierarchicalTree::addCluster( size_t cluster1, size_t cluster2, float customData )
{
    m_leafesLocked = true;

    size_t level = std::max( m_level[cluster1], m_level[cluster1] ) + 1;
    m_level.push_back( level );

    m_maxLevel = std::max( m_maxLevel, level );

    m_parents.push_back( m_clusterCount );
    m_customData.push_back( customData );
    m_colors.push_back( WColor( 0.3, 0.3, 0.3, 1.0 ) );

    std::pair<size_t, size_t>childs( cluster1, cluster2 );
    m_children.push_back( childs );

    m_parents[cluster1] = m_clusterCount;
    m_parents[cluster2] = m_clusterCount;

    ++m_clusterCount;
}
