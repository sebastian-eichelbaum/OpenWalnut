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

#ifndef WDISCONNECTCOMBINER_H
#define WDISCONNECTCOMBINER_H

#include <list>
#include <map>
#include <string>
#include <utility>

#include <boost/shared_ptr.hpp>

#include "../WModule.h"
#include "../WModuleCombinerTypes.h"
#include "WModuleOneToOneCombiner.h"

#include "../WModuleConnector.h"

#include "../WExportKernel.h"

/**
 * Combiner which disconnects the specified connection.
 */
class OWKERNEL_EXPORT WDisconnectCombiner: public WModuleOneToOneCombiner
{
public:

    /**
     * Creates a combiner which disconnects the specified connection. If the specified connection does not exist (anymore), nothing will happen.
     *
     * \param target            the target container
     * \param srcModule         the module whose connector should be disconnected
     * \param srcConnector      the srcModule connector to disconnect
     * \param targetModule      the module whose connector should be disconnected from srcConnector
     * \param targetConnector   the targetModule connector to disconnect.
     */
    WDisconnectCombiner( boost::shared_ptr< WModuleContainer > target,
                         boost::shared_ptr< WModule > srcModule, std::string srcConnector,
                         boost::shared_ptr< WModule > targetModule, std::string targetConnector );

    /**
     * Creates a combiner which disconnects the specified connection. If the specified connection does not exist (anymore), nothing will happen.
     *
     * \param srcModule         the module whose connector should be disconnected
     * \param srcConnector      the srcModule connector to disconnect
     * \param targetModule      the module whose connector should be disconnected from srcConnector
     * \param targetConnector   the targetModule connector to disconnect.
     */
    WDisconnectCombiner( boost::shared_ptr< WModule > srcModule, std::string srcConnector,
                         boost::shared_ptr< WModule > targetModule, std::string targetConnector );

    /**
     * Creates a combiner which disconnects the specified connection. If the specified connection does not exist (anymore), nothing will happen.
     *
     * \param srcConnector connector 1
     * \param targetConnector connector 2
     */
    WDisconnectCombiner( boost::shared_ptr< WModuleConnector > srcConnector,
                         boost::shared_ptr< WModuleConnector > targetConnector );

    /**
     * Destructor.
     */
    virtual ~WDisconnectCombiner();

    /**
     * Disconnects the specified connection.
     */
    virtual void apply();

protected:

private:
};

#endif  // WDISCONNECTCOMBINER_H

