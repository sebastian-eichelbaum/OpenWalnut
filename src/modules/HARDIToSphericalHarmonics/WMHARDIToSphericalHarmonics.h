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

#ifndef WMHARDITOSPHERICALHARMONICS_H
#define WMHARDITOSPHERICALHARMONICS_H

#include <map>
#include <string>
#include <vector>


#include "core/common/WThreadedRunner.h"
#include "core/common/math/WMatrix.h"
#include "core/dataHandler/WDataSetRawHARDI.h"
#include "core/dataHandler/WDataSetSphericalHarmonics.h"
#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"
#include "core/dataHandler/WGridRegular3D.h"

#include "WSphericalHarmonicsCoefficientsThread.h"

// forward declaration
template< typename T >
class WSphericalHarmonicsCoefficientsThread;

/**
 * Module for the creation of a spherical harmonic data set from raw HARDI data.
 * \ingroup modules
 */
class WMHARDIToSphericalHarmonics : public WModule
{
/**
 * Only UnitTests may be friends.
 */
friend class WMHARDIToSphericalHarmonicsTest;

public:
    /**
     * Standard constructor.
     */
    WMHARDIToSphericalHarmonics();

    /**
     * Destructor.
     */
    ~WMHARDIToSphericalHarmonics();

    /**
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Gives back a description of this module.
     * \return description of module.
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

    // TODO(philips): can we move this method to private or at least protected
    /**
     * Stop the threads.
     */
    void stopThreads();

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

    WPropInt m_order; //!< Property holding the order of the spherical harmonics

    WPropDouble m_regularisationFactorLambda; //!< Property holding the regularisation factor lambda

    WPropBool m_doFunkRadonTransformation; //!< Property indicating whether to do the Funk-Radon-transformation with calculated spherical harmonics.

    WPropBool m_doErrorCalculation; //!< Property indicating whether a reprojection error of the spherical harmonics is calculated.

    /**
     * Property indicating whether the reprojection error (measurement relative to spherical harmonic calculation) is stored into a separate dataset.
     */
    WPropBool m_doResidualCalculation;

    /**
     * Property indicating whether the spherical harmonics calculation is done multithreaded.
     */
    WPropBool m_multiThreaded;

    /**
     * Property indicating whether the measurements are normalized from 0 to 1 for each voxel.
     */
    WPropBool m_doNormalisation;

    /**
     * This is a pointer to the dataset the module is currently working on.
     */
    boost::shared_ptr< WDataSetRawHARDI > m_dataSet;

    /**
     * Input connector required by this module. The HARDI measurements.
     */
    boost::shared_ptr< WModuleInputData< WDataSetRawHARDI > > m_input;

    /**
     * Output connector provided by this module. The calculated spherical harmonics.
     */
    boost::shared_ptr< WModuleOutputData< WDataSetSphericalHarmonics > > m_output;

    /**
     * The reprojection error for each measurement.
     */
    boost::shared_ptr< WModuleOutputData< WDataSetRawHARDI > > m_outputResiduals;

    /**
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    /**
     * This class is derived from PropertyConstraint and ensures that only even values are valid.
     */
    class evenInt: public WPropertyVariable< WPVBaseTypes::PV_INT >::PropertyConstraint
    {
        /**
         * You need to overwrite this method. It decides whether the specified new value should be accepted or not.
         *
         * \param property the property thats going to be changed.
         * \param value the new value
         *
         * \return true if the new value is OK.
         */
        virtual bool accept( boost::shared_ptr< WPropertyVariable< WPVBaseTypes::PV_INT > >  property, const WPVBaseTypes::PV_INT& value );

        /**
         * Method to clone the constraint and create a new one with the correct dynamic type.
         *
         * \return the constraint.
         */
        virtual boost::shared_ptr< WPropertyVariable< WPVBaseTypes::PV_INT >::PropertyConstraint > clone();
    };

    /**
     * \class HARDICalculation
     *
     * Functor that does multithreaded spherical harmonic fitting.
     *
     * This functor provides a template operator() that has the type of the values stored in the valueset
     * as a template parameter. WValueSet's applyFunction will call the version with the correct type.
     */
    class HARDICalculation;
};

class WMHARDIToSphericalHarmonics::HARDICalculation : public boost::static_visitor< std::pair< boost::shared_ptr< WDataSetSphericalHarmonics >,
                                                                                               boost::shared_ptr< WDataSetRawHARDI > > >
{
public:
    /**
     * Constructor.
     *
     * \param threadParams The partially initialised thread parameter struct that will be forwarded to the worker threads.
     * \param multiThreaded If true, the maximum number of threads should be used.
     * \param grid The grid of the input data.
     * \param gradients The gradients of the hardi data.
     */
    HARDICalculation( WSphericalHarmonicsCoefficientsThread<>::ThreadParameter threadParams, bool multiThreaded,
                      boost::shared_ptr< WGrid > grid, std::vector< WVector3d > const& gradients );

    /**
     * Destructor.
     */
    ~HARDICalculation();

    /**
     * Allocate shared memory and construct and run threads, construct output datasets.
     *
     * \tparam T The integral type of values in the valueset.
     *
     * \return result value
     */
    template< typename T >
    result_type operator() ( WValueSet< T > const* /* vs */ ) const;

private:

    //! The parameters that will be forwarded to the threads.
    WSphericalHarmonicsCoefficientsThread<>::ThreadParameter m_parameter;

    //! If more than 1 thread should be used.
    bool m_multiThreaded;

    //! The grid of the data.
    boost::shared_ptr< WGrid > m_grid;

    //! The gradients of the hardi data.
    std::vector< WVector3d > const& m_gradients;
};

template< typename T >
WMHARDIToSphericalHarmonics::HARDICalculation::result_type
WMHARDIToSphericalHarmonics::HARDICalculation::operator() ( WValueSet< T > const* /* vs */ ) const
{
    // the number of coeffs for the spherical harmonics of the given order
    int dimension = ( m_parameter.m_order + 1 ) * ( m_parameter.m_order + 2 ) / 2;
    size_t voxelCount = m_parameter.m_valueSet->size();

    WSphericalHarmonicsCoefficientsThread<>::ThreadParameter parameter = m_parameter;

    // data vector stores spherical harmonics coefficients
    parameter.m_data = boost::shared_ptr< std::vector< double > >( new std::vector< double >( voxelCount * dimension ) );

    const unsigned int threadCount = m_multiThreaded ?
                                     ( boost::thread::hardware_concurrency() == 0 ? 1 : boost::thread::hardware_concurrency() ) : 1;

    // data vector to store reprojection residuals
    if( parameter.m_doResidualCalculation )
    {
        parameter.m_dataResiduals = boost::shared_ptr< std::vector< double > >(
                        new std::vector< double >( parameter.m_valueSet->size() * parameter.m_validIndices.size() ) );
    }

    std::pair< size_t, size_t > range;

    typename std::vector< WSphericalHarmonicsCoefficientsThread< T >* > threads;

    // create Threads
    for( unsigned int i = 0; i < threadCount; i++ )
    {
        range.first = ( voxelCount / threadCount ) * i;
        range.second = ( i == ( threadCount - 1 ) ) ? voxelCount : ( voxelCount / threadCount ) * ( i + 1 );
        threads.push_back( new WSphericalHarmonicsCoefficientsThread< T >( parameter, range ) );
        threads.back()->run();
    }

    for( unsigned int i = 0; i < threads.size(); i++ )
    {
        threads[ i ]->wait();
        delete threads[ i ];
    }

    threads.clear();

    parameter.m_progress->finish();

    result_type result;

    // create final output data
    boost::shared_ptr< WValueSet< double > > sphericalHarmonicsData
            = boost::shared_ptr< WValueSet< double > >( new WValueSet< double >( 1, dimension, parameter.m_data, W_DT_DOUBLE ) );

    result.first = boost::shared_ptr< WDataSetSphericalHarmonics >(
                        new WDataSetSphericalHarmonics( sphericalHarmonicsData, m_grid ) );

    if( parameter.m_doResidualCalculation )
    {
        boost::shared_ptr< WValueSet< double > > residualsData = boost::shared_ptr< WValueSet< double > >(
                new WValueSet< double >( 1, parameter.m_validIndices.size(), parameter.m_dataResiduals, W_DT_DOUBLE ) );

        result.second = boost::shared_ptr< WDataSetRawHARDI >( new WDataSetRawHARDI( residualsData, m_grid,
                                                                                     boost::shared_ptr< std::vector< WVector3d > >(
                                                                                         new std::vector< WVector3d >( m_gradients ) ) ) );
    }

    return result;
}

#endif  // WMHARDITOSPHERICALHARMONICS_H
