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

#ifndef WMODULEOUTPUTCONNECTOR_H
#define WMODULEOUTPUTCONNECTOR_H

#include "WModuleConnector.h"

class WModuleInputConnector;

/**
 * Class implementing output connection functionality between modules.
 */
class WModuleOutputConnector: public WModuleConnector
{
public:

    /**
     * Default constructor.
     */
    WModuleOutputConnector();

    /**
     * Destructor.
     */
    virtual ~WModuleOutputConnector();

    /** 
     * Checks whether the specified module is an input connector.
     * 
     * \param con the connector to check.
     * 
     * \return true if con is an input connector.
     */
    virtual bool connectable( boost::shared_ptr<WModuleInputConnector> con );

    /** 
     * Checks whether the specified module is an input connector.
     * 
     * \param con the connector to check.
     * 
     * \return false since it whenever connectable is called with an WModuleInputConnector the above one is used instead.
     */
    virtual bool connectable( boost::shared_ptr<WModuleConnector> con );

protected:

private:
};

#endif  // WMODULEOUTPUTCONNECTOR_H

