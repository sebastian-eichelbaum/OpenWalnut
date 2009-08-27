//---------------------------------------------------------------------------
//
// Project: OpenWalnut
//
// Copyright 2009 SomeCopyrightowner
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
#include "../gui/qt4/WMainApplication.h"
#include "../dataHandler/WDataHandler.h"

/**
 * \par Description:
 * OpenWalnut kernel, managing modules and interaction between GUI, GE and DataHandler
 */
class WKernel
{
public:

    /**
     * \par Description
     * Default constructor. Also initializes Graphics Engine.
     *
     * \param argc number of arguments
     * \param argv arguments
     */
    WKernel( int argc, char* argv[] );

    /**
     * \par Description
     * Destructor.
     */
    virtual ~WKernel();

    /**
     * \par Description
     * Copy constructor
     * \param other Reference on object to copy.
     */
    WKernel( const WKernel& other );

    /**
     * \par Description
     * Starts execution.
     *
     * \return Execution code.
     */
    int run();

    /**
     * \par Description
     * Returns pointer to currently running instance of graphics engine.
     *
     * \return the ge instance.
     */
    boost::shared_ptr<WGraphicsEngine> getGraphicsEngine() const;

    /**
     * \par Description
     * Returns pointer to the DataHandler.
     *
     * \return the dh instance.
     */
    boost::shared_ptr<WDataHandler> getDataHandler() const;

    /**
     * \par Description
     * Returns pointer to the currently running kernel.
     *
     * \return the kernel instance.
     */
    static WKernel* getRunningKernel();

    /**
     * \par Description
     * Returns argument count.
     *
     * \return argument count.
     */
    int getArgumentCount() const;

    /**
     * \par Description
     * Returns argument array specified to the app.
     *
     * \return the argument array.
     */
    char** getArguments() const;

    /**
     * \par Description
     * Determines whether all threads should finish.
     *
     * \return true if so.
     */
    bool isFinishRequested() const;

    /**
     *
     */
    void doLoadDataSets( std::vector< std::string > fileNames );

protected:

    /**
     * \par Description
     * All the loaded modules.
     */
    std::list<WModule*> m_modules;

    /**
     * \par Description
     * Pointer to an initialized graphics engine.
     */
    boost::shared_ptr<WGraphicsEngine> m_GraphicsEngine;

    /**
     * \par Description
     * The Gui.
     */
    boost::shared_ptr<WMainApplication> m_Gui;

    /**
     * \par Description
     * The Datahandler.
     */
    boost::shared_ptr<WDataHandler> m_DataHandler;

private:
    /**
     * \par Description
     * Loads all the modules it can find.
     */
    void loadModules();

    /**
     * \par Description
     * Initializes the graphics engine, data handler and so on.
     */
    void init();

    /**
     * \par Description
     * Number of arguments given to application.
     */
    int m_ArgC;

    /**
     * \par Description
     * Arguments given to application
     */
    char** m_ArgV;

    /**
     * \par Description
     * true if threads should finish.
     */
    bool m_FinishRequested;
};

#endif  // WKERNEL_H

