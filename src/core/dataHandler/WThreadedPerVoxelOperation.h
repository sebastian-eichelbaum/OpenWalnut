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

#ifndef WTHREADEDPERVOXELOPERATION_H
#define WTHREADEDPERVOXELOPERATION_H

#include <vector>
#include <string>

#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

#include "../common/WException.h"
#include "../common/WThreadedJobs.h"
#include "../common/WSharedObject.h"
#include "../common/WSharedSequenceContainer.h"
#include "WDataSetSingle.h"
#include "WDataSetScalar.h"
#include "WValueSet.h"
#include "WDataHandlerEnums.h"

//! forward declaration for the test
class WThreadedPerVoxelOperationTest;

/**
 * \class WThreadedPerVoxelOperation
 *
 * A template that performs an operation on a per voxel basis. This
 * template is intended to be used in combination with \see WThreadedFunction.
 *
 * The template parameters are the internal datatype of the input datasets valueset,
 * the number of input data elements per voxel, the type of the output data and the number of
 * outputs per voxel.
 *
 * Example: Suppose one wants to calculate the largest eigenvector of a symmetric tensor of order
 * 2 per voxel, where the input tensors are stored als 6 floats. The output could be 3 double values.
 * The corresponding template parameters would be float, 6, double, 3.
 *
 * A function that converts the input values to output values needs to be given via a boost::function
 * object. The correct 'signature' is:
 *
 * boost::array< Output_T, numOutputs > func( WValueSet< Value_T >::SubArray const& );
 *
 * The subarray will have exactly numInputs entries.
 */
template< typename Value_T, std::size_t numValues, typename Output_T, std::size_t numOutputs >
class WThreadedPerVoxelOperation : public WThreadedStripingJobs< WValueSet< Value_T >, std::size_t >
{
    //! the test is a friend
    friend class WThreadedPerVoxelOperationTest;

    //! the base class
    typedef WThreadedStripingJobs< WValueSet< Value_T >, std::size_t > BaseType;

public:
    //! the input valueset's type
    typedef WValueSet< Value_T > ValueSetType;

    //! the output valueset's type
    typedef WValueSet< Output_T > OutValueSetType;

    //! the input type for the per-voxel operation
    typedef typename ValueSetType::SubArray const TransmitType;

    //! the output type for the per-voxel operation
    typedef boost::array< Output_T, numOutputs > OutTransmitType;

    //! the function type
    typedef boost::function< OutTransmitType const ( TransmitType const& ) > FunctionType;

    /**
     * Construct a per-voxel operation.
     *
     * \param dataset The input dataset.
     * \param func The function to be evaluated per voxel.
     */
    WThreadedPerVoxelOperation( boost::shared_ptr< WDataSetSingle const > dataset, FunctionType func );

    /**
     * Destructor.
     */
    virtual ~WThreadedPerVoxelOperation();

    /**
     * Perform the computation for a specific voxel.
     *
     * \param input The input dataset.
     * \param voxelNum The voxel number to operate on.
     */
    virtual void compute( boost::shared_ptr< ValueSetType const > input, std::size_t voxelNum );

    /**
     * Get the output dataset.
     *
     * \return The oupput dataset.
     */
    boost::shared_ptr< WDataSetSingle > getResult();

protected:
    using BaseType::m_input;

private:
    //! a threadsafe vector (container)
    typedef boost::shared_ptr< std::vector< Output_T > > OutputVectorType;

    //! stores the output of the per-voxel-operation
    OutputVectorType m_output;

    //! the function applied to every voxel
    FunctionType m_func;

    //! store the grid
    boost::shared_ptr< WGrid > m_grid;
};

template< typename Value_T, std::size_t numValues, typename Output_T, std::size_t numOutputs >
WThreadedPerVoxelOperation< Value_T, numValues, Output_T, numOutputs >::WThreadedPerVoxelOperation(
                                                        boost::shared_ptr< WDataSetSingle const > dataset,
                                                        FunctionType func )
    : BaseType( ( dataset ? boost::shared_dynamic_cast< ValueSetType >( dataset->getValueSet() )
                          : boost::shared_ptr< ValueSetType >() ) ) // NOLINT
{
    if( !dataset )
    {
        throw WException( std::string( "No input dataset." ) );
    }
    if( !dataset->getValueSet() )
    {
        throw WException( std::string( "The input dataset has no valueset." ) );
    }
    if( !dataset->getGrid() )
    {
        throw WException( std::string( "The input dataset has no grid." ) );
    }
    if( dataset->getValueSet()->order() > 1 )
    {
        throw WException( std::string( "An order of 2 or more is currently not supported." ) );
    }
    if( dataset->getValueSet()->dimension() != numValues )
    {
        throw WException( std::string( "Invalid valueset dimension." ) );
    }
    if( !func )
    {
        throw WException( std::string( "No valid function provided." ) );
    }

    try
    {
        // allocate enough memory for the output data
        m_output = OutputVectorType( new std::vector< Output_T >( m_input->size() * numOutputs ) );
    }
    catch( std::exception const& e )
    {
        throw WException( std::string( e.what() ) );
    }
    m_func = func;
    m_grid = dataset->getGrid();
}

template< typename Value_T, std::size_t numValues, typename Output_T, std::size_t numOutputs >
WThreadedPerVoxelOperation< Value_T, numValues, Output_T, numOutputs >::~WThreadedPerVoxelOperation()
{
}

template< typename Value_T, std::size_t numValues, typename Output_T, std::size_t numOutputs >
void WThreadedPerVoxelOperation< Value_T, numValues, Output_T, numOutputs >::compute( boost::shared_ptr< ValueSetType const > input,
                                                               std::size_t voxelNum )
{
    TransmitType t = input->getSubArray( voxelNum * numValues, numValues );
    OutTransmitType o = m_func( t );
    for( std::size_t k = 0; k < numOutputs; ++k )
    {
        ( *m_output )[ voxelNum * numOutputs + k ] = o[ k ];
    }
}

template< typename Value_T, std::size_t numValues, typename Output_T, std::size_t numOutputs >
boost::shared_ptr< WDataSetSingle > WThreadedPerVoxelOperation< Value_T, numValues, Output_T, numOutputs >::getResult()
{
    boost::shared_ptr< OutValueSetType > values;
    switch( numOutputs )
    {
    case 1:
        values = boost::shared_ptr< OutValueSetType >( new OutValueSetType( 0, 1, m_output,
                                                                            DataType< Output_T >::type ) );
        return boost::shared_ptr< WDataSetScalar >( new WDataSetScalar( values, m_grid ) );
    default:
        values = boost::shared_ptr< OutValueSetType >( new OutValueSetType( 1, numOutputs, m_output,
                                                                            DataType< Output_T >::type ) );
        return boost::shared_ptr< WDataSetSingle >( new WDataSetSingle( values, m_grid ) );
    }
}

#endif  // WTHREADEDPERVOXELOPERATION_H
