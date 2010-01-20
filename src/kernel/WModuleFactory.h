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

#ifndef WMODULEFACTORY_H
#define WMODULEFACTORY_H

#include <set>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include "WModule.h"

/**
 * Class able to create a new copy of an arbitrary module. It uses the Factory and Prototype design pattern.
 */
class WModuleFactory
{
friend class WModuleFactoryTest;
public:

    /**
     * Default constructor.
     */
    WModuleFactory();

    /**
     * Destructor.
     */
    virtual ~WModuleFactory();

    /**
     * Loads the modules and creates prototypes.
     */
    void load();

    /**
     * Create a new and initialized module using the specified prototype.
     *
     * \param prototype the prototype to clone.
     *
     * \return the module created using the prototype.
     */
    boost::shared_ptr< WModule > create( boost::shared_ptr< WModule > prototype );

    /**
     * Returns instance of the module factory to use to create modules.
     *
     * \return the running module factory.
     */
    static boost::shared_ptr< WModuleFactory > getModuleFactory();

    /**
     * Searches a prototype by name. It returns the prototype, or a NULL pointer if it is not found. The difference to
     * getPrototypeByName() is, that an unavailable prototype does not throw an exception. This is nice for checking whether a
     * prototype exists or not.
     *
     * \param name name of the prototype to search
     *
     * \return the prototype if it exists, or NULL if not.
     */
    const boost::shared_ptr< WModule > isPrototypeAvailable( std::string name );

    /**
     * Finds a prototype using the specified name.
     *
     * \param name the name.
     *
     * \return the prototype whose name is equal to the specified one.
     */
    const boost::shared_ptr< WModule > getPrototypeByName( std::string name );

    /**
     * Finds a prototype using an instance of a module. This uses the type_info to find a proper prototype.
     *
     * \param instance the instance to use.
     *
     * \return the prototype.
     * \throw WPrototypeUnknown if prototype can not be found.
     */
    const boost::shared_ptr< WModule > getPrototypeByInstance( boost::shared_ptr< WModule > instance );

    /**
     * Checks whether the first instance can be casted to the second one.
     *
     * \param module the module to check.
     *
     * \return true if the dynamic_cast is successful
     */
    template <typename T>
    static bool isA( boost::shared_ptr< WModule > module );

    /**
     * Returns a set of modules compatible with the specified one.
     *
     * \param module the module to find prototype for.
     *
     * \return set of compatible prototypes.
     */
    std::set< boost::shared_ptr< WModule > > getCompatiblePrototypes( boost::shared_ptr< WModule > module );
protected:

    /**
     * The module prototypes available.
     */
    std::set< boost::shared_ptr< WModule > > m_prototypes;

    /**
     * The lock for the prototypes set.
     */
    boost::shared_mutex m_prototypesLock;

private:
    /**
     * Singleton instance of WModuleFactory.
     */
    static boost::shared_ptr< WModuleFactory > m_instance;
};

template <typename T>
bool WModuleFactory::isA( boost::shared_ptr< WModule > module )
{
    // NOTE: this is RTTI. Everybody says: do not use it but nearly nobody says in which cases and why. So we ignore them and use
    // it.
    return ( dynamic_cast< T* >( module.get() ) );  // NOLINT
}

#endif  // WMODULEFACTORY_H

