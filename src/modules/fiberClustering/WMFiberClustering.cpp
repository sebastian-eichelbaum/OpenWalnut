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
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include <osg/Geode>
#include <osg/Geometry>

#include "WMFiberClustering.h"
#include "../../math/WFiber.h"
#include "../../common/WColor.h"
#include "../../common/WLogger.h"
#include "../../common/WStatusReport.h"
#include "../../dataHandler/WDataHandler.h"
#include "../../dataHandler/WSubject.h"
#include "../../dataHandler/WDataSetFibers.h"
#include "../../dataHandler/WLoaderManager.h"
#include "../../kernel/WKernel.h"
#include "../../utils/WColorUtils.h"

WMFiberClustering::WMFiberClustering()
    : WModule(),
      m_maxDistance_t( 0.0 ),
      m_dLtTableExists( false ),
      m_minClusterSize( 10 ),
      m_separatePrimitives( true ),
      m_proximity_t( 0.0 ),
      m_lastFibsSize( 0 )
{
}

WMFiberClustering::~WMFiberClustering()
{
}

void WMFiberClustering::threadMain()
{
    boost::shared_ptr< WDataHandler > dataHandler;
    // TODO(math): fix this hack when possible by using an input connector.
    while( !WKernel::getRunningKernel() )
    {
        sleep( 1 );
    }
    while( !( dataHandler = WKernel::getRunningKernel()->getDataHandler() ) )
    {
        sleep( 1 );
    }
    while( !dataHandler->getNumberOfSubjects() )
    {
        sleep( 1 );
    }

    assert( m_fibs = boost::shared_dynamic_cast< WDataSetFibers >( dataHandler->getSubject( 0 )->getDataSet( 0 ) ) );

    checkDLtLookUpTable();
    cluster();
    paint();

    // Since the modules run in a separate thread: such loops are possible
    while ( !m_FinishRequested )
    {
        // do fancy stuff
        sleep( 1 );
    }
}

void WMFiberClustering::checkDLtLookUpTable()
{
    if( m_dLtTableExists )
    {
        if( m_fibs->size() != m_lastFibsSize )
        {
            // throw away old invalid table
            m_dLtTable.reset();
            m_dLtTableExists = false;
        }
    }
}

void WMFiberClustering::cluster()
{
    std::cout << "Start clustering with " << m_fibs->size() << " fibers." << std::endl;
    m_clusters.clear();  // remove evtl. old clustering
    size_t numFibers = m_fibs->size();
    std::vector< size_t > cid( numFibers, 0 );  // cluster number for each fib where it belongs to
    for( size_t i = 0; i < numFibers; ++i )
    {
        m_clusters.push_back( WFiberCluster( i, m_fibs ) );
        cid[i] = i;
    }
    if( !m_dLtTableExists )  // Refactor: methode mit bool returnvalue aufrufen
    {
        m_dLtTable.reset( new WDXtLookUpTable( numFibers ) );
    }

    for( size_t i = 0; i < numFibers; ++i )  // loop over all "symmetric" fibers pairs
    {
        const wmath::WFiber &q = (*m_fibs)[i];
        for( size_t j = i + 1;  j < numFibers; ++j )
        {
            const wmath::WFiber& r = (*m_fibs)[j];

            if( cid[i] != cid[j] )  // both fibs are in different clusters
            {
                if( !m_dLtTableExists )
                {
                    (*m_dLtTable)( i, j ) = q.dLt( r, m_proximity_t );
                }
                double dLt = (*m_dLtTable)( i, j );

                if( dLt < m_maxDistance_t )  // q and r provide an inter-cluster-link
                {
                    size_t qID = cid[i];
                    size_t rID = cid[j];
                    size_t newID = qID;
                    if( qID > rID )  // merge always to the cluster with the smaller id
                    {
                        newID = rID;
                        std::swap( qID, rID );
                    }

                    WFiberCluster& qCluster = m_clusters[ qID ];
                    WFiberCluster& rCluster = m_clusters[ rID ];
                    assert( !qCluster.empty() && !rCluster.empty() );
                    rCluster.updateClusterIndices( cid, newID );
                    qCluster.merge( rCluster );
                    assert( rCluster.empty() );
                }
            }
        }
    }
    m_dLtTableExists = true;

    // remove empty clusters
    WFiberCluster emptyCluster( m_fibs );
    m_clusters.erase( std::remove( m_clusters.begin(), m_clusters.end(), emptyCluster ), m_clusters.end() );

    // determine #clusters and #small_clusters which are below a certain size
    std::cout << "Found " << m_clusters.size() << " clusters where ";
    size_t numSmallClusters = 0;
    for( size_t i = 0; i < m_clusters.size(); ++i )
    {
        m_clusters[i].sort();  // Refactor: why do we need sorting here?
        if( m_clusters[i].size() < m_minClusterSize )
        {
            m_clusters[i].clear();  // make small clusters empty to remove them easier
            ++numSmallClusters;
        }
    }
    std::cout << numSmallClusters << " clusters are only of size ";
    std::cout << m_minClusterSize << " or less." << std::endl;
    m_clusters.erase( std::remove( m_clusters.begin(), m_clusters.end(), emptyCluster ), m_clusters.end() );
    std::cout << "Erased small clusters too." << std::endl;
    std::cout << "Using " << m_clusters.size() << " clusters.";

    m_lastFibsSize = m_fibs->size();
}

void WMFiberClustering::paint()
{
}
