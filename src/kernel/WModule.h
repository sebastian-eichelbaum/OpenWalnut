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

#ifndef WMODULE_H
#define WMODULE_H

#include <string>

#include "../common/WThreadedRunner.h"
#include "WModuleInputConnector.h"
#include "WModuleOutputConnector.h"


/**
 * Class representing a single module of OpenWalnut.
 * \ingroup kernel
 */
class WModule: public WThreadedRunner
{
public:

    /**
     * Constructs a new WModule instance
     */
    WModule();

    /**
     * Destructor.
     */
    virtual ~WModule();

    /**
     * Copy constructor
     * \param other Reference on object to copy.
     */
    WModule( const WModule& other );

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
     * Adds the specified connector to the list of inputs.
     * 
     * \param con the connector.
     */
    virtual void addInput( boost::shared_ptr<WModuleInputConnector> con );

    /** 
     * Adds the specified connector to the list of outputs.
     * 
     * \param con the connector.
     */
    virtual void addOutput( boost::shared_ptr<WModuleOutputConnector> con );

protected:

    /**
     * Entry point after loading the module. Runs in separate thread.
     */
    virtual void threadMain();

private:
};

#endif  // WMODULE_H

