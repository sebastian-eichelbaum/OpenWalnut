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

#include <list>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "WModule.h"
#include "WModuleFactory.h"
#include "WModuleContainer.h"
#include "../common/WLogger.h"
#include "../common/WThreadedRunner.h"
#include "../common/WFlag.h"
#include "../graphicsEngine/WGraphicsEngine.h"
#include "../gui/WGUI.h"
#include "../modules/fiberDisplay2/WROIManagerFibers.h"

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
class WKernel: public WThreadedRunner
{
public:

    /**
     * Constructor. Awaits an INITIALIZED graphics engine an gui.
     *
     * \param ge initialized graphics engine.
     * \param gui initialized gui.
     */
    WKernel( boost::shared_ptr< WGraphicsEngine > ge, boost::shared_ptr< WGUI > gui );

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
     * Function combines to modules. This is a simple alias for "getRootContainer()->applyModule".
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
     * get for application path
     */
    static std::string getAppPath();

    /**
     * getter for shader path
     */
    static std::string getShaderPath();


    /**
     * get for roi manager
     */
    boost::shared_ptr< WROIManagerFibers>getRoiManager();


    /**
     * getter for font path
     */
    static std::string getFontPath();


protected:

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
    boost::shared_ptr< WROIManagerFibers >m_roiManager;

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
     * Determine and store the application path
     */
    static void findAppPath();

    /**
     * The location of the openwalnut executable.
     */
    static std::string m_appPath;

    /**
     * The location of the shaders.
     */
    static std::string m_shaderPath;

   /**
     * The location of the font files.
     */
    static std::string m_fontPath;
};

#endif  // WKERNEL_H

