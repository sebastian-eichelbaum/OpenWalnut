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

#include <map>
#include <string>
#include <list>

#include "WDataSetHierarchicalClustering.h"

// The prototype as singleton. Created during first getPrototype() call
boost::shared_ptr< WPrototyped > WDataSetHierarchicalClustering::m_prototype = boost::shared_ptr< WPrototyped >();

WDataSetHierarchicalClustering::WDataSetHierarchicalClustering()
{
    // initialize members
}

WDataSetHierarchicalClustering::WDataSetHierarchicalClustering( WTreeNode::SPtr rootNode,
        std::map< size_t, WFiberCluster::SPtr > allClusters )
{
    m_rootNode = rootNode;
    m_clusters = allClusters;
}


WDataSetHierarchicalClustering::~WDataSetHierarchicalClustering()
{
    // cleanup
}

boost::shared_ptr< WPrototyped > WDataSetHierarchicalClustering::getPrototype()
{
    if( !m_prototype )
    {
        m_prototype = boost::shared_ptr< WPrototyped >( new WDataSetHierarchicalClustering() );
    }
    return m_prototype;
}

const std::string WDataSetHierarchicalClustering::getName() const
{
    return "DataSetHierarchicalClustering";
}

const std::string WDataSetHierarchicalClustering::getDescription() const
{
    return "A tree of fiber clusters.";
}

WTreeNode::SPtr WDataSetHierarchicalClustering::getRootNode()
{
    return m_rootNode;
}

std::map< size_t, WFiberCluster::SPtr > WDataSetHierarchicalClustering::getClusterMap()
{
    return m_clusters;
}


std::vector< WTreeNode::SPtr > WDataSetHierarchicalClustering::getClustersDownToLevel( WTreeNode::SPtr node, size_t level )
{
    std::vector< WTreeNode::SPtr > result;

    if( node->level() <= level )
    {
        result.push_back( node );
        return result;
    }

    for( size_t i = 0; i < node->getChildren().size(); i++ )
    {
        std::vector< WTreeNode::SPtr > c = getClustersDownToLevel( node->getChildren()[i], level );
        result.reserve( result.size() + c.size() ); // ensure capacity will last for insertion.
        result.insert( result.end(), c.begin(), c.end() );
    }

    return result;
}





