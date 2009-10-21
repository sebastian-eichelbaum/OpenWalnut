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
#include "../graphicsEngine/WGraphicsEngine.h"
#include "../dataHandler/WDataHandler.h"
#include "../common/WLogger.h"
#include "../gui/WGUI.h"

/**
 * OpenWalnut kernel, managing modules and interaction between
 * GUI, GE and DataHandler
 * \ingroup kernel
 */
class WKernel
{
public:

    /**
     * Default constructor. Also initializes Graphics Engine.
     *
     * \param argc number of arguments
     * \param argv arguments
     * \param gui pointer to the gui interface
     */
    WKernel( int argc, char* argv[], boost::shared_ptr< WGUI > gui );

    /**
     * Destructor.
     */
    virtual ~WKernel();

    /**
     * Copy constructor
     * \param other Reference on object to copy.
     */
    WKernel( const WKernel& other );

    /**
     * Starts execution.
     *
     * \return Execution code.
     */
    int run();

    /**
     * Returns pointer to currently running instance of graphics engine.
     *
     * \return the graphics engine instance.
     */
    boost::shared_ptr<WGraphicsEngine> getGraphicsEngine() const;

    /**
     * Returns pointer to the DataHandler.
     *
     * \return the data handler instance.
     */
    boost::shared_ptr<WDataHandler> getDataHandler() const;

    /**
     * Returns pointer to the currently running kernel.
     *
     * \return the kernel instance.
     */
    static WKernel* getRunningKernel();

    /**
     * Returns argument count.
     *
     * \return argument count.
     */
    int getArgumentCount() const;

    /**
     * Returns argument array specified to the app.
     *
     * \return the argument array.
     */
    char** getArguments() const;

    /**
     * Determines whether all threads should finish.
     *
     * \return true if so.
     */
    bool isFinishRequested() const;

    /**
     * function to call the datahandler load method
     */
    void doLoadDataSets( std::vector< std::string > fileNames );

    /**
     * getter for datahandler
     */
    boost::shared_ptr<WDataHandler> getDataHandler();

    /**
     * getter for gui
     */
    boost::shared_ptr<WGUI> getGui();

    /**
     *
     */
    void setGui( boost::shared_ptr< WGUI > gui );

    /**
     * get for application path
     */
    std::string getAppPath();

    /**
     * getter for shader path
     */
    std::string getShaderPath();

protected:

    /**
     * All the loaded modules.
     */
    std::list<boost::shared_ptr<WModule> > m_modules;

    /**
     * Pointer to an initialized graphics engine.
     */
    boost::shared_ptr<WGraphicsEngine> m_graphicsEngine;

    /**
     * The Gui.
     */
    boost::shared_ptr<WGUI> m_gui;

    /**
     * The Datahandler.
     */
    boost::shared_ptr<WDataHandler> m_dataHandler;

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
    bool findAppPath();

    /**
     *
     */
    void slotFinishLoadData( boost::shared_ptr< WDataSet > );

    /**
     * Number of arguments given to application.
     */
    int m_ArgC;

    /**
     * Arguments given to application
     */
    char** m_ArgV;

    /**
     * true if threads should finish.
     */
    bool m_FinishRequested;

    /**
     * the location of the openwalnut executable
     */
    std::string m_AppPath;

    /**
     * the location of the openwalnut executable
     */
    std::string m_shaderPath;
};

/**
 * \defgroup kernel Kernel
 *
 * \brief
 * This module implements the central part of OpenWalnut that manages
 * the interaction between GUI, GraphicsEngine and DataHandler.
 */

#endif  // WKERNEL_H

