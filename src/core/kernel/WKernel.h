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

#ifndef WKERNEL_H
#define WKERNEL_H

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "../common/WTimer.h"
#include "../common/WLogger.h"
#include "../graphicsEngine/WGraphicsEngine.h"
#include "WExportKernel.h"
#include "WModule.h"

// forward declarations
class WGUI;
class WModuleContainer;
class WModuleFactory;
class WROIManager;
class WSelectionManager;
class WThreadedRunner;

/**
 * \defgroup kernel Kernel
 *
 * \brief
 * This library implements the central part of OpenWalnut that manages
 * the interaction between GUI, GraphicsEngine and DataHandler.
 */

/**
 * OpenWalnut kernel, managing modules and interaction between
 * GUI, GE and DataHandler
 * \ingroup kernel
 */
class OWKERNEL_EXPORT WKernel: public WThreadedRunner
{
public:

    /**
     * Returns pointer to the running kernel or a new if no kernel was there.
     * If a running kernel exists the function return it and does not check if
     * ge and gui of the running kernel are equivalent to the ones given as parameters.
     *
     * \param ge initialized graphics engine.
     * \param gui initialized gui.
     * \return the kernel instance.
     */
    static WKernel* instance( boost::shared_ptr< WGraphicsEngine > ge, boost::shared_ptr< WGUI > gui );

    /**
     * Destructor.
     */
    virtual ~WKernel();

    /**
     * Stops execution of the modules in the root container. Note that this does not wait for the kernel thread since this could
     * cause a dead lock. This is actually an alias for getRootContainer()->stop().
     */
    void finalize();

    /**
     * Returns pointer to currently running instance of graphics engine.
     *
     * \return the graphics engine instance.
     */
    boost::shared_ptr< WGraphicsEngine > getGraphicsEngine() const;

    /**
     * Returns pointer to the currently running kernel.
     *
     * \return the kernel instance.
     */
    static WKernel* getRunningKernel();

    /**
     * Determines whether all threads should finish.
     *
     * \return true if so.
     */
    const WBoolFlag& isFinishRequested() const;

    /**
     * Load specified datasets. It immediately returns and starts another thread, which actually loads the data.
     *
     * \param fileNames list of filenames to load. The registered notification handler for the root container will get notified on
     * error and success.
     */
    void loadDataSets( std::vector< std::string > fileNames );

    /**
     * Loads the specified files synchronously.
     *
     * \param fileNames list of filenames to load. The registered notification handler for the root container will get notified on
     * error and success.
     */
    void loadDataSetsSynchronously( std::vector< std::string > fileNames );

    /**
     * Function combines to modules. This is a simple alias for "getRootContainer()->applyModule". It runs synchronously, which
     * could freeze the calling thread for a couple of time.
     *
     * \param applyOn the module which already has to be in the container and to apply the other one on.
     * \param prototype the prototype of the module to apply on the other one specified.
     *
     * \return the newly created module connected with the one specified in applyOn.
     */
    boost::shared_ptr< WModule > applyModule( boost::shared_ptr< WModule > applyOn, boost::shared_ptr< WModule > prototype );

    /**
     * Returns the root module container. This is the actual module graph container.
     *
     * \return the root container.
     */
    boost::shared_ptr< WModuleContainer > getRootContainer() const;

    /**
     * Getter for the associated GUI.
     *
     * \return the GUI.
     */
    boost::shared_ptr< WGUI > getGui() const;

    /**
     * get for roi manager
     *
     * \return Pointer to the ROI manager.
     */
    boost::shared_ptr< WROIManager> getRoiManager();

    /**
     * get for selection manager
     *
     * \return Pointer to the selection manager.
     */
    boost::shared_ptr< WSelectionManager> getSelectionManager();

    /**
     * Returns the system timer. If you need timing for animations and similar, use this one. This timer can change to frame based timing if the
     * user plays back some animation. So, everything which uses this timer can always do accurate per-frame animations even if frame time and
     * real-time differ.
     *
     * \return the timer
     */
    WTimer::ConstSPtr getTimer() const;

protected:
    /**
     * Constructor is protected because this class is a singleton. Awaits an INITIALIZED graphics engine an gui.
     *
     * \param ge initialized graphics engine.
     * \param gui initialized gui.
     */
    WKernel( boost::shared_ptr< WGraphicsEngine > ge, boost::shared_ptr< WGUI > gui );

    /**
     * Function that has to be overwritten for execution. It gets executed in a separate thread after run()
     * has been called.
     */
    virtual void threadMain();

    /**
     * The Gui.
     */
    boost::shared_ptr< WGUI > m_gui;

    /**
     * Pointer to an initialized graphics engine.
     */
    boost::shared_ptr< WGraphicsEngine > m_graphicsEngine;

    /**
     * Pointer to a roi manager
     */
    boost::shared_ptr< WROIManager >m_roiManager;

    /**
     * pointer to a selection manager
     */
    boost::shared_ptr< WSelectionManager >m_selectionManager;

    /**
     * The module factory to use.
     */
    boost::shared_ptr< WModuleFactory > m_moduleFactory;

    /**
     * The container containing the modules.
     */
    boost::shared_ptr< WModuleContainer > m_moduleContainer;

private:
    /**
     * Loads all the modules it can find.
     */
    void loadModules();

    /**
     * Initializes the graphics engine, data handler and so on.
     */
    void init();

    /**
     * Pointer to the unique instance of this singleton class.
     */
    static WKernel* m_kernel;

    /**
     * The ow system timer.
     */
    WTimer::SPtr m_timer;
};

#endif  // WKERNEL_H

