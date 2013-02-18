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

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <cmath>

#include <boost/shared_ptr.hpp>

#include "WMEffectiveConnectivityCluster.h"

#include <osg/Geode>
#include <osg/Geometry>
#include <osgText/Text>
#include <osgText/FadeText>
#include <osg/StateSet>
#include <osg/StateAttribute>
#include <osg/PolygonMode>
#include <osg/LightModel>
#include <osgDB/WriteFile>

#include "core/common/WProgress.h"
#include "core/common/math/linearAlgebra/WLinearAlgebra.h"
#include "core/dataHandler/WSubject.h"
#include "core/dataHandler/WGridRegular3D.h"
#include "core/dataHandler/datastructures/WFiberCluster.h"
#include "core/kernel/WKernel.h"
#include "core/kernel/WModuleFactory.h"
#include "core/kernel/WModuleOutputData.h"
#include "core/graphicsEngine/widgets/labeling/WGEBorderLayout.h"
#include "core/graphicsEngine/widgets/labeling/WGELabel.h"
#include "core/graphicsEngine/WGraphicsEngine.h"
#include "core/graphicsEngine/WGEUtils.h"

#include "WMEffectiveConnectivityCluster.xpm"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMEffectiveConnectivityCluster )

WMEffectiveConnectivityCluster::WMEffectiveConnectivityCluster():
    WModuleContainer( "Effective Connectivity Cluster", "This module is able to visualize effective connectivity cluster." ),
    m_labelActive( false ),
    m_rootNode( new WGEGroupNode() )
{
    // WARNING: initializing connectors inside the constructor will lead to an exception.
    // NOTE: Do not use the module factory inside this constructor. This will cause a dead lock as the module factory is locked
    // during construction of this instance and can then not be used to create another instance (Isosurface in this case). If you
    // want to initialize some modules using the module factory BEFORE the moduleMain() call, overwrite WModule::initialize().
}

WMEffectiveConnectivityCluster::~WMEffectiveConnectivityCluster()
{
    // cleanup
    removeConnectors();
}

boost::shared_ptr< WModule > WMEffectiveConnectivityCluster::factory() const
{
    return boost::shared_ptr< WModule >( new WMEffectiveConnectivityCluster() );
}

const char** WMEffectiveConnectivityCluster::getXPMIcon() const
{
    return WMEffectiveConnectivityCluster_xpm;
}

const std::string WMEffectiveConnectivityCluster::getName() const
{
    return WModuleContainer::getName();
}

const std::string WMEffectiveConnectivityCluster::getDescription() const
{
    return WModuleContainer::getDescription();
}

void WMEffectiveConnectivityCluster::fiberDataChange( boost::shared_ptr< WModuleConnector > /*input*/,
                                                      boost::shared_ptr< WModuleConnector > output )
{
    if( !output )
    {
        m_labelActive = false;
        m_propCondition->notify();

        // if the connector gets reset -> ignore this case
        return;
    }

    // cast it to the target type
    boost::shared_ptr< WModuleOutputData < WFiberCluster > > o = boost::static_pointer_cast< WModuleOutputData< WFiberCluster > >( output );
    if( !o )
    {
        errorLog() << "New data is not a WFiberCluster? That should not happen!";
    }

    // grab data
    boost::shared_ptr< WFiberCluster > fibs = o->getData();
    boost::shared_ptr< WFiber > lline = fibs->getLongestLine();

    // the first and the last point of the longest line are required:
    m_labelPos2 = ( *lline )[ lline->size() - 2 ];
    m_labelPos1 = ( *lline )[ 1 ];

    m_labelActive = true;

    m_propCondition->notify();
}

void WMEffectiveConnectivityCluster::moduleMain()
{
    //////////////////////////////////////////////////////////////////////////////////
    // Fiber Selection
    //////////////////////////////////////////////////////////////////////////////////

    // create an instance using the prototypes
    m_fiberSelection = WModuleFactory::getModuleFactory()->create( WModuleFactory::getModuleFactory()->getPrototypeByName( "Fiber Selection" ) );

    // add to container
    add( m_fiberSelection );

    // now wait for it to be ready
    m_fiberSelection->isReady().wait();

    // set some props
    boost::shared_ptr< WProperties >  props = m_fiberSelection->getProperties();
    props->getProperty( "VOI1 threshold" )->toPropDouble()->set( 50.0 );
    props->getProperty( "VOI2 threshold" )->toPropDouble()->set( 50.0 );
    props->getProperty( "Cut fibers" )->toPropBool()->set( true );
    m_properties->addProperty( props->getProperty( "Cut fibers" ) );
    props->getProperty( "Prefer shortest path" )->toPropBool()->set( false );
    m_properties->addProperty( props->getProperty( "Prefer shortest path" ) );

    // as this module needs the centerline / longest line -> subscribe to the output connector DATA_CHANGE signal
    m_fiberSelection->getOutputConnector( "cluster" )->subscribeSignal( DATA_CHANGED,
        boost::bind( &WMEffectiveConnectivityCluster::fiberDataChange, this, _1, _2 )
    );

    //////////////////////////////////////////////////////////////////////
    // Voxelize
    //////////////////////////////////////////////////////////////////////

    // create an instance using the prototypes
    m_voxelizer = WModuleFactory::getModuleFactory()->create( WModuleFactory::getModuleFactory()->getPrototypeByName( "Voxelizer" ) );

    // add to container
    add( m_voxelizer );

    // now wait for it to be ready
    m_voxelizer->isReady().wait();

    // set/forward some props
    props = m_voxelizer->getProperties();
    props->getProperty( "Center line" )->toPropBool()->set( false );
    props->getProperty( "active" )->toPropBool()->set( false );
    props->getProperty( "Fiber tracts" )->toPropBool()->set( false );
    props->getProperty( "Display voxels" )->toPropBool()->set( false );
    props->getProperty( "Lighting" )->toPropBool()->set( false );

    // set longest line based parameterization
    props->getProperty( "Voxels per unit" )->toPropInt()->set( 2 );
    WItemSelector::IndexList idx;
    idx.push_back( 1 );
    props->getProperty( "Parameterization" )->toPropSelection()->set( props->getProperty( "Parameterization"
                )->toPropSelection()->get().newSelector( idx ) );

    m_properties->addProperty( props->getProperty( "Voxels per unit" ) );

    //////////////////////////////////////////////////////////////////////
    // Gauss Filter the voxel output
    //////////////////////////////////////////////////////////////////////

    // create an instance using the prototypes
    m_gauss = WModuleFactory::getModuleFactory()->create( WModuleFactory::getModuleFactory()->getPrototypeByName( "Gauss Filtering" ) );

    // add to container
    add( m_gauss );

    // now wait for it to be ready
    m_gauss->isReady().wait();

    // set/forward some props
    props = m_gauss->getProperties();
    props->getProperty( "Iterations" )->toPropInt()->set( 2 );

    //////////////////////////////////////////////////////////////////////
    // Animation
    //////////////////////////////////////////////////////////////////////

    // create an instance using the prototypes
    m_animation = WModuleFactory::getModuleFactory()->create(
            WModuleFactory::getModuleFactory()->getPrototypeByName( "Surface Parameter Animator" )
    );

    // add to container
    add( m_animation );

    // now wait for it to be ready
    m_animation->isReady().wait();

    // set/forward some props
    props = m_animation->getProperties();
    props->getProperty( "Isovalue" )->toPropInt()->set( 32 );
    m_properties->addProperty( props->getProperty( "Isovalue" ) );
    props->getProperty( "Step count" )->toPropInt()->set( 500 );
    m_properties->addProperty( props->getProperty( "Step count" ) );
    props->getProperty( "Iso color" )->toPropColor()->set( WColor( 0.0, 0.5, 1.0, 1.0 ) );
    m_properties->addProperty( props->getProperty( "Iso color" ) );
    props->getProperty( "Opacity %" )->toPropInt()->set( 100 );
    m_properties->addProperty( props->getProperty( "Opacity %" ) );
    m_properties->addProperty( props->getProperty( "Saturation %" ) );
    m_properties->addProperty( props->getProperty( "Beam1 size" ) );
    m_properties->addProperty( props->getProperty( "Beam2 size" ) );
    m_properties->addProperty( props->getProperty( "Beam1 speed" ) );
    m_properties->addProperty( props->getProperty( "Beam2 speed" ) );
    m_properties->addProperty( props->getProperty( "Parameter scale" ) );


    //////////////////////////////////////////////////////////////////////////////////
    // Hard wire the modules
    //////////////////////////////////////////////////////////////////////////////////

    // Connect voxelizer input with the selected fibers
    m_voxelizer->getInputConnector( "tractInput" )->connect( m_fiberSelection->getOutputConnector( "cluster" ) );

    // Connect voxelizer output with the Gauss filter
    m_gauss->getInputConnector( "in" )->connect( m_voxelizer->getOutputConnector( "voxelOutput" ) );

    // Connect voxelizer output with the animation
    m_animation->getInputConnector( "in" )->connect( m_gauss->getOutputConnector( "out" ) );
    m_animation->getInputConnector( "traces" )->connect( m_voxelizer->getOutputConnector( "parameterizationOutput" ) );

    // Connect inputs
    m_fiberInput->forward( m_fiberSelection->getInputConnector( "fibers" ) );
    m_VOI1->forward( m_fiberSelection->getInputConnector( "VOI1" ) );
    m_VOI2->forward( m_fiberSelection->getInputConnector( "VOI2" ) );

    // forward some results
    m_paramOutput->forward( m_voxelizer->getOutputConnector( "parameterizationOutput" ) );
    m_voxelOutput->forward( m_gauss->getOutputConnector( "out" ) );
    m_fiberOutput->forward( m_fiberSelection->getOutputConnector( "out" ) );

    //////////////////////////////////////////////////////////////////////////////////
    // Done!
    //////////////////////////////////////////////////////////////////////////////////

    // signal ready state
    ready();

    // wake up on property change
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_propCondition );

    // Signal ready state.
    ready();

    // add this module's group node
    m_rootNode->setNodeMask( m_active->get() ? 0xFFFFFFFF : 0x0 );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootNode );

    // Now wait for data
    bool lastLabelActiveState = m_labelActive;
    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        // woke up since the module is requested to finish
        if( m_shutdownFlag() )
        {
            break;
        }
        // has one of the properties changed?
        if( m_labelActive && (
                    ( lastLabelActiveState != m_labelActive ) || m_voi1Name->changed() || m_voi2Name->changed() || m_labelCharacterSize->changed() )
           )
        {
            lastLabelActiveState = true;
            osg::ref_ptr< WGEBorderLayout > layouter = new WGEBorderLayout();

            std::string voi1 = m_voi1Name->get( true );
            std::string voi2 = m_voi2Name->get( true );
            if( !voi1.empty() )
            {
                osg::ref_ptr< WGELabel > label1 = new WGELabel();
                label1->setText( voi1 );
                label1->setAnchor( m_labelPos1 ); // the position relative to the current world coordinate system
                label1->setCharacterSize( m_labelCharacterSize->get( true ) );
                layouter->addLayoutable( label1 );
            }

            if( !voi2.empty() )
            {
                osg::ref_ptr< WGELabel > label2 = new WGELabel();
                label2->setText( voi2 );
                label2->setAnchor( m_labelPos2 ); // the position relative to the current world coordinate system
                label2->setCharacterSize( m_labelCharacterSize->get( true ) );
                layouter->addLayoutable( label2 );
            }

            m_rootNode->clear();
            m_rootNode->insert( layouter );
        }

        // remove labels if no dataset is connected anymore
        if( !m_labelActive )
        {
            m_rootNode->clear();
        }
    }

    // At this point, the container managing this module signalled to shutdown. The main loop has ended and you should clean up. Always remove
    // allocated memory and remove all OSG nodes.
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );

    // stop container and the contained modules.
    stop();
}

void WMEffectiveConnectivityCluster::connectors()
{
    // this is the scalar field input
    m_VOI1 = boost::shared_ptr< WModuleInputForwardData< WDataSetSingle > >(
        new WModuleInputForwardData< WDataSetSingle >( shared_from_this(),
                              "VOI1", "The first volume of interest." )
        );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_VOI1 );

    // this is the scalar field input
    m_VOI2 = boost::shared_ptr< WModuleInputForwardData< WDataSetSingle > >(
        new WModuleInputForwardData< WDataSetSingle >( shared_from_this(),
                              "VOI2", "The second volume of interest." )
        );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_VOI2 );

    // this is the scalar field input
    m_fiberInput = boost::shared_ptr< WModuleInputForwardData< WDataSetFibers > >(
        new WModuleInputForwardData< WDataSetFibers >( shared_from_this(),
                              "fibers", "The fiber dataset used to find connection path." )
        );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_fiberInput );

    // forwarder for some results
    // this is the parameter field
    m_paramOutput = boost::shared_ptr< WModuleOutputForwardData< WDataSetScalar > >(
        new WModuleOutputForwardData< WDataSetScalar >( shared_from_this(),
                              "paramOut", "The voxelized fiber parameterization field." )
        );
    addConnector( m_paramOutput );

    // this is the voxel field
    m_voxelOutput = boost::shared_ptr< WModuleOutputForwardData< WDataSetScalar > >(
        new WModuleOutputForwardData< WDataSetScalar >( shared_from_this(),
                              "voxelsOut", "The voxelized fibers." )
        );
    addConnector( m_voxelOutput );

    // these are the fibers
    m_fiberOutput = boost::shared_ptr< WModuleOutputForwardData< WDataSetFibers > >(
        new WModuleOutputForwardData< WDataSetFibers >( shared_from_this(),
                              "fibersOut", "The voxelized fibers." )
        );
    addConnector( m_fiberOutput );

    // call WModules initialization
    WModule::connectors();
}

void WMEffectiveConnectivityCluster::properties()
{
    // Initialize the properties
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_voi1Name = m_properties->addProperty( "Name of VOI1", "The name of the VOI1.", std::string( "" ), m_propCondition );
    m_voi2Name = m_properties->addProperty( "Name of VOI2", "The name of the VOI2.", std::string( "" ), m_propCondition );

    m_labelCharacterSize = m_properties->addProperty( "Font size", "The size of the label fonts.", 20, m_propCondition );

    WModule::properties();
}

void WMEffectiveConnectivityCluster::activate()
{
    m_animation->getProperties()->getProperty( "active" )->toPropBool()->set( m_active->get() );
}

