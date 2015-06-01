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

#ifndef WMFIBERFILTERINDEX_H
#define WMFIBERFILTERINDEX_H

#include <string>

#include "core/dataHandler/WDataSetFibers.h"
#include "core/kernel/WFiberSelector.h"

#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"

/**
 * This module is intended to allow the user to filter a fiber dataset using the current ROI config
 *
 * \ingroup modules
 */
class WMFiberFilterIndex: public WModule
{
public:
    /**
     * Default constructor.
     */
    WMFiberFilterIndex();

    /**
     * Destructor.
     */
    virtual ~WMFiberFilterIndex();

    /**
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Gives back a description of this module.
     * \return description to module.
     */
    virtual const std::string getDescription() const;

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

private:
    /**
     * An input for the fiber data.
     */
    boost::shared_ptr< WModuleInputData< WDataSetFibers > > m_input;

    /**
     * The output connector for the filtered data.
     */
    boost::shared_ptr< WModuleOutputData< WDataSetFibers > > m_output;

    /**
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    /**
     * Trigger output update
     */
    WPropInt  m_indexProp;

    /**
     * Fiber data.
     */
    WDataSetFibers::SPtr m_fibers;

    /**
     * Updates the output using the ROI configuration.
     */
    void updateOutput();
};

#endif  // WMFIBERFILTERINDEX_H
