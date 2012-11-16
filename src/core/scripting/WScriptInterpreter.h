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

#ifndef WSCRIPTINTERPRETER_H
#define WSCRIPTINTERPRETER_H

#include <string>
#include <vector>

/**
 * \class WScriptInterpreter
 *
 * An abstract base class for a script interpreter.
 */
class WScriptInterpreter
{
public:
    /**
     * Destructor.
     */
    virtual ~WScriptInterpreter();

    /**
     * Initialize OpenWalnut-bindings. These allow OW-classes to be used via the script interpreter.
     */
    virtual void initBindings() = 0;

    /**
     * Sets the script parameters. These are the parameters you would normally call your script with, e.g.
     * "./myscript.py param 1 param2".
     *
     * \param params The parameters to the script. In our example, they would be "./myscript.py", "param", "1" and "param2".
     */
    virtual void setParameters( std::vector< std::string > const& params ) = 0;

    /**
     * Execute some code.
     *
     * \param line The code to interpret.
     */
    virtual void execute( std::string const& line ) = 0;

    /**
     * Execute a file.
     *
     * \param filename The script file to execute.
     */
    virtual void executeFile( std::string const& filename ) = 0;

    /**
     * Get the name of the language interpreted by this interpreter.
     *
     * \return The name of the script language.
     */
    virtual std::string const getName() const = 0;

    /**
     * Get the default extension for script file belonging to the script interpreter's language.
     *
     * \return The default file extension.
     */
    virtual std::string const getExtension() const = 0;
};

#endif  // WSCRIPTINTERPRETER_H
