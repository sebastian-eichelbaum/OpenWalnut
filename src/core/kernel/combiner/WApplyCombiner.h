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

#ifndef WAPPLYCOMBINER_H
#define WAPPLYCOMBINER_H

#include <list>
#include <map>
#include <string>
#include <utility>

#include <boost/shared_ptr.hpp>

#include "../WModule.h"
#include "../WModuleCombinerTypes.h"
#include "WModuleOneToOneCombiner.h"

#include "../WModuleInputConnector.h"
#include "../WModuleOutputConnector.h"


/**
 * Base class for all combiners which apply one connection between two connectors of two modules.
 */
class  WApplyCombiner: public WModuleOneToOneCombiner
{
public:
    /**
     * Creates a combiner which sets up the specified modules and prototype combination. Specifying a NULL pointer to the srcModule parameter
     * causes the combiner to only add the target module without any connections. This is especially useful for modules which do not provide any
     * input which must be connected. It is possible to specify prototypes here. The will get created upon apply.
     *
     *
     * \param target            the target container
     * \param srcModule         the module whose output should be connected with the prototypes input
     * \param srcConnector      the output connector of the module
     * \param targetModule      the module/prototype to use for connecting the module with
     * \param targetConnector   the input connector of the prototype to connect with srcConnector.
     */
    WApplyCombiner( boost::shared_ptr< WModuleContainer > target,
                    WModule::SPtr srcModule, std::string srcConnector,
                    WModule::SPtr targetModule, std::string targetConnector );

    /**
     * Creates a combiner which sets up the specified modules and prototype combination. This constructor automatically uses the kernel's root
     * container as target container. Specifying a NULL pointer to the srcModule parameter
     * causes the combiner to only add the target module without any connections. This is especially useful for modules which do not provide any
     * input which must be connected. It is possible to specify prototypes here. The will get created upon apply.
     *
     * \param srcModule         the module whose output should be connected with the prototypes input
     * \param srcConnector      the output connector of the module
     * \param targetModule      the module/prototype to use for connecting the module with
     * \param targetConnector   the input connector of the prototype to connect with srcConnector.
     */
    WApplyCombiner( WModule::SPtr srcModule, std::string srcConnector,
                    WModule::SPtr targetModule, std::string targetConnector );

    /**
     * Creates a combiner which only adds the given module. This constructor automatically uses the kernel's root
     * container as target container. Specifying a NULL pointer to the srcModule parameter
     * causes the combiner to only add the target module without any connections. This is especially useful for modules which do not provide any
     * input which must be connected. It is possible to specify prototypes here. The will get created upon apply.
     *
     * \param module the module to add
     */
    WApplyCombiner( WModule::SPtr module );

    /**
     * Destructor.
     */
    virtual ~WApplyCombiner();

    /**
     * Apply the internal module structure to the target container. Be aware, that this operation might take some time, as modules can be
     * connected only if they are "ready", which, at least with WMData modules, might take some time. It applies the loaded project file.
     */
    virtual void apply();

    /**
     * This method creates a list of possible combiners for connections between the specified modules. Both modules can be prototypes. This
     * method lists only connections from module1's outputs to module2's inputs.
     *
     * \param module1 the first module
     * \param module2 the second module
     *
     * \return the list of combiner for one-to-one connections
     */
    template < typename T >
    static WCombinerTypes::WOneToOneCombiners createCombinerList( WModule::SPtr module1, WModule::SPtr module2 )
    {
        // this list contains all connections for the current module with the other one
        WCombinerTypes::WOneToOneCombiners lComp;

        // get offered outputs
        WModule::OutputConnectorList cons = module1->getOutputConnectors();

        // get connectors of this prototype
        WModule::InputConnectorList pcons = module2->getInputConnectors();

        // ensure we have 1 connector
        if( ( pcons.size() == 0 ) || ( cons.size() == 0 ) )
        {
            return lComp;
        }

        // iterate connector list, first find all matches of the output connectors with all inputs
        for( WModule::OutputConnectorList::const_iterator outIter = cons.begin(); outIter != cons.end(); ++outIter )
        {
            // now go through each input iterator of the current prototype
            for( WModule::InputConnectorList::const_iterator inIter = pcons.begin(); inIter != pcons.end(); ++inIter )
            {
                // compatible?
                if( ( *outIter )->connectable( *inIter ) &&  ( *inIter )->connectable( *outIter ) )
                {
                    // create a apply-prototype combiner
                    lComp.push_back( boost::shared_ptr< WApplyCombiner >(
                        new T( module1, ( *outIter )->getName(), module2, ( *inIter )->getName() ) )
                    );

                    // wlog::debug( "ModuleFactory" ) << ( *outIter )->getCanonicalName() << " -> " << ( *inIter )->getCanonicalName();
                }
            }
        }

        return lComp;
    }

protected:
private:
};

#endif  // WAPPLYCOMBINER_H

