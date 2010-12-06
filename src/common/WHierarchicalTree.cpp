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

std::vector< size_t > WHierarchicalTree::findXBiggestClusters( size_t cluster, size_t number )
{
    //std::cout << number << " largest clusters for cluster: " << cluster << std::endl;

    if ( number > m_containsLeafes[cluster].size() )
    {
        number = m_containsLeafes[cluster].size();
    }

    // init
    std::list<size_t>worklist;
    worklist.push_back( cluster );

    while ( worklist.size() < number )
    {
        size_t current = worklist.front();
        worklist.pop_front();
        if ( m_containsLeafes[current].size() > 1 )
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
    sortList( worklist );

    bool newSplit = true;

    while ( newSplit )
    {
        newSplit = false;
        size_t current = worklist.front();

        if ( m_containsLeafes[current].size() > 1 )
        {
            size_t left = m_children[current].first;
            size_t right = m_children[current].second;
            size_t last = worklist.back();

            if ( m_containsLeafes[left].size() > m_containsLeafes[last].size() )
            {
                worklist.pop_front();
                worklist.push_back( left );
                sortList( worklist );
                newSplit = true;
            }

            last = worklist.back();
            if ( m_containsLeafes[right].size() > m_containsLeafes[last].size() )
            {
                if ( !newSplit )
                {
                    worklist.pop_front();
                }
                if ( worklist.size() == number )
                {
                    worklist.pop_back();
                }
                worklist.push_back( right );
                sortList( worklist );
                newSplit = true;
            }
        }
        sortList( worklist );
    }

    std::vector<size_t>returnVector;
    std::list<size_t>::iterator it;
    for ( it = worklist.begin(); it != worklist.end(); ++it )
    {
        size_t current = *it;
        //std::cout << "cluster:" << current << "  size:" << m_containsLeafes[current].size() << std::endl;
        returnVector.push_back( current );
    }

    return returnVector;
}

std::vector< size_t > WHierarchicalTree::findXBiggestClusters2( size_t cluster, size_t number )
{
    //std::cout << number << " largest clusters for cluster: " << cluster << std::endl;

    if ( number > m_containsLeafes[cluster].size() )
    {
        number = m_containsLeafes[cluster].size();
    }

    // init
    std::list<size_t>worklist;
    worklist.push_back( cluster );

    while ( worklist.size() < number )
    {
        size_t current = worklist.front();
        worklist.pop_front();
        if ( m_containsLeafes[current].size() > 1 )
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
    sortList( worklist );

    bool newSplit = true;

    while ( newSplit )
    {
        newSplit = false;
        size_t current = worklist.front();

        if ( m_containsLeafes[current].size() > 1 )
        {
            size_t left = m_children[current].first;
            size_t right = m_children[current].second;
            size_t last = worklist.back();

            if ( ( m_containsLeafes[left].size() > m_containsLeafes[last].size() ) &&
                 ( m_containsLeafes[right].size() > m_containsLeafes[last].size() ) )
            {
                if ( m_containsLeafes[left].size() > m_containsLeafes[last].size() )
                {
                    worklist.pop_front();
                    worklist.push_back( left );
                    sortList( worklist );
                    newSplit = true;
                }

                last = worklist.back();
                if ( m_containsLeafes[right].size() > m_containsLeafes[last].size() )
                {
                    if ( !newSplit )
                    {
                        worklist.pop_front();
                    }
                    if ( worklist.size() == number )
                    {
                        worklist.pop_back();
                    }
                    worklist.push_back( right );
                    sortList( worklist );
                    newSplit = true;
                }
            }
        }
        sortList( worklist );
    }

    std::vector<size_t>returnVector;
    std::list<size_t>::iterator it;
    for ( it = worklist.begin(); it != worklist.end(); ++it )
    {
        size_t current = *it;
        //std::cout << "cluster:" << current << "  size:" << m_containsLeafes[current].size() << std::endl;
        returnVector.push_back( current );
    }

    return returnVector;
}


std::vector< size_t > WHierarchicalTree::downXLevelsFromTop( size_t level, bool hideOutliers )
{
    if ( level > m_maxLevel )
    {
        level = m_maxLevel -1;
    }

    std::vector<size_t>returnVector;

    std::list<size_t>worklist;
    worklist.push_back( m_clusterCount - 1 );

    for ( size_t i = 0; i < level; ++i )
    {
        std::list<size_t>newChildList;
        while ( !worklist.empty() )
        {
            size_t current = worklist.front();
            worklist.pop_front();
            if ( m_containsLeafes[current].size() > 1 )
            {
                size_t left = m_children[current].first;
                size_t right = m_children[current].second;

                if ( hideOutliers )
                {
                    if ( m_containsLeafes[left].size() > 1 )
                    {
                        newChildList.push_back( left );
                    }
                    if ( m_containsLeafes[right].size() > 1 )
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

    sortList( worklist );

    std::list<size_t>::iterator it;
    for ( it = worklist.begin(); it != worklist.end(); ++it )
    {
        size_t current = *it;
        returnVector.push_back( current );
    }

    return returnVector;
}


void WHierarchicalTree::sortList( std::list<size_t> &input ) //NOLINT
{
    if ( input.size() == 0 )
    {
        return;
    }

    std::vector<size_t>vec;
    std::list<size_t>::iterator it;

    for ( it = input.begin(); it != input.end(); ++it )
    {
        vec.push_back( *it );
    }

    for ( size_t n = vec.size() - 1; n > 0; --n )
    {
        for ( size_t i = 0; i < n; ++i )
        {
            if ( m_containsLeafes[vec[i]].size() < m_containsLeafes[vec[i+1]].size() )
            {
                size_t tmp = vec[i];
                vec[i] = vec[i+1];
                vec[i+1] = tmp;
            }
        }
    }

    input.clear();
    for ( size_t k = 0; k < vec.size(); ++k )
    {
        input.push_back( vec[k] );
    }
}

void WHierarchicalTree::colorCluster( size_t cluster, WColor color )
{
    std::list<size_t>worklist;
    worklist.push_back( cluster );

    while ( !worklist.empty() )
    {
        size_t current = worklist.front();
        worklist.pop_front();

        m_colors[current] = color;

        if ( m_containsLeafes[current].size() > 1 )
        {
            size_t left = m_children[current].first;
            size_t right = m_children[current].second;
            worklist.push_back( left );
            worklist.push_back( right );
        }
    }
}
