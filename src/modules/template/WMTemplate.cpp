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

#include <osg/ShapeDrawable>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Material>
#include <osg/StateAttribute>

#include "../../kernel/WKernel.h"
#include "../../common/WColor.h"

#include "WMTemplate.h"

WMTemplate::WMTemplate():
    WModule()
{
    // In the constructor, you can initialize your members and all this stuff. You must not initialize connectors or properties here! You also
    // should avoid doing computationally expensive stuff, since every module has its own thread which is intended to be used for such calculations.
    // Please keep in mind, that every member initialized here is also initialized in the prototype, which may be a problem if the member is large,
    // and therefore, wasting a lot of memory in your module's prototype instance.
}

WMTemplate::~WMTemplate()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMTemplate::factory() const
{
    // To properly understand what this is, we need to have a look at how module instances get created. At first, if you are not familiar with the
    // design patterns "Prototype", "Abstract Factory" and "Factory Method" you should probably read about them first. For short: while the kernel
    // is starting up, it also creates an instance of WModuleFactory, which creates a prototype instance of every module that can be loaded.
    // These prototypes are then used to create new instances of modules, check compatibility of modules and identify the type of modules.
    // If someone, in most cases the module container, wants a new instance of a module with a given prototype, it asks the factory class for it,
    // which uses the prototype's factory() method. Since the method is virtual, it returns a module instance, created with the correct type.
    // A prototype itself is an instance of your module, with the constructor run, as well as connectors() and properties(). What does this mean
    // to your module? Unlike the real "Prototype"- Design pattern, the module prototypes do not get cloned to retrieve a new instance,
    // they get constructed using "new" and this factory method.
    //
    // Here is a short overview of the lifetime of a module instance:
    //
    //    * constructor
    //    * connectors()
    //    * properties()
    //    * now isInitialized() will return true
    //    * the module will be associated with a container
    //    * now isAssociated() will return true
    //          o isUsable() will return true
    //    * after it got added, moduleMain() will be called
    //    * run, run, run, run
    //    * notifyStop gets called
    //    * moduleMain() should end
    //    * destructor
    //
    // So you always have to write this method and always return a valid pointer to an object of your module class.
    // Never initialize something else in here!
    return boost::shared_ptr< WModule >( new WMTemplate() );
}

const std::string WMTemplate::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "Template";
}

const std::string WMTemplate::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    return "This module is intended to be a module template and an example for writing modules.";
}

void WMTemplate::connectors()
{
    // How will your module know on which data it should work? Through its input connector(s). How will other modules get to know about your
    // calculated output data? Through your output connector(s). Simple isn't it? You may assume your module as some kind of function, as in
    // common programming languages, where your connectors denote its function signature. The method "connectors()" is for initializing your
    // connectors, your function signature. Now, a short excursion on how the module container and kernel knows which connector can be connected
    // to which. Generally, there are only two types of connectors available for your usage: WModuleInputData and WModuleOutputData and they can
    // only be connected to each other. So, it is not possible to connect an input with an input, nor an output with an output. Both of them are
    // template classes and therefore are associated with a type. This type determines if an input connector is compatible with an output connector.
    // A simple example: assume you have a class hierarchy:
    // Initialize your connectors here. Give them proper names and use the type your module will create or rely on. Do not use types unnecessarily
    // high in class hierarchy. The list of your connectors is fixed after connectors() got called. As in common imperative programming languages
    // the function signature can not be changed during runtime (which, in our case, means after connectors() got called).

    // Here is an example of how to create connectors. This module wants to have an input connector. This connector is defined by the type of
    // data that should be transferred, an module-wide unique name and a proper description:
    m_input = boost::shared_ptr< WModuleInputData < WDataSetSingle  > >(
        new WModuleInputData< WDataSetSingle >( shared_from_this(),
                                                               "in", "The dataset to display" )
        );

    // This creates an input connector which can receive WDataSetSingle. It will never be able to connect to output connectors providing just a
    // WDataSet (which is the father class of WDataSetSingle), but it will be able to be connected to an output connector with a type derived
    // from WDataSetSingle.

    // As properties, every connector needs to be added to the list of connectors.
    addConnector( m_input );

    // Now, lets add an output connector. We want to provide data calculated here to other modules. The output connector is initialized the same
    // way as input connectors. You need the type, the module-wide unique name and the description. The type you specify here also determines
    // which input connectors can be connected to this output connector: only connectors with a type equal or lower in class hierarchy.
    m_output = boost::shared_ptr< WModuleOutputData < WDataSetSingle  > >(
        new WModuleOutputData< WDataSetSingle >( shared_from_this(),
                                                               "out", "The calculated dataset" )
        );

    // As above: make it known.
    addConnector( m_output );

    // call WModules initialization
    WModule::connectors();
}

void WMTemplate::properties()
{
    // Every module can provide properties to the outside world. These properties can be changed by the user in the GUI or simply by other
    // modules using yours. Properties NEED to be created and added here. Doing this outside this function will lead to severe problems.
    //
    // Theoretically, you can specify properties of every type possible in C++. Therefore, see WPropertyVariable. But in most cases, the
    // predefined properties (WPropertyTypes.h) are enough, besides being the only properties shown and supported by the GUI.
    //
    // To create and add a new property, every module has a member m_properties. It is a set of properties this module provides to the outer
    // world. As with connectors, a property which not has been added to m_properties is not visible for others. Now, how to add a new property?

    propCondition = boost::shared_ptr< WCondition >( new WCondition() );
    m_enableFeature  = m_properties2->addProperty( "Enable Feature",           "Description.", true );
    m_anInteger      = m_properties2->addProperty( "Number of Shape Rows",     "Number of shape rows.", 10, propCondition );
    m_aDouble        = m_properties2->addProperty( "Shape Radii",              "Shape radii.", 20.0, propCondition );
    m_aString        = m_properties2->addProperty( "A String",                 "Something.", std::string( "hello" ), propCondition );
    m_aFile          = m_properties2->addProperty( "A Filenname",              "Description.", WKernel::getAppPathObject(), propCondition );
    m_aColor         = m_properties2->addProperty( "A Color",                  "Description.", WColor( 1.0, 0.0, 0.0, 1.0 ) );

    // These lines create some new properties and add them to the property list of this module. The specific type to create is determined by the
    // initial value specified in the third argument. The first argument is the name of the property, which needs to be unique among all
    // properties of this module. The second argument is a description. A nice feature is the possibility to specify an own condition, which gets
    // fired when the property gets modified. This is especially useful to wake up the module's thread on property changes. So, the property
    // m_anInteger will wake the module thread on changes. m_enableFeature and m_aColor should not wake up the module thread. They get read by
    // the update callback of this modules OSG node, to update the color.

    // How can the values of the properties be changed? You can take a look at moduleMain where this is shown. For short: m_anInteger->set( 2 )
    // and m_anInteger->get().

    // The properties offer another nice feature: property constraints. You can enforce your properties to be in a special range, to not be
    // empty, to contain a valid directory name and so on. This is done with the class WPropertyVariable< T >::WPropertyConstraint. There are
    // several predefined you can use directly: WPropertyConstraintTypes.h. The constants defined there can be supplied to
    // WPropertyVariable< T >::PropertyConstraint::create( ... ). As an example, we want the property m_aFile to only contain existing
    // directories;
    m_aFile->addConstraint( PC_PATHEXISTS );
    m_aFile->addConstraint( PC_ISDIRECTORY );
    // Thats it. To set minimum and maximum value for a property the convenience methods setMin and setMax are defined. setMin and setMax are
    // allowed for all property types with defined <= and >= operator.
    m_anInteger->setMin( 1 );
    m_anInteger->setMax( 15 );
    m_aDouble->setMin( 5.0 );
    m_aDouble->setMax( 50.0 );

    // the most amazing feature is: custom constraints. Similar to OSG update callbacks, you just need to write your own PropertyConstraint class
    // to define the allowed values for your constraint. Take a look at the StringLength class on how to do it.
    m_aString->addConstraint( boost::shared_ptr< StringLength >( new StringLength ) );
}

void WMTemplate::moduleMain()
{
    // This is the modules working thread. Its the most important part of your module.
    // When you enter this method, all connectors and properties the module provides are fixed. They get initialized in connectors() and
    // properties(). You always can assume the kernel, the GUI, the graphics engine and the data handler to be initialized and ready. Please keep
    // in mind, that this method is running in its own thread.

    // You can output log messages everywhere and everytime in your module. The WModule base class therefore provides debugLog, inforLog, warnLog
    // and errorLog. You can use them very similar to the common std::cout streams.
    debugLog() << "Entering moduleMain()";

    // Your module can notify everybody that it is ready to be used. The member function ready() does this for you. The ready state is especially
    // useful whenever your module needs to do long operations to initialize. No other module can connect to your module before it signals its
    // ready state. You can assume the code before ready() to be some kind of initialization code.
    debugLog() << "Doing time consuming operations";
    sleep( 5 );

    // Your module can use an moduleState variable to wait for certain events. Most commonly, these events are new data on input connectors or
    // changed properties. You can decide which events the moduleState should handle. Therefore, use m_moduleState.add( ... ) to insert every
    // condition you want to wait on. As every input connector provides an changeCondition, we now add this condition to the moduleState:
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    // Remember the condition provided to some properties in properties()? The condition can now be used with this condition set.
    m_moduleState.add( propCondition );
    // One note about "setResetable": It might happen, that a condition fires and your thread does not currently waits on it. This would mean,
    // that your thread misses the event. The resetable flag for those condition sets can help here. Whenever a condition, managed by the
    // condition set, fires, the moduleState variable remembers it. So, the next call to m_moduleState.wait() will immediately return and reset
    // the "memory" of the moduleState. For more details, see: http://berkeley.informatik.uni-leipzig.de/trac/ow-public/wiki/HowtoWaitCorrectly

    // Signal ready state. Now your module can be connected by the container, which owns the module.
    ready();
    debugLog() << "Module is now ready.";

    // Normally, you will have a loop which runs as long as the module should not shutdown. In this loop you can react on changing data on input
    // connectors or on changed in your properties.
    debugLog() << "Entering main loop";
    while ( !m_shutdownFlag() )
    {
        // Now, the moduleState variable comes into play. The module can wait for the condition, which gets fired whenever the input receives data
        // or an property changes. The main loop now waits until something happens.
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // The next part is the collection part. We collect the information we need and check whether they changed.
        // Do not recalculate everything in every loop. Always check whether the data changed or some property and handle those cases properly.
        // After collection, the calculation work can be done.


        // Now, we can check the input, whether it changed the data. Therefore, we try to grab some data and check whether it is different from
        // the currently used data. Of course, you do not need to check whether the data really is different, but you should do it as you do not
        // want permanent recalculation which is actually not needed.
        //
        // Note: do not call m_input->getData() twice; one for checking if it is different and on for copying the pointer, since the result of
        // getData might be different among both calls.
        boost::shared_ptr< WDataSetSingle > newDataSet = m_input->getData();
        bool dataChanged = ( m_dataSet != newDataSet );
        if ( dataChanged || !m_dataSet )
        // this condition will become true whenever the new data is different from the current one or our actual data is NULL. This handles all
        // cases.
        {
            // The data is different. Copy it to our internal data variable:
            debugLog() << "Received Data.";
            m_dataSet = newDataSet;

            // An output sent us an invalid dataset. You should always check this. You module most probably does not work on invalid pointers
            // ;-).
            if ( !m_dataSet )
            {
                debugLog() << "Invalid Data. Disabling.";

                // In this situation it is the best to turn of the visualization from the last dataset and begin the loop from the beginning.
                // This way you can ensure you always have valid data available.
                continue;
            }
        }

        // Here we collect our properties. You, as with input connectors, always check if a property really has changed. You most probably do not
        // want to check properties which are used exclusively inside the update callback of your OSG node. As the properties are thread-safe, the
        // update callback can check them and apply it correctly to your visualization.
        //
        // To check whether a property changed, WPropertyVariable provides a changed() method which is true whenever the property has changed.
        // Please note: creating the property with addProperty( ... ) will set changed to true.
        if ( m_aFile->changed() )
        {
            // To reset the changed flag, supply a "true" to the get method. This resets the changed-flag and next loop you can again check
            // whether it has been changed externally.

            // This is a simple example for doing an operation which is not depending on any other property.
            debugLog() << "Doing an operation on the file \"" << m_aFile->get( true ).file_string() << "\".";
        }

        // m_aFile got handled above. Now, handle two properties which together are used as parameters for an operation.
        if ( m_aString->changed() )
        {
            // This is a simple example for doing an operation which is depends on all, but m_anFile,  properties.
            debugLog() << "Doing an operation basing on m_aString ... ";
            debugLog() << "m_aString: " << m_aString->get( true );
        }

        // This example code now shows how to modify your OSG nodes basing on changes in your dataset or properties.
        if ( m_anInteger->changed() || m_aDouble->changed() || dataChanged )
        {
            debugLog() << "Creating new OSG node";

            // You should grab your values at the beginning of such calculation blocks, since the property might change at any time!
            int rows = m_anInteger->get( true );
            double radii = m_aDouble->get( true );

            debugLog() << "Number of Rows: " << rows;
            debugLog() << "Radii: " << radii;
            debugLog() << "Current dataset: " << m_dataSet->getFileName() << " with name: " << m_dataSet->getName();

            // This block will be executed whenever we have a new dataset or the m_anInteger property has changed. This example codes produces
            // some shapes and replaces the existing root node by a new (updated) one. Therefore, a new root node is needed:
            osg::ref_ptr< osg::Geode > newRootNode = new osg::Geode();
            // When working with the OpenSceneGraph, always use ref_ptr to store pointers to OSG objects. This allows OpenSceneGraph to manage
            // its resources automatically.
            for ( int32_t i = 0; i < rows; ++i )
            {
                newRootNode->addDrawable(
                        new osg::ShapeDrawable( new osg::Box(             osg::Vec3(  25, 128, i * 15 ), radii ) ) );
                newRootNode->addDrawable(
                        new osg::ShapeDrawable( new osg::Sphere(          osg::Vec3(  75, 128, i * 15 ), radii ) ) );
                newRootNode->addDrawable(
                        new osg::ShapeDrawable( new osg::Cone(            osg::Vec3( 125, 128, i * 15 ), radii, radii ) ) );
                newRootNode->addDrawable(
                        new osg::ShapeDrawable( new osg::Cylinder(        osg::Vec3( 175, 128, i * 15 ), radii, radii ) ) );
                newRootNode->addDrawable(
                        new osg::ShapeDrawable( new osg::Capsule(         osg::Vec3( 225, 128, i * 15 ), radii, radii ) ) );
            }

            // The old root node needs to be removed safely. The OpenSceneGraph traverses the graph at every frame. This traversion is done in a
            // separate thread. Therefore, adding a Node directly may cause the OpenSceneGraph to crash. Thats why the Group node provided by
            // getScene offers safe remove and insert methods. Use them to manipulate the scene node.
            WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );
            m_rootNode = newRootNode;

            // OSG allows you to add custom callbacks. These callbacks get executed on each update traversal. They can be used to modify several
            // attributes and modes of existing nodes. You do not want to remove the node and recreate another one to simply change some color,
            // right? Setting the color can be done in such an update callback. See in the header file, how this class is defined.
            m_rootNode->addUpdateCallback( new SafeUpdateCallback( this ) );

            WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootNode );
        }

        // Now we updated the visualization after the dataset has changed. Your module might also calculate some other datasets basing on the
        // input data.
        if ( dataChanged )
        {
            debugLog() << "Data changed. Recalculating output.";

            // Calculate your new data here. This example just forwards the input to the output ;-).
            boost::shared_ptr< WDataSetSingle > newData = m_dataSet;

            // Doing a lot of work without notifying the user visually is not a good idea. So how is it possible to report progress? Therefore,
            // the WModule class provides a member m_progress which is of type WPropgressCombiner. You can create own progress objects and count
            // them individually. The m_progress combiner provides this information to the GUI and the user.
            // Here is a simple example:
            int steps = 10;
            boost::shared_ptr< WProgress > progress1 = boost::shared_ptr< WProgress >( new WProgress( "Doing work 1", steps ) );
            m_progress->addSubProgress( progress1 );
            for ( int i = 0; i < steps; ++i )
            {
                ++*progress1;
                sleep( 1 );
            }
            progress1->finish();
            // This creates a progress object with a name and a given number of steps. Your work loop can now increment the progress object. The
            // progress combiner m_progress collects the progress and provides it to the GUI. When finished, the progress MUST be marked as
            // finished using finish(). It is no problem to have several progress objects at the same time!

            // Sometimes, the number of steps is not known. WProgress can also handle this. Simply leave away the last parameter (the number of
            // steps. As with the other progress, you need to add it to the modules progress combiner and you need to mark it as finished with
            // finish() if you are done with your work.
            boost::shared_ptr< WProgress > progress2 = boost::shared_ptr< WProgress >( new WProgress( "Doing work 2" ) );
            m_progress->addSubProgress( progress2 );
            sleep( 2 );
            progress2->finish();

            // How to set the data to the output and how to notify other modules about it?
            m_output->updateData( newData );
            // This sets the new data to the output connector and automatically notifies all modules connected to your output.
        }
    }

    // At this point, the container managing this module signalled to shutdown. The main loop has ended and you should clean up. Always remove
    // allocated memory and remove all OSG nodes.
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );
}

void WMTemplate::SafeUpdateCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    if ( m_module->m_aColor->changed() )
    {
        // Grab the color
        WColor c = m_module->m_aColor->get( true );

        // Set the diffuse color and material:
        osg::ref_ptr< osg::Material > mat = new osg::Material();
        mat->setDiffuse( osg::Material::FRONT, osg::Vec4( c.getRed(), c.getGreen(), c.getBlue(), c.getAlpha() ) );
        node->getOrCreateStateSet()->setAttribute( mat, osg::StateAttribute::ON );
    }
    traverse( node, nv );
}

bool WMTemplate::StringLength::accept( boost::shared_ptr< WPropertyVariable< WPVBaseTypes::PV_STRING > > /* property */,
                                       WPVBaseTypes::PV_STRING value )
{
    // This method gets called everytime the m_aString property is going to be changed. It can decide whether the new value is valid or not. If
    // the method returns true, the new value is set. If it returns false, the value is rejected.
    //
    // Note: always use WPVBaseTypes when specializing the WPropertyVariable template.

    // simple example: just accept string which are at least 5 chars long and at most 10.
    return ( value.length() <= 10 ) && ( value.length() >= 5 );
}

