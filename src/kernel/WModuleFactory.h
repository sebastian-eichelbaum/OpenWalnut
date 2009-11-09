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

#endif  // WMODULEFACTORY_H

