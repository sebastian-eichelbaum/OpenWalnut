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

#include "../../kernel/WKernel.h"
#include "../../common/WColor.h"
#include "WMClusterSlicer.h"

WMClusterSlicer::WMClusterSlicer():
    WModule()
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
    m_drawISOVoxels = m_properties2->addProperty( "Show/Hide ISO Voxels", "En/Disables to draw the voxels withing a given ISOSurface.", true );
    m_isoValue      = m_properties2->addProperty( "Iso Value", "", 0.01 );
}

void WMClusterSlicer::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_inputDataSet->getDataChangedCondition() );
    m_moduleState.add( m_isoValue->getCondition() );
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
        if( dataChanged )
        {
            m_dataSet = newDataSet;
            m_cluster = newCluster;
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
            updateDisplay();
        }
    }

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );
}

void WMClusterSlicer::updateDisplay()
{
    if( !m_drawISOVoxels->get( true ) )
    {
        m_rootNode->remove( m_isoVoxelGeode );
        m_isoVoxelGeode = osg::ref_ptr< osg::Geode >( new osg::Geode() ); // discard old geode
        return;
    }
    else
    {
        assert( m_isoVoxels && "JoinTree cannot be valid since there is no valid m_dataSet." );
        m_isoVoxelGeode = generateISOVoxelGeode( *m_isoVoxels );
        m_rootNode->insert( m_isoVoxelGeode );
    }
}

osg::ref_ptr< osg::Geode > WMClusterSlicer::generateISOVoxelGeode( const std::set< size_t >& voxelIDs ) const
{
    osg::ref_ptr< osg::Geode > result;
    // TODO(math): fill the geode
    return result;
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
