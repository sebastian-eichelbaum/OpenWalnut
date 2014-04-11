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

#ifndef WQT4GUI_H
#define WQT4GUI_H

#include <string>
#include <vector>

#include <boost/program_options.hpp>
#include <boost/function.hpp>

#include <QtGui/QApplication>
#include <QtCore/QSettings>
#include <QtCore/QMutex>
#include <QtGui/QSplashScreen>

#include "core/graphicsEngine/WROI.h"
#include "core/graphicsEngine/WGraphicsEngine.h"

#include "core/kernel/WModule.h"
#include "core/common/WDefines.h"

#include "core/ui/WUI.h"

#include "events/WDeferredCallEvent.h"
#include "abstractUI/WUIQtWidgetFactory.h"

#include "WIconManager.h"

class WMainWindow;
class WKernel;

/**
 * The QT4 Based GUI implementation. Implements WUI.
 * \ingroup ui
 */
class WQt4Gui : public WUI
{
public:
    /**
     * Constructor.
     *
     * \param options the option-variable map
     * \param argc number of arguments given on command line.
     * \param argv arguments given on command line.
     */
    WQt4Gui( const boost::program_options::variables_map& options, int argc, char** argv );

    /**
     * Default destructor.
     */
    virtual ~WQt4Gui();

    /**
     * Runs the GUI. All initialization should be done here.
     *
     * \return the return code.
     */
    virtual int run();

    /**
     * returns a pointer to the selected module in the control panel
     *
     * \return the module
     */
    virtual boost::shared_ptr< WModule > getSelectedModule();

    /**
     * Slot gets called whenever we need to update the texture sorter.
     */
    virtual void slotUpdateTextureSorter();

    /**
     * Slot gets called whenever a new module is added.
     *
     * \param module the module to be added
     *
     * \note This can be used to add datasets or other modules.
     */
    virtual void slotAddDatasetOrModuleToTree( boost::shared_ptr< WModule > module );

    /**
     * Slot gets called whenever a new ROI is added.
     *
     * \param roi the ROI to be added
     */
    virtual void slotAddRoiToTree( osg::ref_ptr< WROI > roi );

    /**
     * Slot gets called whenever a ROI is removed.
     *
     * \param roi the ROI to be removed
     */
    virtual void slotRemoveRoiFromTree( osg::ref_ptr< WROI > roi );

    /**
     * Slot gets called whenever a module switches its state to "ready".
     *
     * \param module the module.
     */
    virtual void slotActivateDatasetOrModuleInTree( boost::shared_ptr< WModule > module );

    /**
     * Slot gets called whenever a module has been removed from the root container.
     *
     * \param module the module
     */
    virtual void slotRemoveDatasetOrModuleInTree( boost::shared_ptr< WModule > module );

    /**
     * Slot gets called whenever a connector pair got connected.
     *
     * \param in input connector
     * \param out output connector
     */
    virtual void slotConnectionEstablished( boost::shared_ptr<WModuleConnector> in, boost::shared_ptr<WModuleConnector> out );

    /**
     * Slot gets called whenever a connector pair got disconnected.
     *
     * \param in input connector
     * \param out output connector
     */
    virtual void slotConnectionClosed( boost::shared_ptr<WModuleConnector> in, boost::shared_ptr<WModuleConnector> out );

    /**
     * getter functions for the signales proved by the gui
     */
    boost::signals2::signal1< void, std::vector< std::string > >* getLoadButtonSignal();

    /**
     * Returns the current main window instance or NULL if not existent.
     *
     * \return the main window instance.
     */
    static WMainWindow* getMainWindow();

    /**
     * Returns the settings object.
     *
     * \return settings object.
     */
    static QSettings& getSettings();

    /**
     * Get the icon manager of this gui instance.
     *
     * \return the icon manager.
     */
    static WIconManager* getIconManager();

    /**
     * Returns the option map for the current instance of this GUI. This can be useful to parse further commandline parameters
     *
     * \return the option map
     */
    const boost::program_options::variables_map& getOptionMap() const;

    /**
     * Returns the widget factory of the UI.
     *
     * \return the factory. Use this to create your widget instances.
     */
    virtual WUIWidgetFactory::SPtr getWidgetFactory() const;

    /**
     * Call a given function from within the GUI thread. The function posts an event and waits for its execution.
     *
     * \param functor the function to call (you can implement structs with operator() too if you need parameters and return values)
     * \param notify specify your own condition to wait for. This is needed since the QApplication doc tells us that ownership of an event is
     * handed over to QT and that it is not save to use the event after posting it. This means we cannot utilize an internal condition in the
     * event as it might be deleted already when calling wait() on it. Do not specify this variable to get a fire-and-forget call (but still the
     * method blocks until execution).
     */
    static void execInGUIThread( boost::function< void( void ) > functor, WCondition::SPtr notify = WCondition::SPtr() );

    /**
     * Call a given function from within the GUI thread. The function posts an event and waits for its execution.
     *
     * \param functor the function to call (you can implement structs with operator() too if you need parameters and return values)
     * \param notify specify your own condition to wait for. This is needed since the QApplication doc tells us that ownership of an event is
     * handed over to QT and that it is not save to use the event after posting it. This means we cannot utilize an internal condition in the
     * event as it might be deleted already when calling wait() on it. Do not specify this variable to get a fire-and-forget call (but still the
     * method blocks until execution).
     *
     * \return the result
     * \tparam Result the type of the function return value.
     */
    template< typename Result >
    static Result execInGUIThread( boost::function< Result( void ) > functor, WCondition::SPtr notify = WCondition::SPtr() )
    {
        if( !notify )
        {
            // the user did not specify a condition. We create our own
            notify = WCondition::SPtr( new WConditionOneShot() );
        }
        Result result; // stores result
        WDeferredCallResultEvent< Result >* ev = new WDeferredCallResultEvent< Result >( functor, &result, notify );
        QCoreApplication::postEvent( getMainWindow(), ev );
        notify->wait();
        return result;
    }

    /**
     * Call a given function from within the GUI thread. The function posts an event and DOES NOT wait for its execution.
     *
     * \param functor the function to call (you can implement structs with operator() too if you need parameters and return values)
     * \param notify specify your own condition to wait for. This is needed since the QApplication doc tells us that ownership of an event is
     * handed over to QT and that it is not save to use the event after posting it. This means we cannot utilize an internal condition in the
     * event as it might be deleted already when calling wait() on it. Do not specify this variable to get a fire-and-forget call.
     */
    static void execInGUIThreadAsync( boost::function< void( void ) > functor, WCondition::SPtr notify = WCondition::SPtr() );

protected:
    /**
     * Called whenever a module crashes.
     *
     * \param module the module which has thrown the exception
     * \param exception the exception the module has thrown
     */
    void moduleError( boost::shared_ptr< WModule > module, const WException& exception );

private:
    /**
     * Object storing certain persistent application settings.
     */
    static QSettings* m_settings;

    /**
     * Main window containing all needed widgets.
     */
    static WMainWindow* m_mainWindow;

    /**
     * Graphics Engine instance.
     */
    boost::shared_ptr< WGraphicsEngine > m_ge;

    /**
     * The connection to the AddLog signal of the logger
     */
    boost::signals2::connection m_loggerConnection;

    /**
     * Kernel instance.
     */
    boost::shared_ptr< WKernel > m_kernel;

    const boost::program_options::variables_map& m_optionsMap; //!< Map storing the program options.

    /**
     * If true, the next trigger of deferredLoad will actually do loading. This variable and the deferredLoad function are protected
     * with m_deferredLoadMutex.
     */
    bool m_loadDeferredOnce;

    /**
     * This mutex protects the deferredLoad method from being called in parallel or twice.
     */
    QMutex m_deferredLoadMutex;

    /**
     * New log item added. Pushing event to QT's event queue.
     *
     * \param entry the entry added.
     */
    void slotAddLog( const WLogEntry& entry );

    /**
     * This is called by the GE when the osg was set-up correctly. This triggers project and data file loading.
     *
     * \note can be called from an arbitrary thread. Protected by m_deferredLoadMutex.
     */
    void deferredLoad();

    /**
     * The splash screen.
     */
    QSplashScreen* m_splash;

    /**
     * The widget factory which handles WUI widget creation.
     */
    WUIQtWidgetFactory::SPtr m_widgetFactory;
};

#endif  // WQT4GUI_H

