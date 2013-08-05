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

#ifndef WMODULELOADER_H
#define WMODULELOADER_H

#include <set>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>

#include "../common/WSharedAssociativeContainer.h"
#include "../common/WSharedLib.h"

#include "WModule.h"

/**
 * Loads module prototypes from shared objects in a given directory and injects it into the module factory.
 */
class  WModuleLoader
{
public:
    /**
     * Shared pointer abbreviation.
     */
    typedef boost::shared_ptr< WModuleLoader > SPtr;

    /**
     * Const pointer abbreviation.
     */
    typedef boost::shared_ptr< const WModuleLoader > ConstSPtr;

    /**
     * Constructor. It does not load any files. Use load to do this.
     *
     */
    explicit WModuleLoader();

    /**
     * Destructor, closes all handles to shared libraries.
     */
    ~WModuleLoader();

    /**
     * Load the module prototypes from the shared libraries.
     *
     * \param ticket A write ticket to a shared container.
     */
    void load( WSharedAssociativeContainer< std::set< boost::shared_ptr< WModule > > >::WriteTicket ticket );

    /**
     * Returns the prefix of a shared module library filename.
     *
     * \return the prefix.
     */
    static std::string getModulePrefix();

    /**
     * The loader also stores information on which library provides the arbitrary registration mechanism. This cannot be called during load, as
     * OW is not completely initialized at this point. So we do this here. Call this after startup, before project loading.
     */
    void initializeExtensions();

private:
    /**
     * All the loaded shared libraries. Get freed on destruction. So do NOT free this instance while the libs are used.
     */
    std::vector< boost::shared_ptr< WSharedLib > > m_libs;

    /**
     * Load the module prototypes from the shared libraries from the specified directory. It traverses the subdirectories and searches there.
     * Traversion depth is 1.
     *
     * \param ticket A write ticket to a shared container.
     * \param dir the directory to load
     * \param level the traversion level
     */
    void load( WSharedAssociativeContainer< std::set< boost::shared_ptr< WModule > > >::WriteTicket ticket, boost::filesystem::path dir,
               unsigned int level = 0 );

    /**
     * Helper to store information on a lib which gets initialized later. This basically is used for the arbitrary registration feature.
     */
    struct PostponedLoad
    {
        PostponedLoad( boost::shared_ptr< WSharedLib > lib, boost::filesystem::path path ):
            m_lib( lib ),
            m_path( path )
        {
        }

        /**
         * The library. Need to keep this to avoid freeing the lib beforehand.
         */
        boost::shared_ptr< WSharedLib > m_lib;

        /**
         * The path of the resources.
         */
        boost::filesystem::path m_path;
    };

    /**
     * The libs which need to be initialized when OW is loaded completely.
     */
    std::vector< PostponedLoad > m_arbitraryRegisterLibs;

};

#endif  // WMODULELOADER_H
