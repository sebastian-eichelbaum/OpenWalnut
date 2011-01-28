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

#ifndef WMDETERMINISTICFTMORI_H
#define WMDETERMINISTICFTMORI_H

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include "../../kernel/WModule.h"
#include "../../common/math/WVector3D.h"
#include "../../common/WThreadedFunction.h"
#include "../../dataHandler/WThreadedPerVoxelOperation.h"
#include "../../dataHandler/WThreadedTrackingFunction.h"
#include "../../dataHandler/WFiberAccumulator.h"

// forward delcarations
class WDataSetFiberVector;
class WDataSetSingle;
template< class T > class WModuleInputData;
template< class T > class WModuleOutputData;

#define WM_MORI_NUM_CORES W_AUTOMATIC_NB_THREADS

/**
 * \class WMDeterministicFTMori
 *
 * This module implements the simple fiber tracking algorithm by Mori et al.
 *
 * S. Mori, B. Crain, V. Chacko, and P. van Zijl,
 * "Three-dimensional tracking of axonal projections in the brain by magnetic resonance imaging",
 * Annals of Neurology 45, pp. 265-269, 1999
 *
 * \ingroup modules
 */
class WMDeterministicFTMori: public WModule
{
    //! the class itself
    typedef WMDeterministicFTMori This;

public:
    /**
     * Standard Constructor.
     */
    WMDeterministicFTMori();

    /**
     * Destructor.
     */
    virtual ~WMDeterministicFTMori();

    /**
     * Returns a new instance of this module.
     *
     * \return A new instance of this module.
     */
    virtual boost::shared_ptr< WModule > factory() const;

    /**
     * Get the icon for this module in XPM format.
     */
    virtual const char** getXPMIcon() const;

    /**
     * Return the name of this module.
     *
     * \return The name of this module.
     */
    virtual const std::string getName() const;

    /**
     * Return the description of this module.
     *
     * \return This module's description.
     */
    virtual const std::string getDescription() const;

protected:

    /**
     * The worker function, runs in its own thread.
     */
    virtual void moduleMain();

    /**
     * Initialize the module's connectors.
     */
    virtual void connectors();

    /**
     * Initialize the module's properties.
     */
    virtual void properties();

    /**
     * Callback. This function gets called whenever the modules "active" state changes.
     */
    virtual void activate();

private:

    //! the threaded per-voxel function for the eigenvector computation
    typedef WThreadedPerVoxelOperation< float, 6, double, 4 > TPVO;

    //! the thread pool type for the eigencomputation
    typedef WThreadedFunction< TPVO > EigenFunctionType;

    //! the input of the per-voxel operation
    typedef TPVO::TransmitType EigenInArrayType;

    //! the output of the per-voxel operation
    typedef TPVO::OutTransmitType EigenOutArrayType;

    //! the valueset type
    typedef WValueSet< double > FloatValueSetType;

    //! the fiber type
    typedef std::vector< wmath::WVector3D > FiberType;

    //! the threaded tracking functor
    typedef wtracking::WThreadedTrackingFunction Tracking;

    //! the tracking threadpool
    typedef WThreadedFunction< Tracking > TrackingFuncType;

    /**
     * The function that computes the eigenvectors from the input tensor field.
     *
     * \param input A subarray of a valueset that consists of the 6 floats that make up the tensor.
     * \return The components of the largest eigenvector and the fa value in a 4-double array.
     */
    EigenOutArrayType const eigenFunc( EigenInArrayType const& input );

    /**
     * Calculate the direction of the eigenvector with largest magnitude.
     *
     * \param ds The dataset.
     * \param j The job, that means the current position and direction of the last fiber segment.
     *
     * \return The direction to follow.
     */
    wmath::WVector3D getEigenDirection( boost::shared_ptr< WDataSetSingle const > ds,
                                        wtracking::WTrackingUtility::JobType const& j );

    /**
     * The fiber visitor. Adds a fiber to the result data and increment the progress.
     *
     * \param f The fiber.
     */
    void fiberVis( FiberType const& f );

    /**
     * The point visitor. Does nothing.
     */
    void pointVis( wmath::WVector3D const& );

    /**
     * Reset the tracking function and abort the current one, if there is a current one.
     */
    void resetTracking();

    /**
     * Resets the threaded function/threadpool.
     */
    void resetEigenFunction();

    /**
     * Resets the current progress to 0.
     *
     * \param todo The number of operations of the new progress.
     */
    void resetProgress( std::size_t todo );

    //! A condition for property changes.
    boost::shared_ptr< WCondition > m_propCondition;

    //! A pointer to the input tensor dataset.
    boost::shared_ptr< WDataSetSingle > m_dataSet;

    //! The output dataset. Stores all fibers extracted from the input tensor field.
    boost::shared_ptr< WDataSetFibers > m_fiberSet;

    //! The output Connector.
    boost::shared_ptr< WModuleOutputData< WDataSetFibers > > m_output;

    //! The input Connector.
    boost::shared_ptr< WModuleInputData< WDataSetSingle > > m_input;

    //! Stores eigenvectors and fractional anisotropy of the input dataset.
    boost::shared_ptr< WDataSetSingle > m_eigenField;

    //! the functor used for the calculation of the eigenvectors
    boost::shared_ptr< TPVO > m_eigenOperation;

    //! the object that keeps track of the current progress
    boost::shared_ptr< WProgress > m_currentProgress;

    //! The threadpool for the eigenvector and fa computations.
    boost::shared_ptr< EigenFunctionType > m_eigenPool;

    //! The threadpool for the tracking
    boost::shared_ptr< TrackingFuncType > m_trackingPool;

    //! The fiber accumulator
    WFiberAccumulator m_fiberAccu;

    //! The minimum FA property.
    WPropDouble m_minFA;

    //! The minimum number of points property.
    WPropInt m_minPoints;

    //! The minimum cosine property.
    WPropDouble m_minCos;

    //! The current minimum FA property.
    double m_currentMinFA;

    //! The current minimum number of points property.
    std::size_t m_currentMinPoints;

    //! The current minimum cosine property.
    double m_currentMinCos;
};

#endif  // WMDETERMINISTICFTMORI_H
