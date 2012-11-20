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

#ifndef WMODULECONTAINERWRAPPER_H
#define WMODULECONTAINERWRAPPER_H

#include <string>

#include <boost/shared_ptr.hpp>

#include "core/kernel/WModuleContainer.h"

#include "WModuleWrapper.h"

/**
 * \class WModuleContainerWrapper
 *
 * Encapsulates a module container. This class is exposed to scripts.
 */
class WModuleContainerWrapper
{
public:
    /**
     * Constructor. Creates an empty wrapper.
     */
    WModuleContainerWrapper();

    /**
     * Constructor.
     *
     * \param mc The module container.
     */
    explicit WModuleContainerWrapper( boost::shared_ptr< WModuleContainer > mc );

    /**
     * Destructor.
     */
    ~WModuleContainerWrapper();

    /**
     * Creates a module from the prototype with the given name.
     *
     * \param name The name of the module prototype.
     * \return The module.
     */
    WModuleWrapper create( std::string const& name );

    /**
     * Creates a data module and load the file given via filename.
     *
     * \param filename The name of the file to load.
     * \return The module.
     */
    WModuleWrapper createDataModule( std::string const& filename );

    /**
     * Remove a module from the container.
     *
     * \param module The module to remove.
     */
    void remove( WModuleWrapper module );

private:
    //! The module container.
    boost::shared_ptr< WModuleContainer > m_mc;
};

#endif  // WMODULECONTAINERWRAPPER_H
