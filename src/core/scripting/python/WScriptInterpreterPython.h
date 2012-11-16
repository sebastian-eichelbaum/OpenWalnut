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

#ifndef WSCRIPTINTERPRETERPYTHON_H
#define WSCRIPTINTERPRETERPYTHON_H

#include <string>
#include <vector>

#include <boost/thread/mutex.hpp>
#include <boost/python.hpp>

#include "../wrappers/WLoggerWrapper.h"
#include "../wrappers/WModuleContainerWrapper.h"

#include "../WScriptInterpreter.h"

namespace pb = boost::python;

#ifdef PYTHON_FOUND

/**
 * \class WScriptInterpreterPython
 *
 * An interpreter for python scripts.
 */
class WScriptInterpreterPython : public WScriptInterpreter
{
public:
    /**
     * Constructor. Creates the interpreter.
     */
    explicit WScriptInterpreterPython( boost::shared_ptr< WModuleContainer > const& rootContainer );

    /**
     * Destructor. Destroys the interpreter.
     */
    virtual ~WScriptInterpreterPython();

    /**
     * Initializes bindings for OpenWalnut classes. Call this after starting the kernel.
     */
    virtual void initBindings();

    /**
     * Sets the script parameters. These are the parameters you would normally call your script with, e.g.
     * "./myscript.py param 1 param2".
     *
     * \param params The parameters to the script. In our example, they would be "./myscript.py", "param", "1" and "param2".
     */
    virtual void setParameters( std::vector< std::string > const& params );

    /**
     * Execute some python code.
     *
     * \param line The code to execute.
     */
    virtual void execute( std::string const& line );

    /**
     * Execute a file.
     *
     * \param filename The script file to execute.
     */
    virtual void executeFile( std::string const& filename );

    /**
     * Get the name of the language interpreted by this interpreter.
     *
     * \return The name of the script language.
     */
    virtual std::string const getName() const;

    /**
     * Get the default extension for script file belonging to the script interpreter's language.
     *
     * \return The default file extension.
     */
    virtual std::string const getExtension() const;

private:
    //! The python module.
    pb::object m_pyModule;

    //! The namespace where we will be working. Bindings are declared in this workspace.
    pb::object m_pyMainNamespace;

    //! A pointer to the kernel's root container. We can use this to manipulate modules.
    WModuleContainerWrapper m_rootContainer;

    //! The logger.
    WLoggerWrapper m_logger;

    //! The number of args passed when calling the script.
    int m_argc;

    //! The args passed to the script.
    char** m_argv;

    //! A mutex for thread-safe script execution.
    boost::mutex m_mutex;
};

#endif  // PYTHON_FOUND

#endif  // WSCRIPTINTERPRETERPYTHON_H
