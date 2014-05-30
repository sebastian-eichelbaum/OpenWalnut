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

#ifndef WMODULECOMBINER_H
#define WMODULECOMBINER_H

#include <boost/shared_ptr.hpp>

#include "../common/WThreadedRunner.h"

#include "WModuleContainer.h"

/**
 * This is a base class for all module combination classes. The basic idea is to hide the actual combination work from others. These classes may
 * be useful in the GUI. The GUI can create a module combiner instance in a special way, with an interface the GUI wants to have. Then, the
 * kernel can construct the actual module graph out of it. Another purpose is some kind of project file loading. One can write a combiner which
 * loads projects files and creates a module graph out of it. The only think which all the combiners need to know: the target container. Derive
 * all specific combiner classes from this one.
 */
class WModuleCombiner: public WThreadedRunner,
                       public boost::enable_shared_from_this< WModuleCombiner >
{
public:
    /**
     * Creates an empty combiner.
     *
     * \param target the target container where to add the modules to.
     */
    explicit WModuleCombiner( boost::shared_ptr< WModuleContainer > target );

    /**
     * Creates an empty combiner. This constructor automatically uses the kernel's root container as target container.
     */
    WModuleCombiner();

    /**
     * Destructor.
     */
    virtual ~WModuleCombiner();

    /**
     * Apply the internal module structure to the target container. Be aware, that this operation might take some time, as modules can be
     * connected only if they are "ready", which, at least with WDataModule modules, might take some time.
     *
     * \note: to have asynchronous loading, use run().
     */
    virtual void apply() = 0;

    /**
     * Run thread and call apply(). This is useful to apply a combiner asynchronously.
     */
    virtual void run();

protected:
    /**
     * Function that has to be overwritten for execution. It gets executed in a separate thread after run()
     * has been called. It actually calls apply() in another thread.
     */
    virtual void threadMain();

    /**
     * The module container to use for the modules.
     */
    boost::shared_ptr< WModuleContainer > m_container;

private:
};

#endif  // WMODULECOMBINER_H

