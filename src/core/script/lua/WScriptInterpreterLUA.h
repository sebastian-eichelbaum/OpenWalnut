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

#ifndef WSCRIPTINTERPRETERLUA_H
#define WSCRIPTINTERPRETERLUA_H

#include <lua.hpp>
#include <luabind/luabind.hpp>

#include <string>

#include "../wrappers/WModuleContainerWrapper.h"

#include "../WScriptInterpreter.h"

/**
 * \class WScriptInterpreterLUA
 *
 * A script interpreter for the lua language.
 */
class WScriptInterpreterLUA : public WScriptInterpreter
{
public:
    /**
     * Constructor. Creates the interpreter.
     */
    WScriptInterpreterLUA();

    /**
     * Destructor. Destroys the interpreter.
     */
    virtual ~WScriptInterpreterLUA();

    /**
     * Initializes bindings for OpenWalnut classes. Call this after starting the kernel.
     */
    virtual void initBindings();

    /**
     * Execute some LUA code.
     *
     * \param line The code to execute.
     */
    virtual void execute( std::string const& line );

private:
    //! The lua state.
    lua_State* m_lua;

    //! A pointer to the kernel's root container. We can use this to manipulate modules.
    WModuleContainerWrapper m_rootContainer;
};

#endif  // WSCRIPTINTERPRETERLUA_H
