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

#ifndef WMDATASETMANIPULATOR_H
#define WMDATASETMANIPULATOR_H

#include <string>

#include "core/dataHandler/WDataSet.h"

#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"

#include "core/common/WStrategyHelper.h"
#include "core/common/WObjectNDIP.h"

/**
 * Module allowing manipulation of the dataset scaling, orientation and position.
 *
 * \ingroup modules
 */
class WMDatasetManipulator : public WModule
{
public:
    /**
     * Constructor.
     */
    WMDatasetManipulator();

    /**
     * Destructor.
     */
    virtual ~WMDatasetManipulator();

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

private:
    /**
     * Init the matrix to be applied from the transformations stored in the current dataset.
     */
    void initMatrix();

    /**
     * Transform the data using the provided transformation matrix.
     *
     * \param mat The Transform to apply.
     *
     * \return The new and transformed dataset.
     */
    boost::shared_ptr< WDataSet > transformData( WMatrixFixed< double, 4, 4 > const& mat );

    //! A condition for property updates.
    boost::shared_ptr< WCondition > m_propCondition;

    //! A trigger to reset the current transformation to the initial one.
    WPropTrigger m_resetTrigger;

    //! A trigger to add the transformation from the currently active strategy to the current transformation.
    WPropTrigger m_applyTrigger;

    //! The current data.
    boost::shared_ptr< WDataSet > m_data;

    //! The current transformation.
    WMatrixFixed< double, 4, 4 > m_currentMat;

    //! An input connector that accepts any dataset.
    boost::shared_ptr< WModuleInputData< WDataSet > > m_input;

    //! An output connector for the transformed dataset.
    boost::shared_ptr< WModuleOutputData< WDataSet > > m_output;

    //! The strategy to use for dataset transformation manipulation.
    WStrategyHelper< WObjectNDIP< WManipulatorInterface > > m_strategy;
};

#endif  // WMDATASETMANIPULATOR_H
