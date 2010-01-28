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

#include "../../common/WColor.h"
#include "../../common/WIOTools.h"
#include "../../common/WLogger.h"
#include "../../common/WProgress.h"
#include "../../common/WStringUtils.h"
#include "../../common/datastructures/WDXtLookUpTable.h"
#include "../../common/datastructures/WFiberCluster.h"
#include "../../dataHandler/WDataSetFibers.h"
#include "../../dataHandler/WSubject.h"
#include "../../dataHandler/io/WReaderLookUpTableVTK.h"
#include "../../dataHandler/io/WWriterLookUpTableVTK.h"
#include "../../graphicsEngine/WGEUtils.h"
#include "../../kernel/WKernel.h"
#include "../../math/WFiber.h"
#include "../../math/fiberSimilarity/WDLTMetric.h"
#include "WMFiberClustering.h"

WMFiberClustering::WMFiberClustering()
    : WModule(),
      m_dLtTableExists( false ),
      m_maxDistance_t( 6.5 ),
      m_minClusterSize( 10 ),
      m_separatePrimitives( true ),
      m_proximity_t( 0.0 ),
      m_clusterOutputID( 0 ),
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
    // additional fire-condition: "data changed" flag
    m_moduleState.add( m_fiberInput->getDataChangedCondition() );

    ready();

    while ( !m_shutdownFlag() ) // loop until the module container requests the module to quit
    {
        m_fibs = m_fiberInput->getData();
        if ( !m_fibs.get() ) // ok, the output has not yet sent data
        {
            m_moduleState.wait();
            continue;
        }

        update();

        m_moduleState.wait(); // waits for firing of m_moduleState ( dataChanged, shutdown, etc. )

        WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_osgNode.get() );
    }
}

void WMFiberClustering::update()
{
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_osgNode.get() );

    if( !( m_dLtTableExists = dLtTableExists() ) )
    {
        debugLog() << "Consider old table as invalid.";
        m_dLtTable.reset( new WDXtLookUpTable( m_fibs->size() ) );
    }

    infoLog() << "Proximity threshold: " << m_proximity_t;
    infoLog() << "Maximum inter cluster distance threshold: " << m_maxDistance_t;

    cluster();
    paint();
    // TODO(math): For reasons of simplicity just forward one cluster to the voxelizer
    for( size_t i = 0; i < m_clusters.size(); ++i )
    {
        m_clusters[ i ].setDataSetReference( m_fibs );
    }

    boost::shared_ptr< WFiberCluster > c;
    if( m_clusterOutputID > m_clusters.size() )
    {
        errorLog() << "Invalid cluster ID for output selected: " << m_clusterOutputID << " using default ID 0";
        m_clusterOutputID = 0;
    }
    debugLog() << "Cluster ID for output: " << m_clusterOutputID;
    c = boost::shared_ptr< WFiberCluster >( new WFiberCluster( m_clusters[ m_clusterOutputID ] ) );
    m_output->updateData( c );
}

boost::shared_ptr< WDataSetSingle > WMFiberClustering::blurClusters() const
{
    boost::shared_ptr< WValueSet< double > > valueSet;
    boost::shared_ptr< WGridRegular3D > grid;
    std::vector< double > data;
    valueSet = boost::shared_ptr< WValueSet< double > >( new WValueSet< double >( 0, 3, data, W_DT_DOUBLE ) );
    grid = boost::shared_ptr< WGridRegular3D >( new WGridRegular3D( 100, 100, 100, 0.5, 0.5, 0.5 ) );
    return boost::shared_ptr< WDataSetSingle >( new WDataSetSingle( valueSet, grid ) );
}

bool WMFiberClustering::dLtTableExists()
{
    std::string dLtFileName = lookUpTableFileName();

    // TODO(math): replace this hard coded path when properties are available
    if( wiotools::fileExists( dLtFileName ) )
    {
        try
        {
            debugLog() << "trying to read table from: " << dLtFileName;
            // TODO(math): replace this hard coded path when properties are available
            WReaderLookUpTableVTK r( dLtFileName );
            using boost::shared_ptr;
            using std::vector;
            shared_ptr< vector< double > > data = shared_ptr< vector < double > >( new vector< double >() );
            r.readTable( data );
            m_dLtTable.reset( new WDXtLookUpTable( static_cast< size_t >( data->back() ) ) );
            m_lastFibsSize = static_cast< size_t >( data->back() );

            // remove the dimension from data array since it's not representing any distance
            data->pop_back();

            m_dLtTable->setData( *data );

            return true;
        }
        catch( WDHException e )
        {
            debugLog() << e.what() << std::endl;
        }
    }
    return false;
}

void WMFiberClustering::cluster()
{
    infoLog() << "Start clustering with " << m_fibs->size() << " fibers.";
    m_clusters.clear();  // remove evtl. old clustering
    size_t numFibers = m_fibs->size();

    m_clusterIDs = std::vector< size_t >( numFibers, 0 );

    for( size_t i = 0; i < numFibers; ++i )
    {
        m_clusters.push_back( WFiberCluster( i ) );
        m_clusterIDs[i] = i;
    }

    boost::shared_ptr< WProgress > progress = boost::shared_ptr< WProgress >( new WProgress( "Fiber clustering", numFibers ) );
    m_progress->addSubProgress( progress );

    WDLTMetric dLt( m_proximity_t * m_proximity_t );  // metric instance for computation of the dLt measure
    for( size_t q = 0; q < numFibers; ++q )  // loop over all "symmetric" fibers pairs
    {
        for( size_t r = q + 1;  r < numFibers; ++r )
        {
            if( m_clusterIDs[q] != m_clusterIDs[r] )  // both fibs are in different clusters
            {
                if( !m_dLtTableExists )
                {
                    (*m_dLtTable)( q, r ) = dLt.dist( (*m_fibs)[q], (*m_fibs)[r] );
                }
                if( (*m_dLtTable)( q, r ) < m_maxDistance_t )  // q and r provide an inter-cluster-link
                {
                    meld( m_clusterIDs[q], m_clusterIDs[r] );
                }
            }
        }

        ++*progress;
    }
    m_dLtTableExists = true;

    // remove empty clusters
    WFiberCluster emptyCluster;
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
    infoLog() << "Using " << m_clusters.size() << " clusters.";

    m_lastFibsSize = m_fibs->size();

    WWriterLookUpTableVTK w( lookUpTableFileName(), true );
    w.writeTable( m_dLtTable->getData(), m_lastFibsSize );
}

osg::ref_ptr< osg::Geode > WMFiberClustering::genFiberGeode( const WFiberCluster &cluster ) const
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
        geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP,
                                                        vertices->size() - fib.size(),
                                                        fib.size() ) );
    }

    geometry->setVertexArray( vertices );

    ref_ptr< osg::Vec4Array > colors = ref_ptr< osg::Vec4Array >( new osg::Vec4Array );
    colors->push_back( wge::osgColor( cluster.getColor() ) );
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
    m_osgNode = osg::ref_ptr< WGEGroupNode >( new WGEGroupNode );
    size_t numFibers = 0;
    std::stringstream clusterLog;
    for( size_t i = 0; i < m_clusters.size(); ++i, hue += hue_increment )
    {
        color.setHSV( hue, 1.0, 0.75 );
        m_clusters[i].setColor( color );
        m_osgNode->insert( genFiberGeode( m_clusters[i] ).get() );
        clusterLog << m_clusters[i].size() << " ";
        numFibers += m_clusters[i].size();
    }
    debugLog() << "Clusters of sizes: " << clusterLog.str();
    debugLog() << "Painted: " << numFibers << " fibers out of: " << m_fibs->size();
    m_osgNode->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_osgNode.get() );
}

void WMFiberClustering::meld( size_t qClusterID, size_t rClusterID )
{
    // first choose the cluster with the smaller ID
    if( qClusterID > rClusterID )  // merge always to the cluster with the smaller id
    {
        std::swap( qClusterID, rClusterID );
    }

    WFiberCluster& qCluster = m_clusters[ qClusterID ];
    WFiberCluster& rCluster = m_clusters[ rClusterID ];

    assert( !qCluster.empty() && !rCluster.empty() );

    // second update m_clusterIDs array
    std::list< size_t >::const_iterator cit = rCluster.getIndices().begin();
    std::list< size_t >::const_iterator cit_end = rCluster.getIndices().end();
    for( ; cit != cit_end; ++cit )
    {
        m_clusterIDs[*cit] = qClusterID;
    }

    // and at last merge them
    qCluster.merge( rCluster );

    assert( rCluster.empty() );
}

void WMFiberClustering::connectors()
{
    using boost::shared_ptr;
    typedef WModuleInputData< WDataSetFibers > InputData;  // just an alias
    typedef WModuleOutputData< WFiberCluster > OutputData; // -"-

    m_fiberInput = shared_ptr< InputData >( new InputData( shared_from_this(), "fiberInput", "A loaded fiber dataset." ) );
    m_output = shared_ptr< OutputData >( new OutputData( shared_from_this(), "One Clulster", "One Cluster" ) );

    addConnector( m_fiberInput );
    addConnector( m_output );
    WModule::connectors();  // call WModules initialization
}

void WMFiberClustering::properties()
{
    // this bool is hidden and used for hiding the osgNode
    m_properties->addBool( "active", true, true )->connect( boost::bind( &WMFiberClustering::slotPropertyChanged, this, _1 ) );
    m_properties->addDouble( "max distance threshold",
                             m_maxDistance_t,
                             false,
                             "Maximum distance of two fibers in one cluster."
                           )->connect( boost::bind( &WMFiberClustering::slotPropertyChanged, this, _1 ) );
    m_properties->addDouble( "proximity threshold",
                             m_proximity_t,
                             false,
                             "defines the minimum distance between two fibers which should be considered in distance measure."
                           )->connect( boost::bind( &WMFiberClustering::slotPropertyChanged, this, _1 ) );
    m_properties->addInt( "min cluster size",
                          m_minClusterSize,
                          false,
                          "All clusters up to this size will be discarded."
                        )->connect( boost::bind( &WMFiberClustering::slotPropertyChanged, this, _1 ) );
    m_properties->addInt( "output cluster id",
                          m_clusterOutputID,
                          false,
                          "Only the cluster with this ID will be connected to the output."
                        )->connect( boost::bind( &WMFiberClustering::slotPropertyChanged, this, _1 ) );
    m_properties->addBool( "separate primitives",
                           m_separatePrimitives,
                           false,
                           "If true each cluster has its own OSG node"
                         )->connect( boost::bind( &WMFiberClustering::slotPropertyChanged, this, _1 ) );
}

void WMFiberClustering::slotPropertyChanged( std::string propertyName )
{
    if( propertyName == "active" )
    {
        if ( m_properties->getValue< bool >( propertyName ) )
        {
            m_osgNode->setNodeMask( 0xFFFFFFFF );
        }
        else
        {
            m_osgNode->setNodeMask( 0x0 );
        }
    }
    else if( propertyName == "max distance threshold" )
    {
        m_maxDistance_t = m_properties->getValue< double >( propertyName );
        update();
    }
    else if( propertyName == "min cluster size" )
    {
        m_minClusterSize = m_properties->getValue< double >( propertyName );
        update();
    }
    else if( propertyName == "proximity threshold" )
    {
        m_proximity_t = m_properties->getValue< double >( propertyName );
        update();
    }
    else if( propertyName == "output cluster id" )
    {
        m_clusterOutputID = m_properties->getValue< size_t >( propertyName );
        boost::shared_ptr< WFiberCluster > c;
        if( m_clusterOutputID > m_clusters.size() )
        {
            errorLog() << "Invalid cluster ID for output selected: " << m_clusterOutputID << " using default ID 0";
            m_clusterOutputID = 0;
        }
        debugLog() << "Cluster ID for output: " << m_clusterOutputID;
        // TODO(math): For reasons of simplicity just forward one cluster to the voxelizer
        c = boost::shared_ptr< WFiberCluster >( new WFiberCluster( m_clusters[ m_clusterOutputID ] ) );
        m_output->updateData( c );
    }
    else
    {
        // instead of WLogger we must use std::cerr since WLogger needs to much time!
        std::cerr << propertyName << std::endl;
        assert( 0 && "This property name is not supported by this function yet." );
    }
}

std::string WMFiberClustering::lookUpTableFileName() const
{
    std::stringstream newExtension;
    newExtension << std::fixed << std::setprecision( 2 );
    newExtension << ".pt-" << m_proximity_t << ".dlt";
    boost::filesystem::path fibFileName( m_fibs->getFileName() );
    return fibFileName.replace_extension( newExtension.str() ).string();
}
