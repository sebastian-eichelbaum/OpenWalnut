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

#ifndef WMFIBERSELECTION_H
#define WMFIBERSELECTION_H

#include <string>

#include <osg/Geode>

#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"

#include "core/dataHandler/WDataSetFibers.h"
#include "core/dataHandler/datastructures/WFiberCluster.h"

/**
 * This module handles selection of fibers based on two volumes of interest. It can filter out ALL fibers which do not go through both VOI.
 * \ingroup modules
 */
class WMFiberSelection: public WModule
{
public:
    /**
     * Default constructor.
     */
    WMFiberSelection();

    /**
     * Destructor.
     */
    virtual ~WMFiberSelection();

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

    /**
     * Get the icon for this module in XPM format.
     * \return The icon.
     */
    virtual const char** getXPMIcon() const;

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
     * Callback for m_active. Overwrite this in your modules to handle m_active changes separately.
     */
    virtual void activate();

private:
    //////////////////////////////////////////////////////////////////////////////////////////////
    // Input Data
    //////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * The first VOI
     */
    boost::shared_ptr< WModuleInputData< WDataSetSingle > > m_voi1Input;

    /**
     * The second VOI.
     */
    boost::shared_ptr< WModuleInputData< WDataSetSingle > > m_voi2Input;

    /**
     * The fiber dataset which is going to be filtered.
     */
    boost::shared_ptr< WModuleInputData< WDataSetFibers > > m_fiberInput;

    /**
     * The output connector used to provide the calculated data to other modules.
     */
    boost::shared_ptr< WModuleOutputData< WDataSetFibers > > m_fiberOutput;

    /**
     * The cluster dataset created from m_fiberOutput.
     */
    boost::shared_ptr< WModuleOutputData< WFiberCluster > > m_clusterOutput;

    /**
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    /**
     * The fiber dataset (from m_fiberInput).
     */
    boost::shared_ptr< WDataSetFibers > m_fibers;

    /**
     * The VOI1 dataset (from m_voi1Input).
     */
    boost::shared_ptr< WDataSetSingle > m_voi1;

    /**
     * The VOI2 dataset (from m_voi2Input).
     */
    boost::shared_ptr< WDataSetSingle > m_voi2;

    //////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    //////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * VOI1 threshold.
     */
    WPropDouble   m_voi1Threshold;

    /**
     * VOI2 threshold.
     */
    WPropDouble   m_voi2Threshold;

    /**
     * Cut the fibers when they are outside the VOI?
     */
    WPropBool     m_cutFibers;

    /**
     * Should the fibers be cut to avoid having them inside the VOI
     */
    WPropBool     m_preferShortestPath;
};

#endif  // WMFIBERSELECTION_H

