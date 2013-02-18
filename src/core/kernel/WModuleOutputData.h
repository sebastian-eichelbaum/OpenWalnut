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

#include "../common/WLogger.h"

// this is necessary since we have some kind of cyclic includes
template < typename T > class WModuleInputData;
#include "WModuleInputData.h"
#include "../common/WPrototyped.h"
#include "../common/WTransferable.h"

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
     * Pointer to this. For convenience.
     */
    typedef boost::shared_ptr< WModuleOutputData< T > > PtrType;

    /**
     * Pointer to this. For convenience.
     */
    typedef boost::shared_ptr< WModuleOutputData< T > > SPtr;

    /**
     * Pointer to this. For convenience.
     */
    typedef boost::shared_ptr< const WModuleOutputData< T > > ConstSPtr;

    /**
     * Reference to this type.
     */
    typedef WModuleOutputData< T >& RefType;

    /**
     * Type of the connector.
     */
    typedef WModuleOutputData< T > Type;

    /**
     * Typedef to the contained transferable.
     */
    typedef T TransferType;

    /**
     * Convenience method to create a new instance of this out data connector with proper type.
     *
     * \param module    the module owning this instance
     * \param name      the name of this connector.
     * \param description the description of this connector.
     *
     * \return the pointer to the created connector.
     */
    static PtrType create( boost::shared_ptr< WModule > module, std::string name = "", std::string description = "" );

    /**
     * Convenience method to create a new instance of this out data connector with proper type and add it to the list of connectors of the
     * specified module.
     *
     * \param module    the module owning this instance
     * \param name      the name of this connector.
     * \param description the description of this connector.
     *
     * \return the pointer to the created connector.
     */
    static PtrType createAndAdd( boost::shared_ptr< WModule > module, std::string name = "", std::string description = "" );

    /**
     * Constructor.
     *
     * \param module the module which is owner of this connector.
     * \param name The name of this connector.
     * \param description Short description of this connector.
     */
    WModuleOutputData( boost::shared_ptr< WModule > module, std::string name = "", std::string description = "" )
        :WModuleOutputConnector( module, name, description )
    {
        m_data = boost::shared_ptr< T >();
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
     * \param data the data do send
     */
    virtual void updateData( boost::shared_ptr< T > data )
    {
        m_data = data;

        // broadcast this event
        triggerUpdate();
    };

    /**
     * Resets the data on this output. It actually sets NULL and triggers an update.
     */
    virtual void reset()
    {
        updateData( boost::shared_ptr< T >() );
    }

    /**
     * This method simply propagates an update but does not actually change the data.
     */
    virtual void triggerUpdate()
    {
        // broadcast this event
        propagateDataChange();
    };

    /**
     * Gives back the currently set data as WTransferable.
     *
     * \return the data. If no data has been set: a NULL pointer is returned.
     */
    virtual const boost::shared_ptr< WTransferable > getRawData() const
    {
        return m_data;
    };

    /**
     * Gives back the currently set data.
     *
     * \return the data. If no data has been set: a NULL pointer is returned.
     */
    const boost::shared_ptr< T > getData() const
    {
        return m_data;
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
        // since WModuleInputData::connectable already does all the type checking, we simply forward the call
        return WModuleOutputConnector::connectable( con );
    };

    /**
     * Returns the prototype of the Type T used in this connector.
     *
     * \return the prototype of the transfered type.
     */
    virtual boost::shared_ptr< WPrototyped > getTransferPrototype()
    {
        // get prototype or the data pointer currently set
        return ( m_data == boost::shared_ptr< T >() ) ? T::getPrototype() : boost::static_pointer_cast< WPrototyped >( m_data );
    };

protected:
private:
    /**
     * The data associated with this connector.
     */
    boost::shared_ptr< T > m_data;
};

template < typename T >
typename WModuleOutputData< T >::PtrType WModuleOutputData< T >::create( boost::shared_ptr< WModule > module, std::string name,
                                                                                                              std::string description )
{
    typedef typename WModuleOutputData< T >::PtrType PTR;
    typedef typename WModuleOutputData< T >::Type TYPE;
    return PTR( new TYPE( module, name, description ) );
}

template < typename T >
typename WModuleOutputData< T >::PtrType WModuleOutputData< T >::createAndAdd( boost::shared_ptr< WModule > module, std::string name,
                                                                                                                    std::string description )
{
    typename WModuleOutputData< T >::PtrType c = create( module, name, description );
    module->addConnector( c );
    return c;
}

#endif  // WMODULEOUTPUTDATA_H

