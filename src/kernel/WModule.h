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

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

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
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const = 0;

    /**
     * Gives back a description of this module.
     * \return description to module.
     */
    virtual const std::string getDescription() const = 0;

protected:

    /**
     * Entry point after loading the module. Runs in separate thread.
     */
    virtual void threadMain() = 0;

    /** 
     * Set of input connectors associated with this module.
     */
    std::set<boost::shared_ptr<WModuleInputConnector> > m_InputConnectors;

    /** 
     * Set of output connectors associated with this module.
     */
    std::set<boost::shared_ptr<WModuleOutputConnector> > m_OutputConnectors;

    /** 
     * Adds the specified connector to the list of inputs. This function is NOT thread-safe which is not needed since modules
     * should never add/remove connectors outside their constructor.
     * 
     * \param con the connector.
     */
    void addConnector( boost::shared_ptr<WModuleInputConnector> con );

    /** 
     * Adds the specified connector to the list of outputs. This function is NOT thread-safe which is not needed since modules
     * should never add/remove connectors outside their constructor. 
     * 
     * \param con the connector.
     */
    void addConnector( boost::shared_ptr<WModuleOutputConnector> con );

    // XXX corresponding removeConnector methods should not be needed
private:
};

#endif  // WMODULE_H

