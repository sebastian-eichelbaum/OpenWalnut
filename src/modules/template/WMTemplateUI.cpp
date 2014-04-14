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
// This is a tutorial on how to create custom views and widgets from within your modules.
//
// If you want to learn how to program a module, refer to WMTemplate.cpp. It is an extensive tutorial on all the details.
// In this tutorial, we assume you already know how to write modules.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <string>

#include <boost/bind.hpp>

#include "core/kernel/WKernel.h"
#include "core/common/WPathHelper.h"
#include "core/graphicsEngine/WGERequirement.h"
#include "core/ui/WUI.h"

#include "WMTemplateUI.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// All the basic setup ...
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WMTemplateUI::WMTemplateUI()
    : WModule()
{
}

WMTemplateUI::~WMTemplateUI()
{
}

boost::shared_ptr< WModule > WMTemplateUI::factory() const
{
    return boost::shared_ptr< WModule >( new WMTemplateUI() );
}

const std::string WMTemplateUI::getName() const
{
    return "Template UI";
}

const std::string WMTemplateUI::getDescription() const
{
    return "Show some custom module UI.";
}

void WMTemplateUI::connectors()
{
    // We do not need any connectors. Have a look at WMTemplate.cpp if you want to know what this means.
    WModule::connectors();
}

void WMTemplateUI::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    // We create some dummy preferences here to use in our widgets:
    m_properties->addProperty( "Do it now!",               "Trigger Button Text.", WPVBaseTypes::PV_TRIGGER_READY );
    m_properties->addProperty( "Enable feature",           "Description.", true );
    m_properties->addProperty( "Number of shape rows",     "Number of shape rows.", 10 );
    m_properties->addProperty( "CLONE!Number of shape rows",
                                "A property which gets modified if \"Number of shape rows\" gets modified.", 10 );
    m_properties->addProperty( "Shape radii",              "Shape radii.", 20.0 );
    m_properties->addProperty( "A string",                 "Something.", std::string( "hello" ) );
    m_properties->addProperty( "A filename",               "Description.", WPathHelper::getAppPath() );
    m_properties->addProperty( "A color",                  "Description.", WColor( 1.0, 0.0, 0.0, 1.0 ) );
    m_properties->addProperty( "Somewhere",                "Description.", WPosition( 0.0, 0.0, 0.0 ) );

    WModule::properties();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ATTENTION: now it gets interesting ...
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void WMTemplateUI::requirements()
{
    // We need graphics to draw anything:
    m_requirements.push_back( new WGERequirement() );
    // This method is already known from the WMTemplate example. But there is one difference:
    m_requirements.push_back( new WUIRequirement() );
    // This tells OpenWalnut, that you need the WUI features. One might assume that this is self-evident but it is not.
    // What if you use a python based interface to OpenWalnut? If you embed the OpenWalnut library in another tool
    // without GUI? The requirement ensures that your module can only be used with a working GUI.
}

void WMTemplateUI::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_propCondition );

    // Now lets create some widgets. In OpenWalnut, all widgets need to be created using a factory. We can get a factory instance from the
    // currently running GUI.
    WUIWidgetFactory::SPtr factory = WKernel::getRunningKernel()->getUI()->getWidgetFactory();
    // The factory works thread-safe. This means, you can ask for widgets from everywhere and any time. Once the create*** function returns, the
    // widget is visible to the user. In other words, the factory blocks until creation was completed.

    // Lets create the top-level widget first. The first created widget cannot be nested into other widgets. In the Qt4 gui we deliver along
    // the OpenWalnut core library, the top-level widget will be a dock window.
    WUIGridWidget::SPtr widgetGrid = factory->createGridWidget(
        getName() // All widgets require a title. No need for unique names.
    );
    // Thats it. Now we have a widget which itself is empty. It provides simple grid functionality and allows nesting other widgets.

    // One important thing to remember about titles. Titles are not required to be unique. But ...
    //  1) You should always add your module name to the title of top-level widgets (getName()). This ensures that the user can distinguish
    //     between different views.
    //  2) You should ensure unique names inside your own set of modules, at least at the top-level. This might be important to the GUI
    //     to differentiate your widgets and restore their previous state properly.

    // Lets create a second top-level widget. This time, it is a WUIViewWidget. This basically is an OpenGL render area to be filled by OSG.
    // Creating view widgets is a bit more complex as the view needs some parameters:
    WUIViewWidget::SPtr widgetView = factory->createViewWidget(
        getName() + " - Standalone View",           // All widgets require a title. Module-wide uniqueness recommended but not needed.
        WGECamera::TWO_D,                           // Specify an initial camera mode.
        m_shutdownFlag.getValueChangeCondition()    // define a condition that, when notified, aborts creation of the view
    );
    // Again, the same scheme as above. But there is something interesting here:
    //  *  The name of the viewer. In OpenWalnut, the name of a viewer needs to be unique. If your name is not unique, you will get a
    //     WNameNotUnique exception. So ensure you use some unique name here. Such a name can be created as above. As the module name (via
    //     getName() ) is ensured to be unique, you can add some view identifier (MainView in our case) and an instance number to ensure a unique
    //     string. The instance number is useful to ensure that the string still is unique if you start your module multiple times.

    // Let us summarize: we have two top-level widgets. This means, two separate windows, docks whatever (depending on GUI implementation). The
    // creation is always done using a factory we get from the GUI.

    // Lets go on and fill the grid. Here is the plan:
    // We divide the grid in a left and right part.
    //  * The left part will contain two tabbed OSG view widgets.
    //  * The right part contains two vertically placed OSG view widgets and a widget showing our module properties.

    // Start on the left side. Create a WUITabbedWidget. This widget itself is empty. Similar to WUIGridWidget. It is only useful in conjunction
    // with other widgets embedded into the tabs. But first things first -> create the widget:
    WUITabbedWidget::SPtr widgetGridLeftTabs = factory->createTabbedWidget(
        "Left Side Tabs", // A title as it is required.
        widgetGrid        // specify the parent widget which will contain this WUITabbedWidget.
    );
    // This is our first non-top-level widget. This means we want to embed it into another widget. To achieve this, we HAVE TO specify the widget
    // which will contain this one: called the parent widget. The WUI interface forces us to specify a parent if a widget will be embedded. If
    // you not specify a parent, you get a top-level widget. Re-parenting (moving a widget to another parent widget) is not possible.

    // Now we can create the tabbed view widgets. We do this after the WUITabbedWidget creation because? ... we need a parent for the views!
    WUIViewWidget::SPtr widgetGridLeftTabbedView1 = factory->createViewWidget(
        getName() + " - Left Tabbed 1",
        WGECamera::TWO_D,
        m_shutdownFlag.getValueChangeCondition(),
        widgetGridLeftTabs                          // we want to embed the view into the tabbed widget -> so specify it as parent
    );
    WUIViewWidget::SPtr widgetGridLeftTabbedView2 = factory->createViewWidget(
        getName() + " - Left Tabbed 2",
        WGECamera::TWO_D,
        m_shutdownFlag.getValueChangeCondition(),
        widgetGridLeftTabs                          // we want to embed the view into the tabbed widget -> so specify it as parent
    );

    // At this point, we have two views as children of widgetGridLeftTabs. We need to specify which tabs both views will be in:
    int widgetGridLeftTabbedView1Index = widgetGridLeftTabs->addTab(
            widgetGridLeftTabbedView1,  // The widget to place. This throws an exception if widgetGridLeftTabs is not the parent of
                                        // widgetGridLeftTabbedView1!
            "View Widget 1"             // The tab label
    );
    int widgetGridLeftTabbedView2Index = widgetGridLeftTabs->addTab( widgetGridLeftTabbedView2, "View Widget 2" );
    debugLog() << "Index 1 = " << widgetGridLeftTabbedView1Index << " -- Index 2 = " << widgetGridLeftTabbedView2Index;
    // This adds the views to two tabs. The values returned are the indices of the tabs. They might come in handy later, when modifying the tabs.
    // You could also have added another grid! Again, remember that container widgets (like WUIGridWidget and WUITabbedWidget) can be nested into
    // each other too. This gives you a max flexibility for placing widgets.
    // This being said, we forgot to place the WUITabbedWidget into our top-level grid widget:
    widgetGrid->placeWidget(
        widgetGridLeftTabs,  // The widget to place. This throws an exception if widgetGrid is not the parent of widgetGridLeftTabs!
        0,                   // row coordinate in grid; 0 is top
        0                    // column coordinate in grid; 0 is left
    );

    // Now we want to fill the right side of the grid. Please remember that we want to align 2 views and a property widget vertically. To achieve
    // this, we create a nested grid inside the right-grid cell of widgetGrid -> this can be seen as row/column spanning known from several GUI
    // frameworks like Qt
    WUIGridWidget::SPtr widgetGridRightGrid = factory->createGridWidget(
        "Right Side Grid",
        widgetGrid            // Remember: we need this to be our parent or else we are not able to add this new grid into the top-level grid.
    );
    // As done above, we need to place the widgets at the right position:
    widgetGrid->placeWidget(
        widgetGridRightGrid,  // The widget to place.
        0,                    // row coordinate in grid; 0 is top
        1                     // column coordinate in grid; 0 is left
    );

    // we not populate the grid with additional views:
    WUIViewWidget::SPtr widgetGridRightVerticalView1 = factory->createViewWidget(
        getName() + " - Right Vertical View 1",
        WGECamera::TWO_D,
        m_shutdownFlag.getValueChangeCondition(),
        widgetGridRightGrid                          // NEVER forget the parent if you want to nest the widgets!
    );
    WUIViewWidget::SPtr widgetGridRightVerticalView2 = factory->createViewWidget(
        getName() + " - Right Vertical View 2",
        WGECamera::TWO_D,
        m_shutdownFlag.getValueChangeCondition(),
        widgetGridRightGrid                          // And once again: NEVER forget the parent if you want to nest the widgets!
    );

    // As done above, we need to place the widgets at the right position:
    widgetGridRightGrid->placeWidget(
        widgetGridRightVerticalView1,  // The widget to place.
        0,                             // row coordinate in grid; 0 is top
        0                              // column coordinate in grid; 0 is left
    );
    widgetGridRightGrid->placeWidget(
        widgetGridRightVerticalView2,  // The widget to place.
        1,                             // row coordinate in grid; 0 is top
        0                              // column coordinate in grid; 0 is left
    );

    // Finally, we add a new property widget. These widgets show the given property group. This can be very handy to provide some additional
    // controls to your view. In our case, we are lazy and add our own m_properties group (the module properties). Same creation procedure:
    WUIPropertyGroupWidget::SPtr widgetRightProps = factory->createPropertyGroupWidget(
        "Control Properties",          // Name. As this is not a top-level widget, we can leave getName() out.
        m_properties,                  // The properties to show.
        widgetGridRightGrid            // The parent for this widget. We want to add it to the right, vertical grid.
    );

    // ... and place the widget:
    widgetGridRightGrid->placeWidget(
        widgetRightProps,  // The widget to place.
        3,                 // row coordinate in grid; 0 is top
        0                  // column coordinate in grid; 0 is left
    );

    // We are done. The created several widgets and nested them into others. And it is awesomely easy, isn't it? The scheme is always the same.
    // First, create the widgets. Second, place them inside the parent.

    // We can now modify our widgets. The WUI***Widget interface is thread-safe and you can modify your widgets without having to take care about
    // threads and stuff. Please read the documentation inside the WUI*****Widget classes to find out which further widget modifications and
    // tools are available.
    // Here is an example: we want the right side to be smaller than the left side. We can define stretch factors for this. This idea was taken
    // from Qt initially.
    widgetGrid->setColumnStretch(
        0,  // the first column (the left one)
        2   // set a stretch factor. What does it mean? Please go on reading.
    );
    widgetGrid->setColumnStretch( 1, 1 );   // the same for the second column
    // Now we can describe the meaning of the stretch factor. The factor you define is the fraction of space relative to the sum of all stretch
    // factors.

    // Let us summarize: we now have created several widgets. By creating and nesting them. But now one question arises: how to interact with the
    // widgets?
    //  * WUIPropertyGroupWidget: via its properties of course
    //  * WUIViewWidget: via WUIViewEventHandler and OSG!

    // Here is a very simple view event handler for one of the view widgets. Event handlers are used to be notified about certain events in your
    // view widget, like clicks, mouse movement or the resize event. For a complete list, please refer to the doc in core/ui/WUIViewEventHandler.h.
    //
    // Create the event handler:
    osg::ref_ptr< WUIViewEventHandler > eh = new WUIViewEventHandler( widgetView );

    // To get notified about events, there are two possible solutions:
    //  1) derive from WUIViewEventHandler (not shown here)
    //  2) subscribe to each event:
    eh->subscribeMove( boost::bind( &WMTemplateUI::handleMouseMove, this, _1 ) );
    eh->subscribeResize( boost::bind( &WMTemplateUI::handleResize , this, _1, _2, _3, _4 ) );
    // This binds our member functions handleMouseMove and handleResize to the event handler.
    // Finally, we add the event handler:
    widgetView->addEventHandler( eh );
    // If you now move your mouse around in the view, you will get a lots of debug output.

    // You can now fill your view with OSG nodes. This is done as shown in WMTemplate. Each WUIViewWidget can deliver its WGEViewer.
    // The viewer than allows you to add nodes to the scene, modify camera and similar
    // widgetView->getViewer()->getScene()->insert( myNode );
    // We want cool backgrounds and stuff for our first view widget. Please note that these settings are defaults only. The user might have
    // overwritten them.
    widgetView->getViewer()->setEffectsActiveDefault();

    // Ok. Now we have a whole bunch of modules, made an event handler and set some default effects. Is there more? Yes! In the next view code
    // lines you will learn how to customize and add "actions" to your widget.
    // TODO(ebaum) write

    // Finally, mark the module as ready.
    //
    // ... But wait. Something to keep in mind here. If you remember from WMTemplate, the project file loader in OpenWalnut FIRST initializes the
    // modules, waits for their ready() signal, and THEN sets the properties from the project file. This means, setting ready() after widget
    // creation allows you to utilize the properties mechanism with WUIViewerWidget::getViewer()::getProperties to save the configuration of a
    // certain view if you like. If you consider to NOT save a view's config, please disable the view configuration action as shown above. The
    // user should never be able to configure the view if he looses his settings all the time.
    //
    // Here is an example how:
    WPropGroup viewProps = m_properties->addPropertyGroup( "Hidden View Properties", "View properties." );
    viewProps->setHidden(); // we do not want to show these to the user in our module control panel.
    viewProps->addProperty( widgetView->getViewer()->getProperties() );

    // Now, we can mark the module ready.
    ready();

    // Now the remaining module code. In our case, this is empty.
    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();
        if( m_shutdownFlag() )
        {
            break;
        }
    }

    // Never miss to clean up. If you miss this step, the shared_ptr might get deleted -> GUI crash
    debugLog() << "Shutting down ...";
    widgetGrid->close();
    widgetView->close();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Event Handler
//  These functions demonstrate how to interact with WUIViewWidgets. Have a look at core/ui/WUIViewEventHandler.h for a list of the possible
//  handler functions
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void WMTemplateUI::handleMouseMove( WVector2f pos )
{
    // This function just prints the mouse position.
    debugLog() << "MOUSE MOVED: " << pos;
}

void WMTemplateUI::handleResize( int /* x */, int /* y */, int width, int height )
{
    // This function just prints the current view size.
    debugLog() << "RESIZE: " << width << ", " << height;
}
