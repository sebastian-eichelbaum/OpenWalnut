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
#include <sstream>

#include <boost/shared_ptr.hpp>
#include <boost/thread/locks.hpp>

// this is necessary since we have some kind of cyclic includes
template < typename T > class WModuleOutputData;
#include "WModuleOutputData.h"
#include "exceptions/WModuleConnectorUnconnected.h"
#include "../common/WTransferable.h"
#include "../common/WPrototyped.h"

#include "WModuleInputConnector.h"
#include "WModuleOutputConnector.h"

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
    WModuleInputData( boost::shared_ptr< WModule > module, std::string name="", std::string description="" )
        :WModuleInputConnector( module, name, description )
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
     * \return the data currently set. NULL if no data has been sent yet or the connector is unconnected.
     */
    const boost::shared_ptr< T > getData()
    {
        // get a lock
        boost::shared_lock<boost::shared_mutex> lock = boost::shared_lock<boost::shared_mutex>( m_connectionListLock );

        handledUpdate();

        // is there something in the list?
        if ( m_connected.begin() == m_connected.end() )
        {
            lock.unlock();
            return boost::shared_ptr< T >();
        }

        // get data
        boost::shared_ptr< T > dat = boost::shared_dynamic_cast< T >(
                boost::shared_dynamic_cast< WModuleOutputConnector >( *m_connected.begin() )->getRawData()
        );

        // unlock and return
        lock.unlock();

        return dat;
    };

    /**
     * Checks whether the specified connector is an input connector and compatible with T.
     *
     * \param con the connector to check against.
     *
     * \return true if compatible.
     */
    virtual bool connectable( boost::shared_ptr<WModuleConnector> con )
    {
        // NOTE: please consider the following: the input only accepts data which is of type T or higher. So only up casts from
        // con's type T2 to T are needed/allowed what ever

        if ( !WModuleInputConnector::connectable( con ) )
        {
            return false;
        }

        // this calls virtual function to achieve the prototype of the WTransferable created with the type specified in
        // WOutputData< XYZ >
        boost::shared_ptr< WPrototyped > tProto =
            dynamic_cast< WModuleOutputConnector* >( con.get() )->getTransferPrototype(); // NOLINT

        // NOTE: Check the type of the transfered object and whether the connector is an output
        return dynamic_cast< T* >( tProto.get() ); // NOLINT
    };

protected:

private:
};

#endif  // WMODULEINPUTDATA_H

