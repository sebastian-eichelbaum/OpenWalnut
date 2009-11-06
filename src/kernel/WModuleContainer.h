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

#ifndef WMODULECONTAINER_H
#define WMODULECONTAINER_H

#include <set>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

class WModule;

/**
 * Class able to contain other modules. It manages several tasks like finding appropriate modules, managing data modules and
 * module initialization.
 *
 * \ingroup Kernel
 */
class WModuleContainer: public boost::enable_shared_from_this< WModuleContainer >
{
public:

    /**
     * Default constructor.
     */
    WModuleContainer();

    /**
     * Destructor.
     */
    virtual ~WModuleContainer();

    /**
     * Add a module to this container. Please note, that a module can be added only once. If it already is associated with this
     * container nothing happens.
     * 
     * \param module the module to add.
     * \throw WModuleUninitialized thrown whenever someone wants to add a module not yet initialized.
     */
    virtual void add( boost::shared_ptr< WModule > module );

    /**
     * Remove the given module from this container if it is associated with it. TODO(ebaum): deep removal? flat removal?
     * 
     * \param module the module to remove.
     */
    virtual void remove( boost::shared_ptr< WModule > module );

protected:

    /**
     * Lock for module set.
     */
    boost::shared_mutex m_moduleSetLock;

    /**
     * The modules associated with this container.
     */
    std::set< boost::shared_ptr< WModule > > m_modules;

private:
};

#endif  // WMODULECONTAINER_H

