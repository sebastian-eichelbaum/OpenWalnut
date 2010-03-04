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

#include <list>
#include <map>
#include <string>
#include <utility>

#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>

#include "WModuleCombiner.h"

/**
 * This is a base class for all module combination classes. The basic idea is to hide the actual combination work from others. These classes may
 * be useful in the GUI. The GUI can create a module combiner instance in a special way, with an interface the GUI wants to have. Then, the
 * kernel can construct the actual module graph out of it. Another purpose is some kind of project file loading. One can write a combiner which
 * loads projects files and creates a module graph out of it. The only think which all the combiners need to know: the target container. Derive
 * all specific combiner classes from this one.
 */
class WModuleProjectFileCombiner: public WModuleCombiner
{
public:

    /**
     * Creates an empty combiner.
     *
     * \param target the target container where to add the modules to.
     * \param project the project file to load.
     */
    WModuleProjectFileCombiner( boost::filesystem::path project, boost::shared_ptr< WModuleContainer > target );

    /**
     * Creates an empty combiner. This constructor automatically uses the kernel's root container as target container.
     *
     * \param project the project file to load.
     */
    explicit WModuleProjectFileCombiner( boost::filesystem::path project );

    /**
     * Destructor.
     */
    virtual ~WModuleProjectFileCombiner();

    /**
     * Apply the internal module structure to the target container. Be aware, that this operation might take some time, as modules can be
     * connected only if they are "ready", which, at least with WMData modules, might take some time. It applies the loaded project file.
     *
     * \note the loader for project files is very tolerant. It does not abort. It tries to load as much as possible. The only exception that gets
     * thrown whenever the file could not be opened.
     *
     * \throw WFileNotFound whenever the project file could not be opened.
     */
    virtual void apply();

protected:

    /**
     * The project file path. This file gets loaded during apply().
     */
    boost::filesystem::path m_project;

    /**
     * The module ID type. A pair of ID and pointer to module.
     */
    typedef std::pair< unsigned int, boost::shared_ptr< WModule > > ModuleID;

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

    /**
     * Parses the input file and fills m_modules, m_connections and m_properites.
     */
    void parse();

private:
};

#endif  // WMODULEPROJECTFILECOMBINER_H

