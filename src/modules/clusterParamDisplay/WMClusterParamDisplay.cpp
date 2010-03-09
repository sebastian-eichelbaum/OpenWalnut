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

#include "../../common/WColor.h"
#include "../../kernel/WKernel.h"
#include "WMClusterParamDisplay.h"

WMClusterParamDisplay::WMClusterParamDisplay()
    : WModuleContainer( "Cluster Param Display", "Displays various parameters on a cluster surface." )
{
}

WMClusterParamDisplay::~WMClusterParamDisplay()
{
}

boost::shared_ptr< WModule > WMClusterParamDisplay::factory() const
{
    return boost::shared_ptr< WModule >( new WMClusterParamDisplay() );
}

void WMClusterParamDisplay::connectors()
{
//    typedef WModuleInputForwardData< WDataSetFibers > InputType;
//    m_input = boost::shared_ptr< InputType >( new InputType( shared_from_this(), "fiberInput", "DataSetFibers to cluster and display" ) );
//    addConnector( m_input );

    WModule::connectors();
}

void WMClusterParamDisplay::properties()
{
    // look into initSubModules for forwarded submodule properties
    m_drawISOVoxels = m_properties2->addProperty( "Show/Hide ISO Voxels", "En/Disables to draw the voxels withing a given ISOSurface.", true );
    m_isoValue      = m_properties2->addProperty( "Iso Value", "", 0.01 );
}

void WMClusterParamDisplay::moduleMain()
{
    m_moduleState.setResetable( true, true );

    initSubModules();

    // on those changes the loop should repeat
    // TODO(math): implement the ContourTree module since output connectors doesn't have change conditions
    // m_moduleState.add( m_gaussFiltering->getOutputConnector( "out" )->getDataChangedCondition() );
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

        // TODO(math): implement new module which doing new visualization work
        boost::shared_ptr< WDataSetSingle > newDataSet; // = m_gaussFiltering->getOutputConnector()->getData();
        bool dataChanged = ( m_dataSet != newDataSet );
        if( dataChanged )
        {
            m_dataSet = newDataSet;
        }
        if( !m_dataSet ) // incase of invalid data (yet) skip and continue
        {
            continue;
        }

        if( dataChanged ) // valid data diffrent from current has arrived beeing not null
        {
            if( !m_dataSet )
            {
                // if there is no valid data anymore discard old join tree
                m_joinTree.reset();
                m_isoVoxels.reset();
            }
            else
            {
                m_joinTree = boost::shared_ptr< WJoinContourTree >( new WJoinContourTree( m_dataSet ) );
                m_joinTree->buildJoinTree();
            }
        }

        if( m_isoValue->changed() || dataChanged )
        {
            m_isoSurface->getProperties2()->getProperty( "Iso Value" )->toPropDouble()->set( m_isoValue->get( true ) );

            if( m_joinTree ) // incase there is a valid join tree already computed, get the enclosed voxels
            {
                assert( m_dataSet && "JoinTree cannot be valid since there is no valid m_dataSet." );
                m_isoVoxels = m_joinTree->getVolumeVoxelsEnclosedByISOSurface( m_isoValue->get() );
            }
        }

        if( m_drawISOVoxels->changed() || m_isoValue->changed() || dataChanged )
        {
            updateDisplay();
        }
    }

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );
}

void WMClusterParamDisplay::updateDisplay()
{
    if( !m_dataSet || !m_drawISOVoxels->get() )
    {
        m_rootNode->remove( m_isoVoxelGeode );
        m_isoVoxelGeode = osg::ref_ptr< osg::Geode >( new osg::Geode() ); // discard old geode
        return;
    }

    assert( m_isoVoxels && "JoinTree cannot be valid since there is no valid m_dataSet." );

    if( m_drawISOVoxels->get( true ) )
    {
        m_isoVoxelGeode = generateISOVoxelGeode( *m_isoVoxels );
        m_rootNode->insert( m_isoVoxelGeode );
    }
}

osg::ref_ptr< osg::Geode > WMClusterParamDisplay::generateISOVoxelGeode( const std::set< size_t >& voxelIDs ) const
{
    osg::ref_ptr< osg::Geode > result;
    // TODO(math): fill the geode
    return result;
}

void WMClusterParamDisplay::activate()
{
    if( m_rootNode ) // prevent segfault when there is no root node yet
    {
        if ( m_active->get() )
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

void WMClusterParamDisplay::initSubModules()
{
    // instantiation
    m_fiberClustering = WModuleFactory::getModuleFactory()->create( WModuleFactory::getModuleFactory()->getPrototypeByName( "Fiber Clustering" ) );
    add( m_fiberClustering );
    m_fiberClustering->isReady().wait();

    m_voxelizer = WModuleFactory::getModuleFactory()->create( WModuleFactory::getModuleFactory()->getPrototypeByName( "Voxelizer" ) );
    add( m_voxelizer );
    m_voxelizer->isReady().wait();

    m_gaussFiltering = WModuleFactory::getModuleFactory()->create( WModuleFactory::getModuleFactory()->getPrototypeByName( "Gauss Filtering" ) );
    add( m_gaussFiltering );
    m_gaussFiltering->isReady().wait();

    m_isoSurface = WModuleFactory::getModuleFactory()->create( WModuleFactory::getModuleFactory()->getPrototypeByName( "Isosurface" ) );
    add( m_isoSurface );
    m_isoSurface->isReady().wait();

    // wiring
    m_input->forward( m_fiberClustering->getInputConnector( "fiberInput" ) );

    m_voxelizer->getInputConnector( "voxelInput" )->connect( m_fiberClustering->getOutputConnector( "clusterOutput" ) );
    m_gaussFiltering->getInputConnector( "in" )->connect( m_voxelizer->getOutputConnector( "voxelOutput" ) );
    m_isoSurface->getInputConnector( "in" )->connect( m_gaussFiltering->getOutputConnector( "out" ) );

    // preset properties
    m_fiberClustering->getProperties2()->getProperty( "Invisible fibers" )->toPropBool()->set( true );
    m_voxelizer->getProperties2()->getProperty( "Fiber Tracts" )->toPropBool()->set( false );
    m_voxelizer->getProperties2()->getProperty( "Display Voxels" )->toPropBool()->set( false );

    // forward properties
    m_properties2->addProperty( m_fiberClustering->getProperties2()->getProperty( "Output cluster ID" ) );
    m_properties2->addProperty( m_fiberClustering->getProperties2()->getProperty( "Go" ) );
    m_properties2->addProperty( m_voxelizer->getProperties2()->getProperty( "Fiber Tracts" ) );
    m_properties2->addProperty( m_gaussFiltering->getProperties2()->getProperty( "Iterations" ) );
    m_properties2->addProperty( m_isoSurface->getProperties2()->getProperty( "Iso Value" ) );

    // property alias
    m_isoValue = m_isoSurface->getProperties2()->getProperty( "Iso Value" )->toPropDouble();
}
