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

#include <list>
#include <vector>

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

std::vector< size_t > WHierarchicalTree::findXBiggestClusters( size_t cluster, size_t number )
{
    //std::cout << number << " largest clusters for cluster: " << cluster << std::endl;

    if( number > m_containsLeafes[cluster].size() )
    {
        number = m_containsLeafes[cluster].size();
    }

    // init
    std::list<size_t>worklist;
    worklist.push_back( cluster );

    while( worklist.size() < number )
    {
        size_t current = worklist.front();
        worklist.pop_front();
        if( m_containsLeafes[current].size() > 1 )
        {
            size_t left = m_children[current].first;
            size_t right = m_children[current].second;
            worklist.push_back( left );
            worklist.push_back( right );
        }
        else
        {
            worklist.push_back( current );
        }
    }

    worklist.sort( compSize( this ) );

    bool newSplit = true;

    while( newSplit )
    {
        newSplit = false;
        size_t current = worklist.front();

        if( m_containsLeafes[current].size() > 1 )
        {
            size_t left = m_children[current].first;
            size_t right = m_children[current].second;
            size_t last = worklist.back();

            if( m_containsLeafes[left].size() > m_containsLeafes[last].size() )
            {
                worklist.pop_front();
                worklist.push_back( left );
                worklist.sort( compSize( this ) );
                newSplit = true;
            }

            last = worklist.back();
            if( m_containsLeafes[right].size() > m_containsLeafes[last].size() )
            {
                if( !newSplit )
                {
                    worklist.pop_front();
                }
                if( worklist.size() == number )
                {
                    worklist.pop_back();
                }
                worklist.push_back( right );
                worklist.sort( compSize( this ) );
                newSplit = true;
            }
        }
    }

    std::vector<size_t>returnVector;
    std::list<size_t>::iterator it;
    for( it = worklist.begin(); it != worklist.end(); ++it )
    {
        size_t current = *it;
        //std::cout << "cluster:" << current << "  size:" << m_containsLeafes[current].size() << std::endl;
        returnVector.push_back( current );
    }

    return returnVector;
}

std::vector< size_t > WHierarchicalTree::downXLevelsFromTop( size_t level, bool hideOutliers )
{
    if( level > m_maxLevel )
    {
        level = m_maxLevel -1;
    }

    std::vector< size_t > returnVector;

    std::list< size_t > worklist;
    worklist.push_back( m_clusterCount - 1 );

    for( size_t i = 0; i < level; ++i )
    {
        std::list<size_t>newChildList;
        while( !worklist.empty() )
        {
            size_t current = worklist.front();
            worklist.pop_front();
            if( m_containsLeafes[current].size() > 1 )
            {
                size_t left = m_children[current].first;
                size_t right = m_children[current].second;

                if( hideOutliers )
                {
                    if( m_containsLeafes[left].size() > 1 )
                    {
                        newChildList.push_back( left );
                    }
                    if( m_containsLeafes[right].size() > 1 )
                    {
                        newChildList.push_back( right );
                    }
                }
                else
                {
                    newChildList.push_back( left );
                    newChildList.push_back( right );
                }
            }
        }
        worklist = newChildList;
    }

    worklist.sort( compSize( this ) );

    std::list<size_t>::iterator it;
    for( it = worklist.begin(); it != worklist.end(); ++it )
    {
        size_t current = *it;
        returnVector.push_back( current );
    }

    return returnVector;
}

void WHierarchicalTree::colorCluster( size_t cluster, WColor color )
{
    m_colors[cluster] = color;
    if( m_containsLeafes[cluster].size() > 1 )
    {
        colorCluster( m_children[cluster].first, color );
        colorCluster( m_children[cluster].second, color );
    }
}

