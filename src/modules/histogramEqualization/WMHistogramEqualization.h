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

#ifndef WMHISTOGRAMEQUALIZATION_H
#define WMHISTOGRAMEQUALIZATION_H

#include <string>

#include "core/dataHandler/WDataSetScalar.h"

#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"

/**
 * This modules takes a dataset and equalizes its histogram.
 *
 * \ingroup modules
 */
class WMHistogramEqualization: public WModule
{
public:
    /**
     * Default constructor.
     */
    WMHistogramEqualization();

    /**
     * Destructor.
     */
    virtual ~WMHistogramEqualization();

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
     * An input connector used to get datasets from other modules. The connection management between connectors must not be handled by the module.
     */
    boost::shared_ptr< WModuleInputData< WDataSetScalar > > m_input;

    /**
     * The output connector used to provide the calculated data to other modules.
     */
    boost::shared_ptr< WModuleOutputData< WDataSetScalar > > m_output;

    /**
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    /**
     * If true, histogram equalization is turned on.
     */
    WPropBool m_equalize;

    /**
     * True if the values should be clamped before further processing
     */
    WPropBool m_clamp;

    /**
     * How many percent should be clamped from the histogram.
     */
    WPropDouble m_clampPerc;

    /**
     * Resolution of the initial histogram.
     */
    WPropInt m_histogramResolution;

    /**
     * Resolution with which the CDF gets calculated.
     */
    WPropInt m_cdfResolution;

    /**
     * Group for keeping all the clamping related props
     */
    WPropGroup m_clamping;

    /**
     * Group for keeping all the equalizing-related props
     */
    WPropGroup m_equalizing;
};

#endif  // WMHISTOGRAMEQUALIZATION_H

