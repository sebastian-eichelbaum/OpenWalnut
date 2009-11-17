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

#include <list>
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
     * Constructor. Initializes container.
     * 
     * \param name  name of the container
     * \param description short description.
     */
    WModuleContainer( std::string name, std::string description );

    /**
     * Destructor.
     */
    virtual ~WModuleContainer();

    /**
     * Add a module to this container and start it. Please note, that a module can be added only once. If it already is 
     * associated with this container nothing happens.
     * 
     * \param module the module to add.
     * \param run true when the module should be run automatically after adding it.
     * \throw WModuleUninitialized thrown whenever someone wants to add a module not yet initialized.
     */
    virtual void add( boost::shared_ptr< WModule > module, bool run = true );

    /**
     * Remove the given module from this container if it is associated with it. TODO(ebaum): deep removal? flat removal?
     * 
     * \param module the module to remove.
     */
    virtual void remove( boost::shared_ptr< WModule > module );

    /**
     * Stops all modules inside this container. Note that this function could take some time, since it waits until the last module
     * has quit.
     */
    virtual void stop();

    /**
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Gives back a description of this module.
     * \return description to module.
     */
    virtual const std::string getDescription() const;

    /**
     * Add a specified notifier to the list of default notifiers which get connected to each added module.
     * 
     * \param signal    the signal the notifier should get connected to
     * \param notifier  the notifier function
     */
    virtual void addDefaultNotifier( MODULE_SIGNAL signal, t_ModuleErrorSignalHandlerType notifier );
    virtual void addDefaultNotifier( MODULE_SIGNAL signal, t_ModuleGenericSignalHandlerType notifier );

protected:

    /**
     * Lock for module set.
     */
    boost::shared_mutex m_moduleSetLock;

    /**
     * The modules associated with this container.
     */
    std::set< boost::shared_ptr< WModule > > m_modules;

    /**
     * Name of the module.
     */
    std::string m_name;

    /**
     * Description of the module.
     */
    std::string m_description;

    /**
     * Lock for error notifiers set.
     */
    boost::shared_mutex m_errorNotifiersLock;

    /**
     * The error notifiers connected to added modules by default.
     */
    std::list< t_ModuleErrorSignalHandlerType > m_errorNotifiers;

    /**
     * Lock for ready notifiers set.
     */
    boost::shared_mutex m_readyNotifiersLock;

    /**
     * The ready notifiers connected to added modules by default.
     */
    std::list< t_ModuleGenericSignalHandlerType > m_readyNotifiers;

private:
};

#endif  // WMODULECONTAINER_H

