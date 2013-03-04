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

#ifndef WMFIBERSTOPOINTS_H
#define WMFIBERSTOPOINTS_H

#include <string>

#include "core/dataHandler/WDataSetFibers.h"
#include "core/dataHandler/WDataSetPoints.h"
#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"

/**
 * This modules takes a fiber dataset and extracts its vertices to build a point dataset
 *
 * \ingroup modules
 */
class WMFibersToPoints: public WModule
{
public:
    /**
     * Default constructor.
     */
    WMFibersToPoints();

    /**
     * Destructor.
     */
    virtual ~WMFibersToPoints();

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

private:
    /**
     * The fiber dataset which is going to be used.
     */
    boost::shared_ptr< WModuleInputData< WDataSetFibers > > m_fiberInput;

    /**
     * The output connector used to provide the calculated point data to other modules.
     */
    boost::shared_ptr< WModuleOutputData< WDataSetPoints > > m_pointsOutput;

    /**
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    /**
     * A string which informs the user whether filtering is possible.
     */
    WPropString m_paramHint;

    /**
     * The color to use for the resulting points.
     */
    WPropColor m_color;

    /**
     * Group contains the filtering options
     */
    WPropGroup m_filterGroup;

    /**
     * Filter fib point according to its parameter value and this value.
     */
    WPropDouble m_parametersFilterValue;

    /**
     * Filter width.
     */
    WPropDouble m_parametersFilterWidth;

    /**
     * fiber parameter max
     */
    double m_parameterMax;

    /**
     * fiber parameter min
     */
    double m_parameterMin;
};

#endif  // WMFIBERSTOPOINTS_H

