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

#ifndef WMODULEOUTPUTDATA_H
#define WMODULEOUTPUTDATA_H

#include <string>

#include <boost/shared_ptr.hpp>

// this seems to be necessary
template < typename T > class WModuleOutputData;
#include "WModuleInputData.hpp"

#include "WModuleOutputConnector.h"

/**
 * Class offering an instantiate-able data connection between modules.
 * Due to is template style it is possible to bind nearly arbitrary data.
 */
template < typename T >
class WModuleOutputData: public WModuleOutputConnector
{
public:

    /** 
     * Constructor.
     * 
     * \param module the module which is owner of this connector.
     * \param name The name of this connector.
     * \param description Short description of this connector.
     */
    WModuleOutputData( boost::shared_ptr<WModule> module, std::string name="", std::string description="" ):
            WModuleOutputConnector( module, name, description )
    {
    };

    /**
     * Destructor.
     */
    virtual ~WModuleOutputData()
    {
    };

    /** 
     * Update the data associated 
     * 
     * \param data 
     */
    virtual void updateData( boost::shared_ptr<T> data )
    {
        m_data=data;

        // broadcast this event
        propagateDataChange();
    };

    /** 
     * Gives the currently set data.
     * 
     * \return the data currently set.
     */
    const boost::shared_ptr<T> getData() const
    {
        return m_data;
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
        if ( dynamic_cast<WModuleInputData<T>* >( con.get() ) )  // NOLINT - since we really need them here
        {
            // NOTE: the upper cast already checked the compatibility completely. WModuleOutputConnector::connectable does the
            // same check again. But since we do not know what checks will be added to WModuleOutputConnector::connectable in the
            // future we forward the call.
            return WModuleOutputConnector::connectable( con );
        }

        return false;
    };

private:

    /** 
     * The data associated with this connector.
     */
    boost::shared_ptr<T> m_data;
};

#endif  // WMODULEOUTPUTDATA_H

