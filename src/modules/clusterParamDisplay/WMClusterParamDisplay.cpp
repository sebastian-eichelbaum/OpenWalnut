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
    typedef WModuleInputForwardData< WDataSetFibers > InFiberType;
    m_fibers = boost::shared_ptr< InFiberType >( new InFiberType( shared_from_this(), "fiberInput", "DataSetFibers to cluster and display" ) );
    addConnector( m_fibers );

    typedef WModuleInputForwardData< WDataSetSingle > InParamDSType;
    m_paramDS = boost::shared_ptr< InParamDSType >( new InParamDSType( shared_from_this(), "paramDS", "Parameter Dataset such as FA" ) );
    addConnector( m_paramDS );

    WModule::connectors();
}

void WMClusterParamDisplay::properties()
{
    m_isoValue = m_properties2->addProperty( "Iso Value", "", 0.2 );
    m_drawISOSurface = m_properties2->addProperty( "ISO Surface", "En/Disables the display of the ISO Surface", true );

    // TODO(math): when project files can handle forwarded properties => forward this again, not wrapping
    m_go = m_properties2->addProperty( "Go", "Runs the fiberClustering submodule", false );
}

void WMClusterParamDisplay::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_isoValue->getCondition() );
    m_moduleState.add( m_drawISOSurface->getCondition() );

    // TODO(math): when project files can handle forwarded properties => forward this again, not wrapping
    m_moduleState.add( m_go->getCondition() );

    initSubModules();

    // initial values
    m_isoSurface->getProperties2()->getProperty( "Iso Value" )->toPropDouble()->set( m_isoValue->get() );
    m_clusterSlicer->getProperties2()->getProperty( "Iso Value" )->toPropDouble()->set( m_isoValue->get() );
    m_fiberClustering->getProperties2()->getProperty( "Go" )->toPropBool()->set( true );

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
            m_isoSurface->getProperties2()->getProperty( "Iso Value" )->toPropDouble()->set( m_isoValue->get() );
            m_clusterSlicer->getProperties2()->getProperty( "Iso Value" )->toPropDouble()->set( m_isoValue->get( true ) );
        }

        if( m_drawISOSurface->changed() )
        {
            m_meshRenderer->getProperties2()->getProperty( "active" )->toPropBool()->set( m_drawISOSurface->get( true ) );
        }

        // TODO(math): when project files can handle forwarded properties => forward this again, not wrapping
        if( m_go->changed() )
        {
            m_fiberClustering->getProperties2()->getProperty( "Go" )->toPropBool()->set( m_go->get( true ) );
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

    m_meshRenderer = WModuleFactory::getModuleFactory()->create( WModuleFactory::getModuleFactory()->getPrototypeByName( "Triangle Mesh Renderer" ) );
    add( m_meshRenderer );
    m_meshRenderer->isReady().wait();

    // preset properties
    debugLog() << "Start step submodule properties";
    m_fiberClustering->getProperties2()->getProperty( "Invisible fibers" )->toPropBool()->set( true );
    m_voxelizer->getProperties2()->getProperty( "Fiber Tracts" )->toPropBool()->set( false );
    m_voxelizer->getProperties2()->getProperty( "Display Voxels" )->toPropBool()->set( false );
    m_voxelizer->getProperties2()->getProperty( "BoundingBox" )->toPropBool()->set( false );
    m_voxelizer->getProperties2()->getProperty( "Lighting" )->toPropBool()->set( false );
    m_gaussFiltering->getProperties2()->getProperty( "Iterations" )->toPropInt()->set( 3 );
    m_clusterSlicer->getProperties2()->getProperty( "Show/Hide ISO Voxels" )->toPropBool()->set( false );
    m_clusterSlicer->getProperties2()->getProperty( "Biggest Component Only" )->toPropBool()->set( true );
    m_isoSurface->getProperties2()->getProperty( "active" )->toPropBool()->set( false );
    debugLog() << "Submodule properties set";

    // wiring
    debugLog() << "Start wiring";
    m_paramDS->forward( m_clusterSlicer->getInputConnector( "paramDS" ) );

    m_gaussFiltering->getInputConnector( "in" )->connect( m_voxelizer->getOutputConnector( "voxelOutput" ) );
    m_isoSurface->getInputConnector( "in" )->connect( m_gaussFiltering->getOutputConnector( "out" ) );
    m_clusterSlicer->getInputConnector( "cluster" )->connect( m_fiberClustering->getOutputConnector( "clusterOutput" ) );
    m_clusterSlicer->getInputConnector( "clusterDS" )->connect( m_gaussFiltering->getOutputConnector( "out" ) );
    m_clusterSlicer->getInputConnector( "meshIN" )->connect( m_isoSurface->getOutputConnector( "out" ) );
    m_meshRenderer->getInputConnector( "mesh" )->connect( m_clusterSlicer->getOutputConnector( "meshOUT" ) );
    m_meshRenderer->getInputConnector( "colorMap" )->connect( m_clusterSlicer->getOutputConnector( "colorMap" ) );

    m_voxelizer->getInputConnector( "voxelInput" )->connect( m_fiberClustering->getOutputConnector( "clusterOutput" ) );
    m_fibers->forward( m_fiberClustering->getInputConnector( "fiberInput" ) ); // init rippling
    debugLog() << "Wiring done";

    // forward properties
    m_properties2->addProperty( m_fiberClustering->getProperties2()->getProperty( "Output cluster ID" ) );
    m_properties2->addProperty( m_fiberClustering->getProperties2()->getProperty( "Max cluster distance" ) );
    m_properties2->addProperty( m_fiberClustering->getProperties2()->getProperty( "Min point distance" ) );
    m_properties2->addProperty( m_voxelizer->getProperties2()->getProperty( "Fiber Tracts" ) );
    m_properties2->addProperty( m_voxelizer->getProperties2()->getProperty( "CenterLine" ) );
    m_properties2->addProperty( m_voxelizer->getProperties2()->getProperty( "Lighting" ) );
    m_properties2->addProperty( m_gaussFiltering->getProperties2()->getProperty( "Iterations" ) );
    m_properties2->addProperty( m_meshRenderer->getProperties2()->getProperty( "Opacity %" ) );
    m_properties2->addProperty( m_meshRenderer->getProperties2()->getProperty( "Mesh Color" ) );
    m_properties2->addProperty( m_clusterSlicer->getProperties2()->getProperty( "Show/Hide ISO Voxels" ) );
    m_properties2->addProperty( m_clusterSlicer->getProperties2()->getProperty( "Mean Type" ) );
    m_properties2->addProperty( m_clusterSlicer->getProperties2()->getProperty( "Show/Hide Slices" ) );
    m_properties2->addProperty( m_clusterSlicer->getProperties2()->getProperty( "Planes #X-SamplePoints" ) );
    m_properties2->addProperty( m_clusterSlicer->getProperties2()->getProperty( "Planes #Y-SamplePoints" ) );
    m_properties2->addProperty( m_clusterSlicer->getProperties2()->getProperty( "Planes Step Width" ) );
    m_properties2->addProperty( m_clusterSlicer->getProperties2()->getProperty( "#Planes" ) );
    m_properties2->addProperty( m_clusterSlicer->getProperties2()->getProperty( "Biggest Component Only" ) );
    m_properties2->addProperty( m_clusterSlicer->getProperties2()->getProperty( "Custom Scale" ) );
    m_properties2->addProperty( m_clusterSlicer->getProperties2()->getProperty( "MinScale" ) );
    m_properties2->addProperty( m_clusterSlicer->getProperties2()->getProperty( "MaxScale" ) );
    m_properties2->addProperty( m_clusterSlicer->getProperties2()->getProperty( "MinScaleColor" ) );
    m_properties2->addProperty( m_clusterSlicer->getProperties2()->getProperty( "MaxScaleColor" ) );
    m_properties2->addProperty( m_voxelizer->getProperties2()->getProperty( "Voxels per Unit" ) );

    // TODO(math): when project files can handle forwarded properties => forward this again, not wrapping
    // m_properties2->addProperty( m_fiberClustering->getProperties2()->getProperty( "Go" ) );
}
