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

#include "WHierarchicalTreeFibers.h"

WHierarchicalTreeFibers::WHierarchicalTreeFibers() :
    WHierarchicalTree()
{
}

WHierarchicalTreeFibers::~WHierarchicalTreeFibers()
{
}

void WHierarchicalTreeFibers::addLeaf()
{
    // after a cluster was added no more leafes may be inserted
    if( m_leafesLocked )
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

    ++m_leafCount;
    ++m_clusterCount;
}

void WHierarchicalTreeFibers::addCluster( size_t cluster1, size_t cluster2, size_t level, std::vector<size_t> leafes, float customData )
{
    m_leafesLocked = true;

    m_level.push_back( level );
    m_maxLevel = std::max( m_maxLevel, level );

    m_parents.push_back( m_clusterCount );
    m_containsLeafes.push_back( leafes );
    m_customData.push_back( customData );
    m_colors.push_back( WColor( 0.3, 0.3, 0.3, 1.0 ) );

    std::pair<size_t, size_t>childs( cluster1, cluster2 );
    m_children.push_back( childs );

    m_parents[cluster1] = m_clusterCount;
    m_parents[cluster2] = m_clusterCount;

    ++m_clusterCount;
}

boost::shared_ptr< std::vector<bool> > WHierarchicalTreeFibers::getOutputBitfield( size_t cluster )
{
    boost::shared_ptr< std::vector< bool > > bf;
    // only a single fiber selected
    if( cluster < m_leafCount )
    {
        bf = boost::shared_ptr< std::vector< bool > >( new std::vector< bool >( m_leafCount, false ) );
        ( *bf )[cluster] = true;
    }
    else
    {
        if( cluster >= m_clusterCount )
        {
            return bf;
        }

        bf = boost::shared_ptr< std::vector< bool > >( new std::vector< bool >( m_leafCount, false ) );

        std::vector<size_t> fibers = m_containsLeafes[cluster];
        for( size_t i = 0; i < fibers.size(); ++i )
        {
            ( *bf )[fibers[i]] = true;
        }

        //std::cout << fibers.size() << " fibers selected" << std::endl;
    }
    return bf;
}

boost::shared_ptr< std::vector<bool> >WHierarchicalTreeFibers::getOutputBitfield( std::vector<size_t>clusters )
{
    boost::shared_ptr< std::vector< bool > > bf;
    // only a single fiber selected

    bf = boost::shared_ptr< std::vector< bool > >( new std::vector< bool >( m_leafCount, false ) );

    for( size_t k = 0; k < clusters.size(); ++k )
    {
        size_t cluster = clusters[k];
        std::vector<size_t> fibers = m_containsLeafes[cluster];
        for( size_t i = 0; i < fibers.size(); ++i )
        {
            ( *bf )[fibers[i]] = true;
        }
    }
    return bf;
}

std::vector<size_t> WHierarchicalTreeFibers::getBestClustersFittingRoi( float ratio, size_t number )
{
    if( number == 0 )
    {
        number = 1;
    }
    std::list<size_t>candidateList;

    std::queue<size_t>worklist;
    worklist.push( getClusterCount() - 1 );

    while( !worklist.empty() )
    {
        size_t current = worklist.front();
        worklist.pop();

        if( getRatio( current ) >= ratio )
        {
            candidateList.push_back( current );
        }
        else
        {
            if( getLevel( current ) > 1 )
            {
                std::pair<size_t, size_t> children = getChildren( current );
                if( getLevel( children.first ) > 0 )
                {
                    worklist.push( children.first );
                }
                if( getLevel( children.second ) > 0 )
                {
                    worklist.push( children.second );
                }
            }
        }
    }
    candidateList.sort( compSize( this ) );

    std::vector<size_t>returnList;

    std::list<size_t>::iterator it;
    for( it = candidateList.begin(); it != candidateList.end(); ++it )
    {
        size_t current = *it;
        returnList.push_back( current );
        --number;
        if( number == 0 )
        {
            break;
        }
    }


    return returnList;
}

float WHierarchicalTreeFibers::getRatio( size_t cluster )
{
    std::vector<size_t>fibersInCluster = m_containsLeafes[cluster];

    size_t countFibersInCluster = fibersInCluster.size();
    size_t fibersFromClusterActive = 0;

    for( size_t i = 0; i < countFibersInCluster; ++i )
    {
        if( ( *m_roiSelection )[fibersInCluster[i]] )
        {
            ++fibersFromClusterActive;
        }
    }
    return static_cast<float>( fibersFromClusterActive ) / static_cast<float>( countFibersInCluster );
}
