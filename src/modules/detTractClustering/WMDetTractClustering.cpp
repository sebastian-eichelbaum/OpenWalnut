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

#include "../../common/datastructures/WDXtLookUpTable.h"
#include "../../common/datastructures/WFiber.h"
#include "../../common/WColor.h"
#include "../../common/WIOTools.h"
#include "../../common/WLogger.h"
#include "../../common/WProgress.h"
#include "../../common/WStringUtils.h"
#include "../../dataHandler/datastructures/WFiberCluster.h"
#include "../../dataHandler/exceptions/WDHIOFailure.h"
#include "../../dataHandler/io/WReaderLookUpTableVTK.h"
#include "../../dataHandler/io/WWriterLookUpTableVTK.h"
#include "../../dataHandler/WDataSetFiberVector.h"
#include "../../dataHandler/WSubject.h"
#include "../../graphicsEngine/WGEUtils.h"
#include "../../kernel/WKernel.h"
#include "detTractClustering.xpm"
#include "WMDetTractClustering.h"

WMDetTractClustering::WMDetTractClustering()
    : WModule(),
      m_lastTractsSize( 0 ),
      m_dLtTableExists( false ),
      m_update( new WCondition() )
{
}

WMDetTractClustering::~WMDetTractClustering()
{
}

boost::shared_ptr< WModule > WMDetTractClustering::factory() const
{
    return boost::shared_ptr< WModule >( new WMDetTractClustering() );
}

const char** WMDetTractClustering::getXPMIcon() const
{
    return detTractClustering_xpm;
}

void WMDetTractClustering::moduleMain()
{
    m_moduleState.setResetable( true, true ); // modules state remembers fired events while not waiting
    m_moduleState.add( m_tractInput->getDataChangedCondition() );
    m_moduleState.add( m_update );

    ready();

    while ( !m_shutdownFlag() )
    {
        if ( !m_tractInput->getData().get() ) // ok, the output has not yet sent data
        {
            m_moduleState.wait();
            continue;
        }
        if( m_rawTracts != m_tractInput->getData() ) // in case data has changed
        {
            m_rawTracts = m_tractInput->getData();
            m_tracts = boost::shared_ptr< WDataSetFiberVector >( new WDataSetFiberVector( m_rawTracts ) );
        }

        if( m_run->changed() )
        {
            m_run->get( true );
            update();
        }
        if( m_clusterOutputID->changed() )
        {
            m_clusterOutputID->get( true );
            updateOutput();
        }

        m_moduleState.wait(); // waits for firing of m_moduleState ( dataChanged, shutdown, etc. )
    }
}

void WMDetTractClustering::activate()
{
    if( m_osgNode )
    {
        if ( m_active->get() )
        {
            m_osgNode->setNodeMask( 0xFFFFFFFF );
            if( m_invisibleTracts->get() )
            {
                m_osgNode->setNodeMask( 0x0 );
            }
        }
        else
        {
            m_osgNode->setNodeMask( 0x0 );
        }
    }

    WModule::activate();
}

void WMDetTractClustering::properties()
{
    m_maxDistance_t   = m_properties->addProperty( "Max cluster distance", "Maximum distance of two tracts in one cluster.", 6.5 );
    m_proximity_t     = m_properties->addProperty( "Min point distance", "Min distance of points of two tracts which should be considered", 0.0 );
    m_minClusterSize  = m_properties->addProperty( "Min cluster size", "Minium of tracts per cluster", 10 );
    m_clusterOutputID = m_properties->addProperty( "Output cluster ID", "This cluster ID will be connected to the output.", 0, m_update );
    m_invisibleTracts = m_properties->addProperty( "Invisible tracts", "Trigger tract display", false,
                                                   boost::bind( &WMDetTractClustering::activate, this ) );
    m_run             = m_properties->addProperty( "Start clustering", "Start", WPVBaseTypes::PV_TRIGGER_READY, m_update );
}

void WMDetTractClustering::updateOutput()
{
    if( m_clusters.empty() )
    {
        return;
    }

    if( m_clusterOutputID->get() >= static_cast< int >( m_clusters.size() ) || m_clusterOutputID->get() < 0 )
    {
        warnLog() << "Invalid cluster ID for output selected: " << m_clusterOutputID->get() << " using default ID 0";
        m_clusterOutputID->set( 0, true );
    }
    m_output->updateData( boost::shared_ptr< WFiberCluster >( new WFiberCluster( m_clusters[ m_clusterOutputID->get() ] ) ) );
}

void WMDetTractClustering::update()
{
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_osgNode );

    if( !( m_dLtTableExists = dLtTableExists() ) )
    {
        debugLog() << "Consider old table as invalid.";
        m_dLtTable.reset( new WDXtLookUpTable( m_tracts->size() ) );
    }

    infoLog() << "Proximity threshold: " << m_proximity_t->get();
    infoLog() << "Maximum inter cluster distance threshold: " << m_maxDistance_t->get();

    cluster();
    m_osgNode = paint();
    if( m_invisibleTracts->get() ) // check if the node mask should be set to 0x0
    {
        activate();
    }
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_osgNode );

    // TODO(math): For reasons of simplicity (no multiple input connectors possible) just forward one cluster to the voxelizer
    for( size_t i = 0; i < m_clusters.size(); ++i )
    {
        m_clusters[ i ].setDataSetReference( m_tracts );
        m_clusters[ i ].generateCenterLine();
    }

    m_clusterOutputID->setMin( 0 );
    m_clusterOutputID->setMax( m_clusters.size() - 1 );

    // reset min max of the slider for the cluster ID
    updateOutput();
}

bool WMDetTractClustering::dLtTableExists()
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
            boost::shared_ptr< std::vector< double > > data( new std::vector< double >() );
            r.readTable( data );
            m_dLtTable.reset( new WDXtLookUpTable( static_cast< size_t >( data->back() ) ) );
            m_lastTractsSize = static_cast< size_t >( data->back() );

            // remove the dimension from data array since it's not representing any distance
            data->pop_back();

            m_dLtTable->setData( *data );

            return true;
        }
        catch( const WDHException& e )
        {
            debugLog() << e.what() << std::endl;
        }
    }
    return false;
}

void WMDetTractClustering::cluster()
{
    double proximity_t = m_proximity_t->get();
    double maxDistance_t = m_maxDistance_t->get();
    size_t minClusterSize = m_minClusterSize->get();

    infoLog() << "Start clustering with " << m_tracts->size() << " tracts.";
    m_clusters.clear();  // remove evtl. old clustering
    size_t numTracts = m_tracts->size();

    m_clusterIDs = std::vector< size_t >( numTracts, 0 );

    for( size_t i = 0; i < numTracts; ++i )
    {
        m_clusters.push_back( WFiberCluster( i ) );
        m_clusterIDs[i] = i;
    }

    boost::shared_ptr< WProgress > progress( new WProgress( "Tract clustering", numTracts ) );
    m_progress->addSubProgress( progress );

    boost::function< double ( const wmath::WFiber& q, const wmath::WFiber& r ) > dLt; // NOLINT
    const double proxSquare = proximity_t * proximity_t;
    dLt = boost::bind( wmath::WFiber::distDLT, proxSquare, _1, _2 );

    for( size_t q = 0; q < numTracts; ++q )  // loop over all "symmetric" tract pairs
    {
        for( size_t r = q + 1;  r < numTracts; ++r )
        {
            if( m_clusterIDs[q] != m_clusterIDs[r] )  // both fibs are in different clusters
            {
                if( !m_dLtTableExists )
                {
                    (*m_dLtTable)( q, r ) = dLt( (*m_tracts)[q], (*m_tracts)[r] );
                }
                if( (*m_dLtTable)( q, r ) < maxDistance_t )  // q and r provide an inter-cluster-link
                {
                    meld( m_clusterIDs[q], m_clusterIDs[r] );
                }
            }
        }
        ++*progress;
    }
    progress->finish();
    m_dLtTableExists = true;

    // remove empty clusters
    WFiberCluster emptyCluster;
    m_clusters.erase( std::remove( m_clusters.begin(), m_clusters.end(), emptyCluster ), m_clusters.end() );

    // determine #clusters and #small_clusters which are below a certain size
    size_t numSmallClusters = 0;
    for( size_t i = 0; i < m_clusters.size(); ++i )
    {
        m_clusters[i].sort();  // Refactor: why do we need sorting here?
        if( m_clusters[i].size() < minClusterSize )
        {
            m_clusters[i].clear();  // make small clusters empty to remove them easier
            ++numSmallClusters;
        }
    }
    infoLog() << "Found " << m_clusters.size() << " clusters where " << numSmallClusters << " clusters are smaller than: " << minClusterSize;
    m_clusters.erase( std::remove( m_clusters.begin(), m_clusters.end(), emptyCluster ), m_clusters.end() );
    infoLog() << "Using " << m_clusters.size() << " clusters.";

    m_lastTractsSize = m_tracts->size();

    if( !wiotools::fileExists( lookUpTableFileName() ) )
    {
        WWriterLookUpTableVTK w( lookUpTableFileName(), true );
        try
        {
            w.writeTable( m_dLtTable->getData(), m_lastTractsSize );
        }
        catch( const WDHIOFailure& e )
        {
            errorLog() << "Could write dlt file. check permissions! " << e.what();
        }
    }
}

osg::ref_ptr< osg::Geode > WMDetTractClustering::genTractGeode( const WFiberCluster &cluster, const WColor& color ) const
{
    using osg::ref_ptr;
    ref_ptr< osg::Vec3Array > vertices = ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    ref_ptr< osg::Geometry > geometry = ref_ptr< osg::Geometry >( new osg::Geometry );

    std::list< size_t >::const_iterator cit = cluster.getIndices().begin();
    for( ; cit !=  cluster.getIndices().end(); ++cit )
    {
        const wmath::WFiber &fib = (*m_tracts)[ *cit ];
        for( size_t i = 0; i < fib.size(); ++i )
        {
            vertices->push_back( osg::Vec3( fib[i][0], fib[i][1], fib[i][2] ) );
        }
        geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, vertices->size() - fib.size(), fib.size() ) );
    }

    geometry->setVertexArray( vertices );

    ref_ptr< osg::Vec4Array > colors = ref_ptr< osg::Vec4Array >( new osg::Vec4Array );
    colors->push_back( wge::osgColor( color ) );
    geometry->setColorArray( colors );
    geometry->setColorBinding( osg::Geometry::BIND_OVERALL );
    osg::ref_ptr< osg::Geode > geode = osg::ref_ptr< osg::Geode >( new osg::Geode );
    geode->addDrawable( geometry.get() );

    return geode;
}


osg::ref_ptr< WGEGroupNode > WMDetTractClustering::paint() const
{
    // get different colors via HSV color model for each cluster
    double hue = 0.0;
    double hue_increment = 1.0 / m_clusters.size();
    WColor color;

    infoLog() << "cluster: " << m_clusters.size();
    osg::ref_ptr< WGEGroupNode > result = osg::ref_ptr< WGEGroupNode >( new WGEGroupNode );
    size_t numTracts = 0;
    std::stringstream clusterLog;
    for( size_t i = 0; i < m_clusters.size(); ++i, hue += hue_increment )
    {
        color.setHSV( hue, 1.0, 0.75 );
        result->insert( genTractGeode( m_clusters[i], color ).get() );
        clusterLog << m_clusters[i].size() << " ";
        numTracts += m_clusters[i].size();
    }
    debugLog() << "Clusters of sizes: " << clusterLog.str();
    debugLog() << "Painted: " << numTracts << " tracts out of: " << m_tracts->size();
    result->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    return result;
}

void WMDetTractClustering::meld( size_t qClusterID, size_t rClusterID )
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

void WMDetTractClustering::connectors()
{
    typedef WModuleInputData< WDataSetFibers > InputData;  // just an alias
    typedef WModuleOutputData< WFiberCluster > OutputData; // -"-

    m_tractInput = boost::shared_ptr< InputData >( new InputData( shared_from_this(), "tractInput", "A deterministic tract dataset." ) );
    m_output = boost::shared_ptr< OutputData >( new OutputData( shared_from_this(), "clusterOutput", "A set of tract indices aka cluster" ) );

    addConnector( m_tractInput );
    addConnector( m_output );
    WModule::connectors();  // call WModules initialization
}

std::string WMDetTractClustering::lookUpTableFileName() const
{
    std::stringstream newExtension;
    newExtension << std::fixed << std::setprecision( 2 );
    newExtension << ".pt-" << m_proximity_t->get() << ".dlt";
    boost::filesystem::path tractFileName( m_tracts->getFileName() );
    return tractFileName.replace_extension( newExtension.str() ).string();
}


WMDetTractClustering::OutputIDBound::OutputIDBound( const std::vector< WFiberCluster >& clusters )
    : m_clusters( clusters )
{
}

bool WMDetTractClustering::OutputIDBound::accept( boost::shared_ptr< WPropertyVariable< WPVBaseTypes::PV_INT > > /* property */,
                                               WPVBaseTypes::PV_INT value )
{
    return ( value >= 0 ) && ( value < static_cast< int >( m_clusters.size() ) );
}
