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
//  You will need the knowledge of these tutorials before you can go on:
//  * WMTemplate
//
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
    // NOTE: Refer to WMTemplate.cpp if you do not understand these commands.
    // Same as in standard modules. Create your instance.
    return boost::shared_ptr< WModule >( new WMTemplateContainers() );
}

void WMTemplateContainers::connectors()
{
    // NOTE: Refer to WMTemplate.cpp if you do not understand these commands.

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
    // NOTE: Refer to WMTemplate.cpp if you do not understand these commands.

    // Add properties here. But please note, that we will also forward properties of nested modules. But this has to be done in the moduleMain
    // method.
    WModule::properties();
}

void WMTemplateContainers::requirements()
{
    // NOTE: Refer to WMTemplate.cpp if you do not understand these commands.

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
    //
    // So basically you have three options:
    //  1) risk the exception and let module fail in this case
    //  2) catch the exception and try to circumvent the parameter (might not always be possible, good option if this only is an non-important
    //     parameter)
    //  3) use WPropertyBase::SPtr iter = gauss->getProperties()->findProperty( "Iterations" ); and check this for NULL. Same
    //     advantages/disadvantages as 2).

    // We now want the user to be able to choose the way the original data and the gaussed one get merged. For this, we forward the properties of
    // the scalar operator module:
    m_properties->addProperty( scalarOp->getProperties()->getProperty( "Operation" ) );
    // and we want another default op:
    scalarOp->getProperties()->getProperty( "Operation" )->set( 1 );    // the index 1 operation is A-B

    // The above line showed how to add single properties. But as the WPropGroup also is a property, you can add all properties of a module too:
    m_properties->addProperty( iso->getProperties() );
    iso->getProperties()->getProperty( "Iso value" )->set( 1 );

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 3) Wire them up
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // We now have modules inside our container and configured them as we like. Now, it is time to connect the modules to each other and to the
    // outside world via our forwarding connectors we set up in the connectors() method.

    // First, connect the modules with each other:

    // Set the gauss version as the first operant by querying the connectors and connecting them:
    scalarOp->getInputConnector( "operandA" )->connect( gauss->getOutputConnector( "out" ) );
    // This is the same as doing it the other way around.
    // gauss->getOutputConnector( "out" )->connect( scalarOp->getInputConnector( "operandA" ) );
    // It is important to understand that the connectors need to be compatible and connectable.
    //  * compatibility means that the transfer type (the type you specify as template parameter for the connectors) can be cast into each other
    //  * connectible means that
    //    * you can only connect inputs to outputs (or outputs to inputs)
    //    * you can only connect one output to a single input
    //
    // Keep this in mind.

    // Use the arithmetic result as input for the isosurface:
    iso->getInputConnector( "values" )->connect( scalarOp->getOutputConnector( "result" ) );

    // Now, we connect our forwarding connectors:
    m_input->forward( gauss->getInputConnector( "in" ) );
    m_input->forward( scalarOp->getInputConnector( "operandB" ) );

    // And also forward the result:
    m_output->forward( scalarOp->getOutputConnector( "result" ) );

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 4) Running and stopping
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // You remember that this container is a module for itself? The you will remember to say "ready":
    ready();
    // You are allowed to do this before you add/configure/wire the modules as above. BUT there is one problem. OpenWalnut assumes that you have
    // set up all properties of your modules before calling ready(). So if we would have signalled ready before adding all properties, then a
    // project file could not be loaded properly due to the missing properties.

    // In a standard module, you would now start the main loop. If you are not interested in any further interaction with the modules or your own
    // properties, you can simply wait until OpenWalnut tells your module to stop:
    waitForStop();

    // BUT you can also implement your module main loop here. Set properties, react on property changes, change connections between module and so
    // on. Please refer to WMTemplate.cpp for details on how this works.
    //
    // KEEP IN MIND: properties, connectors and modules provide WConditions and signals for nearly everything. You can register to them and thus
    // get notified about the things you are interested in. This allows a maximum of possibilities to interact with modules and to build highly
    // dynamic module graphs inside your container module.


    // This is an important, additional step. We need to tell our nested modules to stop working:
    debugLog() << "Shutting down ...";
    stop();
    // If you would leave this out, you run into a serious problem: your modules are still running while they get destroyed at the end of this
    // function (as they are shared_ptr defined in this scope). So if you experience crashes at the end of your container module, you probably
    // missed the stop call!
}

