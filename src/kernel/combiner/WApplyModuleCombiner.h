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

#ifndef WAPPLYMODULECOMBINER_H
#define WAPPLYMODULECOMBINER_H

#include <list>
#include <map>
#include <string>
#include <utility>

#include <boost/shared_ptr.hpp>

#include "../WModule.h"
#include "WApplyCombiner.h"

/**
 * This class combines an existing module with an specified module. The connections to use must be explicitly known.
 */
class WApplyModuleCombiner: public WApplyCombiner
{
public:

    /**
     * Creates a combiner which sets up the specified module-to-module connection. Specifying a NULL pointer to the srcModule parameter
     * causes the combiner to only add the target module without any connections. This is especially useful for modules which do not provide any
     * input which must be connected.
     *
     *
     * \param target            the target container
     * \param srcModule         the module whose output should be connected with the target module input
     * \param srcConnector      the output connector of the module
     * \param targetModule      the module to use for connecting the module with
     * \param targetConnector   the input connector of the targetModule to connect with srcConnector.
     */
    WApplyModuleCombiner( boost::shared_ptr< WModuleContainer > target,
                             boost::shared_ptr< WModule > srcModule, std::string srcConnector,
                             boost::shared_ptr< WModule > targetModule, std::string targetConnector );

    /**
     * Creates a combiner which sets up the specified module-to-module connection. This constructor automatically uses the kernel's root
     * container as target container. Specifying a NULL pointer to the srcModule parameter
     * causes the combiner to only add the target module without any connections. This is especially useful for modules which do not provide any
     * input which must be connected.
     *
     * \param srcModule         the module whose output should be connected with the target module input
     * \param srcConnector      the output connector of the module
     * \param targetModule      the module to use for connecting the module with
     * \param targetConnector   the input connector of the module to connect with srcConnector.
     */
    WApplyModuleCombiner( boost::shared_ptr< WModule > srcModule, std::string srcConnector,
                             boost::shared_ptr< WModule > targetModule, std::string targetConnector );

    /**
     * Destructor.
     */
    virtual ~WApplyModuleCombiner();

    /**
     * Apply the internal module structure to the target container. Be aware, that this operation might take some time, as modules can be
     * connected only if they are "ready", which, at least with WMData modules, might take some time. It applies the loaded project file.
     */
    virtual void apply();

protected:

private:
};

#endif  // WAPPLYMODULECOMBINER_H

