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

#ifndef WMEIGENSYSTEM_H
#define WMEIGENSYSTEM_H

#include <string>
#include <utility>
#include <vector>

#include <osg/Geode>

#include "../../common/math/WTensorFunctions.h"
#include "../../common/WThreadedFunction.h"
#include "../../dataHandler/WThreadedPerVoxelOperation.h"
#include "../../dataHandler/WThreadedTrackingFunction.h"
#include "../../kernel/WModule.h"

// forward declaration
class WDataSetDTI;
class WDataSetVector;
template< class T > class WModuleInputData;
template< class T > class WModuleOutputData;

/**
 * Computes the eigensystem of a second order tensor field. For each eigen vector and eigen value an output connector field is
 * available. If you need all you may connect to a special output carrying all eigenvalues.
 *
 * \ingroup modules
 */
class WMEigenSystem: public WModule
{
public:
    /**
     * Constructs a new module for eigensystem computation.
     */
    WMEigenSystem();

    /**
     * Destructs this module.
     */
    virtual ~WMEigenSystem();

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
     * Initialize requirements for this module.
     */
    virtual void requirements();

private:
    //! the threaded per-voxel function for the eigenvector computation (float input)
    typedef WThreadedPerVoxelOperation< float, 6, double, 12 > TPVOFloat;

    //! the threaded per-voxel function for the eigenvector computation (double input)
    typedef WThreadedPerVoxelOperation< double, 6, double, 12 > TPVODouble;

    //! the thread pool type for the eigencomputation (float input)
    typedef WThreadedFunction< TPVOFloat > EigenFunctionTypeFloat;

    //! the thread pool type for the eigencomputation (double input)
    typedef WThreadedFunction< TPVODouble > EigenFunctionTypeDouble;

    //! the valueset type
    typedef WValueSet< double > FloatValueSetType;

    //! the threaded tracking functor
    typedef wtracking::WThreadedTrackingFunction Tracking;

    //! the tracking threadpool
    typedef WThreadedFunction< Tracking > TrackingFuncType;

    /**
     * Resets the current progress to 0.
     *
     * \param todo The number of operations of the new progress.
     * \param name Identifying what kind of work this progress is indicating.
     */
    void resetProgress( std::size_t todo, std::string name );

    /**
     * Resets the threaded function/threadpool.
     *
     * \param tensors The field of computed eigen systems, where each eigensystem is in the form: ev, evc, ev, evec, ev, evec.
     */
    void resetEigenFunction( boost::shared_ptr< WDataSetDTI > tensors );

    /**
     * Update the output connectors out of the computed eigensystem field.
     *
     * \param es Dataset carrying all eigen systems.
     */
    void updateOCs( boost::shared_ptr< const WDataSetSingle > es );

    /**
     * The function that computes the eigenvectors from the input tensor field.
     *
     * \param input A subarray of a valueset that consists of the 6 floats that make up the tensor.
     * \return The components of the largest eigenvector and the fa value in a 4-double array.
     */
    TPVOFloat::OutTransmitType const eigenFuncFloat( TPVOFloat::TransmitType const& input );

    /**
     * The function that computes the eigenvectors from the input tensor field.
     *
     * \param input A subarray of a valueset that consists of the 6 floats that make up the tensor.
     * \return The components of the largest eigenvector and the fa value in a 4-double array.
     */
    TPVODouble::OutTransmitType const eigenFuncDouble( TPVODouble::TransmitType const& input );

    /**
     * Is used by every thread to compute the eigensystem for the given tensor.
     *
     * \param m Tensor to compute the eigensystem for
     *
     * \return Eigen system of the given tensor.
     */
    boost::array< double, 12 > computeEigenSystem( WTensorSym< 2, 3, double > const& m ) const;

    /**
     * Input tensor field.
     */
    boost::shared_ptr< WModuleInputData< WDataSetDTI > > m_tensorIC;

    /**
     * Shortcut for the vector field output connectors.
     */
    typedef boost::shared_ptr< WModuleOutputData< WDataSetVector > > EigenOutputConnector;

    /**
     * Ouput vector field for principal eigenvectors as well as one for all eigenvalues at once.
     */
    std::vector< EigenOutputConnector > m_evecOutputs;

    /**
     * Output scalar field , each for an eigenvalue field.
     */
    std::vector< boost::shared_ptr< WModuleOutputData< WDataSetScalar > > > m_evalOutputs;

    //! The threadpool for the eigenvector computation
    boost::shared_ptr< WThreadedFunctionBase > m_eigenPool;

    //! the functor used for the calculation of the eigenvectors
    boost::shared_ptr< TPVOFloat > m_eigenOperationFloat;

    //! the functor used for the calculation of the eigenvectors
    boost::shared_ptr< TPVODouble > m_eigenOperationDouble;

    /**
     * Indicating current work progress.
     */
    boost::shared_ptr< WProgress > m_currentProgress;
};

#endif  // WMEIGENSYSTEM_H
