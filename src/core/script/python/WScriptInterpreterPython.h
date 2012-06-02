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

#include <boost/python.hpp>

#include "../wrappers/WModuleContainerWrapper.h"

#include "../WScriptInterpreter.h"

namespace pb = boost::python;

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
    WScriptInterpreterPython();

    /**
     * Destructor. Destroys the interpreter.
     */
    virtual ~WScriptInterpreterPython();

    /**
     * Initializes bindings for OpenWalnut classes. Call this after starting the kernel.
     */
    virtual void initBindings();

    /**
     * Execute some python code.
     *
     * \param line The code to execute.
     */
    virtual void execute( std::string const& line );

private:
    //! The python module.
    pb::object m_pyModule;

    //! The namespace where we will be working. Bindings are declared in this workspace.
    pb::object m_pyMainNamespace;

    //! A pointer to the kernel's root container. We can use this to manipulate modules.
    WModuleContainerWrapper m_rootContainer;
};

#endif  // WSCRIPTINTERPRETERPYTHON_H
