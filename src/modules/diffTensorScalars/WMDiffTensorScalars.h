//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2015 OpenWalnut Community, Nemtics, BSV@Uni-Leipzig
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

#ifndef WMDIFFTENSORSCALARS_H
#define WMDIFFTENSORSCALARS_H

#include <string>

#include <boost/shared_ptr.hpp>

#include "core/common/WObjectNDIP.h"
#include "core/common/WStrategyHelper.h"
#include "core/dataHandler/WDataSetDTI.h"
#include "core/dataHandler/WDataSetScalar.h"
#include "core/dataHandler/WDataSetVector.h"
#include "core/kernel/WModule.h"
#include "core/kernel/WModuleContainer.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleInputForwardData.h"
#include "core/kernel/WModuleOutputForwardData.h"
#include "WDataSetDTIToScalar_I.h"

/**
 * Computes a scalar dataset for a given tensor dataset.
 *
 * \ingroup modules
 */
class WMDiffTensorScalars: public WModuleContainer
{
public:
    /**
     * Default constructor.
     */
    WMDiffTensorScalars();

    /**
     * Destructor.
     */
    virtual ~WMDiffTensorScalars();

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
     * Create and initialize submodule instances, wires them and forward connectors as well as some properties.
     */
    virtual void initSubModules();

private:
    /**
     * Input connector required by this module.
     */
    boost::shared_ptr< WModuleInputForwardData< WDataSetDTI > > m_tensorsIC;

    /**
     * Internal input connector for the Eigenvalues computed by the submodule EigenSystem.
     */
    boost::shared_ptr< WModuleInputData< WDataSetVector > > m_evalsIC;

    /**
     * Dataset for the Eigenvalues.
     */
    boost::shared_ptr< WDataSetVector > m_evals;

    /**
     * Dataset for the Tensors.
     */
    boost::shared_ptr< WDataSetDTI > m_tensors;

    /**
     * Output connector for the computed scalars.
     */
    boost::shared_ptr< WModuleOutputData< WDataSetScalar > > m_scalarOC;

    /**
     * Submodule doing computation of eigenvalues and eigenvectors.
     * This uses WMEigenSystem.
     */
    boost::shared_ptr< WModule > m_eigenSystem;

    WStrategyHelper< WObjectNDIP< WDataSetDTIToScalar_I > > m_strategy; //!< the strategy currently active.
};

#endif  // WMDIFFTENSORSCALARS_H
