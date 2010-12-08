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
        if( !dataSet )
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
        // computeDistanceMatrix( dataSet );
        std::cout << computeDendrogram( computeEMST( dataSet ) )->toTXTString();
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

boost::shared_ptr< WMDetTractClusteringGP::MST > WMDetTractClusteringGP::computeEMST( boost::shared_ptr< const WDataSetGP > dataSet ) const
{
    boost::shared_ptr< MST > edges( new MST() );
    if( !dataSet )
    {
        return edges; // consider dataset as empty
    }

    boost::shared_ptr< WProgress > progress( new WProgress( "EMST computation", dataSet->size() - 1 ) );
    m_progress->addSubProgress( progress );

    // we need the diagonal elements: (i,i) of the similarity matrix for poper similarity compuation
    std::vector< double > diagonal( dataSet->size() );
    for( size_t i = 0; i < dataSet->size(); ++i )
    {
        diagonal[i] = std::sqrt( gauss::innerProduct( ( *dataSet )[i], ( *dataSet )[i] ) );
    }

    // initialize the first the similarities to the root node.
    const WGaussProcess& root = dataSet->front(); // is the root vertex of the MST
    std::vector< double > similarities( dataSet->size(), 0.0 );
    for( size_t i = 0; i < dataSet->size(); ++i )
    {
        const WGaussProcess& p = ( *dataSet )[i];
        if( root.getBB().minDistance( p.getBB() ) < root.getMaxSegmentLength() + p.getMaxSegmentLength() )
        {
            similarities[i] = gauss::innerProduct( root, p ) / diagonal[0] / diagonal[i];
        }
        // Note: it is 0.0 otherwise as the initial value is 0.0
    }

    std::vector< size_t > queue( dataSet->size() - 1 );
    for( size_t i = 0; i < dataSet->size() - 1; ++i )
    {
        queue[i] = i + 1;
    }
    std::vector< size_t > parent( dataSet->size(), 0 );

    while( !queue.empty() )
    {
        // find maximal similarity which corresponds to the minimum distance.
        size_t closestTractIndex = 0; // the tract index inside the queue where this tract has highest innerProduct score among all others
        for( size_t i = 1; i < queue.size(); ++i )
        {
            if( similarities[ queue[ i ] ] > similarities[ queue[ closestTractIndex ] ] )
            {
                closestTractIndex = i;
            }
        }

        // remove closest tract from queue
        size_t closestTract = queue[ closestTractIndex ];
        queue[ closestTractIndex ] = queue.back();
        queue.pop_back();

        // add edge to MST
        edges->insert( std::pair< double, Edge >( similarities[ closestTract ], Edge( parent[ closestTract ], closestTract ) ) );

        // update similaritys and parents
        const WGaussProcess& v = ( *dataSet )[ closestTract ];
        for( size_t i = 0; i < queue.size(); ++i )
        {
            // compute similarity to last inserted tract
            const WGaussProcess& p = ( *dataSet )[ queue[ i ] ];
            double similarity = 0.0;
            if( v.getBB().minDistance( p.getBB() ) < v.getMaxSegmentLength() + p.getMaxSegmentLength() )
            {
                similarity = gauss::innerProduct( v, p ) / diagonal[closestTract] / diagonal[ queue[ i ] ];
            }

            // update similarities and parent array if the new edge is closer to the MST sofar
            if( similarities[ queue[ i ] ] < similarity )
            {
                similarities[ queue[ i ] ] = similarity;
                parent[ queue[ i ] ] = closestTract;
            }
        }
        ++*progress;
    }
    progress->finish();
    return edges;
}

boost::shared_ptr< WDendrogram > WMDetTractClusteringGP::computeDendrogram( boost::shared_ptr< const WMDetTractClusteringGP::MST > edges ) const
{
    boost::shared_ptr< WProgress > progress( new WProgress( "MST => Dendrogram", edges->size() ) ); // NOLINT line length
    m_progress->addSubProgress( progress );
    boost::shared_ptr< WDendrogram > result( new WDendrogram( edges->size() + 1 ) ); // there are exactly n-1 edges

    WUnionFind uf( edges->size() + 1 );
    std::vector< size_t > in( edges->size() + 1 ); // The refernces from the canonical Elements (cE) to the inner nodes.
    for( size_t i = 0; i < in.size(); ++i )
    {
        in[i] = i; // initialize them with their corresponding leafs.
    }
#ifdef DEBUG
    double similarity = wlimits::MAX_DOUBLE; // corresponds to the height, and enables the sorting check
#endif
    for( MST::const_reverse_iterator cit = edges->rbegin(); cit != edges->rend(); ++cit ) // NOLINT line length but: note: reverse iterating since the edge with highest similarity is at the end
    {
#ifdef DEBUG
        WAssert( cit->first <= similarity, "Bug: The edges aren't sorted!" );
        similarity = cit->first;
#endif
        // (u,v) - edge
        size_t u = cit->second.first;
        size_t v = cit->second.second;

        // u and v may already be a member of a cluster, thus we need their cannonical elements
        size_t cEu = uf.find( u );
        size_t cEv = uf.find( v );

        // get the references to their inner nodes (of the dendrogram)
        size_t innerNodeU = in[ cEu ];
        size_t innerNodeV = in[ cEv ];

        size_t newInnerNode = result->merge( innerNodeU, innerNodeV, cit->first );
        uf.merge( cEu, cEv );
        in[ uf.find( cEu ) ] = newInnerNode;

        ++*progress;
    }
    progress->finish();
    return result;
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

    m_similarities = WMatrixSymFLT( dataSet->size() );
    for( size_t i = 0; i < dataSet->size(); ++i )
    {
        for( size_t j = i + 1; j < dataSet->size(); ++j )
        {
            const WGaussProcess& p1 = ( *dataSet )[i];
            const WGaussProcess& p2 = ( *dataSet )[j];
            const WBoundingBox& bb1 = p1.getBB();
            const WBoundingBox& bb2 = p2.getBB();
            if( bb1.minDistance( bb2 ) < p1.getMaxSegmentLength() + p2.getMaxSegmentLength() )
            {
                m_similarities( i, j ) = gauss::innerProduct( p1, p2 ) / diagonal[i] / diagonal[j];
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
}
