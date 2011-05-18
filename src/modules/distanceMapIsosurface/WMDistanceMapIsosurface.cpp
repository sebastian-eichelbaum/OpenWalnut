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

#include <stdint.h>
#include <string>
#include <algorithm>
#include <vector>

#include "WMDistanceMapIsosurface.h"
#include "WMDistanceMapIsosurface.xpm"

#include "core/kernel/WKernel.h"
#include "core/kernel/WModuleFactory.h"
#include "core/kernel/WPrototypeRequirement.h"
#include "core/dataHandler/WSubject.h"
#include "core/dataHandler/WGridRegular3D.h"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMDistanceMapIsosurface )

WMDistanceMapIsosurface::WMDistanceMapIsosurface():
    WModuleContainer( "Distance Map Isosurface",
                      "Computes a smoothed version of the dataset"
                      " and a distance map on it. Finally it renders"
                      " this distance map using an isosurface. This isosurface"
                      " can be textured with values from scalar data sets in order to display"
                      " the structures at the given distance."
                      " This is <b>only</b> useful for peeled data." )
{
    // WARNING: initializing connectors inside the constructor will lead to an exception.
    // NOTE: Do not use the module factory inside this constructor. This will cause a dead lock as the module factory is locked
    // during construction of this instance and can then not be used to create another instance (Isosurface in this case). If you
    // want to initialize some modules using the module factory BEFORE the moduleMain() call, overwrite WModule::initialize().
}

WMDistanceMapIsosurface::~WMDistanceMapIsosurface()
{
}

boost::shared_ptr< WModule > WMDistanceMapIsosurface::factory() const
{
    return boost::shared_ptr< WModule >( new WMDistanceMapIsosurface() );
}

const char** WMDistanceMapIsosurface::getXPMIcon() const
{
    return distancemapIsosurface_xpm;
}

void WMDistanceMapIsosurface::moduleMain()
{
    //////////////////////////////////////////////////////////////////////////////////
    // Marching Cubes
    //////////////////////////////////////////////////////////////////////////////////

    // create an instance using the prototypes
    m_marchingCubesModule = WModuleFactory::getModuleFactory()->create( WModuleFactory::getModuleFactory()->getPrototypeByName( "Isosurface" ) );

    // add the marching cubes to the container
    add( m_marchingCubesModule );

    // now wait for it to be ready
    m_marchingCubesModule->isReady().wait();
    boost::shared_ptr< WProperties >  mcProps = m_marchingCubesModule->getProperties();
    m_isoValueProp = mcProps->getProperty( "Iso value" )->toPropDouble();
    m_isoValueProp->set( 0.2 );
    m_isoValueProp->setMin( 0.0 );
    m_isoValueProp->setMax( 1.0 );
    m_properties->addProperty( m_isoValueProp );


    m_useTextureProp = mcProps->getProperty( "Use texture" )->toPropBool();
    m_useTextureProp->set( true );
    m_properties->addProperty( m_useTextureProp );

    m_surfaceColorProp = mcProps->getProperty( "Surface color" )->toPropColor();
    m_properties->addProperty( m_surfaceColorProp );

    m_opacityProp = mcProps->getProperty( "Opacity %" )->toPropInt();
    m_properties->addProperty( m_opacityProp );


    //////////////////////////////////////////////////////////////////////////////////
    // Distance Map
    //////////////////////////////////////////////////////////////////////////////////

    // create a new instance of WMDistanceMap
    m_distanceMapModule = WModuleFactory::getModuleFactory()->create( WModuleFactory::getModuleFactory()->getPrototypeByName( "Distance Map" ) );

    // add it to the container
    add( m_distanceMapModule );

    // wait until it is ready
    m_distanceMapModule->isReady().wait();

    //////////////////////////////////////////////////////////////////////////////////
    // Hard wire both modules
    //////////////////////////////////////////////////////////////////////////////////

    // NOTE: you can use the WModuleContainer::applyModule functions here, which, in this case, is possible, since the connectors
    // can be connected unambiguously (one to one connection). But to show how hard wiring works, we do it manually here.
    m_marchingCubesModule->getInputConnector( "values" )->connect( m_distanceMapModule->getOutputConnector( "out" ) );
    // this is the same as doing it the other way around.
    // m_distanceMapModule->getOutputConnector( "out" )->connect( m_marchingCubesModule->getInputConnector( "values" ) );
    // simple, isn't it? ;-)

    //////////////////////////////////////////////////////////////////////////////////
    // Setup forwarding of this modules connectors with the contained ones
    //////////////////////////////////////////////////////////////////////////////////

    // connect the distance map output to the container output to ensure other modules can use the distance map if they want to
    m_output->forward( m_distanceMapModule->getOutputConnector( "out" ) );
    // we want the container input connector "in" to be connected to the input of WMDistanceMap
    m_input->forward( m_distanceMapModule->getInputConnector( "in" ) );

    //////////////////////////////////////////////////////////////////////////////////
    // Done! Modules are set up.
    //////////////////////////////////////////////////////////////////////////////////

    // signal ready state
    ready();

    // wait for stop request
    waitForStop();

    // stop container and the contained modules.
    stop();
}

void WMDistanceMapIsosurface::connectors()
{
    // initialize connectors

    // this is the scalar field input
    m_input = boost::shared_ptr< WModuleInputForwardData< WDataSetScalar > >(
        new WModuleInputForwardData< WDataSetScalar >( shared_from_this(),
                                                               "in", "Dataset to compute distance map for." )
        );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_input );

    // this output is used to provide the distance map to other modules.
    m_output = boost::shared_ptr< WModuleOutputForwardData< WDataSetScalar > >(
        new WModuleOutputForwardData< WDataSetScalar >( shared_from_this(),
                                                               "out", "Distance map for the input data set." )
        );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_output );

    // call WModules initialization
    WModule::connectors();
}

void WMDistanceMapIsosurface::requirements()
{
    m_requirements.push_back( new WPrototypeRequirement( "Distance Map" ) );
    m_requirements.push_back( new WPrototypeRequirement( "Isosurface" ) );
}

void WMDistanceMapIsosurface::activate()
{
    m_marchingCubesModule->getProperties()->getProperty( "active" )->toPropBool()->set( m_active->get() );
}

