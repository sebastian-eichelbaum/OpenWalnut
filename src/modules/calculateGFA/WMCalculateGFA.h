//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV-Leipzig and CNCF-CBS
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

#ifndef WMCALCULATEGFA_H
#define WMCALCULATEGFA_H

#include <string>
#include <vector>

#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"
#include "core/common/WThreadedFunction.h"
#include "core/common/math/WMatrix.h"
#include "core/dataHandler/WThreadedPerVoxelOperation.h"
#include "core/dataHandler/WDataSetSphericalHarmonics.h"
#include "core/dataHandler/WDataSetScalar.h"

/**
 * \class WMCalculateGFA
 *
 * A module that calculates the generalized fractional anisotropy for every voxel of the input dataset.
 *
 * \ingroup modules
 */
class WMCalculateGFA: public WModule
{
    //! a conveniance typedef
    typedef WMCalculateGFA This;

public:
    /**
     * Standard constructor.
     */
    WMCalculateGFA();

    /**
     * Destructor.
     */
    virtual ~WMCalculateGFA();

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
    //! the threaded function type for gfa computation
    typedef WThreadedPerVoxelOperation< double, 15, double, 1 > GFAFuncType;

    //! the threadpool
    typedef WThreadedFunction< GFAFuncType > GFAPoolType;

    /**
     * A function that gets called for every voxel in the input SH-dataset. Calculates a
     * fractional anisotropy measure.
     *
     * \param s An array of SH-coefficients.
     * \return A boost::array of size 1 that contains the result for the given voxel.
     */
    boost::array< double, 1 > perVoxelGFAFunc( WValueSet< double >::SubArray const& s );

    /**
     * Reset the threaded functions.
     */
    void resetGFAPool();

    /**
     * Handle an exception thrown by a worker thread.
     *
     * \param e The exception that was thrown during multithreaded computation.
     */
    void handleException( WException const& e );

    /**
     * Reset the progress indicator in the ui.
     *
     * \param todo The number of steps needed to complete the job.
     */
    void resetProgress( std::size_t todo );

    //! A pointer to the input dataset.
    boost::shared_ptr< WDataSetSphericalHarmonics > m_dataSet;

    //! The output dataset.
    boost::shared_ptr< WDataSetScalar > m_result;

    //! The output Connector.
    boost::shared_ptr< WModuleOutputData< WDataSetScalar > > m_output;

    //! The input Connector for the SH data.
    boost::shared_ptr< WModuleInputData< WDataSetSphericalHarmonics > > m_input;

    //! The object that keeps track of the current progress.
    boost::shared_ptr< WProgress > m_currentProgress;

    //! The last exception thrown by any worker thread.
    boost::shared_ptr< WException > m_lastException;

    //! Condition indicating if any exception was thrown.
    boost::shared_ptr< WCondition > m_exceptionCondition;

    //! The threaded function object.
    boost::shared_ptr< GFAFuncType > m_gfaFunc;

    //! The threadpool.
    boost::shared_ptr< GFAPoolType > m_gfaPool;

    //! A matrix of SH base function values for various gradients.
    WMatrix<double> m_BMat;
};

#endif  // WMCALCULATEGFA_H
