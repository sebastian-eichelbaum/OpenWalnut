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

#include <stdint.h>

#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "core/common/WAssert.h"
#include "core/common/WProgress.h"
#include "core/common/WStringUtils.h"
#include "core/dataHandler/WGridRegular3D.h"
#include "core/kernel/WKernel.h"
#include "WMGaussFiltering.xpm"
#include "WMGaussFiltering.h"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMGaussFiltering )

WMGaussFiltering::WMGaussFiltering() :
    WModule()
{
    // WARNING: initializing connectors inside the constructor will lead to an exception.
    // Implement WModule::initializeConnectors instead.
}

WMGaussFiltering::~WMGaussFiltering()
{
    // cleanup
    removeConnectors();
}

boost::shared_ptr< WModule > WMGaussFiltering::factory() const
{
    return boost::shared_ptr< WModule >( new WMGaussFiltering() );
}

const char** WMGaussFiltering::getXPMIcon() const
{
    return gaussfiltering_xpm;
}

const std::string WMGaussFiltering::getName() const
{
    return "Gauss Filtering";
}

const std::string WMGaussFiltering::getDescription() const
{
    return "Runs a discretized Gauss filter as mask over a simple scalar field.";
}

size_t getId( size_t xDim, size_t yDim, size_t /*zDim*/, size_t x, size_t y, size_t z, size_t offset = 0, size_t elements = 1 )
{
    return offset + ( elements * ( z * xDim * yDim + y * xDim + x ) );
}

double mask( size_t i, size_t j, size_t k )
{
    WAssert( i < 3, "Index larger than two where [0,1,2] is expected." );
    WAssert( j < 3, "Index larger than two where [0,1,2] is expected."  );
    WAssert( k < 3, "Index larger than two where [0,1,2] is expected."  );
    double maskEntries[3][3][3] = { // NOLINT
            { { 0, 1, 0 }, { 1, 2, 1 }, { 0, 1, 0 } }, // NOLINT
            { { 1, 2, 1 }, { 2, 4, 2 }, { 1, 2, 1 } }, // NOLINT
            { { 0, 1, 0 }, { 1, 2, 1 }, { 0, 1, 0 } } }; // NOLINT
    return maskEntries[i][j][k];
}

template<typename T> double WMGaussFiltering::filterAtPosition(
        boost::shared_ptr<WValueSet<T> > vals, size_t nX, size_t nY, size_t nZ,
        size_t x, size_t y, size_t z, size_t offset )
{
    double filtered = 0;
    double maskSum = 0;
    size_t filterSize = 3;
    for( size_t k = 0; k < filterSize; ++k )
    {
        for( size_t j = 0; j < filterSize; ++j )
        {
            for( size_t i = 0; i < filterSize; ++i )
            {
                filtered += mask( i, j, k ) * vals->getScalar(
                        getId( nX, nY, nZ, x - 1 + i, y - 1 + j, z - 1 + k, offset, vals->elementsPerValue() ) );
                maskSum += mask( i, j, k );
            }
        }
    }
    filtered /= maskSum;

    return filtered;
}

template< typename T >
std::vector<double> WMGaussFiltering::filterField( boost::shared_ptr< WValueSet< T > > vals, boost::shared_ptr<WGridRegular3D> grid,
                                                   boost::shared_ptr< WProgress > prog )
{
    size_t nX = grid->getNbCoordsX();
    size_t nY = grid->getNbCoordsY();
    size_t nZ = grid->getNbCoordsZ();

    if( m_3DMaskMode->get(true) )
    {
        std::vector<double> newVals( vals->elementsPerValue() * nX * nY * nZ, 0. );

        for( size_t z = 1; z < nZ - 1; z++ )
        {
            ++*prog;
            for( size_t y = 1; y < nY - 1; y++ )
            {
                for( size_t x = 1; x < nX - 1; x++ )
                {
                    for( size_t offset = 0; offset < vals->elementsPerValue(); ++offset )
                    {
                        newVals[getId( nX, nY, nZ, x, y, z, offset, vals->elementsPerValue() )] =
                            filterAtPosition( vals, nX, nY, nZ, x, y, z, offset );
                    }
                }
            }
        }
        return newVals;
    }
    else
    {
        std::vector<double> newVals1( vals->elementsPerValue() * nX * nY * nZ, 0. );
        std::vector<double> newVals2( vals->elementsPerValue() * nX * nY * nZ, 0. );

        filterField1D( &newVals1, vals,     prog, nX, nY, nZ, 1, nX, nX*nY ); // run in X direction
        filterField1D( &newVals2, newVals1, prog, nY, nX, nZ, nX, 1, nX*nY ); // run in Y direction
        filterField1D( &newVals1, newVals2, prog, nZ, nX, nY, nX*nY, 1, nX ); // run in Z direction

        return newVals1;
    }
}

template< typename T >
void WMGaussFiltering::filterField1D( std::vector<double>* newVals,
                                      boost::shared_ptr< WValueSet< T > > vals,
                                      boost::shared_ptr< WProgress > prog,
                                      size_t nX, size_t nY, size_t nZ, size_t dx, size_t dy, size_t dz )
{
    for( size_t z = 1; z < nZ - 1; ++z )
    {
        ++*prog;
        for( size_t y = 1; y < nY - 1; ++y )
        {
            for( size_t x = 1; x < nX - 1; ++x )
            {
                size_t id = x*dx + y*dy + z*dz;
                ( *newVals )[ id ] =
                    0.25 * ( vals->getScalar( id - dx ) + 2. * vals->getScalar( id ) + vals->getScalar( id + dx ) );
            }
        }
    }
}

template< typename T >
void WMGaussFiltering::filterField1D( std::vector<T>* newVals,
                                      const std::vector<T>& vals,
                                      boost::shared_ptr< WProgress > prog,
                                      size_t nX, size_t nY, size_t nZ, size_t dx, size_t dy, size_t dz )
{
    for( size_t z = 1; z < nZ - 1; ++z )
    {
        ++*prog;
        for( size_t y = 1; y < nY - 1; ++y )
        {
            for( size_t x = 1; x < nX - 1; ++x )
            {
                size_t id = x*dx + y*dy + z*dz;
                ( *newVals )[ id ] =
                    0.25 * ( vals[ id - dx ] + 2. * vals[ id ] + vals[  id + dx ] );
            }
            }
    }
}

template< typename T >
boost::shared_ptr< WValueSet< double > > WMGaussFiltering::iterativeFilterField( boost::shared_ptr< WValueSet< T > > vals, unsigned int iterations )
{
    // the grid used
    boost::shared_ptr<WGridRegular3D> grid = boost::dynamic_pointer_cast< WGridRegular3D >( m_dataSet->getGrid() );
    WAssert( grid, "Grid is not of type WGridRegular3D." );

    // use a custom progress combiner
    boost::shared_ptr< WProgress > prog;

    if( m_3DMaskMode->get() )
    {
        prog = boost::shared_ptr< WProgress >(
            new WProgress( "Gauss Filter Iteration", iterations * grid->getNbCoordsZ() ) );
    }
    else
    {
        prog = boost::shared_ptr< WProgress >(
            new WProgress( "Gauss Filter Iteration", 3 * iterations * grid->getNbCoordsZ() ) );
    }
    m_progress->addSubProgress( prog );

    // iterate filter, apply at least once
    boost::shared_ptr< WValueSet< double > > valueSet = boost::shared_ptr< WValueSet< double > >(
        new WValueSet<double> ( vals->order(), vals->dimension(),
        boost::shared_ptr< std::vector< double > >( new std::vector< double >( filterField( vals, grid, prog ) ) ),
        W_DT_DOUBLE )
    );
    for( unsigned int i = 1; i < iterations; ++i )    // this only runs if iter > 1
    {
        valueSet = boost::shared_ptr< WValueSet< double > >(
            new WValueSet<double> ( valueSet->order(), valueSet->dimension(),
            boost::shared_ptr< std::vector< double > >( new std::vector< double >( filterField( valueSet, grid, prog ) ) ),
            W_DT_DOUBLE )
        );
    }

    prog->finish();

    return valueSet;
}

void WMGaussFiltering::moduleMain()
{
    // let the main loop awake if the data changes or the properties changed.
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    // signal ready state
    ready();

    // the number of iterations
    unsigned int iterations = 1;

    // loop until the module container requests the module to quit
    while( !m_shutdownFlag() )
    {
        // Now, the moduleState variable comes into play. The module can wait for the condition, which gets fired whenever the input receives data
        // or an property changes. The main loop now waits until something happens.
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        // woke up since the module is requested to finish
        if( m_shutdownFlag() )
        {
            break;
        }

        // has the data changed?
        boost::shared_ptr< WDataSetScalar > newDataSet = m_input->getData();
        bool dataChanged = ( m_dataSet != newDataSet );
        if( dataChanged || !m_dataSet )
        // this condition will become true whenever the new data is different from the current one or our actual data is NULL. This handles all
        // cases.
        {
            // The data is different. Copy it to our internal data variable:
            debugLog() << "Received Data.";
            m_dataSet = newDataSet;

            // invalid data
            if( !m_dataSet )
            {
                debugLog() << "Resetting output.";
                m_output->reset();
                // NOTE: m_dataSet is already reset
                continue;
            }
        }

        // m_iterations changed
        if( m_iterations->changed() )
        {
            // a changed number of iteration also requires recalculation
            iterations = m_iterations->get( true );
            dataChanged = ( iterations >= 1 );
        }

        if( m_3DMaskMode->changed() )
        {
            dataChanged = true;
        }

        if( dataChanged )
        {
            boost::shared_ptr< WValueSet< double > >  newValueSet;

            switch( (*m_dataSet).getValueSet()->getDataType() )
            {
                case W_DT_UNSIGNED_CHAR:
                {
                    boost::shared_ptr<WValueSet<unsigned char> > vals;
                    vals = boost::dynamic_pointer_cast<WValueSet<unsigned char> >( ( *m_dataSet ).getValueSet() );
                    WAssert( vals, "Data type and data type indicator must fit." );
                    newValueSet = iterativeFilterField( vals, iterations );
                    break;
                }
                case W_DT_INT16:
                {
                    boost::shared_ptr<WValueSet<int16_t> > vals;
                    vals = boost::dynamic_pointer_cast<WValueSet<int16_t> >( ( *m_dataSet ).getValueSet() );
                    WAssert( vals, "Data type and data type indicator must fit." );
                    newValueSet = iterativeFilterField( vals, iterations );
                    break;
                }
                case W_DT_UINT16:
                {
                    boost::shared_ptr<WValueSet<uint16_t> > vals;
                    vals = boost::dynamic_pointer_cast<WValueSet<uint16_t> >( ( *m_dataSet ).getValueSet() );
                    WAssert( vals, "Data type and data type indicator must fit." );
                    newValueSet = iterativeFilterField( vals, iterations );
                    break;
                }
                case W_DT_SIGNED_INT:
                {
                    boost::shared_ptr<WValueSet<int32_t> > vals;
                    vals = boost::dynamic_pointer_cast<WValueSet<int32_t> >( ( *m_dataSet ).getValueSet() );
                    WAssert( vals, "Data type and data type indicator must fit." );
                    newValueSet = iterativeFilterField( vals, iterations );
                    break;
                }
                case W_DT_FLOAT:
                {
                    boost::shared_ptr<WValueSet<float> > vals;
                    vals = boost::dynamic_pointer_cast<WValueSet<float> >( ( *m_dataSet ).getValueSet() );
                    WAssert( vals, "Data type and data type indicator must fit." );
                    newValueSet = iterativeFilterField( vals, iterations );
                    break;
                }
                case W_DT_DOUBLE:
                {
                    boost::shared_ptr<WValueSet<double> > vals;
                    vals = boost::dynamic_pointer_cast<WValueSet<double> >( ( *m_dataSet ).getValueSet() );
                    WAssert( vals, "Data type and data type indicator must fit." );
                    newValueSet = iterativeFilterField( vals, iterations );
                    break;
                }
                default:
                    WAssert( false, "Unknown data type in Gauss Filtering module" );
            }

            m_output->updateData( boost::shared_ptr<WDataSetScalar>( new WDataSetScalar( newValueSet, m_dataSet->getGrid() ) ) );
        }

        // if the number of iterations is 0 -> simply set the input as output
        if( iterations == 0 )
        {
            m_output->updateData( m_dataSet );
        }
    }
}

void WMGaussFiltering::connectors()
{
    // initialize connectors
    m_input = boost::shared_ptr<WModuleInputData<WDataSetScalar> >(
            new WModuleInputData<WDataSetScalar> ( shared_from_this(), "in",
                    "The dataset to filter" ) );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_input );

    // initialize connectors
    m_output = boost::shared_ptr<WModuleOutputData<WDataSetScalar> >(
            new WModuleOutputData<WDataSetScalar> ( shared_from_this(), "out",
                    "The filtered data set." ) );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_output );

    // call WModules initialization
    WModule::connectors();
}

void WMGaussFiltering::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_iterations = m_properties->addProperty( "Iterations", "How often should the filter be applied.", 1, m_propCondition );
    m_iterations->setMin( 0 );
    m_iterations->setMax( 100 );

    m_3DMaskMode = m_properties->addProperty( "Filter 3D", "Filter with a 3D mask instead of three 1D masks.", false, m_propCondition );

    WModule::properties();
}
