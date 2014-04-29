//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV-Leipzig and CNCF-CBS
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This is a tutorial on how to re-use other modules and how to combine them into a new module, a so called container-module. Lets assume the
// following scenatio: we want a module to get a scalar dataset as input. It then Gauss-filters the data (smoothes it), and combines this with
// the original data again (multiply, add, substract,...). The result will be rendered as isosurface.
//
// Would you program this from scratch even though you already have a gauss module, a dataset arithmetic
// module and an isosurface module? In this example we will implement this scenario by re-using the existing tools of OpenWalnut.
//
// If you want to learn how to program a module, refer to WMTemplate.cpp. It is an extensive tutorial on all the details.
// In this tutorial, we assume you already know how to write modules. For other examples, refer to the README file.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <string>

#include "core/kernel/WKernel.h"
#include "core/kernel/WModuleFactory.h"
#include "core/kernel/WPrototypeRequirement.h"

#include "WMTemplateContainers.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Setting up the representation of THIS module to the outside world. So basically, this module's interface (properties and connectors).
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Unlike a standard WModule, you have to specify the name and description of the container module in its constructor:
WMTemplateContainers::WMTemplateContainers():
    WModuleContainer( "Template Containers",
                      "Show the possibilities of combining and re-using existing modules in container modules." )
{
    // Thats it. Nothing else is needed or different from WModule. You now have a container module.
    //
    // WARNING: As in WModule, creating connectors and properties inside the constructor will fail.
}

WMTemplateContainers::~WMTemplateContainers()
{
}

boost::shared_ptr< WModule > WMTemplateContainers::factory() const
{
    // Same as in standard modules. Create your instance.
    return boost::shared_ptr< WModule >( new WMTemplateContainers() );
}

void WMTemplateContainers::connectors()
{
    // Remember the above scenario description? We want an scalar dataset to be modified and rendered. Thus we need an input where the user can
    // define the scalar data. In a normal module, you would use WModuleInputData< WDataSetScalar >. But our arithmetic module already has this
    // input. We just need to forward it:
    m_input = WModuleInputForwardData< WDataSetScalar >::createAndAdd( shared_from_this(), "in", "The dataset to modify and display" );

    // So, now we have a connector which only forwards data without ever storing it. Imagine it as a gate for connections from the outside world
    // to your own little world of modules.

    // Additionally, we would like to output the gaussed and recombined input data again. Maybe someone needs it. For this purpose, we also
    // create a forwarding output connector:
    m_output = WModuleOutputForwardData< WDataSetScalar  >::createAndAdd( shared_from_this(), "out", "The noised input" );

    // HINT: always keep in mind that a module container itself is a module. You can add as much connectors of any type as you like. In this
    // example, we only demonstrate forwarding connectors. For more details on standard connectors, have a look at WMTemplate.cpp

    // We do not need any connectors. Have a look at WMTemplate.cpp if you want to know what this means.
    WModule::connectors();
}

void WMTemplateContainers::properties()
{
    // Add properties here. But please note, that we will also forward properties of nested modules. But this has to be done in the moduleMain
    // method.
    WModule::properties();
}

void WMTemplateContainers::requirements()
{
    // You already know this method from WMTemplate. We define the requirements of this module. In our case, we want to combine other modules
    // in our container, so we need to tell OpenWalnut that we require them:
    m_requirements.push_back( new WPrototypeRequirement( "Scalar Operator" ) );
    m_requirements.push_back( new WPrototypeRequirement( "Gauss Filtering" ) );
    m_requirements.push_back( new WPrototypeRequirement( "Isosurface" ) );

    // Thats it. Although the Isosurface module requires a running graphics engine, we are not needed to add a WGERequirement here, as the
    // requirements of the nested modules get propagated automatically.
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// We now setup the contents of the container.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void WMTemplateContainers::moduleMain()
{
    m_moduleState.setResetable( true, true );
    //m_moduleState.add( m_propCondition );

    // Now it gets interesting. At this point, we create the nested modules and wire them up.

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 1) Create all modules
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////////////
    // Gauss filter

    // Lets create our noise generator. We use the module factory to create it for us:
    WModule::SPtr gauss = WModuleFactory::getModuleFactory()->create( "Gauss Filtering" );
    // Please note that the specified module might not exist. BUT as we specified this module as a requirement, our module would not be run in
    // this case. So, we can safely assume that we get a proper module pointer here.

    // Add the module to THIS container
    add( gauss );

    // Wait for the module to finish initialization. This is very important. You are not allowed to use or access the module before it has called
    // its ready() method.
    gauss->isReady().wait();

    //////////////////////////////////////////////////////////////////////////////////
    // Scalar Operator

    // Create the scalar operator. This is all the same as above.
    // 1) Create
    WModule::SPtr scalarOp = WModuleFactory::getModuleFactory()->create( "Scalar Operator" );
    // 2) Add
    add( scalarOp );
    // 3) Wait
    scalarOp->isReady().wait();

    //////////////////////////////////////////////////////////////////////////////////
    // Isosurface

    // Create the scalar operator. This is all the same as above.
    // 1) Create
    WModule::SPtr iso = WModuleFactory::getModuleFactory()->create( "Isosurface" );
    // 2) Add
    add( iso );
    // 3) Wait
    iso->isReady().wait();

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 2) Setup the properties
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Ok. Let us summarize. We have created our modules, added them to THIS container and ensured that they are ready.
    // We can go on and configure the modules using their properties and forward some of them to the user of this module.

    // We start by changing the amount of smoothing in the gauss module:
    gauss->getProperties()->getProperty( "Iterations" )->set( 5 );
    // Let us analyze the above line:
    //  1) we query all properties of the module
    //  2) we query a property called "Iterations" in the module's main property group
    //  3) we set the value
    // Be warned! There might be one problem: the name of the property might be wrong. In this case, getProperty thows an exception. You should
    // take care about this. If not, your module will forward the exception to the top-level container -> marking your module as crashed.

    // We now want the user to be able to choose the way the original data and the gaussed one get merged. For this, we forward the properties of
    // the scalar operator module:
    m_properties->addProperty( scalarOp->getProperties()->getProperty( "Operation" ) );

    // The above line showed how to add single properties. But as the WPropGroup also is a property, you can add all properties of a module too:
    m_properties->addProperty( iso->getProperties() );

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 3) Wire them up
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // scalaOp  operandA operandB result













    // signal ready state
    ready();

    // wait for stop request
    waitForStop();

    // stop container and the contained modules.
    stop();

    // Never miss to clean up. If you miss this step, the shared_ptr might get deleted -> GUI crash
    debugLog() << "Shutting down ...";
}

