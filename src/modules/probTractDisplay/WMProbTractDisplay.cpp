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
#include <utility>
#include <vector>

#include <osg/ShapeDrawable>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Material>
#include <osg/StateAttribute>

#include "../../common/WColor.h"
#include "../../dataHandler/WDataSetScalar.h"
#include "../../dataHandler/WDataTexture3D.h"
#include "../../graphicsEngine/WGEGeodeUtils.h"
#include "../../graphicsEngine/WGEUtils.h"
#include "../../graphicsEngine/WShader.h"
#include "../../kernel/WKernel.h"
#include "directvolumerendering.xpm"
#include "WMProbTractDisplay.h"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMProbTractDisplay )

WMProbTractDisplay::WMProbTractDisplay():
    WModuleContainer(),
    m_rootNode( new osg::Node() )
{
}

WMProbTractDisplay::~WMProbTractDisplay()
{
}

boost::shared_ptr< WModule > WMProbTractDisplay::factory() const
{
    return boost::shared_ptr< WModule >( new WMProbTractDisplay() );
}

const char** WMProbTractDisplay::getXPMIcon() const
{
    return directvolumerendering_xpm;
}

const std::string WMProbTractDisplay::getName() const
{
    return "Probabilistic Tract Rendering with multi transparent iso surfaces.";
}

const std::string WMProbTractDisplay::getDescription() const
{
    return "This module display probabilistic DTI tractograms with iso surfaces.";
}

void WMProbTractDisplay::connectors()
{
    m_input = boost::shared_ptr< WModuleInputData < WDataSetScalar  > >(
        new WModuleInputData< WDataSetScalar >( shared_from_this(), "probTract", "The probabilistic tractogram as scalar dataset." )
    );

    addConnector( m_input );
    WModule::connectors();
}

void WMProbTractDisplay::properties()
{
    m_numSurfaces    = m_properties->addProperty( "#Surfaces", "The number of how many surfaces should be used for display", 2 );
    m_numSurfaces->setMin( 1 );
    updateSubmoduleInstances();
}

void WMProbTractDisplay::updateSubmoduleInstances()
{
    for( int i = 0; i < m_numSurfaces->get(); ++i )
    {
        m_isoSurfaces.push_back( WModuleFactory::getModuleFactory()->create( WModuleFactory::getModuleFactory()->getPrototypeByName( "Isosurface" ) ) ); //NOLINT
        add( m_isoSurfaces.back() );
        m_isoSurfaces.back()->isReady().wait();
    }
}

void WMProbTractDisplay::updateProperties()
{
}

void WMProbTractDisplay::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    initSubModules();
    ready();
    debugLog() << "Module is now ready.";
    while ( !m_shutdownFlag() )
    {
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        if ( m_shutdownFlag() )
        {
            break;
        }

        // has the data changed?
        boost::shared_ptr< WDataSetScalar > newDataSet = m_input->getData();
        bool dataChanged = ( m_dataSet != newDataSet );
        if ( dataChanged || !m_dataSet )
        // this condition will become true whenever the new data is different from the current one or our actual data is NULL. This handles all
        // cases.
        {
            // The data is different. Copy it to our internal data variable:
            debugLog() << "Received Data.";
            m_dataSet = newDataSet;

            // invalid data
            if ( !m_dataSet )
            {
                debugLog() << "Invalid Data. Disabling.";
                continue;
            }
        }
    }

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );
}

void WMProbTractDisplay::initSubModules()
{
//   m_isoSurface = WModuleFactory::getModuleFactory()->create( WModuleFactory::getModuleFactory()->getPrototypeByName( "Isosurface" ) );
//   add( m_isoSurface );
//   m_isoSurface->isReady().wait();

//   m_clusterSlicer = WModuleFactory::getModuleFactory()->create( WModuleFactory::getModuleFactory()->getPrototypeByName( "Cluster Slicer" ) );
//   add( m_clusterSlicer );
//   m_clusterSlicer->isReady().wait();

//   m_meshRenderer = WModuleFactory::getModuleFactory()->create( WModuleFactory::getModuleFactory()->getPrototypeByName( "Triangle Mesh Renderer" ) ); // NOLINT
//   add( m_meshRenderer );
//   m_meshRenderer->isReady().wait();

//   // preset properties
//   debugLog() << "Start step submodule properties";
//   m_detTractClustering->getProperties()->getProperty( "active" )->toPropBool()->set( false, true );
//   m_voxelizer->getProperties()->getProperty( "Fiber Tracts" )->toPropBool()->set( false, true );
//   m_voxelizer->getProperties()->getProperty( "Display Voxels" )->toPropBool()->set( false, true );
//   m_voxelizer->getProperties()->getProperty( "Bounding Box Enable Feature" )->toPropBool()->set( false, true );
//   m_voxelizer->getProperties()->getProperty( "Lighting" )->toPropBool()->set( false, true );
//   m_gaussFiltering->getProperties()->getProperty( "Iterations" )->toPropInt()->set( 3, true );
//   m_clusterSlicer->getProperties()->getProperty( "Show|Hide ISO Voxels" )->toPropBool()->set( false );
//   m_clusterSlicer->getProperties()->getProperty( "Biggest Component Only" )->toPropBool()->set( false );
//   m_isoSurface->getProperties()->getProperty( "active" )->toPropBool()->set( false, true );
//   m_isoSurface->getProperties()->getProperty( "Iso Value" )->toPropDouble()->set( 0.2, true );
//   m_clusterSlicer->getProperties()->getProperty( "Iso Value" )->toPropDouble()->set(  0.2, true );
//   debugLog() << "Submodule properties set";

//   // wiring
//   debugLog() << "Start wiring";
//   m_paramDS->forward( m_clusterSlicer->getInputConnector( "paramDS" ) );

//   m_gaussFiltering->getInputConnector( "in" )->connect( m_voxelizer->getOutputConnector( "voxelOutput" ) );
//   m_isoSurface->getInputConnector( "values" )->connect( m_gaussFiltering->getOutputConnector( "out" ) );
//   m_clusterSlicer->getInputConnector( "cluster" )->connect( m_detTractClustering->getOutputConnector( "clusterOutput" ) );
//   m_clusterSlicer->getInputConnector( "clusterDS" )->connect( m_gaussFiltering->getOutputConnector( "out" ) );
//   m_clusterSlicer->getInputConnector( "meshIN" )->connect( m_isoSurface->getOutputConnector( "surface mesh" ) );
//   m_meshRenderer->getInputConnector( "mesh" )->connect( m_clusterSlicer->getOutputConnector( "meshOUT" ) );
//   m_meshRenderer->getInputConnector( "colorMap" )->connect( m_clusterSlicer->getOutputConnector( "colorMap" ) );

//   m_voxelizer->getInputConnector( "voxelInput" )->connect( m_detTractClustering->getOutputConnector( "clusterOutput" ) );
//   m_fibers->forward( m_detTractClustering->getInputConnector( "tractInput" ) ); // init rippling
//   debugLog() << "Wiring done";

//   // forward properties
//   m_properties->addProperty( m_detTractClustering->getProperties()->getProperty( "Output cluster ID" ) );
//   m_properties->addProperty( m_detTractClustering->getProperties()->getProperty( "Max cluster distance" ) );
//   m_properties->addProperty( m_detTractClustering->getProperties()->getProperty( "Min point distance" ) );
//   m_properties->addProperty( m_voxelizer->getProperties()->getProperty( "Fiber Tracts" ) );
//   m_properties->addProperty( m_voxelizer->getProperties()->getProperty( "Center Line" ) );
//   m_properties->addProperty( m_voxelizer->getProperties()->getProperty( "Lighting" ) );
//   m_properties->addProperty( m_voxelizer->getProperties()->getProperty( "Fiber Transparency" ) );
//   m_properties->addProperty( m_gaussFiltering->getProperties()->getProperty( "Iterations" ) );
//   m_properties->addProperty( m_meshRenderer->getProperties()->getProperty( "Opacity %" ) );
//   m_properties->addProperty( m_meshRenderer->getProperties()->getProperty( "Mesh Color" ) );
//   m_properties->addProperty( m_clusterSlicer->getProperties()->getProperty( "Show|Hide ISO Voxels" ) );
//   m_properties->addProperty( m_clusterSlicer->getProperties()->getProperty( "Mean Type" ) );
//   m_properties->addProperty( m_clusterSlicer->getProperties()->getProperty( "Show|Hide Slices" ) );
//   m_properties->addProperty( m_clusterSlicer->getProperties()->getProperty( "Planes #X-SamplePoints" ) );
//   m_properties->addProperty( m_clusterSlicer->getProperties()->getProperty( "Planes #Y-SamplePoints" ) );
//   m_properties->addProperty( m_clusterSlicer->getProperties()->getProperty( "Planes Step Width" ) );
//   m_properties->addProperty( m_clusterSlicer->getProperties()->getProperty( "#Planes" ) );
//   m_properties->addProperty( m_clusterSlicer->getProperties()->getProperty( "Biggest Component Only" ) );
//   m_properties->addProperty( m_clusterSlicer->getProperties()->getProperty( "Custom Scale" ) );
//   m_properties->addProperty( m_clusterSlicer->getProperties()->getProperty( "MinScale" ) );
//   m_properties->addProperty( m_clusterSlicer->getProperties()->getProperty( "MaxScale" ) );
//   m_properties->addProperty( m_clusterSlicer->getProperties()->getProperty( "MinScaleColor" ) );
//   m_properties->addProperty( m_clusterSlicer->getProperties()->getProperty( "MaxScaleColor" ) );
//   m_properties->addProperty( m_voxelizer->getProperties()->getProperty( "Voxels per Unit" ) );
//   m_properties->addProperty( m_detTractClustering->getProperties()->getProperty( "Start clustering" ) );
}

void WMProbTractDisplay::activate()
{
    if ( m_rootNode )
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

