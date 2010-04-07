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
#include "WMFiberClustering.h"
#include "fiberClustering.xpm"

WMFiberClustering::WMFiberClustering()
    : WModule(),
      m_lastFibsSize( 0 ),
      m_dLtTableExists( false ),
      m_update( new WCondition() ),
      m_updateOutput( new WCondition() )
{
}

WMFiberClustering::~WMFiberClustering()
{
}

boost::shared_ptr< WModule > WMFiberClustering::factory() const
{
    return boost::shared_ptr< WModule >( new WMFiberClustering() );
}

const char** WMFiberClustering::getXPMIcon() const
{
    return fiberClustering_xpm;
}

void WMFiberClustering::moduleMain()
{
    // when conditions are fireing while wait() is not reached: wait terminates
    // and behaves as if the appropriate conditions have had fired. But it is
    // not detectable how many times a condition has fired.
    m_moduleState.setResetable();
    m_moduleState.add( m_fiberInput->getDataChangedCondition() );
    m_moduleState.add( m_updateOutput );
    m_moduleState.add( m_update );

    ready();

    while ( !m_shutdownFlag() ) // loop until the module container requests the module to quit
    {
        if ( !m_fiberInput->getData().get() ) // ok, the output has not yet sent data
        {
            m_moduleState.wait();
            continue;
        }
        if( m_rawFibs != m_fiberInput->getData() ) // in case data has changed
        {
            m_rawFibs = m_fiberInput->getData();
            assert( m_rawFibs.get() );
            infoLog() << "Start: WDataSetFibers => WDataSetFiberVector";
            m_fibs = boost::shared_ptr< WDataSetFiberVector >( new WDataSetFiberVector( m_rawFibs ) );
            infoLog() << "Stop:  WDataSetFibers => WDataSetFiberVector";
        }

        if( m_run->changed() )
        {
            m_run->get( true ); // eat change
            update();
        }
        if( m_clusterOutputID->changed() )
        {
            m_clusterOutputID->get( true ); // eat change
            updateOutput();
        }

        m_moduleState.wait(); // waits for firing of m_moduleState ( dataChanged, shutdown, etc. )
    }
}

void WMFiberClustering::activate()
{
    if( m_osgNode )
    {
        if ( m_active->get() )
        {
            m_osgNode->setNodeMask( 0xFFFFFFFF );
            if( m_invisibleFibers->get() )
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

void WMFiberClustering::properties()
{
    m_maxDistance_t   = m_properties->addProperty( "Max cluster distance", "Maximum distance of two fibers in one cluster.", 6.5 );
    m_proximity_t     = m_properties->addProperty( "Min point distance", "Min distance of points of two fibers which should be considered", 0.0 );
    m_minClusterSize  = m_properties->addProperty( "Min cluster size", "Minium of fibers per cluster", 10 );
    m_clusterOutputID = m_properties->addProperty( "Output cluster ID", "This cluster ID will be connected to the output.", 0, m_updateOutput );
    m_invisibleFibers = m_properties->addProperty( "Invisible fibers", "Trigger fiber display", false,
                                                    boost::bind( &WMFiberClustering::activate, this ) );
    m_run             = m_properties->addProperty( "Go", "Initiate run", false, m_update );
    m_run->get( true ); // disable initial run
}

void WMFiberClustering::updateOutput()
{
    if( m_clusters.empty() )
    {
        return;
    }

    m_clusterOutputID->setMin( 0 );
    m_clusterOutputID->setMax( m_clusters.size() - 1 );

    if( m_clusterOutputID->get() >= static_cast< int >( m_clusters.size() ) || m_clusterOutputID->get() < 0 )
    {
        warnLog() << "Invalid cluster ID for output selected: " << m_clusterOutputID->get() << " using default ID 0";
        m_clusterOutputID->set( 0, true );
    }
    debugLog() << "Cluster ID for output: " << m_clusterOutputID->get();

    m_output->updateData( boost::shared_ptr< WFiberCluster >( new WFiberCluster( m_clusters[ m_clusterOutputID->get() ] ) ) );
}

void WMFiberClustering::update()
{
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_osgNode );

    if( !( m_dLtTableExists = dLtTableExists() ) )
    {
        debugLog() << "Consider old table as invalid.";
        m_dLtTable.reset( new WDXtLookUpTable( m_fibs->size() ) );
    }

    infoLog() << "Proximity threshold: " << m_proximity_t->get();
    infoLog() << "Maximum inter cluster distance threshold: " << m_maxDistance_t->get();

    cluster();
    m_osgNode = paint();
    if( m_invisibleFibers->get() ) // check if the node mask should be set to 0x0
    {
        activate();
    }
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_osgNode );

    // TODO(math): For reasons of simplicity (no multiple input connectors possible) just forward one cluster to the voxelizer
    for( size_t i = 0; i < m_clusters.size(); ++i )
    {
        m_clusters[ i ].setDataSetReference( m_fibs );
        m_clusters[ i ].generateCenterLine();
    }

    // reset min max of the slider for the cluster ID
    updateOutput();
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
            boost::shared_ptr< std::vector< double > > data( new std::vector< double >() );
            r.readTable( data );
            m_dLtTable.reset( new WDXtLookUpTable( static_cast< size_t >( data->back() ) ) );
            m_lastFibsSize = static_cast< size_t >( data->back() );

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

void WMFiberClustering::cluster()
{
    double proximity_t = m_proximity_t->get();
    double maxDistance_t = m_maxDistance_t->get();
    size_t minClusterSize = m_minClusterSize->get();

    infoLog() << "Start clustering with " << m_fibs->size() << " fibers.";
    m_clusters.clear();  // remove evtl. old clustering
    size_t numFibers = m_fibs->size();

    m_clusterIDs = std::vector< size_t >( numFibers, 0 );

    for( size_t i = 0; i < numFibers; ++i )
    {
        m_clusters.push_back( WFiberCluster( i ) );
        m_clusterIDs[i] = i;
    }

    boost::shared_ptr< WProgress > progress( new WProgress( "Fiber clustering", numFibers ) );
    m_progress->addSubProgress( progress );

    boost::function< double ( const wmath::WFiber& q, const wmath::WFiber& r ) > dLt; // NOLINT
    const double proxSquare = proximity_t * proximity_t;
    dLt = boost::bind( wmath::WFiber::distDLT, proxSquare, _1, _2 );

    for( size_t q = 0; q < numFibers; ++q )  // loop over all "symmetric" fibers pairs
    {
        for( size_t r = q + 1;  r < numFibers; ++r )
        {
            if( m_clusterIDs[q] != m_clusterIDs[r] )  // both fibs are in different clusters
            {
                if( !m_dLtTableExists )
                {
                    (*m_dLtTable)( q, r ) = dLt( (*m_fibs)[q], (*m_fibs)[r] );
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

    m_lastFibsSize = m_fibs->size();

    if( !wiotools::fileExists( lookUpTableFileName() ) )
    {
        WWriterLookUpTableVTK w( lookUpTableFileName(), true );
        try
        {
            w.writeTable( m_dLtTable->getData(), m_lastFibsSize );
        }
        catch( const WDHIOFailure& e )
        {
            errorLog() << "Could write dlt file. check permissions! " << e.what();
        }
    }
}

osg::ref_ptr< osg::Geode > WMFiberClustering::genFiberGeode( const WFiberCluster &cluster, const WColor& color ) const
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
    colors->push_back( wge::osgColor( color ) );
    geometry->setColorArray( colors );
    geometry->setColorBinding( osg::Geometry::BIND_OVERALL );
    osg::ref_ptr< osg::Geode > geode = osg::ref_ptr< osg::Geode >( new osg::Geode );
    geode->addDrawable( geometry.get() );

    return geode;
}


osg::ref_ptr< WGEGroupNode > WMFiberClustering::paint() const
{
    // get different colors via HSV color model for each cluster
    double hue = 0.0;
    double hue_increment = 1.0 / m_clusters.size();
    WColor color;

    infoLog() << "cluster: " << m_clusters.size();
    osg::ref_ptr< WGEGroupNode > result = osg::ref_ptr< WGEGroupNode >( new WGEGroupNode );
    size_t numFibers = 0;
    std::stringstream clusterLog;
    for( size_t i = 0; i < m_clusters.size(); ++i, hue += hue_increment )
    {
        color.setHSV( hue, 1.0, 0.75 );
        result->insert( genFiberGeode( m_clusters[i], color ).get() );
        clusterLog << m_clusters[i].size() << " ";
        numFibers += m_clusters[i].size();
    }
    debugLog() << "Clusters of sizes: " << clusterLog.str();
    debugLog() << "Painted: " << numFibers << " fibers out of: " << m_fibs->size();
    result->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    return result;
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
    typedef WModuleInputData< WDataSetFibers > InputData;  // just an alias
    typedef WModuleOutputData< WFiberCluster > OutputData; // -"-

    m_fiberInput = boost::shared_ptr< InputData >( new InputData( shared_from_this(), "fiberInput", "A loaded fiber dataset." ) );
    m_output = boost::shared_ptr< OutputData >( new OutputData( shared_from_this(), "clusterOutput", "One Cluster" ) );

    addConnector( m_fiberInput );
    addConnector( m_output );
    WModule::connectors();  // call WModules initialization
}

std::string WMFiberClustering::lookUpTableFileName() const
{
    std::stringstream newExtension;
    newExtension << std::fixed << std::setprecision( 2 );
    newExtension << ".pt-" << m_proximity_t->get() << ".dlt";
    boost::filesystem::path fibFileName( m_fibs->getFileName() );
    return fibFileName.replace_extension( newExtension.str() ).string();
}


WMFiberClustering::OutputIDBound::OutputIDBound( const std::vector< WFiberCluster >& clusters )
    : m_clusters( clusters )
{
}

bool WMFiberClustering::OutputIDBound::accept( boost::shared_ptr< WPropertyVariable< WPVBaseTypes::PV_INT > > /* property */,
                                               WPVBaseTypes::PV_INT value )
{
    return ( value >= 0 ) && ( value < static_cast< int >( m_clusters.size() ) );
}
