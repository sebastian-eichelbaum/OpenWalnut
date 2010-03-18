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
#include <set>
#include <string>
#include <utility>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <osg/Geode>
#include <osg/LightModel>

#include "../../common/datastructures/WTriangleMesh.h"
#include "../../common/math/WPlane.h"
#include "../../common/WColor.h"
#include "../../graphicsEngine/WGEGeodeUtils.h"
#include "../../graphicsEngine/WGEGeometryUtils.h"
#include "../../graphicsEngine/WGEUtils.h"
#include "../../kernel/WKernel.h"
#include "WMClusterSlicer.h"

WMClusterSlicer::WMClusterSlicer()
    : WModule(),
      m_rootNode( osg::ref_ptr< WGEGroupNode >( new WGEGroupNode() ) ),
      m_maxAVG( 0.0 ),
      m_minAVG( 0.0 )
{
}

WMClusterSlicer::~WMClusterSlicer()
{
}

boost::shared_ptr< WModule > WMClusterSlicer::factory() const
{
    return boost::shared_ptr< WModule >( new WMClusterSlicer() );
}

const std::string WMClusterSlicer::getName() const
{
    return "Cluster Slicer";
}

const std::string WMClusterSlicer::getDescription() const
{
    return "Slices a cluster";
}

void WMClusterSlicer::connectors()
{
    m_fiberClusterInput = boost::shared_ptr< InputClusterType >( new InputClusterType( shared_from_this(), "cluster", "A cluster of fibers" ) );
    addConnector( m_fiberClusterInput );

    m_clusterDataSetInput = boost::shared_ptr< InputDataSetType >( new InputDataSetType( shared_from_this(), "clusterDS", "DataSet from cluster" ) );
    addConnector( m_clusterDataSetInput );

    m_paramDataSetInput = boost::shared_ptr< InputDataSetType >( new InputDataSetType( shared_from_this(), "paramDS", "DataSet of the parameters" ) );
    addConnector( m_paramDataSetInput );

    m_triangleMeshInput = boost::shared_ptr< InputMeshType >( new InputMeshType( shared_from_this(), "mesh", "TrianglMesh" ) );
    addConnector( m_triangleMeshInput );

    m_colorMapOutput = boost::shared_ptr< OutputColorMapType >( new OutputColorMapType( shared_from_this(), "colorMap", "VertexID and colors" ) );
    addConnector( m_colorMapOutput );

    WModule::connectors();
}

void WMClusterSlicer::properties()
{
    m_drawISOVoxels = m_properties2->addProperty( "Show/Hide ISO Voxels", "Show/Hide voxels withing a given ISOSurface.", true );
    m_drawSlices    = m_properties2->addProperty( "Show/Hide Slices", "Show/Hide slices along center line", true );
    m_isoValue      = m_properties2->addProperty( "Iso Value", "", 0.01 );
}

void WMClusterSlicer::moduleMain()
{
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootNode );

    m_moduleState.setResetable( true, true );
    // m_moduleState.add( m_clusterDataSetInput->getDataChangedCondition() );
    m_moduleState.add( m_paramDataSetInput->getDataChangedCondition() );
    m_moduleState.add( m_triangleMeshInput->getDataChangedCondition() );
    m_moduleState.add( m_drawSlices->getCondition() );
    m_moduleState.add( m_drawISOVoxels->getCondition() );

    ready();

    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        if( m_shutdownFlag() )
        {
            break;
        }

        boost::shared_ptr< WDataSetSingle > newClusterDS = m_clusterDataSetInput->getData();
        boost::shared_ptr< WFiberCluster >  newCluster   = m_fiberClusterInput->getData();
        boost::shared_ptr< WDataSetSingle > newParamDS   = m_paramDataSetInput->getData();
        boost::shared_ptr< WTriangleMesh >  newMesh      = m_triangleMeshInput->getData();
        bool dataChanged = ( m_clusterDS != newClusterDS ) || ( m_cluster != newCluster ) || ( m_paramDS != newParamDS ) || ( m_mesh != newMesh );
        bool dataValid = m_clusterDS && m_cluster && m_paramDS && m_mesh;
        if( dataChanged )
        {
            if( m_mesh != newMesh )
            {
                m_mesh = newMesh;
                if( m_mesh )
                {
                    m_mesh->computeVertNormals(); // time consuming
                }
            }
            m_clusterDS = newClusterDS;
            m_cluster = newCluster;
            m_paramDS = newParamDS;
            dataValid = m_clusterDS && m_cluster && m_paramDS && m_mesh;
            if( !m_clusterDS || !m_cluster || !m_paramDS || !m_mesh )
            {
                continue;
            }

            assert( m_clusterDS && "Invalid dataset to compute JoinTree on" );
            m_joinTree = boost::shared_ptr< WJoinContourTree >( new WJoinContourTree( m_clusterDS ) );
            m_joinTree->buildJoinTree();
        }

        if( ( m_isoValue->changed() || dataChanged ) && m_joinTree )
        {
            assert( m_clusterDS && "JoinTree cannot be valid since there is no valid m_clusterDS." );
            m_isoVoxels = m_joinTree->getVolumeVoxelsEnclosedByISOSurface( m_isoValue->get() );
        }

        if( m_drawISOVoxels->changed() || m_isoValue->changed() || dataChanged )
        {
            if( dataValid )
            {
                debugLog() << "Full update requested.";
                updateDisplay();
                updateSlices();
                sliceAndColorMesh( *m_mesh );
            }
        }

        if( m_drawSlices->changed() )
        {
            if( dataValid )
            {
                updateSlices();
            }
        }
    }

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );
}

double WMClusterSlicer::averageParameter( boost::shared_ptr< std::set< wmath::WPosition > > samplePoints ) const
{
    size_t valueCount = 0;
    double avg = 0.0;
    bool inParamGrid = false;
    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( m_clusterDS->getGrid() );
    assert( grid != 0 );

    for( std::set< wmath::WPosition >::iterator pos = samplePoints->begin(); pos != samplePoints->end(); )
    {
        // ATTENTION: We don't interpolate in m_clusterDS since it might be the wrong component!!
        int id = grid->getVoxelNum( *pos );
        if( id >= 0 ) // check if the position is inside of the main component (biggest volume)
        {
            if( m_isoVoxels->count( static_cast< size_t >( id ) ) == 1 ) // check if its in main component
            {
                double value = m_paramDS->interpolate( *pos, &inParamGrid );
                if( inParamGrid ) // check if its in paramDS
                {
                    avg += value;
                    ++valueCount;
                    ++pos;
                    continue;
                }
            }
        }
        samplePoints->erase( pos++ ); // erase in case the pos is not in main component or in paramDS or in clusterDS
    }
    return ( !valueCount ? avg : avg / valueCount );
}

void WMClusterSlicer::updateSlices()
{
    m_rootNode->remove( m_sliceGeode );
    boost::shared_ptr< wmath::WFiber > centerLine = m_cluster->getCenterLine();
    if( !centerLine.get() )
    {
        errorLog() << "CenterLine of the bundle is empty => no slices are drawn";
    }
    else
    {
        m_slices = boost::shared_ptr< std::vector< std::pair< double, WPlane > > >( new std::vector< std::pair< double, WPlane > > );
        m_maxAVG = wlimits::MIN_DOUBLE;
        m_minAVG = wlimits::MAX_DOUBLE;
        m_sliceGeode = osg::ref_ptr< WGEGroupNode >( new WGEGroupNode ); // discard old group node
        const wmath::WFiber& cL = *centerLine; // just an alias
        for( size_t i = 1; i < cL.size(); ++i )
        {
            wmath::WVector3D tangent = cL[i] - cL[i-1];
            WPlane p( tangent, cL[i-1] );

            boost::shared_ptr< std::set< wmath::WPosition > > samplePoints = p.samplePoints( 0.5, 40, 40 );
            double avg = averageParameter( samplePoints );
            if( avg > m_maxAVG )
            {
                m_maxAVG = avg;
            }
            if( avg < m_minAVG )
            {
                m_minAVG = avg;
            }
            m_slices->push_back( std::make_pair( avg, p ) );
            if( m_drawSlices->get( true ) )
            {
                m_sliceGeode->insert( wge::genPointBlobs( samplePoints, 0.1 ) );
            }
        }
    }
    if( m_drawSlices->get( true ) )
    {
        for( std::vector< std::pair< double, WPlane > >::const_iterator cit = m_slices->begin(); cit != m_slices->end(); ++cit )
        {
            double scaledAVG = 0.0;
            if( m_maxAVG == m_minAVG )
            {
                scaledAVG = 0;
            }
            else
            {
                scaledAVG = ( cit->first - m_minAVG ) / ( m_maxAVG - m_minAVG );
            }
            WColor color( scaledAVG, scaledAVG, 1 );
            m_sliceGeode->insert( wge::genFinitePlane( 10, 10, cit->second, color, true ) );
        }
        m_rootNode->insert( m_sliceGeode );
    }
}

void WMClusterSlicer::sliceAndColorMesh( const WTriangleMesh& mesh )
{
    m_colorMap = boost::shared_ptr< WColoredVertices >( new WColoredVertices );
    std::map< size_t, WColor > cm;

    for( std::vector< std::pair< double, WPlane > >::const_iterator slice = m_slices->begin(); slice != m_slices->end(); ++slice )
    {
        boost::shared_ptr< std::set< size_t > > coloredVertices = tm_utils::intersection( mesh, slice->second );
        double scaledAVG = ( m_maxAVG == m_minAVG ? 0.0 : ( slice->first - m_minAVG ) / ( m_maxAVG - m_minAVG ) );
        WColor sliceColor( scaledAVG, scaledAVG, 1 );
        for( std::set< size_t >::const_iterator coloredVertex = coloredVertices->begin(); coloredVertex != coloredVertices->end(); ++coloredVertex )
        {
            cm[ *coloredVertex ] = sliceColor;
        }
    }

    m_colorMap->setData( cm );
    m_colorMapOutput->updateData( m_colorMap );
}

void WMClusterSlicer::updateDisplay()
{
    m_rootNode->remove( m_isoVoxelGeode );
    m_isoVoxelGeode = osg::ref_ptr< osg::Geode >( new osg::Geode() ); // discard old geode

    if( m_drawISOVoxels->get( true ) )
    {
        assert( m_isoVoxels && "JoinTree cannot be valid since there is no valid m_clusterDS." );
        m_isoVoxelGeode = generateISOVoxelGeode();
        m_rootNode->insert( m_isoVoxelGeode );
    }
}

osg::ref_ptr< osg::Geode > WMClusterSlicer::generateISOVoxelGeode() const
{
    boost::shared_ptr< std::set< wmath::WPosition > > points( new std::set< wmath::WPosition > );
    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( m_clusterDS->getGrid() );
    assert( grid != 0 );
    for( std::set< size_t >::const_iterator cit = m_isoVoxels->begin(); cit != m_isoVoxels->end(); ++cit )
    {
        points->insert( grid->getPosition( *cit ) );
    }
    return wge::genPointBlobs< std::set< wmath::WPosition > >( points, grid->getOffsetX() );
}

void WMClusterSlicer::activate()
{
    if( m_rootNode )
    {
        if( m_active->get() )
        {
            m_rootNode->setNodeMask( 0xFFFFFFFF );
        }
        else
        {
            m_rootNode->setNodeMask( 0x0 );
        }
    }

    WModule::activate();
}
