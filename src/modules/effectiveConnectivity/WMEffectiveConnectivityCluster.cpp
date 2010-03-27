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

#include "../../common/WProgress.h"
#include "../../common/WPreferences.h"
#include "../../common/math/WVector3D.h"
#include "../../dataHandler/WSubject.h"
#include "../../dataHandler/WGridRegular3D.h"
#include "../../dataHandler/WDataTexture3D.h"
#include "../../kernel/WKernel.h"
#include "../../graphicsEngine/WGEResourceManager.h"
#include "../../graphicsEngine/WGELabel.h"

#include "../data/WMData.h"
#include "effectiveConnectivityCluster.xpm"

WMEffectiveConnectivityCluster::WMEffectiveConnectivityCluster():
    WModuleContainer( "Effective Connectivity Cluster", "This module is able to visualize effective connectivity cluster." ),
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
    return effectiveConnectivityCluster_xpm;
}

const std::string WMEffectiveConnectivityCluster::getName() const
{
    return WModuleContainer::getName();
}

const std::string WMEffectiveConnectivityCluster::getDescription() const
{
    return WModuleContainer::getDescription();
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
    boost::shared_ptr< WProperties2 >  props = m_fiberSelection->getProperties2();
    props->getProperty( "VOI1 Threshold" )->toPropDouble()->set( 50.0 );
    props->getProperty( "VOI2 Threshold" )->toPropDouble()->set( 50.0 );
    props->getProperty( "Cut Fibers" )->toPropBool()->set( true );
    m_properties2->addProperty( props->getProperty( "Cut Fibers" ) );
    props->getProperty( "Prefer Shortest Path" )->toPropBool()->set( false );
    m_properties2->addProperty( props->getProperty( "Prefer Shortest Path" ) );

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
    props = m_voxelizer->getProperties2();
    props->getProperty( "CenterLine" )->toPropBool()->set( true );
    props->getProperty( "active" )->toPropBool()->set( false );
    props->getProperty( "Voxels per Unit" )->toPropInt()->set( 2 );
    m_properties2->addProperty( props->getProperty( "Voxels per Unit" ) );

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
    props = m_gauss->getProperties2();
    props->getProperty( "Iterations" )->toPropInt()->set( 3 );

    //////////////////////////////////////////////////////////////////////
    // Animation
    //////////////////////////////////////////////////////////////////////

    // create an instance using the prototypes
    m_animation = WModuleFactory::getModuleFactory()->create( WModuleFactory::getModuleFactory()->getPrototypeByName( "Surface Bars" ) );

    // add to container
    add( m_animation );

    // now wait for it to be ready
    m_animation->isReady().wait();

    // set/forward some props
    props = m_animation->getProperties2();
    props->getProperty( "Isovalue" )->toPropInt()->set( 30 );
    m_properties2->addProperty( props->getProperty( "Isovalue" ) );
    props->getProperty( "Step Count" )->toPropInt()->set( 500 );
    m_properties2->addProperty( props->getProperty( "Step Count" ) );
    props->getProperty( "Iso Color" )->toPropColor()->set( WColor( 0.0, 0.5, 1.0, 1.0 ) );
    m_properties2->addProperty( props->getProperty( "Iso Color" ) );

    //////////////////////////////////////////////////////////////////////////////////
    // Hard wire the modules
    //////////////////////////////////////////////////////////////////////////////////

    // Connect voxelizer input with the selected fibers
    m_voxelizer->getInputConnector( "voxelInput" )->connect( m_fiberSelection->getOutputConnector( "cluster" ) );

    // Connect voxelizer output with the gauss filter
    m_gauss->getInputConnector( "in" )->connect( m_voxelizer->getOutputConnector( "voxelOutput" ) );

    // Connect voxelizer output with the animation
    m_animation->getInputConnector( "in" )->connect( m_gauss->getOutputConnector( "out" ) );
    m_animation->getInputConnector( "traces" )->connect( m_voxelizer->getOutputConnector( "fiberIntegrationOutput" ) );

    // Connect inputs
    m_fiberInput->forward( m_fiberSelection->getInputConnector( "fibers" ) );
    m_VOI1->forward( m_fiberSelection->getInputConnector( "VOI1" ) );
    m_VOI2->forward( m_fiberSelection->getInputConnector( "VOI2" ) );

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
    bool first = true;
    while ( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        // woke up since the module is requested to finish
        if ( m_shutdownFlag() )
        {
            break;
        }

        // has one of the properties changed?
        if ( m_voi1Name->changed() || m_voi2Name->changed() || first )
        {
            // create some labels
            osg::ref_ptr< osg::Geode > geode = new osg::Geode();

            osg::ref_ptr< osgText::FadeText > label1 = new osgText::FadeText();
            label1->setText( m_voi1Name->get( true ) );
            label1->setCharacterSize( 8 );
            label1->setFont( WGEResourceManager::getResourceManager()->getDefaultFont() );
            label1->setAxisAlignment(osgText::Text::SCREEN);
            label1->setAlignment( osgText::Text::CENTER_CENTER );
            label1->setPosition( osg::Vec3( 84, 36 , 56 ) ); // the position relative to the current coordinate system
            label1->setColor( osg::Vec4( 0, 0, 0, 1 ) );
            label1->setFadeSpeed( 0.05 );
            osg::ref_ptr< osgText::FadeText > label2 = new osgText::FadeText();
            label2->setText( m_voi2Name->get( true ) );
            label2->setCharacterSize( 8 );
            label2->setFont( WGEResourceManager::getResourceManager()->getDefaultFont() );
            label2->setAxisAlignment( osgText::Text::SCREEN );
            label1->setAlignment( osgText::Text::CENTER_CENTER );
            label2->setPosition( osg::Vec3( 69, 36 , 56 ) ); // the position relative to the current coordinate system
            label2->setColor( osg::Vec4( 0, 0, 0, 1 ) );
            label2->setFadeSpeed( 0.05 );
            geode->getOrCreateStateSet()->setRenderBinDetails( 11, "RenderBin");
            geode->getOrCreateStateSet()->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF );



            geode->addDrawable( label1 );
            geode->addDrawable( label2 );

            m_rootNode->clear();
            m_rootNode->insert( geode );
        }

        first = false;
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

    // call WModules initialization
    WModule::connectors();
}

void WMEffectiveConnectivityCluster::properties()
{
    // Initialize the properties
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_voi1Name = m_properties2->addProperty( "Name of VOI1", "The name of the VOI1.", std::string( "" ), m_propCondition );
    m_voi2Name = m_properties2->addProperty( "Name of VOI2", "The name of the VOI2.", std::string( "" ), m_propCondition );
}

void WMEffectiveConnectivityCluster::activate()
{
    m_animation->getProperties2()->getProperty( "active" )->toPropBool()->set( m_active->get() );
}

