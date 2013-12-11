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

#include <string>
#include <map>
#include "WDataSetFiberClustering.h"

// The prototype as singleton. Created during first getPrototype() call
boost::shared_ptr< WPrototyped > WDataSetFiberClustering::m_prototype = boost::shared_ptr< WPrototyped >();

WDataSetFiberClustering::WDataSetFiberClustering()
{
    // initialize members
}

WDataSetFiberClustering::WDataSetFiberClustering( const ClusterMap &clustering )
{
    m_clusters = clustering;
}


WDataSetFiberClustering::~WDataSetFiberClustering()
{
    // cleanup
}

boost::shared_ptr< WPrototyped > WDataSetFiberClustering::getPrototype()
{
    if( !m_prototype )
    {
        m_prototype = boost::shared_ptr< WPrototyped >( new WDataSetFiberClustering() );
    }
    return m_prototype;
}

const std::string WDataSetFiberClustering::getName() const
{
    return "DataSetFiberClustering";
}

const std::string WDataSetFiberClustering::getDescription() const
{
    return "A collection of fiber clusters.";
}

void WDataSetFiberClustering::setCluster( size_t id, WFiberCluster::SPtr cluster )
{
    m_clusters[ id ] = cluster;
}

WFiberCluster::SPtr WDataSetFiberClustering::getCluster( size_t id )
{
    WFiberCluster::SPtr result = m_clusters[ id ];
    if( !result )
    {
        throw WInvalidID( "The cluster with the specified ID does not exist." );
    }
    return result;
}

WFiberCluster::SPtr WDataSetFiberClustering::getOrCreateCluster( size_t id )
{
    WFiberCluster::SPtr result = m_clusters[ id ];
    if( !result )
    {
        // create an empty one
        WFiberCluster::SPtr newCluster( new WFiberCluster() );
        m_clusters[ id ] = newCluster;
        return newCluster;
    }
    return result;
}

void WDataSetFiberClustering::removeCluster( size_t id )
{
    if( !m_clusters.count( id ) )
    {
        return;
    }
    m_clusters.erase( id );
}

WDataSetFiberClustering::ClusterMap::const_iterator WDataSetFiberClustering::begin() const
{
    return m_clusters.begin();
}

WDataSetFiberClustering::ClusterMap::iterator WDataSetFiberClustering::begin()
{
    return m_clusters.begin();
}

WDataSetFiberClustering::ClusterMap::const_iterator WDataSetFiberClustering::end() const
{
    return m_clusters.end();
}

WDataSetFiberClustering::ClusterMap::iterator WDataSetFiberClustering::end()
{
    return m_clusters.end();
}

size_t WDataSetFiberClustering::size() const
{
    return m_clusters.size();
}


