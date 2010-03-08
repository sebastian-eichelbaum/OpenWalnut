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

#include <string>

#include "../../common/WColor.h"
#include "../../kernel/WKernel.h"
#include "WMClusterParamDisplay.h"

WMClusterParamDisplay::WMClusterParamDisplay():
    WModuleContainer( "Cluster Param Display", "Displays various parameters on a cluster surface." )
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
    typedef WModuleInputForwardData< WDataSetFibers > InputType;
    m_input = boost::shared_ptr< InputType >( new InputType( shared_from_this(), "fiberInput", "DataSetFibers to cluster and display" ) );
    addConnector( m_input );

    WModule::connectors();
}

void WMClusterParamDisplay::properties()
{
}

void WMClusterParamDisplay::moduleMain()
{
    m_moduleState.setResetable( true, true );

    initSubModules();

    ready();

    while ( !m_shutdownFlag() )
    {
        m_moduleState.wait();
    }

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );
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

    boost::shared_ptr< WProperties2 > fcProps = m_fiberClustering->getProperties2();
    fcProps->getProperty( "Invisible fibers" )->toPropBool()->set( true );
    m_properties2->addProperty( fcProps->getProperty( "Output cluster ID" ) );
    m_properties2->addProperty( fcProps->getProperty( "Go" ) );

    m_voxelizer = WModuleFactory::getModuleFactory()->create( WModuleFactory::getModuleFactory()->getPrototypeByName( "Voxelizer" ) );
    add( m_voxelizer );
    m_voxelizer->isReady().wait();
    boost::shared_ptr< WProperties2 > voxProps = m_voxelizer->getProperties2();
    voxProps->getProperty( "Fiber Tracts" )->toPropBool()->set( false );
    voxProps->getProperty( "Display Voxels" )->toPropBool()->set( false );
    m_properties2->addProperty( voxProps->getProperty( "Fiber Tracts" ) );

    m_gaussFiltering = WModuleFactory::getModuleFactory()->create( WModuleFactory::getModuleFactory()->getPrototypeByName( "Gauss Filtering" ) );
    add( m_gaussFiltering );
    m_gaussFiltering->isReady().wait();
    m_properties2->addProperty( m_gaussFiltering->getProperties2()->getProperty( "Iterations" ) );

    m_isoSurface = WModuleFactory::getModuleFactory()->create( WModuleFactory::getModuleFactory()->getPrototypeByName( "Isosurface" ) );
    add( m_isoSurface );
    m_isoSurface->isReady().wait();
    m_properties2->addProperty( m_isoSurface->getProperties2()->getProperty( "Iso Value" ) );

    // wiring
    m_input->forward( m_fiberClustering->getInputConnector( "fiberInput" ) );

    m_voxelizer->getInputConnector( "voxelInput" )->connect( m_fiberClustering->getOutputConnector( "clusterOutput" ) );
    m_gaussFiltering->getInputConnector( "in" )->connect( m_voxelizer->getOutputConnector( "voxelOutput" ) );
    m_isoSurface->getInputConnector( "in" )->connect( m_gaussFiltering->getOutputConnector( "out" ) );
}
