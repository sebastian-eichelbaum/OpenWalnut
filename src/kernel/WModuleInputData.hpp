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

#ifndef WMODULEINPUTDATA_H
#define WMODULEINPUTDATA_H

#include <string>

// this seems to be necessary
template < typename T > class WModuleInputData;
#include "WModuleOutputData.hpp"

#include "WModuleInputConnector.h"

/**
 * Class offering an instantiate-able data connection between modules.
 * Due to is template style it is possible to bind nearly arbitrary data.
 */
template < typename T >
class WModuleInputData: public WModuleInputConnector
{
public:

    /** 
     * Constructor.
     * 
     * \param module the module which is owner of this connector.
     * \param name The name of this connector.
     * \param description Short description of this connector.
     */
    WModuleInputData( boost::shared_ptr<WModule> module, std::string name="", std::string description="" ):
        WModuleInputConnector( module, name, description )
    {
    };

    /**
     * Destructor.
     */
    virtual ~WModuleInputData()
    {
    };

    /** 
     * Gives the currently set data.
     * 
     * \return the data currently set.
     */
    const boost::shared_ptr<T> getData() const
    {
        //return m_data;
    };

protected:

    /** 
     * Checks whether the specified connector is an input connector and compatible with T.
     * 
     * \param con the connector to check against.
     * 
     * \return true if compatible.
     */
    virtual bool connectable( boost::shared_ptr<WModuleConnector> con )
    {
        if ( dynamic_cast<WModuleOutputData<T>* >( con.get() ) )  // NOLINT - since we really need them here
        {
            // NOTE: the upper cast already checked the compatibility completely. WModuleInputConnector::connectable does the
            // same check again. But since we do not know what checks will be added to WModuleInputConnector::connectable in the
            // future we forward the call.
            return WModuleInputConnector::connectable( con );
        }
        return false;
    };

private:
};

#endif  // WMODULEINPUTDATA_H

