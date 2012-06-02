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

#ifndef WSCRIPTINTERPRETERFACTORY_H
#define WSCRIPTINTERPRETERFACTORY_H

#include <string>

#include <boost/shared_ptr.hpp>

#include "WScriptInterpreter.h"

/**
 * A factory class that creates instances of WScriptInterpreter subclasses.
 */
class WScriptInterpreterFactory
{
public:
    /**
     * Create a script interpreter that can execute scripts from file with the given extension (for example
     * .py files get executed by a python interpreter).
     *
     * \param ext The extension of the file.
     * \return A matching script interpreter instance or NULL if no such instance could be created.
     */
    static boost::shared_ptr< WScriptInterpreter > constructByFileExtension( std::string const& ext );

    /**
     * Create a script interpreter via language name (for example "python").
     *
     * \param ext The filename.
     * \return A matching script interpreter instance or NULL if no such instance could be created.
     */
    static boost::shared_ptr< WScriptInterpreter > constructByName( std::string const& name );

    /**
     * Returns a formatted string listing the interpreter types that can be instantiated.
     *
     * \return A string listing possible interpreters.
     */
    static std::string getSupportedInterpreterList();
};

#endif  // WSCRIPTINTERPRETERFACTORY_H
