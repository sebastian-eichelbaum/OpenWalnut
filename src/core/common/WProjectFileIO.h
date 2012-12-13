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

#ifndef WPROJECTFILEIO_H
#define WPROJECTFILEIO_H

#include <ostream>
#include <string>
#include <vector>

/**
 * A base class for all parts of OpenWalnut which can be serialized to a project file. It is used by WProjectFile to actually parse the file line
 * by line. Derive from this class if you write your own parser and use it to fill your internal data structures. But write it in a very
 * error-tolerant way. We want to avoid that small problems in the project file cause the whole file to be useless.
 */
class WProjectFileIO // NOLINT
{
public:
    /**
     * Default constructor.
     */
    WProjectFileIO();

    /**
     * Destructor.
     */
    virtual ~WProjectFileIO();

    /**
     * This method parses the specified line and interprets it. It gets called line by line by WProjectFile. You should avoid applying anything
     * of the loaded information here. You should use \ref done for this.
     *
     * \param line the current line as string
     * \param lineNumber the current line number. Useful for error/warning/debugging output.
     *
     * \return true if the line could be parsed.
     */
    virtual bool parse( std::string line, unsigned int lineNumber ) = 0;

    /**
     * Called whenever the end of the project file has been reached. Use this to actually apply your loaded settings. Do this in a error-tolerant
     * way and apply as most settings as possible even if some other settings are erroneous. Add errors with \ref addError. Try avoiding
     * exceptions if possible.
     */
    virtual void done();

    /**
     * Saves the state to the specified stream.
     *
     * \param output the stream to print the state to.
     */
    virtual void save( std::ostream& output ) = 0;   // NOLINT

    /**
     * Checks whether there where errors during load or save.
     *
     * \return true if there where.
     */
    bool hadErrors() const;

    /**
     * Get error list.
     *
     * \return the list
     */
    const std::vector< std::string >& getErrors() const;

    /**
     * Checks whether there where warnings during load or save.
     *
     * \return true if there where.
     */
    bool hadWarnings() const;

    /**
     * Get warnings list.
     *
     * \return the list
     */
    const std::vector< std::string >& getWarnings() const;

protected:
    /**
     * Add an error. Use this when you encounter some difficulties during parsing or applying settings. Provide useful errors. They will be
     * presented to the user.
     *
     * \param description the error description
     */
    void addError( std::string description );

    /**
     * Add an warning. Use this when you encounter some difficulties during parsing or applying settings. Provide useful warnings. They will be
     * presented to the user.
     *
     * \param description the error description
     */
    void addWarning( std::string description );

private:
    /**
     * List of errors if any.
     */
    std::vector< std::string > m_errors;

    /**
     * List of warnings if any.
     */
    std::vector< std::string > m_warnings;
};

#endif  // WPROJECTFILEIO_H

