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
    m_fiberCluster = boost::shared_ptr< InputClusterType >( new InputClusterType( shared_from_this(), "cluster", "A cluster of fibers" ) );
    addConnector( m_fiberCluster );

    m_clusterDataSet = boost::shared_ptr< InputDataSetType >( new InputDataSetType( shared_from_this(), "clusterDS", "DataSet from a cluster" ) );
    addConnector( m_clusterDataSet );

    m_paramDataSet = boost::shared_ptr< InputDataSetType >( new InputDataSetType( shared_from_this(), "paramDS", "DataSet of the parameters" ) );
    addConnector( m_paramDataSet );

    m_triangleMesh = boost::shared_ptr< InputMeshType >( new InputMeshType( shared_from_this(), "mesh", "TrianglMesh" ) );
    addConnector( m_triangleMesh );

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
    m_moduleState.add( m_clusterDataSet->getDataChangedCondition() );
    m_moduleState.add( m_paramDataSet->getDataChangedCondition() );
    m_moduleState.add( m_triangleMesh->getDataChangedCondition() );
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

        boost::shared_ptr< WDataSetSingle > newClusterDS = m_clusterDataSet->getData();
        boost::shared_ptr< WFiberCluster >  newCluster   = m_fiberCluster->getData();
        boost::shared_ptr< WDataSetSingle > newParamDS   = m_paramDataSet->getData();
        boost::shared_ptr< WTriangleMesh >  newMesh      = m_triangleMesh->getData();
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
                updateDisplay();
                updateSlices();
                renderMesh( *m_mesh );
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
    if( m_drawSlices->get( true ) )
    {
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
                m_sliceGeode->insert( wge::genPointBlobs( samplePoints, 0.1 ) );
            }
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
}

void WMClusterSlicer::renderMesh( const WTriangleMesh& mesh )
{
    m_rootNode->remove( m_surfaceGeode );
    osg::Geometry* surfaceGeometry = new osg::Geometry();
    m_surfaceGeode = osg::ref_ptr< osg::Geode >( new osg::Geode );

    osg::Vec3Array* vertices = new osg::Vec3Array;
    for( size_t i = 0; i < mesh.getNumVertices(); ++i )
    {
        wmath::WPosition vertPos;
        vertPos = mesh.getVertex( i );
        vertices->push_back( osg::Vec3( vertPos[0], vertPos[1], vertPos[2] ) );
    }
    surfaceGeometry->setVertexArray( vertices );

    osg::DrawElementsUInt* surfaceElement;

    surfaceElement = new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES, 0 );
    for( unsigned int triId = 0; triId < mesh.getNumTriangles(); ++triId )
    {
        for( unsigned int vertId = 0; vertId < 3; ++vertId )
        {
            surfaceElement->push_back( mesh.getTriangleVertexId( triId, vertId ) );
        }
    }
    surfaceGeometry->addPrimitiveSet( surfaceElement );

    // ------------------------------------------------
    // normals
    osg::ref_ptr< osg::Vec3Array> normals( new osg::Vec3Array() );

    for( unsigned int vertId = 0; vertId < mesh.getNumVertices(); ++vertId )
    {
        wmath::WVector3D tmpNormal = mesh.getVertexNormal( vertId );
        normals->push_back( osg::Vec3( tmpNormal[0], tmpNormal[1], tmpNormal[2] ) );
    }
    surfaceGeometry->setNormalArray( normals.get() );
    surfaceGeometry->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );

    m_surfaceGeode->addDrawable( surfaceGeometry );
    osg::StateSet* state = m_surfaceGeode->getOrCreateStateSet();

    // ------------------------------------------------
    // colors
    osg::ref_ptr< osg::Vec4Array > colors   = osg::ref_ptr< osg::Vec4Array >( new osg::Vec4Array );
    for( size_t i = 0; i < mesh.getNumVertices(); ++i )
    {
        colors->push_back( osg::Vec4( .9f, .9f, 0.9f, 1.0f ) );
    }
    for( std::vector< std::pair< double, WPlane > >::const_iterator slice = m_slices->begin(); slice != m_slices->end(); ++slice )
    {
        boost::shared_ptr< std::set< size_t > > coloredVertices = tm_utils::intersection( mesh, slice->second );
        double scaledAVG = 0.0;
        if( m_maxAVG == m_minAVG )
        {
            scaledAVG = 0;
        }
        else
        {
            scaledAVG = ( slice->first - m_minAVG ) / ( m_maxAVG - m_minAVG );
        }
        WColor sliceColor( scaledAVG, scaledAVG, 1 );
        for( std::set< size_t >::const_iterator coloredVertex = coloredVertices->begin(); coloredVertex != coloredVertices->end(); ++coloredVertex )
        {
            colors->at( *coloredVertex ) = wge::osgColor( sliceColor );
        }
    }

    surfaceGeometry->setColorArray( colors );
    surfaceGeometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );

    osg::ref_ptr<osg::LightModel> lightModel = new osg::LightModel();
    lightModel->setTwoSided( true );
    state->setAttributeAndModes( lightModel.get(), osg::StateAttribute::ON );
    state->setMode(  GL_BLEND, osg::StateAttribute::ON  );

    m_rootNode->insert( m_surfaceGeode );
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
