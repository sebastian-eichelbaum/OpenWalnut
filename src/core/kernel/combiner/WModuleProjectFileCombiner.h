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

#ifndef WMODULEPROJECTFILECOMBINER_H
#define WMODULEPROJECTFILECOMBINER_H

#include <ostream>
#include <list>
#include <map>
#include <string>
#include <utility>

#include <boost/shared_ptr.hpp>

#include "../../common/WProjectFileIO.h"

#include "../WModuleCombiner.h"

class WProjectFile;
class WModule;

/**
 * This class is able to parse project files and create the appropriate module graph inside a specified container. It is also derived from
 * WProjectFileIO to allow WProjectFile to fill this combiner.
 */
class  WModuleProjectFileCombiner: public WModuleCombiner,
                                   public WProjectFileIO
{
public:
    /**
     * Creates an empty combiner.
     *
     * \param target the target container where to add the modules to.
     */
    explicit WModuleProjectFileCombiner( boost::shared_ptr< WModuleContainer > target );

    /**
     * Creates an empty combiner. This constructor automatically uses the kernel's root container as target container.
     */
    WModuleProjectFileCombiner();

    /**
     * Destructor.
     */
    virtual ~WModuleProjectFileCombiner();

    /**
     * Apply the internal module structure to the target container. Be aware, that this operation might take some time, as modules can be
     * connected only if they are "ready", which, at least with WDataModule modules, might take some time. It applies the loaded project file.
     *
     * \note the loader for project files is very tolerant. It does not abort. It tries to load as much as possible. The only exception that gets
     * thrown whenever the file could not be opened.
     *
     * \throw WFileNotFound whenever the project file could not be opened.
     */
    virtual void apply();

    /**
     * This method parses the specified line and interprets it to fill the internal module graph structure.
     *
     * \param line the current line as string
     * \param lineNumber the current line number. Useful for error/warning/debugging output.
     *
     * \return true if the line could be parsed.
     */
    virtual bool parse( std::string line, unsigned int lineNumber );

    /**
     * Called whenever the end of the project file has been reached. This is useful if your specific parser class wants to do some post
     * processing after parsing line by line.
     */
    virtual void done();

    /**
     * Saves the state to the specified stream.
     *
     * \param output the stream to print the state to.
     */
    virtual void save( std::ostream& output );   // NOLINT

    /**
     * Create a clone of the IO. This is especially useful for custom parsers registered at \ref WProjectFile::registerParser. Implement this
     * function.
     *
     * \param project the project file using this parser instance.
     *
     * \return Cloned instance.
     */
    virtual WProjectFileIO::SPtr clone( WProjectFile* project ) const;

    /**
     * Map a given project file ID to a module. This method must not be used by WModuleProjectFileCombiner, as it builds this mapping. All other
     * \ref WProjectFileIO implementations are allowed to use it in their save and apply methods (NOT in parse()).
     *
     * \param id the id
     *
     * \return the module, or NULL if ID is not known.
     */
    virtual boost::shared_ptr< WModule > mapToModule( unsigned int id ) const;

    /**
     * Map a given module to project file ID. This method must not be used by WModuleProjectFileCombiner, as it builds this mapping. All other
     * \ref WProjectFileIO implementations are allowed to use it in their save and apply methods (NOT in parse()).
     *
     * \param module the module to map
     *
     * \return the ID, or numeric_limits< unisigned int >::max() if module not known.
     */
    virtual unsigned int mapFromModule( boost::shared_ptr< WModule > module ) const;

protected:
    /**
     * The module ID type. A pair of ID and pointer to module.
     */
    typedef std::pair< unsigned int, boost::shared_ptr< WModule > > ModuleID;

    /**
     * Map between ID and Module
     */
    typedef std::map< unsigned int, boost::shared_ptr< WModule > > ModuleIDMap;

    /**
     * All Modules.
     */
    std::map< unsigned int, boost::shared_ptr< WModule > > m_modules;

    /**
     * A connector is described by ID and name.
     */
    typedef std::pair< unsigned int, std::string > Connector;

    /**
     * A connection is a pair of connectors.
     */
    typedef std::pair< Connector, Connector > Connection;

    /**
     * All connections.
     */
    std::list< Connection > m_connections;

    /**
     * A property is a pair of ID and name.
     */
    typedef std::pair< unsigned int, std::string > Property;

    /**
     * A property value is a property and the new value as string.
     */
    typedef std::pair< Property, std::string > PropertyValue;

    /**
     * All properties.
     */
    std::list< PropertyValue > m_properties;
private:
};

#endif  // WMODULEPROJECTFILECOMBINER_H

