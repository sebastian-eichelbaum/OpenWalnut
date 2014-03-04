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

#include <boost/shared_ptr.hpp>

#include "WProperties.h"

class WProjectFile;

/**
 * A base class for all parts of OpenWalnut which can be serialized to a project file. It is used by WProjectFile to actually parse the file line
 * by line. Derive from this class if you write your own parser and use it to fill your internal data structures. But write it in a very
 * error-tolerant way. We want to avoid that small problems in the project file cause the whole file to be useless.
 *
 * In general, each IO implementation has the chance to parse each line. After parsing all lines, the done method gets called. This method should
 * contain code to actually apply the settings loaded. You should avoid doing this in the parse method itself.
 */
class WProjectFileIO // NOLINT
{
public:
    /**
     * Abbreviation for a shared pointer.
     */
    typedef boost::shared_ptr< WProjectFileIO > SPtr;

    /**
     * Abbreviation for const shared pointer.
     */
    typedef boost::shared_ptr< const WProjectFileIO > ConstSPtr;

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

    /**
     * Create a clone of the IO. This is especially useful for custom parsers registered at \ref WProjectFile::registerParser. Implement this
     * function.
     *
     * \param project the project file using this parser instance.
     *
     * \return Cloned instance.
     */
    virtual SPtr clone( WProjectFile* project ) const = 0;

    /**
     * Set the project using this parser
     *
     * \param project the project
     */
    void setProject( WProjectFile* project );

    /**
     * When to apply this parser. This might be important in some cases. Note that you can only decide
     * whether you want to apply your changes before or after the modules have been added.
     */
    enum ApplyOrder
    {
        PRE_MODULES = 0,
        POST_MODULES
    };

    /**
     * Return the apply order of this IO.
     *
     * \return the order
     */
    ApplyOrder getApplyOrder() const;

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

    /**
     * Recursively prints the properties and nested properties.
     *
     * \param output    the output stream to print to
     * \param props     the properties to recursively print
     * \param indent    the indentation level
     * \param prefix    the prefix (name prefix of property)
     * \param index     the ID to use
     * \param indexPrefix use this to add a prefix to the index
     */
    void printProperties( std::ostream& output, boost::shared_ptr< WProperties > props, std::string indent, //NOLINT ( non-const ref )
                          std::string prefix, unsigned int index, std::string indexPrefix = "" );

    /**
     * The project using this parser
     */
    WProjectFile* m_project;

    /**
     * Set the order of calls to "done".
     *
     * \param order the order.
     */
    void setApplyOrder( ApplyOrder order );

private:
    /**
     * List of errors if any.
     */
    std::vector< std::string > m_errors;

    /**
     * List of warnings if any.
     */
    std::vector< std::string > m_warnings;

    /**
     * The order in which the "done" functions are called.
     */
    ApplyOrder m_applyOrder;
};

#endif  // WPROJECTFILEIO_H

