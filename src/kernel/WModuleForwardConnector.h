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

#ifndef WMODULEFORWARDCONNECTOR_H
#define WMODULEFORWARDCONNECTOR_H

#include <boost/shared_ptr.hpp>

#include "WModuleInputConnector.h"
#include "WModuleOutputConnector.h"

/**
 * This is a simple class which forwards input connectors to input connectors, and output connectors to output connectors.
 */
class WModuleForwardConnector
{
public:

    /**
     * Create a forwarder which forwards all operations on a input connector to another input connector.
     *
     * \param from the connector whose data should be forwarded
     * \param to the connector receiving forwarded data
     */
    WModuleForwardConnector( boost::shared_ptr< WModuleInputConnector > from,  boost::shared_ptr< WModuleInputConnector > to );

    /**
     * Create a forwarder which forwards all operations on a output connector to another output connector.
     *
     * \param from the connector whose data should be forwarded
     * \param to the connector that gets the data
     */
    WModuleForwardConnector( boost::shared_ptr< WModuleOutputConnector > from,  boost::shared_ptr< WModuleOutputConnector > to );

    /**
     * Destructor.
     */
    virtual ~WModuleForwardConnector();

protected:

    /**
     * The input connector which grabs the data
     */
    boost::shared_ptr< WModuleOutputConnector > m_in;

    /**
     * The output connector which collects data
     */
    boost::shared_ptr< WModuleOutputConnector > m_out;

private:
};

#endif  // WMODULEFORWARDCONNECTOR_H

