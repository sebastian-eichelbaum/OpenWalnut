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

#ifndef WMODULEINPUTFORWARDDATA_H
#define WMODULEINPUTFORWARDDATA_H

#include <string>

#include <boost/shared_ptr.hpp>

#include "../common/WLogger.h"

#include "WModuleInputData.h"
#include "WModuleOutputData.h"

/**
 * This is a simple class which forwards input data to input data connectors. It itself is a input data connector and can be used
 * as one, but also provides the possibility to forward data changes to other input data connectors, using a separate output data
 * connector (which is not visible to the outside world).
 */
template< typename T >
class WModuleInputForwardData: public WModuleInputData< T >
{
public:
    /**
     * Pointer to this. For convenience.
     */
    typedef boost::shared_ptr< WModuleInputForwardData< T > > PtrType;

    /**
     * Reference to this type.
     */
    typedef WModuleInputForwardData< T >& RefType;

    /**
     * Type of the connector.
     */
    typedef WModuleInputForwardData< T > Type;

    /**
     * Typedef to the contained transferable.
     */
    typedef T TransferType;

    /**
     * Constructor. This creates a new input data connector which is able to forward data changes <b>TO</b>  other input data connectors.
     *
     * \param module the module which is owner of this connector.
     * \param name The name of this connector.
     * \param description Short description of this connector.
     */
    WModuleInputForwardData( boost::shared_ptr< WModule > module, std::string name="", std::string description="" )
        :WModuleInputData< T >( module, name, description )
    {
        // initialize the output data connector
        m_out = boost::shared_ptr< WModuleOutputData< T > >( new WModuleOutputData< T >( module, "[FWD]" + name, description ) );
    };

    /**
     * Destructor.
     */
    virtual ~WModuleInputForwardData()
    {
    }

    /**
     * Forward the input to the specified input. The specified input must be compatible with the template parameter of this input.
     *
     * \param to the input connector to forward data to.
     */
    virtual void forward( boost::shared_ptr< WModuleConnector > to )
    {
        m_out->connect( to );
    }

    /**
     * Remove the specified connector from the forwarding list.
     *
     * \param to the input connector to be removed from forwarding list.
     */
    virtual void unforward( boost::shared_ptr< WModuleConnector > to )
    {
        m_out->disconnect( to );
    }

    /**
     * Convenience method to create a new instance of this in forward data connector with proper type.
     *
     * \param module    The module owning this instance
     * \param name      The name of this connector.
     * \param description The description of this connector.
     *
     * \return The pointer to the created forward connector.
     */
    static PtrType create( boost::shared_ptr< WModule > module, std::string name = "", std::string description = "" );

    /**
     * Convenience method to create a new instance of this in forward data connector with proper
     * type and add it to the list of connectors of the specified module.
     *
     * \param module    The module owning this instance
     * \param name      The name of this connector.
     * \param description The description of this connector.
     *
     * \return The pointer to the created forward connector.
     */
    static PtrType createAndAdd( boost::shared_ptr< WModule > module, std::string name = "", std::string description = "" );

protected:
    /**
     * The output connector which collects data and distributes it to all connectors connected using the forwardTo() method.
     */
    boost::shared_ptr< WModuleOutputData< T > > m_out;

    /**
     * Gets called whenever a connected output updates its data. This method uses this callback to update the m_out connector to
     * inform all inputs to which the data should be forwarded.
     *
     * \param input     the input connector receiving the change
     * \param output    the output connector sending the change
     */
    virtual void notifyDataChange( boost::shared_ptr<WModuleConnector> input, boost::shared_ptr<WModuleConnector> output )
    {
        m_out->updateData( WModuleInputData< T >::getData() );

        // simply forward the call
        WModuleInputData< T >::notifyDataChange( input, output );
    }

    /**
     * Gets called whenever a connection between a remote and local connector gets closed. This is used here to forward the NULL data.
     *
     * \param here the connector of THIS module getting disconnected.
     * \param there the connector of the other module getting disconnected.
     */
    virtual void notifyConnectionClosed( boost::shared_ptr<WModuleConnector> here, boost::shared_ptr<WModuleConnector> there )
    {
        m_out->reset();

        // simply forward the call
        WModuleInputData< T >::notifyConnectionClosed( here, there );
    }

private:
};

template < typename T >
inline typename WModuleInputForwardData< T >::PtrType WModuleInputForwardData< T >::create( boost::shared_ptr< WModule > module,
                                                                                            std::string name,
                                                                                            std::string description )
{
    return typename WModuleInputForwardData< T >::PtrType ( new WModuleInputForwardData< T >( module, name, description ) );
}

template < typename T >
inline typename WModuleInputForwardData< T >::PtrType WModuleInputForwardData< T >::createAndAdd( boost::shared_ptr< WModule > module,
                                                                                                  std::string name,
                                                                                                  std::string description )
{
    typename WModuleInputForwardData< T >::PtrType c = create( module, name, description );
    module->addConnector( c );
    return c;
}


#endif  // WMODULEINPUTFORWARDDATA_H

