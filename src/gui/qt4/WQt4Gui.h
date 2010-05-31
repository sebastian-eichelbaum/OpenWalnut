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

#include "../../kernel/WModule.h"
#include "../../modules/fiberDisplay/WRMROIRepresentation.h"
#include "../WGUI.h"

class WMainWindow;
class WGraphicsEngine;
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
     * \param argc number of arguments given on command line.
     * \param argv arguments given on command line.
     */
    WQt4Gui( int argc, char** argv );

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
     * returns a pointer to the selected module in the dataset browser
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
    virtual void slotAddDatasetOrModuleToBrowser( boost::shared_ptr< WModule > module );

    /**
     * Slot gets called whenever a new roi is added.
     *
     * \param roi the roi to be added
     */
    virtual void slotAddRoiToBrowser( boost::shared_ptr< WRMROIRepresentation > roi );

    /**
     * Slot gets called whenever a roi is removed.
     *
     * \param roi the roi to be removed
     */
    virtual void slotRemoveRoiFromBrowser( boost::shared_ptr< WRMROIRepresentation > roi );

    /**
     * Slot gets called whenever a module switches its state to "ready".
     *
     * \param module the module.
     */
    virtual void slotActivateDatasetOrModuleInBrowser( boost::shared_ptr< WModule > module );

    /**
     * Slot gets called whenever a module has been removed from the root container.
     *
     * \param module the module
     */
    virtual void slotRemoveDatasetOrModuleInBrowser( boost::shared_ptr< WModule > module );

    /**
     * getter functions for the signales proved by the gui
     */
    boost::signals2::signal1< void, std::vector< std::string > >* getLoadButtonSignal();

    /**
     * Instruct the WMainWindow to open a new custom widget.
     *
     * \param title the title of the widget
     * \param projectionMode the kind of projection which should be used
     * \param shutdownCondition condition to wait for the shutdown of a module
     * \return the created widget
     */
    virtual boost::shared_ptr< WCustomWidget > openCustomWidget( std::string title, WGECamera::ProjectionMode projectionMode,
        boost::shared_ptr< WCondition > shutdownCondition );

    /**
     * Instruct the WMainWindow to close a custom widget.
     *
     * \param title The title of the widget
     */
    virtual void closeCustomWidget( std::string title );

    /**
     * Returns the current main window instance or NULL if not existent.
     *
     * \return the main window instance.
     */
    static WMainWindow* getMainWindow();

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
     * Main window containing all needed widgets.
     */
    static WMainWindow* m_mainWindow;

    /**
     * Graphics Engine instance.
     */
    boost::shared_ptr< WGraphicsEngine > m_ge;

    /**
     * Kernel instance.
     */
    boost::shared_ptr< WKernel > m_kernel;

    boost::program_options::variables_map m_optionsMap; //!< Map storing the program options.

    /**
     * This function defines and parses the valid command line options.
     * This might once be put in a separate class like WOptionHandler.
     * At the moment it seems reasonable that different GUIs might have
     * different command line options, thus we implement their parsing
     * in the GUI implemntation itself, i.e. here.
     */
    bool parseOptions();
};

#endif  // WQT4GUI_H

