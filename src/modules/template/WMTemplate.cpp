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
    // initialize members
}

WMTemplate::~WMTemplate()
{
    // cleanup
}

boost::shared_ptr< WModule > WMTemplate::factory() const
{
    return boost::shared_ptr< WModule >( new WMTemplate() );
}

const std::string WMTemplate::getName() const
{
    // specify your module name here
    return "Template";
}

const std::string WMTemplate::getDescription() const
{
    // specify your module description here. Be detailed. This text is read by the user.
    return "This module is intended to be a module template and an example for writing modules.";
}

void WMTemplate::connectors()
{
    // initialize connectors
    m_input = boost::shared_ptr< WModuleInputData < WDataSetSingle  > >(
        new WModuleInputData< WDataSetSingle >( shared_from_this(),
                                                               "in", "The dataset to display" )
        );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_input );

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
        // want to check properties which are used exclusively inside the update callback of your OSG node. As the properties are thread-safe, the update
        // callback can check them and apply it correctly to your visualization.
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
            debugLog() << "Doing an operation not modifying the OSG node ... ";
            debugLog() << "m_anString: " << m_aString->get( true );
            debugLog() << "Current dataset: " << m_dataSet->getFileName() << " with name: " << m_dataSet->getName();
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

bool WMTemplate::StringLength::accept( boost::shared_ptr< WPropertyVariable< WPVBaseTypes::PV_STRING > > /* property */, WPVBaseTypes::PV_STRING value )
{
    // simple example: just accept string which are at least 5 chars long and at most 10.
    return ( value.length() <= 10 ) && ( value.length() >= 5 );
}

