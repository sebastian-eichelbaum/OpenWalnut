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
#include <list>
#include <sstream>
#include <string>
#include <vector>

#include <osg/Geode>
#include <osg/Geometry>

#include "WMFiberClustering.h"
#include "../../math/WFiber.h"
#include "../../math/fiberSimilarity/WDLTMetric.h"
#include "../../common/WColor.h"
#include "../../common/WLogger.h"
#include "../../common/WStatusReport.h"
#include "../../common/WStringUtils.hpp"
#include "../../dataHandler/WDataHandler.h"
#include "../../dataHandler/WDataSetFibers.h"
#include "../../dataHandler/WSubject.h"
#include "../../dataHandler/io/WIOTools.hpp"
#include "../../dataHandler/io/WWriterLookUpTableVTK.h"
#include "../../dataHandler/io/WReaderLookUpTableVTK.h"
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

boost::shared_ptr< WModule > WMFiberClustering::factory() const
{
    return boost::shared_ptr< WModule >( new WMFiberClustering() );
}

void WMFiberClustering::moduleMain()
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
    // TODO(math): replace this hard coded path when properties are available
    if( wiotools::fileExists( "/tmp/pansen.dist" ) )
    {
        try
        {
            debugLog() << "trying to read table from /tmp/pansen.dist";
            // TODO(math): replace this hard coded path when properties are available
            WReaderLookUpTableVTK r( "/tmp/pansen.dist" );
            using boost::shared_ptr;
            using std::vector;
            shared_ptr< vector< double > > data = shared_ptr< vector < double > >( new vector< double >() );
            r.readTable( data );
            m_dLtTable.reset( new WDXtLookUpTable( static_cast< size_t >( data->back() ) ) );
            m_lastFibsSize = static_cast< size_t >( data->back() );

            // remove the dimension from data array since it's not representing any distance
            data->pop_back();

            m_dLtTable->setData( *data );

            // check if elements match number of fibers and reset m_lastFibsSize
            m_dLtTableExists = true;
        }
        catch( WDHException e )
        {
            debugLog() << e.what() << std::endl;
        }
    }
    if( m_dLtTableExists )
    {
        if( m_fibs->size() != m_lastFibsSize )
        {
            debugLog() << "considered old table as invalid" << std::endl
                     << "current loaded fibers: " << m_fibs->size() << std::endl
                     << "old fibers: " << m_lastFibsSize << std::endl;
            // throw away old invalid table
            m_dLtTable.reset();
            m_dLtTableExists = false;
        }
    }
}

void WMFiberClustering::cluster()
{
    infoLog() << "Start clustering with " << m_fibs->size() << " fibers.";
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
    m_proximity_t = 1.0;
    infoLog() << "Proximity threshold: " << m_proximity_t;
    m_maxDistance_t = 6.5;
    infoLog() << "Maximum inter cluster distance threshold: " << m_maxDistance_t;
    WStatusReport st( numFibers );

    WDLTMetric dLt( m_proximity_t * m_proximity_t );  // metric instance for computation of the dLt measure
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
                    (*m_dLtTable)( i, j ) = dLt.dist( q, r );
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
        std::stringstream ss;
        ss << "\r" << std::fixed << std::setprecision( 2 ) << ( ++st ).progress() << " " << st.stringBar() << std::flush;
        // std::cout << ss.str();
    }
    // std::cout << std::endl;
    m_dLtTableExists = true;

    // remove empty clusters
    WFiberCluster emptyCluster( m_fibs );
    m_clusters.erase( std::remove( m_clusters.begin(), m_clusters.end(), emptyCluster ), m_clusters.end() );

    // determine #clusters and #small_clusters which are below a certain size
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
    infoLog() << "Found " << m_clusters.size() << " clusters where "
              << numSmallClusters << " clusters are only of size "
              << m_minClusterSize << " or less.";
    m_clusters.erase( std::remove( m_clusters.begin(), m_clusters.end(), emptyCluster ), m_clusters.end() );
    infoLog() << "Erased small clusters too.";
    infoLog() << "Using " << m_clusters.size() << " clusters.";

    m_lastFibsSize = m_fibs->size();
    WWriterLookUpTableVTK w( "/tmp/pansen.dist", true );
    w.writeTable( m_dLtTable->getData(), m_lastFibsSize );
}

osg::ref_ptr< osg::Geode > WMFiberClustering::genFiberGeode( const WFiberCluster &cluster, const WColor color ) const
{
    using osg::ref_ptr;
    ref_ptr< osg::Vec3Array > vertices = ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    ref_ptr< osg::Geometry > geometry = ref_ptr< osg::Geometry >( new osg::Geometry );

    std::list< size_t >::const_iterator cit = cluster.getIndices().begin();
    for( ; cit !=  cluster.getIndices().end(); ++cit )
    {
        const wmath::WFiber &fib = (*m_fibs)[ *cit ];
        for( size_t i = 0; i < fib.size(); ++i )
        {
            vertices->push_back( osg::Vec3( fib[i][0], fib[i][1], fib[i][2] ) );
        }
        geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, vertices->size() - fib.size(), fib.size() ) );
    }

    geometry->setVertexArray( vertices );

    ref_ptr< osg::Vec4Array > colors = ref_ptr< osg::Vec4Array >( new osg::Vec4Array );
    colors->push_back( color.getOSGColor() );
    geometry->setColorArray( colors );
    geometry->setColorBinding( osg::Geometry::BIND_OVERALL );

    osg::ref_ptr< osg::Geode > geode = osg::ref_ptr< osg::Geode >( new osg::Geode );
    geode->addDrawable( geometry.get() );
    return geode;
}


void WMFiberClustering::paint()
{
    // get different colors via HSV color model for each cluster
    double hue = 0.0;
    double hue_increment = 1.0 / m_clusters.size();
    WColor color;

    infoLog() << "cluster: " << m_clusters.size();
    osg::ref_ptr< osg::Group > group = osg::ref_ptr< osg::Group >( new osg::Group );
    for( size_t i = 0; i < m_clusters.size(); ++i, hue += hue_increment )
    {
        color.setHSV( hue, 1.0, 0.75 );
        group->addChild( genFiberGeode( m_clusters[i], color ).get() );
    }
    group->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->addChild( group.get() );
}
