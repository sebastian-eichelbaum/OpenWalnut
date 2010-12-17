//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV-Leipzig and CNCF-CBS
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

#include "../../../common/WLimits.h"
#include "../../../common/datastructures/WDendrogram.h"
#include "../../../common/datastructures/WUnionFind.h"
#include "../../../kernel/WKernel.h"
#include "../../emptyIcon.xpm" // Please put a real icon here.
#include "WMDetTractClusteringGP.h"

WMDetTractClusteringGP::WMDetTractClusteringGP():
    WModule()
{
}

WMDetTractClusteringGP::~WMDetTractClusteringGP()
{
}

boost::shared_ptr< WModule > WMDetTractClusteringGP::factory() const
{
    return boost::shared_ptr< WModule >( new WMDetTractClusteringGP() );
}

const char** WMDetTractClusteringGP::getXPMIcon() const
{
    return emptyIcon_xpm; // Please put a real icon here.
}
const std::string WMDetTractClusteringGP::getName() const
{
    return "Deterministic Tract Clustering using Gaussian Proccesses";
}

const std::string WMDetTractClusteringGP::getDescription() const
{
    return "Clusters Gaussian Processes which represents deterministic tracts with the Wassermann "
           "approach described in the paper: http://dx.doi.org/10.1016/j.neuroimage.2010.01.004";
}

void WMDetTractClusteringGP::connectors()
{
    m_gpIC = WModuleInputData< WDataSetGP >::createAndAdd( shared_from_this(), "gpInput", "WDataSetGP providing the gaussian processes" );

    WModule::connectors();
}

void WMDetTractClusteringGP::properties()
{
    WModule::properties();
}

void WMDetTractClusteringGP::moduleMain()
{
    m_moduleState.setResetable( true, true ); // remember actions when actually not waiting for actions
    m_moduleState.add( m_gpIC->getDataChangedCondition() );

    ready();

    while ( !m_shutdownFlag() ) // loop until the module container requests the module to quit
    {
        debugLog() << "Waiting..";
        m_moduleState.wait();
        if ( !m_gpIC->getData().get() ) // ok, the output has not yet sent data
        {
            continue;
        }

        boost::shared_ptr< WDataSetGP > dataSet = m_gpIC->getData();
        if( !dataSet || dataSet->size() == 0 )
        {
            debugLog() << "Invalid data--> continue";
            continue;
        }
        if( m_gpIC->handledUpdate() )
        {
            debugLog() << "Input has been updated...";
        }
        debugLog() << "Start Clustering";

        computeDistanceMatrix( dataSet );
        debugLog() << "done";
    }
}

void WMDetTractClusteringGP::computeDistanceMatrix( boost::shared_ptr< const WDataSetGP > dataSet )
{
    boost::shared_ptr< WProgress > progress( new WProgress( "Similarity matrix computation", ( dataSet->size()*dataSet->size() - dataSet->size() ) / 2 + dataSet->size() ) ); // NOLINT line length
    m_progress->addSubProgress( progress );
    std::vector< double > diagonal( dataSet->size() );
    for( size_t i = 0; i < dataSet->size(); ++i )
    {
        diagonal[i] = std::sqrt( gauss::innerProduct( ( *dataSet )[i], ( *dataSet )[i] ) );
        ++*progress;
    }

    m_similarities = WMatrixSymDBL( dataSet->size() );
    for( size_t i = 0; i < dataSet->size(); ++i )
    {
        for( size_t j = i + 1; j < dataSet->size(); ++j )
        {
            const WGaussProcess& p1 = ( *dataSet )[i];
            const WGaussProcess& p2 = ( *dataSet )[j];
            const WBoundingBox& bb1 = p1.getBB();
            const WBoundingBox& bb2 = p2.getBB();
            if( bb1.minDistance( bb2 ) < p1.getRadius() + p2.getRadius() )
            {
                // m_similarities( i, j ) = gauss::innerProduct( p1, p2 ) / diagonal[i] / diagonal[j];
                m_similarities( i, j ) = gauss::innerProduct( p1, p2 ); // As written in the paper, we don't use the normalized inner product
            }
            else
            {
                m_similarities( i, j ) = 0.0;
            }
        }
        *progress = *progress + ( dataSet->size() - i - 1 );
    }
    progress->finish();
    m_progress->removeSubProgress( progress );

    // TODO(math): The toTXTString function also saves the dendrogram into a hard coded path: /home/math/pansen.txt I need to fix this very very soon
    computeDendrogram( dataSet->size() )->toTXTString();
}

boost::shared_ptr< WDendrogram > WMDetTractClusteringGP::computeDendrogram( size_t n )
{
    boost::shared_ptr< WDendrogram > dend( new WDendrogram( n ) );
    boost::shared_ptr< WProgress > progress( new WProgress( "Matrix => Dendrogram", n - 1 ) ); // NOLINT line length
    m_progress->addSubProgress( progress );

    WUnionFind uf( n );
    std::vector< size_t > innerNode( n ); // The refernces from the canonical Elements (cE) to the inner nodes.
    std::set< size_t > idx; // valid indexes, to keep trac of already erased columns
    std::vector< size_t > clusterSize( n, 1 ); // to keep trac how many elements a cluster has.

    for( size_t i = 0; i < n; ++i )
    {
        innerNode[i] = i; // initialize them with their corresponding leafs.
        idx.insert( i );
    }

    WAssert( idx.size() == n, "Bug: The idx array is invalid, too few elements." );
    // now the clustering starts, p and q are the first to merge
    for( size_t i = 0; i < n - 1; ++i )
    {
        // Nearest Neighbour find: update p, q, and sim, so iterate over all valid matrix entries
        // NOTE, WARNING, ATTENTION: This is brute force NN finding strategy and requires O(n^2) time
        double maxSim = -wlimits::MAX_DOUBLE; // This is not 0.0, due to numerical issue, where the similarity maybe very near to 0.0, and thus no new pair is found!
        size_t p = 0;
        size_t q = 0;
        for( std::set< size_t >::const_iterator it = idx.begin(); it != idx.end(); ++it )
        {
            for( std::set< size_t >::const_iterator jt = boost::next( it ); jt != idx.end(); ++jt )
            {
                if( m_similarities( *it, *jt ) > maxSim )
                {
                    maxSim = m_similarities( *it, *jt );
                    p = *it;
                    q = *jt;
                }
            }
        }

        uf.merge( p, q );
        size_t newCE = uf.find( p );
        innerNode[ newCE ] = dend->merge( innerNode[ p ], innerNode[ q ], maxSim );

        // erase one of the columns
        size_t col_to_delete = p;
        if( newCE == p )
        {
            col_to_delete = q;
        }
        else // hence cE_of_q is the new cannonical element of the merged cluster now
        {
            WAssert( q == newCE, "Bug: The new cannonical element is not p nor q, something bad happend!" );
        }

        if( idx.erase( col_to_delete ) == 0 )
        {
            errorLog() << "Bug: tried to erase idx: " << col_to_delete << "but which was not part of the idx array anymore.";
        }

        // update the column where now pq resides
        for( std::set< size_t >::const_iterator it = idx.begin(); it != idx.end(); ++it )
        {
            if( *it != newCE )
            {
                // we have two gauss processes p and q. We have merged p and q into pq. Hence for all valid indexes we must
                // recompute < pq, k > where k is a GP identified through an valid index, where:
                // < pq, k > = |p| / ( |p| + |q| ) < p, k > + |q| / (|p| + |q|) < q, k >
                double firstFactor = static_cast< double >( clusterSize[ p ] ) / ( clusterSize[ p ] + clusterSize[ q ] );
                double secondFactor = static_cast< double >( clusterSize[ q ] ) / ( clusterSize[ p ] + clusterSize[ q ] );
                m_similarities( newCE, *it ) = firstFactor * m_similarities( p, *it ) + secondFactor * m_similarities( q, *it );
            }
        }
        clusterSize[ newCE ] = clusterSize[ p ] + clusterSize[ q ];
        ++*progress;
    }
    std::stringstream ss;
    ss << "Bug: The idx array is invalid, having to few or to many elements, size()==1 expected, but got: " << idx.size();
    WAssert( idx.size() == 1, ss.str() );

    debugLog() << "Finished building up the dendrogram.";

    progress->finish();
    m_progress->removeSubProgress( progress );
    return dend;
}
