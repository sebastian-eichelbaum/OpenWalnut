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

#include <QtCore/QSettings>
#include <QtCore/QMutex>

#include "core/graphicsEngine/WROI.h"
#include "core/graphicsEngine/WGraphicsEngine.h"

#include "core/kernel/WModule.h"

#include "core/gui/WGUI.h"

class WMainWindow;
class WKernel;

/**
 * The QT4 Based GUI implementation. Implements WGUI.
 * \ingroup gui
 */
class WQt4Gui : public WGUI
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
     * Slot gets called whenever a new roi is added.
     *
     * \param roi the roi to be added
     */
    virtual void slotAddRoiToTree( osg::ref_ptr< WROI > roi );

    /**
     * Slot gets called whenever a roi is removed.
     *
     * \param roi the roi to be removed
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
     * Instruct to open a new custom widget. The specified condition should be the shutdown condition of the module, as the function returns only
     * if the widget was created. To ensure that the creation is aborted properly if the module shuts down in the meantime, this condition is
     * used.
     *
     * \note this function blocks until the widget was created. Check the resulting pointer for NULL.
     *
     * \param title the title of the widget
     * \param projectionMode the kind of projection which should be used
     * \param shutdownCondition a condition enforcing abort of widget creation.
     *
     * \return the created widget
     */
    virtual WCustomWidget::SPtr openCustomWidget( std::string title, WGECamera::ProjectionMode projectionMode,
        boost::shared_ptr< WCondition > shutdownCondition );

    /**
     * Instruct the WMainWindow to close a custom widget.
     *
     * \param title The title of the widget
     */
    virtual void closeCustomWidget( std::string title );

    /**
     * Instruct the WMainWindow to close a custom widget.
     *
     * \param widget the widget to close
     */
    virtual void closeCustomWidget( WCustomWidget::SPtr widget );

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
     * Returns the option map for the current instance of this GUI. This can be useful to parse further commandline parameters
     *
     * \return the option map
     */
    const boost::program_options::variables_map& getOptionMap() const;

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
};

#endif  // WQT4GUI_H

