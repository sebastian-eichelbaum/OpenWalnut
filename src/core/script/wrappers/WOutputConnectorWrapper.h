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

#ifndef WOUTPUTCONNECTORWRAPPER_H
#define WOUTPUTCONNECTORWRAPPER_H

#include <boost/shared_ptr.hpp>

#include "../../kernel/WModuleOutputConnector.h"

/**
 * \class WOutputConnectorWrapper
 *
 * A wrapper for output connectors.
 */
class WOutputConnectorWrapper
{
public:
    /**
     * Constructor.
     *
     * \param conn The input connector.
     */
    explicit WOutputConnectorWrapper( boost::shared_ptr< WModuleOutputConnector > conn );

    /**
     * Disconnect this connector.
     */
    void disconnect();

    /**
     * Get the pointer to the actual connector.
     *
     * \return The pointer to the connector.
     */
    boost::shared_ptr< WModuleOutputConnector > getConnectorPtr();

private:
    //! The actual input connector.
    boost::shared_ptr< WModuleOutputConnector > m_conn;
};

#endif  // WOUTPUTCONNECTORWRAPPER_H
