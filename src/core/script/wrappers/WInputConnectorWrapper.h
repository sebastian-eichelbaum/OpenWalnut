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

#ifndef WINPUTCONNECTORWRAPPER_H
#define WINPUTCONNECTORWRAPPER_H

#include <boost/shared_ptr.hpp>

#include "../../kernel/WModuleInputConnector.h"

#include "WOutputConnectorWrapper.h"

/**
 * \class WInputConnectorWrapper
 *
 * A wrapper for input connectors.
 */
class WInputConnectorWrapper
{
public:
    /**
     * Constructor.
     *
     * \param conn The input connector.
     */
    explicit WInputConnectorWrapper( boost::shared_ptr< WModuleInputConnector > conn );

    /**
     * Connect this input connector to an output connector.
     *
     * \param conn The output connector to connect to.
     */
    void connect( WOutputConnectorWrapper conn );

    /**
     * Disconnect this connector.
     */
    void disconnect();

    /**
     * Wait for new input.
     */
    void waitForInput();

private:
    //! The actual input connector.
    boost::shared_ptr< WModuleInputConnector > m_conn;
};

#endif  // WINPUTCONNECTORWRAPPER_H
