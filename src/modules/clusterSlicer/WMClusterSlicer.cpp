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
#include <vector>

#include "../../common/WColor.h"
#include "../../common/math/WPlane.h"
#include "../../graphicsEngine/WGEGeodeUtils.h"
#include "../../graphicsEngine/WGEGeometryUtils.h"
#include "../../graphicsEngine/WGEUtils.h"
#include "../../kernel/WKernel.h"
#include "WMClusterSlicer.h"

WMClusterSlicer::WMClusterSlicer()
    : WModule(),
      m_rootNode( osg::ref_ptr< WGEGroupNode >( new WGEGroupNode() ) )
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
    m_inputCluster = boost::shared_ptr< InputClusterType >( new InputClusterType( shared_from_this(), "cluster", "A cluster of fibers" ) );
    addConnector( m_inputCluster );

    m_inputDataSet = boost::shared_ptr< InputDataSetType >( new InputDataSetType( shared_from_this(), "dataset", "DataSet derived from a cluster" ) );
    addConnector( m_inputDataSet );

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
    m_moduleState.add( m_inputDataSet->getDataChangedCondition() );
    m_moduleState.add( m_isoValue->getCondition() );
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

        boost::shared_ptr< WDataSetSingle > newDataSet = m_inputDataSet->getData();
        boost::shared_ptr< WFiberCluster > newCluster = m_inputCluster->getData();
        bool dataChanged = ( m_dataSet != newDataSet ) || ( m_cluster != newCluster );
        bool dataValid = m_dataSet && m_cluster;
        if( dataChanged )
        {
            m_dataSet = newDataSet;
            m_cluster = newCluster;
            dataValid = m_dataSet && m_cluster;
            if( !m_dataSet || !m_cluster )
            {
                continue;
            }

            assert( m_dataSet && "Invalid dataset to compute JoinTree on" );
            m_joinTree = boost::shared_ptr< WJoinContourTree >( new WJoinContourTree( m_dataSet ) );
            m_joinTree->buildJoinTree();
        }

        if( ( m_isoValue->changed() || dataChanged ) && m_joinTree )
        {
            assert( m_dataSet && "JoinTree cannot be valid since there is no valid m_dataSet." );
            m_isoVoxels = m_joinTree->getVolumeVoxelsEnclosedByISOSurface( m_isoValue->get() );
        }

        if( m_drawISOVoxels->changed() || m_isoValue->changed() || dataChanged )
        {
            if( dataValid )
            {
                updateDisplay();
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

void WMClusterSlicer::updateSlices()
{
    if( m_drawSlices->get( true ) )
    {
        boost::shared_ptr< wmath::WFiber > centerLine = m_cluster->getCenterLine();
        if( !centerLine.get() )
        {
            errorLog() << "CenterLine of the bundle is empty => no slices are drawn";
        }
        else
        {
            m_sliceGeode = osg::ref_ptr< WGEGroupNode >( new WGEGroupNode ); // discard old group node
            const wmath::WFiber& cL = *centerLine; // just an alias
            for( size_t i = 1; i < cL.size(); ++i )
            {
                wmath::WVector3D tangent = cL[i] - cL[i-1];
                WPlane p( tangent, cL[i-1] );

                boost::shared_ptr< std::set< wmath::WPosition > > samplePoints = p.samplePoints( 0.5, 20, 20 );
                m_sliceGeode->insert( wge::genPointBlobs( samplePoints, 0.1 ) );
                m_sliceGeode->insert( wge::genFinitePlane( 10, 10, p, WColor( 0, 0, 1 ), true ) );
            }
            m_rootNode->insert( m_sliceGeode );
        }
    }
    else
    {
        m_rootNode->remove( m_sliceGeode );
    }
}

void WMClusterSlicer::updateDisplay()
{
    m_rootNode->remove( m_isoVoxelGeode );
    m_isoVoxelGeode = osg::ref_ptr< osg::Geode >( new osg::Geode() ); // discard old geode

    if( m_drawISOVoxels->get( true ) )
    {
        assert( m_isoVoxels && "JoinTree cannot be valid since there is no valid m_dataSet." );
        m_isoVoxelGeode = generateISOVoxelGeode();
        m_rootNode->insert( m_isoVoxelGeode );
    }
}

osg::ref_ptr< osg::Geode > WMClusterSlicer::generateISOVoxelGeode() const
{
    boost::shared_ptr< std::set< wmath::WPosition > > points( new std::set< wmath::WPosition > );
    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( m_dataSet->getGrid() );
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
