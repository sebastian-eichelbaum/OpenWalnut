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

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMClusterParamDisplay )

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
    typedef WModuleInputForwardData< WDataSetFibers > InputType;
    m_input = boost::shared_ptr< InputType >( new InputType( shared_from_this(), "fiberInput", "DataSetFibers to cluster and display" ) );
    addConnector( m_input );

    WModule::connectors();
}

void WMClusterParamDisplay::properties()
{
    m_isoValue = m_properties->addProperty( "Iso value", "", 0.01 );
}

void WMClusterParamDisplay::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_isoValue->getCondition() );
    initSubModules();

    ready();

    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        if( m_shutdownFlag() )
        {
            break;
        }

        if( m_isoValue->changed() )
        {
            m_isoSurface->getProperties()->getProperty( "Iso value" )->toPropDouble()->set( m_isoValue->get() );
            m_clusterSlicer->getProperties()->getProperty( "Iso value" )->toPropDouble()->set( m_isoValue->get( true ) );
        }
    }
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

    m_clusterSlicer = WModuleFactory::getModuleFactory()->create( WModuleFactory::getModuleFactory()->getPrototypeByName( "Cluster Slicer" ) );
    add( m_clusterSlicer );
    m_clusterSlicer->isReady().wait();

    // wiring
    m_input->forward( m_fiberClustering->getInputConnector( "fiberInput" ) );

    m_voxelizer->getInputConnector( "voxelInput" )->connect( m_fiberClustering->getOutputConnector( "clusterOutput" ) );
    m_gaussFiltering->getInputConnector( "in" )->connect( m_voxelizer->getOutputConnector( "voxelOutput" ) );
    m_isoSurface->getInputConnector( "in" )->connect( m_gaussFiltering->getOutputConnector( "out" ) );
    m_clusterSlicer->getInputConnector( "cluster" )->connect( m_fiberClustering->getOutputConnector( "clusterOutput" ) );
    m_clusterSlicer->getInputConnector( "dataset" )->connect( m_gaussFiltering->getOutputConnector( "out" ) );

    // preset properties
    m_fiberClustering->getProperties()->getProperty( "Invisible fibers" )->toPropBool()->set( true );
    m_voxelizer->getProperties()->getProperty( "Fiber tracts" )->toPropBool()->set( false );
    m_voxelizer->getProperties()->getProperty( "Display voxels" )->toPropBool()->set( false );

    // forward properties
    m_properties->addProperty( m_fiberClustering->getProperties()->getProperty( "Output cluster ID" ) );
    m_properties->addProperty( m_fiberClustering->getProperties()->getProperty( "Go" ) );
    m_properties->addProperty( m_voxelizer->getProperties()->getProperty( "Fiber tracts" ) );
    m_properties->addProperty( m_gaussFiltering->getProperties()->getProperty( "Iterations" ) );
    m_properties->addProperty( m_clusterSlicer->getProperties()->getProperty( "Show or hide iso voxels" ) );
}
