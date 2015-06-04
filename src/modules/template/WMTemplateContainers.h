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

#ifndef WMTEMPLATECONTAINERS_H
#define WMTEMPLATECONTAINERS_H

#include <string>
#include <vector>

#include <boost/thread.hpp>

#include "core/dataHandler/WDataSetScalar.h"
#include "core/kernel/WModule.h"
#include "core/kernel/WModuleContainer.h"
#include "core/kernel/WModuleInputForwardData.h"
#include "core/kernel/WModuleOutputForwardData.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// If you want to learn how to program a module, refer to WMTemplate.cpp. It is an extensive tutorial on all the details.
// In this tutorial, we assume you already know how to write modules. For other examples, refer to the README file.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * \class WMTemplateContainers
 *
 * A module that explains re-use and re-combination of existing modules. Therefore, we derive from WModuleContainer. A module container itself is
 * a module, but can additionally contain modules for itself. This way, we are able to re-use other modules and re-combine them in a different
 * way. We will be able to connect inputs and outputs of inner modules to our own connectors and, thus, will be able to forward data easily.
 * Please read the WMTemplateContainers.cpp file for all further descriptions.
 *
 * \ingroup modules
 */
class WMTemplateContainers : public WModuleContainer
{
public:
    /**
     * Constuctor.
     */
    WMTemplateContainers();

    /**
     * Destructor.
     */
    virtual ~WMTemplateContainers();

    /**
     * Due to the prototype design pattern used to build modules, this method returns a new instance of this method. NOTE: it
     * should never be initialized or modified in some other way. A simple new instance is required.
     *
     * \return the prototype used to create every module in OpenWalnut.
     */
    virtual boost::shared_ptr< WModule > factory() const;

protected:
    /**
     * Entry point after loading the module. Runs in separate thread.
     */
    virtual void moduleMain();

    /**
     * Initialize the connectors this module is using.
     */
    virtual void connectors();

    /**
     * Initialize the properties for this module.
     */
    virtual void properties();

    /**
     * Initialize requirements for this module.
     */
    virtual void requirements();

private:
    /**
     * Define an input connector, which only forwards the data. This is very useful since we want this to be some kind of "gate" to the outside
     * world. Data going into this connector can be used by a real WModuleInputData connector of an embedded module.
     */
    WModuleInputForwardData< WDataSetScalar >::SPtr  m_input;

    /**
     * Define an output connector, which only forwards the data. This is very useful since we want this to be some kind of "gate" to the outside
     * world. Data going out of this connector is originating from a real WModuleInputData connector of an embedded module.
     */
    WModuleOutputForwardData< WDataSetScalar >::SPtr m_output;
};

#endif  // WMTEMPLATECONTAINERS_H
