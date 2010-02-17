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

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <cmath>

#include "../../common/WStringUtils.h"
#include "../../common/WProgress.h"
#include "../../dataHandler/WGridRegular3D.h"
#include "../../kernel/WKernel.h"
#include "../../math/WPosition.h"
#include "../../math/WVector3D.h"
#include "../data/WMData.h"
#include "WMGaussFiltering.h"
#include "gaussfiltering.xpm"

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

boost::shared_ptr<WModule> WMGaussFiltering::factory() const
{
    return boost::shared_ptr<WModule>( new WMGaussFiltering() );
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

size_t getId( size_t xDim, size_t yDim, size_t /*zDim*/, size_t x, size_t y,
        size_t z )
{
    return z * xDim * yDim + y * xDim + x;
}

double mask( size_t i, size_t j, size_t k )
{
    assert( i < 3 );
    assert( j < 3 );
    assert( k < 3 );
    double maskEntries[3][3][3] = { // NOLINT
            { { 0, 1, 0 }, { 1, 2, 1 }, { 0, 1, 0 } }, // NOLINT
            { { 1, 2, 1 }, { 2, 4, 2 }, { 1, 2, 1 } }, // NOLINT
            { { 0, 1, 0 }, { 1, 2, 1 }, { 0, 1, 0 } } }; // NOLINT
    return maskEntries[i][j][k];
}

template<typename T> double WMGaussFiltering::filterAtPosition(
        boost::shared_ptr<WValueSet<T> > vals, size_t nX, size_t nY, size_t nZ,
        size_t x, size_t y, size_t z )
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
                filtered += mask( i, j, k ) * vals->getScalar( getId( nX, nY,
                        nZ, x - 1 + i, y - 1 + j, z - 1 + k ) );
                maskSum += mask( i, j, k );
            }
        }
    }
    filtered /= maskSum;

    return filtered;
}

template<typename T> std::vector<double> WMGaussFiltering::filterField(
        boost::shared_ptr<WValueSet<T> > vals )
{
    boost::shared_ptr<WGridRegular3D> grid = boost::shared_dynamic_cast<
            WGridRegular3D>( m_dataSet->getGrid() );
    assert( grid );
    size_t nX = grid->getNbCoordsX();
    size_t nY = grid->getNbCoordsY();
    size_t nZ = grid->getNbCoordsZ();
    std::vector<double> newVals( nX * nY * nZ, 0. );

    boost::shared_ptr<WProgress> progress = boost::shared_ptr<WProgress>(
            new WProgress( "Gauss Filtering", nZ ) );
    m_progress->addSubProgress( progress );
    for( size_t z = 1; z < nZ - 1; z++ )
    {
        ++*progress;
        for( size_t y = 1; y < nY - 1; y++ )
        {
            for( size_t x = 1; x < nX - 1; x++ )
            {
                newVals[getId( nX, nY, nZ, x, y, z )] = filterAtPosition( vals,
                        nX, nY, nZ, x, y, z );
            }
        }
    }
    progress->finish();

    return newVals;
}

void WMGaussFiltering::moduleMain()
{
    // use the m_input "data changed" flag
    m_moduleState.add( m_input->getDataChangedCondition() );

    // signal ready state
    ready();

    // loop until the module container requests the module to quit
    while( !m_shutdownFlag() )
    {
        sleep( 3 ); // TODO(wiebel): remove this
        // acquire data from the input connector
        m_dataSet = m_input->getData();
        if( !m_dataSet )
        {
            // ok, the output has not yet sent data
            // NOTE: see comment at the end of this while loop for m_moduleState
            debugLog() << "Waiting for data ...";
            m_moduleState.wait();
            continue;
        }
        assert( m_dataSet );

        std::vector<double> newVals;

        switch( (*m_dataSet).getValueSet()->getDataType() )
        {
            case W_DT_UNSIGNED_CHAR:
            {
                boost::shared_ptr<WValueSet<unsigned char> > vals;
                vals = boost::shared_dynamic_cast<WValueSet<unsigned char> >( ( *m_dataSet ).getValueSet() );
                assert( vals );
                newVals = filterField( vals );
                break;
            }
            case W_DT_INT16:
            {
                boost::shared_ptr<WValueSet<int16_t> > vals;
                vals = boost::shared_dynamic_cast<WValueSet<int16_t> >( ( *m_dataSet ).getValueSet() );
                assert( vals );
                newVals = filterField( vals );
            }
            case W_DT_SIGNED_INT:
            {
                boost::shared_ptr<WValueSet<int32_t> > vals;
                vals = boost::shared_dynamic_cast<WValueSet<int32_t> >( ( *m_dataSet ).getValueSet() );
                assert( vals );
                newVals = filterField( vals );
                break;
            }
            case W_DT_FLOAT:
            {
                boost::shared_ptr<WValueSet<float> > vals;
                vals = boost::shared_dynamic_cast<WValueSet<float> >( ( *m_dataSet ).getValueSet() );
                assert( vals );
                newVals = filterField( vals );
                break;
            }
            case W_DT_DOUBLE:
            {
                boost::shared_ptr<WValueSet<double> > vals;
                vals = boost::shared_dynamic_cast<WValueSet<double> >( ( *m_dataSet ).getValueSet() );
                assert( vals );
                newVals = filterField( vals );
                break;
            }
            default:
                assert( false && "Unknow data type in Gauss Filtering module" );
        }

        boost::shared_ptr<WValueSet<double> > valueSet;
        valueSet = boost::shared_ptr<WValueSet<double> >(
                new WValueSet<double> ( 0, 1, newVals, W_DT_DOUBLE ) );

        m_dataSet = boost::shared_ptr<WDataSetSingle>( new WDataSetSingle(
                valueSet, m_dataSet->getGrid() ) );
        m_output->updateData( m_dataSet );

        // this waits for m_moduleState to fire. By default, this is only the m_shutdownFlag condition.
        // NOTE: you can add your own conditions to m_moduleState using m_moduleState.add( ... )
        m_moduleState.wait();
    }
}

void WMGaussFiltering::connectors()
{
    // initialize connectors
    m_input = boost::shared_ptr<WModuleInputData<WDataSetSingle> >(
            new WModuleInputData<WDataSetSingle> ( shared_from_this(), "in",
                    "The dataset to filter" ) );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_input );

    // initialize connectors
    m_output = boost::shared_ptr<WModuleOutputData<WDataSetSingle> >(
            new WModuleOutputData<WDataSetSingle> ( shared_from_this(), "out",
                    "The filtered data set." ) );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_output );

    // call WModules initialization
    WModule::connectors();
}

void WMGaussFiltering::properties()
{
    //     ( m_properties->addInt( "Filter Size", 1 ) )->connect( boost::bind( &WMGaussFiltering::slotPropertyChanged, this, _1 ) );
}

void WMGaussFiltering::slotPropertyChanged( std::string propertyName )
{
    if( propertyName == "Filter Size" )
    {
        // TODO(wiebel): need code here
    }
    else
    {
        std::cout << propertyName << std::endl;
        assert( 0 && "This property name is not supported by this function yet." );
    }
}
