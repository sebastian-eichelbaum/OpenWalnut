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

#ifndef WMSCALARSEGMENTATION_H
#define WMSCALARSEGMENTATION_H

#include <string>
#include <vector>

#include "core/common/WItemSelection.h"
#include "core/common/WItemSelector.h"

#include "core/dataHandler/WDataHandler.h"

#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"
#include "core/dataHandler/WDataSetScalar.h"

#include "WSegmentationAlgoThreshold.h"

#include "WSegmentationAlgoWatershed.h"
#include "WSegmentationAlgoOtsu.h"
#include "WSegmentationAlgoRegionGrowingConfidenceConnected.h"
#include "WSegmentationAlgoLevelSetCanny.h"

/**
 * First version of a module that implements 3D-image segmentation algorithms.
 *
 * \ingroup modules
 */
class WMScalarSegmentation: public WModule
{
public:

    /**
     * Default constructor.
     */
    WMScalarSegmentation();

    /**
     * Destructor.
     */
    virtual ~WMScalarSegmentation();

    /**
     * Return the name of this module.
     *
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Return a description of this module.
     *
     * \return description to module.
     */
    virtual const std::string getDescription() const;

    /**
     * Return a new instance of this module.
     *
     * \return the prototype used to create every module in OpenWalnut.
     */
    virtual boost::shared_ptr< WModule > factory() const;

    /**
     * Get the icon for this module in XPM format.
     *
     * \return the icon.
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

    //! A List type for all available algorithms.
    typedef std::vector< boost::shared_ptr< WSegmentationAlgo > > AlgoList;

    /**
     * Do a segmentation depending on the current module property values.
     */
    void doSegmentation();

    //! An input connector used to get datasets from other modules.
    boost::shared_ptr< WModuleInputData< WDataSetScalar > > m_input;

    //! The output connector used to provide the calculated data to other modules.
    boost::shared_ptr< WModuleOutputData< WDataSetScalar > > m_output;

    //! This is a pointer to the dataset the module is currently working on.
    boost::shared_ptr< WDataSetScalar > m_dataSet;

    //! This is a pointer to the segmented dataset.
    boost::shared_ptr< WDataSetScalar > m_result;

    //! A condition used to notify about changes in several properties.
    boost::shared_ptr< WCondition > m_propCondition;

    //! The number of the currently selected segmentation method.
    std::size_t m_algoIndex;

    //! A list of possible segmentation algorithms.
    boost::shared_ptr< WItemSelection > m_algoSelection;

    //! A selection box for segmentation algorithms.
    WPropSelection m_algoType;

    //! A list of algorithm objects.
    AlgoList m_algos;
};

#endif  // WMSCALARSEGMENTATION_H
