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

#ifndef WSCRIPTENGINE_H
#define WSCRIPTENGINE_H

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "../kernel/WModuleContainer.h"

#include "WScriptInterpreter.h"

/**
 * The script engine. Provides all script interpreters available for the OpenWalnut installation. Which
 * interpreters are available depends on the libs available at OpenWalnut build time.
 */
class WScriptEngine
{
public:
    /**
     * Constructs a new script engine.
     *
     * \param rootContainer The root module container to use for modules inserted via scripts.
     */
    explicit WScriptEngine( boost::shared_ptr< WModuleContainer > const& rootContainer );

    /**
     * Destructor.
     */
    virtual ~WScriptEngine();

    /**
     * This finds an interpreter suitable for executing script files ending with the given extension.
     *
     * \param ext The extension of the script file to execute.
     *
     * \return The matching script interpreter or a NULL-pointer, if no such interpreter was available.
     */
    boost::shared_ptr< WScriptInterpreter > getInterpreterByFileExtension( std::string const& ext );

    /**
     * This finds an interpreter by script language name.
     *
     * \param name The name of the script language.
     *
     * \return The matching script interpreter or a NULL-pointer, if no such interpreter was available.
     */
    boost::shared_ptr< WScriptInterpreter > getInterpreter( std::string const& name );

    /**
     * Get the number of script interpreters available.
     *
     * \return The number of available script interpreters.
     */
    std::size_t getNumInterpreters() const;

    /**
     * Get the i'th script interpreter.
     *
     * \param index The index of the script interpreter to retrieve, must be in [0,getNumInterpreters()-1].
     *
     * \return The script interpreter or a NULL-pointer if the index was invalid.
     */
    boost::shared_ptr< WScriptInterpreter > getInterpreter( std::size_t index );

private:
    //! The list of available script interpreters.
    std::vector< boost::shared_ptr< WScriptInterpreter > > m_interpreters;
};

#endif  // WSCRIPTENGINE_H
