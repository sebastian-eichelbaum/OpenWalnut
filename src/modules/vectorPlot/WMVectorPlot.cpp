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

#include "WMVectorPlot.h"
#include "template.xpm"

WMVectorPlot::WMVectorPlot():
    WModule()
{
    // In the constructor, you can initialize your members and all this stuff. You must not initialize connectors or properties here! You also
    // should avoid doing computationally expensive stuff, since every module has its own thread which is intended to be used for such calculations.
    // Please keep in mind, that every member initialized here is also initialized in the prototype, which may be a problem if the member is large,
    // and therefore, wasting a lot of memory in your module's prototype instance.
}

WMVectorPlot::~WMVectorPlot()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMVectorPlot::factory() const
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
    return boost::shared_ptr< WModule >( new WMVectorPlot() );
}

const char** WMVectorPlot::getXPMIcon() const
{
    // The template_xpm char array comes from the template.xpm file as included above.
    // Such char arrays, i.e. files, can be easily created using an image manipulation program
    // like GIMP. Be aware that the xpm file is a simple header file. Thus it contains real
    // code. This code can be manipulated by hand. Unfortunately, you really have to fix the
    // xpm files produced by gimp. You need to make the char array const in order to prevent
    // compiler warnings or even errors.
    return template_xpm;
}

const std::string WMVectorPlot::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "VectorPlot";
}

const std::string WMVectorPlot::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    return "This module display vector data as small vector representation on navigation slice and other surfaces.";
}

void WMVectorPlot::connectors()
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
        new WModuleInputData< WDataSetSingle >( shared_from_this(), "in", "The dataset to display" )
        );

    // This creates an input connector which can receive WDataSetSingle. It will never be able to connect to output connectors providing just a
    // WDataSet (which is the father class of WDataSetSingle), but it will be able to be connected to an output connector with a type derived
    // from WDataSetSingle.

    // As properties, every connector needs to be added to the list of connectors.
    addConnector( m_input );

    // call WModules initialization
    WModule::connectors();
}

void WMVectorPlot::properties()
{
    // Every module can provide properties to the outside world. These properties can be changed by the user in the GUI or simply by other
    // modules using yours. Properties NEED to be created and added here. Doing this outside this function will lead to severe problems.
    //
    // Theoretically, you can specify properties of every type possible in C++. Therefore, see WPropertyVariable. But in most cases, the
    // predefined properties (WPropertyTypes.h) are enough, besides being the only properties shown and supported by the GUI.
    //
    // To create and add a new property, every module has a member m_properties. It is a set of properties this module provides to the outer
    // world. As with connectors, a property which not has been added to m_properties is not visible for others. Now, how to add a new property?

    m_propCondition  = boost::shared_ptr< WCondition >( new WCondition() );
    m_xPos           = m_properties2->addProperty( "X Pos of the slice", "Description.", 80 );
    m_yPos           = m_properties2->addProperty( "Y Pos of the slice", "Description.", 100 );
    m_zPos           = m_properties2->addProperty( "Z Pos of the slice", "Description.", 80 );

    m_xPos->setHidden( true );
    m_yPos->setHidden( true );
    m_zPos->setHidden( true );

    m_projectOnSlice = m_properties2->addProperty( "projection",           "Description.", false );
    m_coloringMode   = m_properties2->addProperty( "color mode",           "Description.", false );
    m_aColor         = m_properties2->addProperty( "A Color",                  "Description.", WColor( 1.0, 0.0, 0.0, 1.0 ) );

    // These lines create some new properties and add them to the property list of this module. The specific type to create is determined by the
    // initial value specified in the third argument. The first argument is the name of the property, which needs to be unique among all
    // properties of this module. The second argument is a description. A nice feature is the possibility to specify an own condition, which gets
    // fired when the property gets modified. This is especially useful to wake up the module's thread on property changes. So, the property
    // m_anInteger will wake the module thread on changes. m_enableFeature and m_aColor should not wake up the module thread. They get read by
    // the update callback of this modules OSG node, to update the color.

    // How can the values of the properties be changed? You can take a look at moduleMain where this is shown. For short: m_anInteger->set( 2 )
    // and m_anInteger->get().

    // Thats it. To set minimum and maximum value for a property the convenience methods setMin and setMax are defined. setMin and setMax are
    // allowed for all property types with defined <= and >= operator.
    m_xPos->setMin( 0 );
    m_xPos->setMax( 160 );
    m_yPos->setMin( 0 );
    m_yPos->setMax( 200 );
    m_zPos->setMin( 0 );
    m_zPos->setMax( 160 );

    // One last thing to mention is the active property. This property is available in all modules and represents the activation state of the
    // module. Int the GUI this is simply a checkbox beneath the module. The active property should be taken into account in ALL modules.
    // Visualization modules should turn off their graphics. There are basically three ways to react on changes in m_active, which is the member
    // variable for this property.
    // 1: overwrite WModule::activate() in your module
    // 2: register your own handler: m_active->getCondition()->subscribeSignal( boost::bind( &WMVectorPlot::myCustomActiveHandler, this ) );
    // 3: react during your module main loop using the moduleState: m_moduleState.add( m_active->getCondition );
    // Additionally, your can also use the m_active variable directly in your update callbacks to en-/disable some OSG nodes.
    // This template module uses method number 1. This might be the easiest and most commonly used way.
}

void WMVectorPlot::moduleMain()
{
    // This is the modules working thread. Its the most important part of your module.
    // When you enter this method, all connectors and properties the module provides are fixed. They get initialized in connectors() and
    // properties(). You always can assume the kernel, the GUI, the graphics engine and the data handler to be initialized and ready. Please keep
    // in mind, that this method is running in its own thread.

    // You can output log messages everywhere and everytime in your module. The WModule base class therefore provides debugLog, infoLog, warnLog
    // and errorLog. You can use them very similar to the common std::cout streams.
    debugLog() << "Entering moduleMain()";

    // Your module can notify everybody that it is ready to be used. The member function ready() does this for you. The ready state is especially
    // useful whenever your module needs to do long operations to initialize. No other module can connect to your module before it signals its
    // ready state. You can assume the code before ready() to be some kind of initialization code.

    // Your module can use an moduleState variable to wait for certain events. Most commonly, these events are new data on input connectors or
    // changed properties. You can decide which events the moduleState should handle. Therefore, use m_moduleState.add( ... ) to insert every
    // condition you want to wait on. As every input connector provides an changeCondition, we now add this condition to the moduleState:
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );

    // One note about "setResetable": It might happen, that a condition fires and your thread does not currently wait on it. This would mean,
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
        // After waking up, the module has to check whether the shutdownFlag fired. If yes, simply quit the module.

        // woke up since the module is requested to finish
        if ( m_shutdownFlag() )
        {
            break;
        }

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
        bool dataValid   = ( newDataSet );

        // To check validity of multiple inputs at once, you can also use dataChanged and dataValid:
        // bool dataChanged = ( m_dataSet != newDataSet ) || ( m_dataSet2 != newDataSet2 ) || ( m_dataSet3 != newDataSet3 );
        // bool dataValid   = newDataSet && newDataSet2 && newDataSet3;
        // This way, you can easily ensure that ALL your inputs are set and the module can do its job

        // now, copy the new data to out local member variables
        if ( dataChanged && dataValid )
        // this condition will become true whenever the new data is different from the current one or our actual data is NULL. This handles all
        // cases.
        {
            // The data is different. Copy it to our internal data variable:
            debugLog() << "Received Data.";
            m_dataSet = newDataSet;

            // For multiple inputs:
            // m_dataSet2 = newDataSet2;
            // m_dataSet3 = newDataSet3;
        }

        // Here we collect our properties. You, as with input connectors, always check if a property really has changed. You most probably do not
        // want to check properties which are used exclusively inside the update callback of your OSG node. As the properties are thread-safe, the
        // update callback can check them and apply it correctly to your visualization.
        //
        // To check whether a property changed, WPropertyVariable provides a changed() method which is true whenever the property has changed.
        // Please note: creating the property with addProperty( ... ) will set changed to true.

        // This example code now shows how to modify your OSG nodes basing on changes in your dataset or properties.
        // The if statement also checks for data validity as it uses the data! You should also always do that.
        if ( m_xPos->changed() || m_yPos->changed() || m_zPos->changed() )
        {
            debugLog() << "Creating new OSG node";

            // This block will be executed whenever we have a new dataset or the m_anInteger property has changed. This example codes produces
            // some shapes and replaces the existing root node by a new (updated) one. Therefore, a new root node is needed:
            osg::ref_ptr< osg::Geode > newRootNode = new osg::Geode();

            newRootNode->addDrawable( buildPlotSlices() );

            // The old root node needs to be removed safely. The OpenSceneGraph traverses the graph at every frame. This traversion is done in a
            // separate thread. Therefore, adding a Node directly may cause the OpenSceneGraph to crash. Thats why the Group node provided by
            // getScene offers safe remove and insert methods. Use them to manipulate the scene node.
            WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );

            m_rootNode = newRootNode;

            // please also ensure that, according to m_active, the node is active or not. Setting it here allows the user to deactivate modules
            // in project files for example.
            m_rootNode->setNodeMask( m_active->get() ? 0xFFFFFFFF : 0x0 );

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

osg::ref_ptr<osg::Geometry> WMVectorPlot::buildPlotSlices()
{
    // You should grab your values at the beginning of such calculation blocks, since the property might change at any time!
    wmath::WPosition current = WKernel::getRunningKernel()->getSelectionManager()->getCrosshair()->getPosition();

    int xPos = current[0];
    int yPos = current[1];
    int zPos = current[2];

    m_xPos->set( xPos );
    m_yPos->set( yPos );
    m_zPos->set( zPos );

    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( m_dataSet->getGrid() );
    boost::shared_ptr< WValueSet< float > > vals = boost::shared_dynamic_cast< WValueSet<float> >( m_dataSet->getValueSet() );

    m_xPos->setMax( grid->getNbCoordsX() );
    m_yPos->setMax( grid->getNbCoordsY() );
    m_zPos->setMax( grid->getNbCoordsZ() );


    // When working with the OpenSceneGraph, always use ref_ptr to store pointers to OSG objects. This allows OpenSceneGraph to manage
    // its resources automatically.

    using osg::ref_ptr;
    ref_ptr< osg::Vec3Array > vertices = ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    ref_ptr< osg::Vec4Array > colors = ref_ptr< osg::Vec4Array >( new osg::Vec4Array );
    ref_ptr< osg::Geometry > geometry = ref_ptr< osg::Geometry >( new osg::Geometry );

    WColor c = m_aColor->get( true );
    osg::Vec4 cc( c.getRed(), c.getGreen(), c.getBlue(), c.getAlpha() );

    if ( ( ( *m_dataSet ).getValueSet()->order() == 1 ) && ( ( *m_dataSet ).getValueSet()->dimension() == 3 )
            && ( ( *m_dataSet ).getValueSet()->getDataType() == 16 ) )
    {
        int maxX = m_xPos->getMax()->getMax();
        int maxY = m_yPos->getMax()->getMax();
        int maxZ = m_zPos->getMax()->getMax();

        for ( int x = 0; x < maxX; ++x )
        {
            for ( int y = 0; y < maxY; ++y )
            {
                float xx = vals->getScalar( ( x + y * maxX + zPos * maxX * maxY ) * 3 );
                float yy = vals->getScalar( ( x + y * maxX + zPos * maxX * maxY ) * 3 + 1 );
                float zz = vals->getScalar( ( x + y * maxX + zPos * maxX * maxY ) * 3 + 2 );

                if ( !m_projectOnSlice->get() )
                {
                    vertices->push_back( osg::Vec3( x + 0.5f, y + 0.5f, zPos + 0.5f ) );
                    vertices->push_back( osg::Vec3( x + 0.5f + xx, y + 0.5f + yy, zPos + 0.5f + zz ) );
                    if ( m_coloringMode->get() )
                    {
                        colors->push_back( osg::Vec4( fabs( xx ), fabs( yy ), fabs( zz ), 1.0 ) );
                        colors->push_back( osg::Vec4( fabs( xx ), fabs( yy ), fabs( zz ), 1.0 ) );
                    }
                    else
                    {
                        colors->push_back( cc );
                        colors->push_back( cc );
                    }
                }
                else
                {
                    vertices->push_back( osg::Vec3( x + 0.5f, y + 0.5f, zPos + 0.4f ) );
                    vertices->push_back( osg::Vec3( x + 0.5f + xx, y + 0.5f + yy, zPos + 0.4f ) );
                    vertices->push_back( osg::Vec3( x + 0.5f, y + 0.5f, zPos + 0.6f ) );
                    vertices->push_back( osg::Vec3( x + 0.5f + xx, y + 0.5f + yy, zPos + 0.6f ) );
                    if ( m_coloringMode->get() )
                    {
                        colors->push_back( osg::Vec4( fabs( xx ), fabs( yy ), fabs( zz ), 1.0 ) );
                        colors->push_back( osg::Vec4( fabs( xx ), fabs( yy ), fabs( zz ), 1.0 ) );
                        colors->push_back( osg::Vec4( fabs( xx ), fabs( yy ), fabs( zz ), 1.0 ) );
                        colors->push_back( osg::Vec4( fabs( xx ), fabs( yy ), fabs( zz ), 1.0 ) );
                    }
                    else
                    {
                        colors->push_back( cc );
                        colors->push_back( cc );
                        colors->push_back( cc );
                        colors->push_back( cc );
                    }
                }
            }
        }

        for ( int x = 0; x < maxX; ++x )
        {
            for ( int z = 0; z < maxZ; ++z )
            {
                float xx = vals->getScalar( ( x + yPos * maxX + z * maxX * maxY ) * 3 );
                float yy = vals->getScalar( ( x + yPos * maxX + z * maxX * maxY ) * 3 + 1 );
                float zz = vals->getScalar( ( x + yPos * maxX + z * maxX * maxY ) * 3 + 2 );

                if ( !m_projectOnSlice->get() )
                {
                    vertices->push_back( osg::Vec3( x + 0.5f, yPos + 0.5f, z + 0.5f ) );
                    vertices->push_back( osg::Vec3( x + 0.5f + xx, yPos + 0.5f + yy, z + 0.5f + zz ) );
                    if ( m_coloringMode->get() )
                    {
                        colors->push_back( osg::Vec4( fabs( xx ), fabs( yy ), fabs( zz ), 1.0 ) );
                        colors->push_back( osg::Vec4( fabs( xx ), fabs( yy ), fabs( zz ), 1.0 ) );
                    }
                    else
                    {
                        colors->push_back( cc );
                        colors->push_back( cc );
                    }
                }
                else
                {
                    vertices->push_back( osg::Vec3( x + 0.5f, yPos + 0.4f, z + 0.5f ) );
                    vertices->push_back( osg::Vec3( x + 0.5f + xx, yPos + 0.4f, z + 0.5f + zz ) );
                    vertices->push_back( osg::Vec3( x + 0.5f, yPos + 0.6f, z + 0.5f ) );
                    vertices->push_back( osg::Vec3( x + 0.5f + xx, yPos + 0.6f, z + 0.5f + zz ) );
                    if ( m_coloringMode->get() )
                    {
                        colors->push_back( osg::Vec4( fabs( xx ), fabs( yy ), fabs( zz ), 1.0 ) );
                        colors->push_back( osg::Vec4( fabs( xx ), fabs( yy ), fabs( zz ), 1.0 ) );
                        colors->push_back( osg::Vec4( fabs( xx ), fabs( yy ), fabs( zz ), 1.0 ) );
                        colors->push_back( osg::Vec4( fabs( xx ), fabs( yy ), fabs( zz ), 1.0 ) );
                    }
                    else
                    {
                        colors->push_back( cc );
                        colors->push_back( cc );
                        colors->push_back( cc );
                        colors->push_back( cc );
                    }
                }
            }
        }

        for ( int y = 0; y < maxY; ++y )
        {
            for ( int z = 0; z < maxZ; ++z )
            {
                float xx = vals->getScalar( ( xPos + y * maxX + z * maxX * maxY ) * 3 );
                float yy = vals->getScalar( ( xPos + y * maxX + z * maxX * maxY ) * 3 + 1 );
                float zz = vals->getScalar( ( xPos + y * maxX + z * maxX * maxY ) * 3 + 2 );

                if ( !m_projectOnSlice->get() )
                {
                    vertices->push_back( osg::Vec3( xPos + 0.5f, y + 0.5f, z + 0.5f ) );
                    vertices->push_back( osg::Vec3( xPos + 0.5f + xx, y + 0.5f + yy, z + 0.5f + zz ) );
                    if ( m_coloringMode->get() )
                    {
                        colors->push_back( osg::Vec4( fabs( xx ), fabs( yy ), fabs( zz ), 1.0 ) );
                        colors->push_back( osg::Vec4( fabs( xx ), fabs( yy ), fabs( zz ), 1.0 ) );
                    }
                    else
                    {
                        colors->push_back( cc );
                        colors->push_back( cc );
                    }
                }
                else
                {
                    vertices->push_back( osg::Vec3( xPos + 0.4f, y + 0.5f, z + 0.5f ) );
                    vertices->push_back( osg::Vec3( xPos + 0.4f, y + 0.5f + yy, z + 0.5f + zz ) );
                    vertices->push_back( osg::Vec3( xPos + 0.6f, y + 0.5f, z + 0.5f ) );
                    vertices->push_back( osg::Vec3( xPos + 0.6f, y + 0.5f + yy, z + 0.5f + zz ) );
                    if ( m_coloringMode->get() )
                    {
                        colors->push_back( osg::Vec4( fabs( xx ), fabs( yy ), fabs( zz ), 1.0 ) );
                        colors->push_back( osg::Vec4( fabs( xx ), fabs( yy ), fabs( zz ), 1.0 ) );
                        colors->push_back( osg::Vec4( fabs( xx ), fabs( yy ), fabs( zz ), 1.0 ) );
                        colors->push_back( osg::Vec4( fabs( xx ), fabs( yy ), fabs( zz ), 1.0 ) );
                    }
                    else
                    {
                        colors->push_back( cc );
                        colors->push_back( cc );
                        colors->push_back( cc );
                        colors->push_back( cc );
                    }
                }
            }
        }



        for ( size_t i = 0; i < vertices->size(); ++i )
        {
            osg::DrawElementsUInt* line = new osg::DrawElementsUInt( osg::PrimitiveSet::LINES, 0 );
            line->push_back( i );
            line->push_back( i + 1 );
            ++i;
            geometry->addPrimitiveSet( line );
        }
    }

    geometry->setVertexArray( vertices );
    geometry->setColorArray( colors );
    geometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );

    return geometry;
}


void WMVectorPlot::SafeUpdateCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    wmath::WPosition current = WKernel::getRunningKernel()->getSelectionManager()->getCrosshair()->getPosition();
    wmath::WPosition old( m_module->m_xPos->get(), m_module->m_yPos->get(), m_module->m_zPos->get() );

    if ( ( old != current ) || m_module->m_coloringMode->changed() || m_module->m_aColor->changed() || m_initialUpdate )
    {
        osg::ref_ptr<osg::Drawable> old = osg::ref_ptr<osg::Drawable>( m_module->m_rootNode->getDrawable( 0 ) );
        m_module->m_rootNode->removeDrawable( old );
        m_module->m_rootNode->addDrawable( m_module->buildPlotSlices() );
    }

    traverse( node, nv );
}

bool WMVectorPlot::StringLength::accept( boost::shared_ptr< WPropertyVariable< WPVBaseTypes::PV_STRING > > /* property */,
                                       WPVBaseTypes::PV_STRING value )
{
    // This method gets called everytime the m_aString property is going to be changed. It can decide whether the new value is valid or not. If
    // the method returns true, the new value is set. If it returns false, the value is rejected.
    //
    // Note: always use WPVBaseTypes when specializing the WPropertyVariable template.

    // simple example: just accept string which are at least 5 chars long and at most 10.
    return ( value.length() <= 10 ) && ( value.length() >= 5 );
}

void WMVectorPlot::activate()
{
    // This method gets called, whenever the m_active property changes. Your module should always handle this. For more details, see the
    // documentation in properties(). The most simple way is to activate or deactivate your OSG root node in this function according to
    // m_active's value. At the moment, we are not 100% sure whether deactivating a node, which is currently used, is thread-safe and complies to
    // OSG's requirements. Activating an inactive node is not the problem, as OSG does not traverse these nodes (and therefore could possibly
    // produce issues), but deactivating an active node, which might be traversed at the same time, COULD cause problems. We'll see in the future
    // whether this is problematic or not.

    if ( m_rootNode )   // always ensure the root node exists
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

    // Always call WModule's activate!
    WModule::activate();
}

