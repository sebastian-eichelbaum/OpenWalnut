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

        m_maxSegmentLength = searchGlobalMaxSegementLength( dataSet );
        computeDistanceMatrix( dataSet );
        debugLog() << "done";
    }
}

double WMDetTractClusteringGP::searchGlobalMaxSegementLength( boost::shared_ptr< const WDataSetGP > dataSet ) const
{
    WDataSetGP::const_iterator cit = std::max_element( dataSet->begin(), dataSet->end(),
        boost::bind( &WGaussProcess::getMaxSegmentLength, _1 ) < boost::bind( &WGaussProcess::getMaxSegmentLength, _2 ) );

    return cit->getMaxSegmentLength();
    // NOLINT return std::max_element( dataSet->begin(), dataSet->end(), [](WGaussProcess p1, WGaussProcess p2){ return p1.getMaxSegmentLength() < p2.getMaxSegmentLength(); } )->getMaxSegmentLength();
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
    size_t p = 0, q = 0; // indices marking maximal similarity betwee two elements
    double maxSim = 0.0;
    for( size_t i = 0; i < dataSet->size(); ++i )
    {
        for( size_t j = i + 1; j < dataSet->size(); ++j )
        {
            const WGaussProcess& p1 = ( *dataSet )[i];
            const WGaussProcess& p2 = ( *dataSet )[j];
            const WBoundingBox& bb1 = p1.getBB();
            const WBoundingBox& bb2 = p2.getBB();
            if( bb1.minDistance( bb2 ) < 2.0 * p1.getMaxSegmentLength() + 2.0 * p2.getMaxSegmentLength() )
            {
                // m_similarities( i, j ) = gauss::innerProduct( p1, p2 ) / diagonal[i] / diagonal[j];
                m_similarities( i, j ) = gauss::innerProduct( p1, p2 ); // As written in the paper, we don't use the normalized inner product
            }
            else
            {
                m_similarities( i, j ) = 0.0;
            }
            if( m_similarities( i, j ) > maxSim )
            {
                maxSim = m_similarities( i, j );
                p = i;
                q = j;
            }
        }
        *progress = *progress + ( dataSet->size() - i - 1 );
    }
    progress->finish();
    m_progress->removeSubProgress( progress );

    // TODO(math): The toTXTString function also saves the dendrogram into a hard coded path: /home/math/pansen.txt I need to fix this very very soon
    computeDendrogram( dataSet->size(), std::make_pair( p, q ), maxSim )->toTXTString();
}

boost::shared_ptr< WDendrogram > WMDetTractClusteringGP::computeDendrogram( size_t n, std::pair< size_t, size_t > tracts, double similarity )
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

    size_t p = tracts.first;
    size_t q = tracts.second;
    double sim = similarity;

    // now the clustering starts, p and q are the first to merge
    for( size_t i = 0; i < n - 1; ++i )
    {
        size_t cE_of_p = uf.find( p );
        size_t cE_of_q = uf.find( q );
        uf.merge( cE_of_p, cE_of_q );
        size_t newCE = uf.find( cE_of_p );
        innerNode[ newCE ] = dend->merge( innerNode[ cE_of_p ], innerNode[ cE_of_q ], sim );

        // erase one of the columns
        size_t col_to_delete = 0;
        if( newCE == cE_of_p )
        {
            col_to_delete = cE_of_q;
        }
        else // hence cE_of_q is the new cannonical element of the merged cluster now
        {
            WAssert( cE_of_q == newCE, "Bug: The new cannonical element is not cE_of_p nor cE_of_q, something bad happend!" );
            col_to_delete = cE_of_p;
        }
        idx.erase( col_to_delete );

        // update the column where now pq resides
        for( std::set< size_t >::const_iterator it = idx.begin(); it != idx.end(); ++it )
        {
            if( *it != newCE )
            {
                // we have two gauss processes p and q. We have merged p and q into pq. Hence for all valid indexes we must
                // recompute < pq, k > where k is a GP identified through an valid index, where:
                // < pq, k > = |p| / ( |p| + |q| ) < p, k > + |q| / (|p| + |q|) < q, k >
                double firstFactor = static_cast< double >( clusterSize[ cE_of_p ] ) / ( clusterSize[ cE_of_p ] + clusterSize[ cE_of_q ] );
                double secondFactor = static_cast< double >( clusterSize[ cE_of_q ] ) / ( clusterSize[ cE_of_p ] + clusterSize[ cE_of_q ] );
                m_similarities( newCE, *it ) = firstFactor * m_similarities( cE_of_p, *it ) + secondFactor * m_similarities( cE_of_q, *it );
            }
        }
        clusterSize[ newCE ] = clusterSize[ cE_of_p ] + clusterSize[ cE_of_q ];

        // Nearest Neighbour find: update p, q, and sim, so iterate over all valid matrix entries
        // NOTE, WARNING, ATTENTION: This is brute force NN finding strategy and requires O(n^2) time
        double maxSim = -1.0; // This is not 0.0, due to numerical issue, where the similarity maybe very near to 0.0, and thus no new pair is found!
        std::pair< size_t, size_t > newpq = std::make_pair( 0, 0 );
        for( std::set< size_t >::const_iterator it = idx.begin(); it != idx.end(); ++it )
        {
            std::set< size_t >::const_iterator jt = it;
            ++jt; // increment so main diagonal elements are omitted
            for( ; jt != idx.end(); ++jt )
            {
                if( m_similarities( *it, *jt ) > maxSim )
                {
                    maxSim = m_similarities( *it, *jt );
                    newpq.first = *it;
                    newpq.second = *jt;
                }
            }
        }
        p = newpq.first;
        q = newpq.second;
        sim = maxSim;

        ++*progress;
    }
    debugLog() << "Finished building up the dendrogram.";

    progress->finish();
    m_progress->removeSubProgress( progress );
    return dend;
}
