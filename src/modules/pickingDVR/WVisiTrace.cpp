//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2017 OpenWalnut Community
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

#include <string>
#include <utility>
#include <vector>

#include "core/common/math/linearAlgebra/WPosition.h"
#include "core/common/WAssert.h"

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/property_map/property_map.hpp>

#include "WVisiTrace.h"

WVisiTrace::WVisiTrace():
    m_candidatePositions(),
    m_candidateJumps(),
    m_curve3D(),
    m_dataChanged( false )
{
}

std::vector< WPosition > WVisiTrace::getLine()
{
    if( m_dataChanged )
    {
        performVisiTrace();
        m_dataChanged = false;
    }

    return m_curve3D;
}

void WVisiTrace::addCandidatesForRay(  const std::vector< std::pair< double, WPosition > > candidates )
{
    std::vector< double > opacityJumps( 0 );
    std::vector< WPosition > positions( 0 );

    for( size_t id = 0; id < candidates.size(); ++id )
    {
        opacityJumps.push_back( candidates[id].first );
        positions.push_back( candidates[id].second );
    }

    m_candidatePositions.push_back( positions );
    m_candidateJumps.push_back( opacityJumps );

    m_dataChanged = true;
}

std::vector< std::pair< int, int > > WVisiTrace::getLinearizedNodesRefs() const
{
    std::vector< std::pair< int, int > > nodeRefs( 0 );
    for( size_t outerId = 0; outerId < m_candidatePositions.size(); ++outerId )
    {
        for( size_t innerId = 0; innerId < m_candidatePositions[outerId].size(); ++innerId )
        {
            nodeRefs.push_back( std::make_pair( outerId, innerId ) );
        }
    }
    return nodeRefs;
}

std::vector< std::vector< int > > WVisiTrace::getInverseLinearizedNodesRefs() const
{
    std::vector< std::vector< int > > inverseRefs( 0 );
    size_t counter = 0;
    for( size_t outerId = 0; outerId < m_candidatePositions.size(); ++outerId )
    {
        inverseRefs.push_back( std::vector< int >( 0 ) );
        for( size_t innerId = 0; innerId < m_candidatePositions[outerId].size(); ++innerId )
        {
            inverseRefs[outerId].push_back( counter );
            ++counter;
        }
    }
    return inverseRefs;
}

void WVisiTrace::performDijkstra()
{
    // Check if there is something to do
    if( m_candidatePositions.size() == 0 || m_candidateJumps.size() == 0 )
    {
        return;
    }

    typedef boost::adjacency_list< boost::listS, boost::vecS, boost::directedS,
                                   boost::no_property, boost::property< boost::edge_weight_t, double > > graph_t;
    typedef boost::graph_traits< graph_t >::vertex_descriptor vertex_descriptor;
    typedef std::pair<int, int> Edge;

    std::vector< std::pair< int, int > > linearized = getLinearizedNodesRefs();
    std::vector< std::vector< int > > linearizedInverse = getInverseLinearizedNodesRefs();

    const int numVirtNodes = 2; // virtual start an end nodes
    const int startNodeId = 0;
    const int endNodeId = 1;

    const int num_nodes = linearized.size() + numVirtNodes;

    std::vector< std::string > name( 0 );
    name.push_back( "Start" );
    name.push_back( "End" );
    for( int id = 0; id < num_nodes; ++id )
    {
        name.push_back( std::to_string( id + numVirtNodes ) );
    }

    std::vector< Edge > edgeVector( 0 );
    std::vector< double > distanceWeights( 0 );
    std::vector< double > opacityWeights( 0 );

    // Edges from virtual start node to candidates of first ray
    for( auto candi : linearizedInverse[0] )
    {
        edgeVector.push_back( Edge( startNodeId, candi + numVirtNodes ) );
        distanceWeights.push_back( 1 );
        opacityWeights.push_back( 1 );
    }

    // Edges from candidates of one ray to those of the next ray
    for( size_t rayId = 0; rayId < linearizedInverse.size() - 1; ++rayId )
    {
        for( size_t firstId = 0; firstId < linearizedInverse[rayId].size(); ++firstId )
        {
            for( size_t secondId = 0; secondId < linearizedInverse[rayId+1].size(); ++secondId )
            {
                edgeVector.push_back( Edge( linearizedInverse[rayId][firstId] + numVirtNodes,
                                            linearizedInverse[rayId+1][secondId] + numVirtNodes ) );
                WPosition firstPos = m_candidatePositions[rayId][firstId];
                WPosition secondPos = m_candidatePositions[rayId+1][secondId];
                double distance = length( firstPos - secondPos );
                distanceWeights.push_back( distance );
                opacityWeights.push_back( 1 - m_candidateJumps[rayId+1][secondId] );
            }
        }
    }

    // Normalize distance weights
    {
        double maxDistance = 0;
        for( auto distance : distanceWeights )
        {
            if( distance > maxDistance )
            {
                maxDistance = distance;
            }
        }
        for( double& weight : distanceWeights ) // NOLINT
        {
            weight /= maxDistance;
        }
    }

    // Edges from candidates of last ray to virtual end node
    for( auto candi : linearizedInverse[linearizedInverse.size()-1] )
    {
        edgeVector.push_back( Edge( candi + numVirtNodes, endNodeId ) );
        distanceWeights.push_back( 1 );
        opacityWeights.push_back( 1 );
    }

    WAssert( distanceWeights.size() == opacityWeights.size(), "Internal error: Need as many opacities as positions." );
    std::vector< double > overallWeights( distanceWeights.size() );
    for( size_t weightId = 0; weightId < overallWeights.size(); ++weightId )
    {
        overallWeights[weightId] = opacityWeights[weightId] * distanceWeights[weightId] * distanceWeights[weightId];
    }

    Edge* edge_array = &edgeVector[0];
    double* weights = &overallWeights[0];
    int num_arcs = edgeVector.size();

    graph_t g( edge_array, edge_array + num_arcs, weights, num_nodes );
    //property_map<graph_t, edge_weight_t>::type weightmap = get(edge_weight, g);
    std::vector<vertex_descriptor> p( num_vertices( g ) );
    std::vector<double> distResult( num_vertices( g ) );
    vertex_descriptor s = vertex( startNodeId, g );

    dijkstra_shortest_paths( g,
                             s,
                             predecessor_map( boost::make_iterator_property_map( p.begin(), get( boost::vertex_index, g ) ) ).
                             distance_map( boost::make_iterator_property_map( distResult.begin(), get( boost::vertex_index, g ) ) ) );

    std::vector< int > shortestPathIds( 0 );

    int parentId = endNodeId;
    shortestPathIds.push_back( parentId );
    while( parentId != startNodeId )
    {
        parentId = p[parentId];
        shortestPathIds.push_back( parentId );
    }
    std::reverse( shortestPathIds.begin(), shortestPathIds.end() );

    for( size_t id = 1; id < shortestPathIds.size() - 1; ++id )
    {
        int rayId = linearized[shortestPathIds[id]-numVirtNodes].first;
        int candidateId = linearized[shortestPathIds[id]-numVirtNodes].second;
        m_curve3D.push_back( m_candidatePositions[rayId][candidateId] );
    }

    // DEBUGGING: writing solution to file and stdout
    // {
    // #include <iostream>
    // #include <fstream>
    //     std::cout << "distances and parents:" << std::endl;
    //     std::cout << "distance(" << "END" << ") = " << distResult[endNodeId] << std::endl;
    //     std::cout << "distances and parents:" << std::endl;
    //     graph_traits < graph_t >::vertex_iterator vi, vend;
    //     for (boost::tie(vi, vend) = vertices(g); vi != vend; ++vi)
    //     {
    //         std::cout << "distance(" << name[*vi] << ") = " << distResult[*vi] << ", ";
    //         std::cout << "parent(" << name[*vi] << ") = " << name[p[*vi]] << std::endl;
    //     }
    //     std::cout << std::endl;

    //     std::ofstream dot_file("/tmp/dijkstra-eg.dot");

    //     dot_file << "digraph D {\n"
    //              << "  rankdir=LR\n"
    //              << "  size=\"20,20\"\n"
    //              << "  ratio=\"fill\"\n"
    //              << "  edge[style=\"bold\"]\n" << "  node[shape=\"circle\"]\n";

    //     graph_traits < graph_t >::edge_iterator ei, ei_end;
    //     for (boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei) {
    //         graph_traits < graph_t >::edge_descriptor e = *ei;
    //         graph_traits < graph_t >::vertex_descriptor
    //             u = source(e, g), v = target(e, g);
    //         dot_file << name[u] << " -> " << name[v]
    //                  << "[label=\"" << get(weightmap, e) << "\"";
    //         if (p[v] == u)
    //             dot_file << ", color=\"black\"";
    //         else
    //             dot_file << ", color=\"grey\"";
    //         dot_file << "]";
    //     }
    //     dot_file << "}";
    // }
}

void WVisiTrace::performVisiTrace()
{
    performDijkstra();
}

void WVisiTrace::reset()
{
    m_candidatePositions.clear();
    m_candidateJumps.clear();
    m_dataChanged = true;
    m_curve3D.clear(); // not really needed because m_dataChanged is true.
}
