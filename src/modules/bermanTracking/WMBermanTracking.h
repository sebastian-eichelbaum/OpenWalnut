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

#ifndef WMBERMANTRACKING_H
#define WMBERMANTRACKING_H

#include <string>
#include <vector>

#include <boost/random/mersenne_twister.hpp>

#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"
#include "core/common/WThreadedFunction.h"
#include "core/dataHandler/WDataSetSphericalHarmonics.h"
#include "core/dataHandler/WDataSetFibers.h"
#include "core/dataHandler/WDataSetRawHARDI.h"
#include "core/dataHandler/WDataSetScalar.h"
#include "core/dataHandler/WThreadedTrackingFunction.h"
#include "core/dataHandler/WFiberAccumulator.h"

// forward declarations
class WROIBox;

/**
 * Implements the probabilistic tracking algorithm by Berman at al.
 *
 * Jeffrey I. Berman, SungWon Chung, Pratik Mukherjee, Christopher P. Hess, Eric T. Han, Roland G. Henry,
 * "Probabilistic streamline q-ball tractography using the residual bootstrap",
 * NeuroImage, Volume 39, Issue 1, 1 January 2008, Pages 215-222
 *
 * \ingroup modules
 */
class WMBermanTracking: public WModule
{
    //! A convenient typedef.
    typedef WMBermanTracking This;

public:
    /**
     * Standard constructor.
     */
    WMBermanTracking();

    /**
     * Standard destructor.
     */
    virtual ~WMBermanTracking();

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
     *
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
    //! the threaded tracking functor
    typedef wtracking::WThreadedTrackingFunction Tracking;

    //! the tracking threadpool
    typedef WThreadedFunction< Tracking > TrackingFuncType;

    //! a random number generator
    typedef boost::mt19937 Random;

    /**
     * Reset the tracking function object.
     */
    void resetTracking();

    /**
     * Reset the progress to zero and set a new maximum.
     *
     * \param todo The number of jobs to complete.
     */
    void resetProgress( std::size_t todo );

    /**
     * A function that calculates the direction in which to continue fiber integration.
     *
     * \param j The job.
     * \return The direction in which to continue.
     */
    WVector3d getDirFunc( boost::shared_ptr< WDataSetSingle const >, wtracking::WTrackingUtility::JobType const& j );

    /**
     * The fiber visitor. Gets called whenever the tracking function finishes integrating a fiber.
     *
     * \param fiber The fiber.
     */
    void fiberVis( std::vector< WVector3d > const& fiber );

    /**
     * The point visitor. Gets called whenever the tracking function finds a new point for a fiber.
     */
    void pointVis( WVector3d const& );

    /**
     * Handle an exception that was thrown by the tracking function in any worker thread.
     *
     * \param e The exception that was thrown.
     */
    void handleException( WException const& e );

    /**
     * Calculate the spherical harmonics fitting matrix.
     */
    void calcSHFittingMatrix();

    /**
     * Estimates fiber bundle directions from the SH-coefficients of a voxel and finds the direction that best
     * matches the incoming integration direction.
     *
     * \param h A spherical harmonic.
     * \param j The current job.
     * \return The best direction or a zero-vector, if no direction is of any use.
     */
    WVector3d getBestDirectionFromSH( WSymmetricSphericalHarmonic const& h, wtracking::WTrackingUtility::JobType const& j );

    /**
     * Use bootstrapping to create a random ODF for this voxel.
     *
     * \param i The number of the voxel.
     * \return A random spherical harmonic.
     */
    WSymmetricSphericalHarmonic createRandomODF( std::size_t i );

    //! Stores the gfa measure for the input data.
    boost::shared_ptr< WDataSetScalar > m_gfa;

    //! Stores a matrix representing the funk-radon-transform.
    WMatrix_2 m_frtMat;

    //! The matrix use for fitting SH-coeffs to a HARDI measurement.
    WMatrix_2 m_SHFittingMat;

    //! A matrix that produces a fitted HARDI measurement from an 'unfitted' one.
    WMatrix_2 m_HMat;

    //! A matrix of SH-base function values.
    WMatrix_2 m_BMat;

    //! A ROI used for selecting seed voxels.
    boost::shared_ptr< WROIBox > m_seedROI;

    //! A random number generator.
    boost::shared_ptr< Random > m_random;

    //! A condition for property changes.
    boost::shared_ptr< WCondition > m_propCondition;

    //! A pointer to the input dataset.
    boost::shared_ptr< WDataSetSphericalHarmonics > m_dataSet;

    //! A pointer to the input residual data.
    boost::shared_ptr< WDataSetRawHARDI > m_dataSetResidual;

    //! The output dataset.
    boost::shared_ptr< WDataSetScalar > m_result;

    //! Stores the number of hits for a voxel.
    boost::shared_ptr< std::vector< float > > m_hits;

    //! A resulting fiber dataset.
    boost::shared_ptr< WDataSetFibers > m_fiberSet;

    //! The output Connector.
    boost::shared_ptr< WModuleOutputData< WDataSetScalar > > m_output;

    //! The fiber output, used for testing.
    boost::shared_ptr< WModuleOutputData< WDataSetFibers > > m_outputFibers;

    //! The input Connector for the SH data.
    boost::shared_ptr< WModuleInputData< WDataSetSphericalHarmonics > > m_input;

    //! The input for the residual values.
    boost::shared_ptr< WModuleInputData< WDataSetRawHARDI > > m_inputResidual;

    //! The input for the gfa.
    boost::shared_ptr< WModuleInputData< WDataSetScalar > > m_inputGFA;

    //! The threadpool for the tracking.
    boost::shared_ptr< TrackingFuncType > m_trackingPool;

    //! The object that keeps track of the current progress.
    boost::shared_ptr< WProgress > m_currentProgress;

    //! The last exception thrown by any worker thread.
    boost::shared_ptr< WException > m_lastException;

    //! condition indicating if any exception was thrown.
    boost::shared_ptr< WCondition > m_exceptionCondition;

    //! The fiber accumulator.
    WFiberAccumulator m_fiberAccu;

    //! The minimum FA property.
    WPropDouble m_minFA;

    //! The minimum number of points property.
    WPropInt m_minPoints;

    //! The minimum cosine property.
    WPropDouble m_minCos;

    //! The number of seeds per voxel and direction.
    WPropInt m_seedsPerDirection;

    //! The number of seeds per position in a voxel.
    WPropInt m_seedsPerPosition;

    //! Whether the harmonics should be randomized via bootstrapping.
    WPropBool m_probabilistic;

    //! Start the tracking.
    WPropTrigger m_startTrigger;

    //! The current minimum FA property.
    double m_currentMinFA;

    //! The current minimum number of points property.
    std::size_t m_currentMinPoints;

    //! The current minimum cosine property.
    double m_currentMinCos;

    //! The current probabilistic property.
    bool m_currentProbabilistic;

    //! A parameter for the fiber bundle detection.
    WPropDouble m_epsImpr;

    //! A parameter for the fiber bundle detection.
    WPropDouble m_ratio;

    //! A temporary value.
    double m_currentEpsImpr;

    //! A temporary value.
    double m_currentRatio;
};

#endif  // WMBERMANTRACKING_H
