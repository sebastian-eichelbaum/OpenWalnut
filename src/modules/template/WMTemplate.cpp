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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// How to run this module in OpenWalnut:
//   * Download a sample Dataset (http://www.informatik.uni-leipzig.de/~wiebel/public_data/walnut/walnut_masked.nii.gz)
//   * Load the dataset by dragging it into OpenWalnut
//   * Click on it and add the template module
//     * This can be done via right-click menu or the module toolbar below the menu
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// How to create your own module in OpenWalnut? Here are the steps to take:
//   * Setup your module building framework
//     * See http://www.openwalnut.org/projects/openwalnut/wiki/Module_ExternalDevelopment for details
//   * copy the template module directory
//   * think about a name for your module
//   * rename the files from WMTemplate.cpp and WMTemplate.h to WMYourModuleName.cpp and WMYourModuleName.h
//   * rename the class inside these files to WMYourModuleName
//   * rename the class inside "W_LOADABLE_MODULE" to WMYourModuleName
//   * change WMYourModuleName::getName() to a unique name, like "Your Module Name"
//   * read the documentation in this module and modify it to your needs
//   * compile
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Some rules to the inclusion of headers:
//  * Ordering:
//    * C Headers
//    * C++ Standard headers
//    * External Lib headers (like OSG or Boost headers)
//    * headers of other classes inside OpenWalnut
//    * your own header file

#include <string>

#include <osg/ShapeDrawable>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Material>
#include <osg/StateAttribute>

#include "core/kernel/WKernel.h"
#include "core/common/exceptions/WFileNotFound.h"
#include "core/common/WColor.h"
#include "core/common/WPathHelper.h"
#include "core/common/WPropertyHelper.h"
#include "core/graphicsEngine/WGEUtils.h"
#include "core/graphicsEngine/WGERequirement.h"

#include "WMTemplate.xpm"
#include "icons/bier.xpm"
#include "icons/wurst.xpm"
#include "icons/steak.xpm"
#include "WMTemplate.h"

// This line is needed by the module loader to actually find your module. You need to add this to your module too. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMTemplate )

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

const char** WMTemplate::getXPMIcon() const
{
    // The template_xpm char array comes from the template.xpm file as included above.
    // Such char arrays, i.e. files, can be easily created using an image manipulation program
    // like GIMP. Be aware that the xpm file is a simple header file. Thus it contains real
    // code. This code can be manipulated by hand. Unfortunately, you really have to fix the
    // xpm files produced by gimp. You need to make the char array const in order to prevent
    // compiler warnings or even errors.
    return template_xpm;
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
    m_input = WModuleInputData< WDataSetSingle >::createAndAdd( shared_from_this(), "in", "The dataset to display" );

    // This creates an input connector which can receive WDataSetSingle. It will never be able to connect to output connectors providing just a
    // WDataSet (which is the father class of WDataSetSingle), but it will be able to be connected to an output connector with a type derived
    // from WDataSetSingle (like WDataSetScalar or WDataSetVector)

    // Now, lets add an output connector. We want to provide data calculated here to other modules. The output connector is initialized the same
    // way as input connectors. You need the type, the module-wide unique name and the description. The type you specify here also determines
    // which input connectors can be connected to this output connector: only connectors with a type equal or lower in class hierarchy.
    m_output = WModuleOutputData < WDataSetSingle  >::createAndAdd( shared_from_this(), "out", "The calculated dataset" );

    // call WModule's initialization
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

    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );
    m_aTrigger         = m_properties->addProperty( "Do it now!",               "Trigger Button Text.", WPVBaseTypes::PV_TRIGGER_READY,
                                                    m_propCondition );

    m_enableFeature    = m_properties->addProperty( "Enable feature",           "Description.", true );
    m_anInteger        = m_properties->addProperty( "Number of shape rows",     "Number of shape rows.", 10, m_propCondition );
    m_anIntegerClone   = m_properties->addProperty( "CLONE!Number of shape rows",
                                                    "A property which gets modified if \"Number of shape rows\" gets modified.", 10 );
    m_aDouble          = m_properties->addProperty( "Shape radii",              "Shape radii.", 20.0, m_propCondition );
    m_aString          = m_properties->addProperty( "A string",                 "Something.", std::string( "hello" ), m_propCondition );
    m_aFile            = m_properties->addProperty( "A filename",              "Description.", WPathHelper::getAppPath(), m_propCondition );
    m_aColor           = m_properties->addProperty( "A color",                  "Description.", WColor( 1.0, 0.0, 0.0, 1.0 ) );
    m_aPosition        = m_properties->addProperty( "Somewhere",                "Description.", WPosition( 0.0, 0.0, 0.0 ) );

    // These lines create some new properties and add them to the property list of this module. The specific type to create is determined by the
    // initial value specified in the third argument. The first argument is the name of the property, which needs to be unique among all
    // properties of this group and must not contain any slashes (/). The second argument is a description. A nice feature is the possibility
    // to specify an own condition, which gets fired when the property gets modified. This is especially useful to wake up the module's thread
    // on property changes. So, the property m_anInteger will wake the module thread on changes. m_enableFeature and m_aColor should not wake up
    // the module thread. They get read by the update callback of this modules OSG node, to update the color. m_aTrigger is a property which can
    // be used to trigger costly operations. The GUI shows them as buttons with the description as button text. The user can then press them and
    // the WPropTrigger will change its state to PV_TRIGGER_TRIGGERED. In the moduleMain documentation, you'll find a more detailed description
    // of how to use trigger properties. Be aware, that these kind of properties should be used carefully. They somehow inhibit the update flow
    // through the module graph.
    //
    // m_anIntegerClone has a special purpose in this example. It shows that you can simply update properties from within your module whilst the
    // GUI updates itself. You can, for example, set constraints or simply modify values depending on input data, most probably useful to set
    // nice default values or min/max constraints.

    // All these above properties are not that usable for selections. Assume the following situation. Your module allows two different kinds of
    // algorithms to run on some data and you want the user to select which one should do the work. This might be done with integer properties but it
    // is simply ugly. Therefore, properties of type WPropSelection are available. First you need to define a list of alternatives:
    m_possibleSelections = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_possibleSelections->addItem( "Beer", "Cold and fresh.", template_bier_xpm );          // NOTE: you can add XPM images here.
    m_possibleSelections->addItem( "Steaks", "Medium please.",  template_steak_xpm );
    m_possibleSelections->addItem( "Sausages", "With Sauerkraut.", template_wurst_xpm );

    // This list of alternatives is NOT the actual property value. It is the list on which so called "WItemSelector" instances work. These
    // selectors are the actual property. After you created the first selector instance from the list, it can't be modified anymore. This ensures
    // that it is consistent among multiple threads and selection instances. The following two lines create two selectors as initial value and
    // create the property:
    m_aSingleSelection = m_properties->addProperty( "I like most",  "Do you like the most?", m_possibleSelections->getSelectorFirst(),
                                                    m_propCondition );
    m_aMultiSelection  = m_properties->addProperty( "I like", "What do you like.", m_possibleSelections->getSelectorAll(),
                                                    m_propCondition );

    // Adding a lot of properties might confuse the user. Using WPropGroup, you have the possibility to group your properties together. A
    // WPropGroup needs a name and can provide a description. As with properties, the name should not contain any "/" and must be unique.

    m_group1        = m_properties->addPropertyGroup( "First Group",  "A nice group for grouping stuff." );
    m_group1a       = m_group1->addPropertyGroup(     "Group 1a", "A group nested into \"Group 1\"." );
    m_group2        = m_properties->addPropertyGroup( "Second Group",  "Another nice group for grouping stuff." );

    // To understand how the groups can be used, you should consider that m_properties itself is a WPropGroup! This means, you can use your newly
    // created groups exactly in the same way as you would use m_properties.
    m_group1Bool    = m_group1->addProperty( "Funny stuff", "A grouped property", true );

    // You even can add one property multiple times to different groups:
    m_group2->addProperty( m_aColor );
    m_group1a->addProperty( m_aDouble );
    m_group1a->addProperty( m_enableFeature );

    // Properties can be hidden on the fly. The GUI updates automatically. This is a very useful feature. You can create properties which depend
    // on a current selection and blend them in our out accordingly.
    m_aHiddenInt = m_group2->addProperty( "Hide me please", "A property used to demonstrate the hidden feature.", 1, true );
    m_aHiddenGroup = m_group2->addPropertyGroup( "Hide me please too", "A property used to demonstrate the hidden feature.", true );

    // Add another button to group2. But this time, we do not want to wake up the main thread. We handle this directly. Fortunately,
    // WPropertyVariable offers you the possibility to specify your own change callback. This callback is used for hiding the m_aColor property
    // on the fly.
    m_hideButton = m_group2->addProperty( "(Un-)Hide", "Trigger Button Text.", WPVBaseTypes::PV_TRIGGER_READY,
                                          boost::bind( &WMTemplate::hideButtonPressed, this ) );

    // How can the values of the properties be changed? You can take a look at moduleMain where this is shown. For short: m_anInteger->set( 2 )
    // and m_anInteger->get().

    // The properties offer another nice feature: property constraints. You can enforce your properties to be in a special range, to not be
    // empty, to contain a valid directory name and so on. This is done with the class WPropertyVariable< T >::WPropertyConstraint. There are
    // several predefined you can use directly: WPropertyConstraintTypes.h. The constants defined there can be used as namespace in
    // WPropertyHelper. As an example, we want the property m_aFile to only contain existing directories:
    WPropertyHelper::PC_PATHEXISTS::addTo( m_aFile );
    WPropertyHelper::PC_ISDIRECTORY::addTo( m_aFile );

    // Thats it. To set minimum and maximum value for a property the convenience methods setMin and setMax are defined. setMin and setMax are
    // allowed for all property types with defined <= and >= operator.
    m_anInteger->setMin( 1 );
    m_anInteger->setMax( 15 );
    m_aDouble->setMin( 5.0 );
    m_aDouble->setMax( 50.0 );

    // we also want to constraint the both selection properties. One should not allow selecting multiple elements. But both require at least one
    // element to be selected:
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_aSingleSelection );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_aSingleSelection );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_aMultiSelection );

    // The most amazing feature is: custom constraints. Similar to OSG update callbacks, you just need to write your own PropertyConstraint class
    // to define the allowed values for your constraint. Take a look at the StringLength class in this module's code on how to do it.
    m_aString->addConstraint( boost::shared_ptr< StringLength >( new StringLength ) );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_aString );

    // One last thing to mention is the active property. This property is available in all modules and represents the activation state of the
    // module. In the GUI this is simply a checkbox beneath the module. The active property should be taken into account in ALL modules.
    // Visualization modules should turn off their graphics. There are basically three ways to react on changes in m_active, which is the member
    // variable for this property.
    // 1: overwrite WModule::activate() in your module
    // 2: register your own handler: m_active->getCondition()->subscribeSignal( boost::bind( &WMTemplate::myCustomActiveHandler, this ) );
    // 3: react during your module main loop using the moduleState: m_moduleState.add( m_active->getCondition );
    // Additionally, your can also use the m_active variable directly in your update callbacks to en-/disable some OSG nodes.
    // This template module uses method number 1. This might be the easiest and most commonly used way.

    // Sometimes it is desirable to provide some values, statistics, counts, times, ... to the user. This would be possible by using a property
    // and set the value to the value you want to show the user. Nice, but the user can change this value. PropertyConstraints can't help here,
    // as they would forbid writing any value to the property (regardless if the module or the user wants to set it). In other words, these
    // special properties serve another purpose. They are used for information output. Your module already provides another property list only
    // for these kind of properties. m_infoProperties can be used in the same way as m_properties. The only difference is that each property and
    // property group added here can't be modified from the outside world. Here is an example:
    m_aIntegerOutput = m_infoProperties->addProperty( "Run count", "Number of run cycles the module made so far.", 0 );
    // Later on, we will use this property to provide the number of run cycles to the user.
    // In more detail, the purpose type of the property gets set to PV_PURPOSE_INFORMATION automatically by m_infoProperties. You can, of course,
    // add information properties to your custom groups or m_properties too. There, you need to set the purpose flag of the property manually:
    std::string message = std::string( "Hey you! Besides all these parameters, you also can print values, " ) +
                          std::string( "<font color=\"#00f\" size=15>html</font> formatted strings, colors and " ) +
                          std::string( "so on using <font color=\"#ff0000\">properties</font>! Isn't it <b>amazing</b>?" );

    m_aStringOutput = m_group1a->addProperty( "A message", "A message to the user.", message );
    m_aStringOutput->setPurpose( PV_PURPOSE_INFORMATION );
    // This adds the property m_aStringOutput to your group and sets its purpose. The default purpose for all properties is always
    // "PV_PURPOSE_PARAMETER". It simply denotes the meaning of the property - its meant to be used as modifier for the module's behavior; a
    // parameter.
    //
    // Some more examples. Please note: Although every property type can be used as information property, not everything is really useful.
    m_infoProperties->addProperty( m_aStringOutput );   // we can also re-add properties
    m_aTriggerOutput = m_infoProperties->addProperty( "A trigger", "Trigger As String", WPVBaseTypes::PV_TRIGGER_READY );
    m_aDoubleOutput = m_infoProperties->addProperty( "Some double", "a Double. Nice isn't it?", 3.1415 );
    m_aIntOutput = m_infoProperties->addProperty( "Some int", "a int. Nice isn't it?", 123456 );
    m_aColorOutput = m_infoProperties->addProperty( "A color", "Some Color. Nice isn't it?", WColor( 0.5, 0.5, 1.0, 1.0 ) );
    m_aFilenameOutput = m_infoProperties->addProperty( "Nice file", "a Double. Nice isn't it?", WPathHelper::getAppPath() );
    m_aSelectionOutput = m_infoProperties->addProperty( "A selection", "Selection As String",  m_possibleSelections->getSelectorFirst() );
    // One important note regarding information properties. If a property gets added in a group which is an information property-group, then
    // each added property does NOT contain any constraints. If a property gets an information property AFTER its creation, like m_aStringOutput,
    // then it keeps its constraints!

    // We now add another trigger. Pressing this button will cause an exception to be thrown. This demonstrates how the GUI and OpenWalnut
    // handles modules which throw an exception without catching it.
    m_exceptionTrigger = m_properties->addProperty( "Press to crash Module", "Pressing this button lets the module intentionally crash.",
                                                    WPVBaseTypes::PV_TRIGGER_READY, m_propCondition );

    WModule::properties();
}

void WMTemplate::requirements()
{
    // This method allows modules to specify what they need to run properly. This module, for example, needs the WGE. It therefore adds the
    // WGERequirement to the list of requirements. Modules only get started if all the requirements of it are met by the current running
    // OpenWalnut. This is a very handy tool for NO-GUI versions or script versions of OpenWalnut where there simply is no graphics engine
    // running. This way, the kernel can ensure that only modules are allowed to run who do not require the WGE.
    // Another useful example are module containers. Usually, they need several other modules to work properly.
    m_requirements.push_back( new WGERequirement() );
}

void WMTemplate::moduleMain()
{
    // This is the modules working thread. Its the most important part of your module.
    // When you enter this method, all connectors and properties the module provides are fixed. They get initialized in connectors() and
    // properties(). You always can assume the kernel, the GUI, the graphics engine and the data handler to be initialized and ready. Please keep
    // in mind, that this method is running in its own thread.

    // You can output log messages everywhere and any time in your module. The WModule base class therefore provides debugLog, infoLog, warnLog
    // and errorLog. You can use them very similar to the common std::cout streams.
    debugLog() << "Entering moduleMain()";

    // Your module can notify everybody that it is ready to be used. The member function ready() does this for you. The ready state is especially
    // useful whenever your module needs to do long operations to initialize. No other module can connect to your module before it signals its
    // ready state. You can assume the code before ready() to be some kind of initialization code.
    debugLog() << "Doing time consuming operations";
    sleep( 2 );

    // Your module can use an moduleState variable to wait for certain events. Most commonly, these events are new data on input connectors or
    // changed properties. You can decide which events the moduleState should handle. Therefore, use m_moduleState.add( ... ) to insert every
    // condition you want to wait on. As every input connector provides an changeCondition, we now add this condition to the moduleState:
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    // Remember the condition provided to some properties in properties()? The condition can now be used with this condition set.
    m_moduleState.add( m_propCondition );
    // One note about "setResetable": It might happen, that a condition fires and your thread does not currently waits on it. This would mean,
    // that your thread misses the event. The resettable flag for those condition sets can help here. Whenever a condition, managed by the
    // condition set, fires, the moduleState variable remembers it. So, the next call to m_moduleState.wait() will immediately return and reset
    // the "memory" of the moduleState. For more details, see:
    // http://www.openwalnut.org/projects/openwalnut/wiki/MultithreadingHowto#How-to-wait-correctly

    // Signal ready state. Now your module can be connected by the container, which owns the module.
    ready();
    debugLog() << "Module is now ready.";

    // Most probably, your module will be a module providing some kind of visual output. In this case, the WGEManagedGroupNode is very handy.
    // It allows you to insert several nodes and transform them as the WGEGroupNode (from which WGEManagedGroupNode is derived from) is also
    // an osg::MatrixTransform. The transformation feature comes in handy if you want to transform your whole geometry according to a dataset
    // coordinate system for example. Another nice feature in WGEManagedGroupNode is that it can handle the m_active flag for you. Read the
    // documentation of WMTemplate::activate for more details.
    // First, create the node and add it to the main scene. If you insert something into the scene, you HAVE TO remove it after your module
    // has finished!
    m_rootNode = new WGEManagedGroupNode( m_active );
    // Set a new callback for this node which basically transforms the geometry according to m_aPosition. Update callbacks are the thread safe
    // way to manipulate an OSG node while it is inside the scene. This module contains several of these callbacks as an example. The one used
    // here is to translate the root node coordinate system in space according to m_aPosition:
    m_rootNode->addUpdateCallback( new TranslateCallback( this ) );
    // Insert to the scene:
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootNode );

    // Normally, you will have a loop which runs as long as the module should not shutdown. In this loop you can react on changing data on input
    // connectors or on changed in your properties.
    debugLog() << "Entering main loop";
    while( !m_shutdownFlag() )
    {
        // Now, the moduleState variable comes into play. The module can wait for the condition, which gets fired whenever the input receives data
        // or an property changes. The main loop now waits until something happens.
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        // As you might remember, this property is an information property to provide the number of run cycles to the outside world. It won't be
        // modified but the module can modify it. This is useful to provide statistics, counts, times or even a "hello world" string to the user
        // as an information or status report. Please do not abuse these information properties as progress indicators. A short overview on how
        // to make progress indicators is provided some lines below. Here, we simply increase the value.
        m_aIntegerOutput->set( m_aIntegerOutput->get() + 1 );

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // After waking up, the module has to check whether the shutdownFlag fired. If yes, simply quit the module.

        // woke up since the module is requested to finish
        if( m_shutdownFlag() )
        {
            break;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // The next part is the collection part. We collect the information we need and check whether they changed.
        // Do not recalculate everything in every loop. Always check whether the data changed or some property and handle those cases properly.
        // After collection, the calculation work can be done.


        // Now, we can check the input, whether there is an update enqueued for us. But first, we need to understand the meaning of an update on
        // an input connector:
        //  * a module updates its output connector with some new data -> updated
        //  * a module triggers an update on its output connector without an actual data change -> updated
        //  * our own input connector got connected to an output connector -> updated
        //  * our own input connector got DISconnected from an output connector -> NO update
        // You now might ask: "Why can modules trigger updates if they did not change the data?". The answer is simple. Some modules change the
        // grid without actually changing the data for example. They translate the grid in space. This results in an update although the actual
        // data stayed the same.

        // To query whether an input was updated, simply ask the input:
        bool dataUpdated = m_input->updated();

        // Remember the above criteria. We now need to check if the data is valid. After a connect-update, it might be NULL.
        boost::shared_ptr< WDataSetSingle > dataSet = m_input->getData();
        bool dataValid = ( dataSet );
        // After calling getData(), the update flag is reset and false again. Please keep in mind, that the module lives in an multi-threaded
        // world where the update flag and data can change at any time. DO NEVER use getData directly in several places of your module as the
        // data returned may change between two consecutive calls! Always grab it into a local variable and use this variable.

        // Another important hint. For grabbing the data, use a local variable wherever possible. If you use a member variable, the data might
        // never be freed if nobody uses the data anymore because your module holds the last reference. If you need to use a member variable for
        // the received data, subscribe the your input's disconnect signal or overwrite WModule::notifyConnectionClosed and reset your variable
        // there to ensure its proper deletion.

        // do something with the data
        if( dataUpdated && dataValid )
        {
            // The data is valid and we received an update. The data is not NULL but may be the same as in previous loops.
            debugLog() << "Received Data.";
        }

        // If there is no data, this might have the following reasons: the connector never has been connected or it got disconnected. Especially
        // in the case of a disconnect, you should always clean up your renderings and internal states. A disconnected module should not render
        // anything anymore. Locally stored referenced to the old input data have to be reset to. Only this way, it is guaranteed that not used
        // data gets deleted properly.
        if( !dataValid )
        {
            debugLog() << "Data changed. No valid data anymore. Cleaning up.";
            WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );
        }

        // Here we collect our properties. You, as with input connectors, always check if a property really has changed. You most probably do not
        // want to check properties which are used exclusively inside the update callback of your OSG node. As the properties are thread-safe, the
        // update callback can check them and apply it correctly to your visualization.
        //
        // To check whether a property changed, WPropertyVariable provides a changed() method which is true whenever the property has changed.
        // Please note: creating the property with addProperty( ... ) will set changed to true.
        if( m_aFile->changed() )
        {
            // To reset the changed flag, supply a "true" to the get method. This resets the changed-flag and next loop you can again check
            // whether it has been changed externally.

            // This is a simple example for doing an operation which is not depending on any other property.
            debugLog() << "Doing an operation on the file \"" << m_aFile->get( true ).string() << "\".";

            // NOTE: be careful if you want to use dataSet here, as it might be unset. Verify data validity using dataUpdated && dataValid.
        }

        // m_aFile got handled above. Now, handle two properties which together are used as parameters for an operation.
        if( m_aString->changed() )
        {
            // This is a simple example for doing an operation which is depends on all, but m_aFile,  properties.
            debugLog() << "Doing an operation basing on m_aString ... ";
            debugLog() << "m_aString: " << m_aString->get( true );

            // NOTE: be careful if you want to use dataSet here, as it might be unset. Verify data validity using dataUpdated && dataValid.
        }

        // This example code now shows how to modify your OSG nodes basing on changes in your dataset or properties.
        // The if statement also checks for data validity as it uses the data! You should also always do that.
        if( ( m_anInteger->changed() || m_aDouble->changed() || dataUpdated  ) && dataValid )
        {
            debugLog() << "Creating new OSG node";

            // You should grab your values at the beginning of such calculation blocks, since the property might change at any time!
            int rows = m_anInteger->get( true );
            double radii = m_aDouble->get( true );

            // You can set other properties here. This example simply sets the value of m_anIntegerClone. The set command allows an additional
            // parameter. If it is true, the specified property condition does not fire if it is set. This is useful if your module main loop
            // waits for the condition of the property you want to set. Setting the property without suppressing the notification would cause
            // another loop in your module.
            m_anIntegerClone->set( m_anInteger->get(), true );

            debugLog() << "Number of Rows: " << rows;
            debugLog() << "Radii: " << radii;
            debugLog() << "Current dataset: " << dataSet->getFilename() << " with name: " << dataSet->getName();

            // This block will be executed whenever we have a new dataset or the m_anInteger property has changed. This example codes produces
            // some shapes and replaces the existing root node by a new (updated) one. Therefore, a new root node is needed:
            osg::ref_ptr< osg::Geode > newGeode = new osg::Geode();
            // When working with the OpenSceneGraph, always use ref_ptr to store pointers to OSG objects. This allows OpenSceneGraph to manage
            // its resources automatically.
            for( int32_t i = 0; i < rows; ++i )
            {
                newGeode->addDrawable(
                        new osg::ShapeDrawable( new osg::Box(             osg::Vec3(  25, 128, i * 15 ), radii ) ) );
                newGeode->addDrawable(
                        new osg::ShapeDrawable( new osg::Sphere(          osg::Vec3(  75, 128, i * 15 ), radii ) ) );
                newGeode->addDrawable(
                        new osg::ShapeDrawable( new osg::Cone(            osg::Vec3( 125, 128, i * 15 ), radii, radii ) ) );
                newGeode->addDrawable(
                        new osg::ShapeDrawable( new osg::Cylinder(        osg::Vec3( 175, 128, i * 15 ), radii, radii ) ) );
                newGeode->addDrawable(
                        new osg::ShapeDrawable( new osg::Capsule(         osg::Vec3( 225, 128, i * 15 ), radii, radii ) ) );
            }

            // The old root node needs to be removed safely. The OpenSceneGraph traverses the graph at every frame. This traversal is done in a
            // separate thread. Therefore, adding a node directly may cause the OpenSceneGraph to crash. Thats why the Group node (WGEGroupNode)
            // offers safe remove and insert methods. Use them to manipulate the scene node.
            // First remove the old node:
            m_rootNode->remove( m_geode );
            m_geode = newGeode;

            // OSG allows you to add custom callbacks. These callbacks get executed on each update traversal. They can be used to modify several
            // attributes and modes of existing nodes. You do not want to remove the node and recreate another one to simply change some color,
            // right? Setting the color can be done in such an update callback. See in the header file, how this class is defined.
            m_geode->addUpdateCallback( new SafeUpdateCallback( this ) );

            // And insert the new node
            m_rootNode->insert( m_geode );
        }

        // Now we updated the visualization after the dataset has changed. Your module might also calculate some other datasets basing on the
        // input data.
        // To ensure that all datasets are valid, check dataUpdated and dataValid. If both are true, you can safely use the data.
        if( dataUpdated && dataValid )
        {
            debugLog() << "Data changed. Recalculating output.";

            // Calculate your new data here. This example just forwards the input to the output ;-).
            boost::shared_ptr< WDataSetSingle > newData = dataSet;

            // Doing a lot of work without notifying the user visually is not a good idea. So how is it possible to report progress? Therefore,
            // the WModule class provides a member m_progress which is of type WPropgressCombiner. You can create own progress objects and count
            // them individually. The m_progress combiner provides this information to the GUI and the user.
            // Here is a simple example:
            int steps = 10;
            boost::shared_ptr< WProgress > progress1 = boost::shared_ptr< WProgress >( new WProgress( "Doing work 1", steps ) );
            m_progress->addSubProgress( progress1 );
            for( int i = 0; i < steps; ++i )
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

        // As we provided our condition to m_aTrigger too, the main thread will wake up if it changes. The GUI can change the trigger only to the
        // state "PV_TRIGGER_TRIGGERED" (this is the case if the user presses the button).
        if( m_aTrigger->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            // Now that the trigger has the state "triggered", a time consuming operation can be done here.
            debugLog() << "User triggered an important and time consuming operation.";

            // We can exchange the list used for selection properties. This of course invalidates the current user selection. You should avoid
            // changing this too often and too fast as it might confuse the user.
            m_possibleSelections->addItem( "Beer2", "Cold and fresh.", template_bier_xpm );          // NOTE: you can add XPM images here.
            m_possibleSelections->addItem( "Steaks2", "Medium please.",  template_steak_xpm );
            m_possibleSelections->addItem( "Sausages2", "With Sauerkraut.", template_wurst_xpm );
            // Each of the above write operations trigger an invalidation of all currently exiting selectors. You can create a new selector
            // basing on an old invalid one and set it as new value for the selections:

            // Now we set the new selection and selector. Calling newSelector without any argument copies the old selector and tries to resize
            // the selection to match the new size
            m_aSingleSelection->set( m_aSingleSelection->get().newSelector() );
            m_aMultiSelection->set( m_aMultiSelection->get().newSelector() );

            // Update the output property
            m_aTriggerOutput->set( WPVBaseTypes::PV_TRIGGER_TRIGGERED );

            // Do something here. As above, do not forget to inform the user about your progress.
            int steps = 10;
            boost::shared_ptr< WProgress > progress1 = boost::shared_ptr< WProgress >( new WProgress( "Doing something important", steps ) );
            m_progress->addSubProgress( progress1 );
            for( int i = 0; i < steps; ++i )
            {
                ++*progress1;
                sleep( 1 );
            }
            progress1->finish();

            // As long as the module does not reset the trigger to "ready", the GUI disables the trigger button. This is very useful to avoid
            // that a user presses the button multiple times during an operation. When setting the property back to "ready", the GUI re-enables
            // the button and the user can press it again.
            // To avoid the moduleMain- loop to awake every time we reset the trigger, provide a second parameter to the set() method. It denotes
            // whether the change notification should be fired or not. In our case, we avoid this by providing false to the second parameter.
            m_aTrigger->set( WPVBaseTypes::PV_TRIGGER_READY, false );

            // Also update the information property.
            m_aTriggerOutput->set( WPVBaseTypes::PV_TRIGGER_READY );
        }

        // This checks the selections.
        if( m_aMultiSelection->changed() ||  m_aSingleSelection->changed() )
        {
            // The single selector allows only one selected item and requires one item to be selected all the time. So accessing it by index
            // is trivial:
            WItemSelector s = m_aSingleSelection->get( true );
            infoLog() << "The user likes " << s.at( 0 )->getName() << " the most.";

            // The multi property allows the selection of several items. So, iteration needs to be done here:
            s = m_aMultiSelection->get( true );
            for( size_t i = 0; i < s.size(); ++i )
            {
                infoLog() << "The user likes " << s.at( i )->getName();
            }
        }

        // Trigger an exception? We do this whenever the user pressed the exception-button
        if( m_exceptionTrigger->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            // Throw an exception and do not catch it. Please note that OpenWalnut provides several exceptions which usually cover the most
            // needs. If not, derive your own exceptions from WException. Using WExceptions has one nice advantage: it provides a backtrace on
            // systems which support this.
            throw WFileNotFound( "This is a demonstration of an exception being thrown from within a module." );
            // OpenWalnut then automatically catches it and transports it to the kernel and the registered callbacks. This usually is the GUI
            // which shows a dialog or something similar. Additionally, the m_isCrashed flag is set to true. Once a module is crahsed, it cannot
            // be "restored".
        }
    }

    // At this point, the container managing this module signaled to shutdown. The main loop has ended and you should clean up:
    //
    //  * remove allocated memory
    //  * remove all OSG nodes
    //  * stop any pending threads you may have started earlier
    //  * ...
    //  NOTE: as the module gets disconnected prior to shutdown, most of the cleanup should have been done already.
}

void WMTemplate::SafeUpdateCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    // One note about m_aColor: As you might have notices, changing one of the properties, which recreate the OSG node, causes the material to be
    // gray again. This is simply caused by m_aColor->changed() is still false! To resolve this problem, use some m_osgNeedsUpdate boolean which
    // gets set in your thread main and checked here or, as done in this module, by checking whether the callback is called the first time.
    if( m_module->m_aColor->changed() || m_initialUpdate )
    {
        // Set the diffuse color and material:
        osg::ref_ptr< osg::Material > mat = new osg::Material();
        mat->setDiffuse( osg::Material::FRONT, m_module->m_aColor->get( true ) );
        node->getOrCreateStateSet()->setAttribute( mat, osg::StateAttribute::ON );
    }
    traverse( node, nv );
}

void WMTemplate::TranslateCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    // Update the transformation matrix according to m_aPosition if it has changed.
    if( m_module->m_aPosition->changed() || m_initialUpdate )
    {
        // The node to which this callback has been attached needs to be an osg::MatrixTransform:
        osg::ref_ptr< osg::MatrixTransform > transform = static_cast< osg::MatrixTransform* >( node );

        // Build a translation matrix (to comfortably convert between WPosition and osg::Vec3 use the WVector3XXX methods)
        osg::Matrixd translate = osg::Matrixd::translate( m_module->m_aPosition->get( true  ).as< osg::Vec3d >() );

        // and set the translation matrix
        transform->setMatrix( translate );

        // First update done
        m_initialUpdate = false;
    }
    traverse( node, nv );
}

bool WMTemplate::StringLength::accept( boost::shared_ptr< WPropertyVariable< WPVBaseTypes::PV_STRING > > /* property */,
                                       const WPVBaseTypes::PV_STRING& value )
{
    // This method gets called every time the m_aString property is going to be changed. It can decide whether the new value is valid or not. If
    // the method returns true, the new value is set. If it returns false, the value is rejected.
    //
    // Note: always use WPVBaseTypes when specializing the WPropertyVariable template.

    // simple example: just accept string which are at least 5 chars long and at most 10.
    return ( value.length() <= 10 ) && ( value.length() >= 5 );
}

boost::shared_ptr< WPropertyVariable< WPVBaseTypes::PV_STRING >::PropertyConstraint > WMTemplate::StringLength::clone()
{
    // If you write your own constraints, you NEED to provide a clone function. It creates a new copied instance of the constraints with the
    // correct runtime type.
    return boost::shared_ptr< WPropertyVariable< WPVBaseTypes::PV_STRING >::PropertyConstraint >( new WMTemplate::StringLength( *this ) );
}

void WMTemplate::activate()
{
    // This method gets called, whenever the m_active property changes. Your module should always handle this if you do not use the
    // WGEManagedGroupNode for your scene. The user can (de-)activate modules in his GUI and you can handle this case here:
    if( m_active->get() )
    {
        debugLog() << "Activate.";
    }
    else
    {
        debugLog() << "Deactivate.";
    }

    // The simpler way is by using WGEManagedGroupNode which deactivates itself according to m_active. See moduleMain for details.

    // Always call WModule's activate!
    WModule::activate();
}

void WMTemplate::hideButtonPressed()
{
    // This method is called whenever m_hideButton changes its value. You can use such callbacks to avoid waking-up or disturbing the module
    // thread for certain operations.

    // If the button was triggered, switch the hide-state of m_aColor and m_aHiddenInt.
    if( m_hideButton->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
    {
        // switch the hide flag of the color prop.
        m_aColor->setHidden( !m_aColor->isHidden() );
        m_aHiddenInt->setHidden( !m_aHiddenInt->isHidden() );
        m_aHiddenGroup->setHidden( !m_aHiddenGroup->isHidden() );

        // never forget to reset a trigger. If not done, the trigger is disabled in the GUI and can't be used again.
        m_hideButton->set( WPVBaseTypes::PV_TRIGGER_READY );
        // NOTE: this again triggers an update, which is why we need to check the state of the trigger in this if-clause.
    }
}
