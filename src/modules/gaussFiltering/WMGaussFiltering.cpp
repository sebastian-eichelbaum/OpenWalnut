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

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <cmath>

#include "WMGaussFiltering.h"

#include "../../utils/WStringUtils.h"
#include "../../math/WVector3D.h"
#include "../../math/WPosition.h"
#include "../../dataHandler/WGridRegular3D.h"
#include "../../kernel/WKernel.h"
#include "../../graphicsEngine/WShader.h"

#include "../data/WMData.h"


WMGaussFiltering::WMGaussFiltering():
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

const std::string WMGaussFiltering::getName() const
{
    return "Gauss Filtering";
}

const std::string WMGaussFiltering::getDescription() const
{
    return "Runs a discretized Gauss filter as mask over a simple scalar field.";
}

size_t getId( size_t xDim, size_t yDim, size_t /*zDim*/, size_t x, size_t y, size_t z )
{
    return z * xDim * yDim + y * xDim + x;
}

double WMGaussFiltering::filterSimple( size_t nX, size_t nY, size_t nZ, size_t x, size_t y, size_t z )
{
    double filtered = 0;
    filtered += 2 * m_vals->getScalar( getId( nX, nY, nZ, x    , y    , z - 1 ) );
    filtered += 1 * m_vals->getScalar( getId( nX, nY, nZ, x + 1, y    , z - 1 ) );
    filtered += 1 * m_vals->getScalar( getId( nX, nY, nZ, x - 1, y    , z - 1 ) );
    filtered += 1 * m_vals->getScalar( getId( nX, nY, nZ, x    , y + 1, z - 1 ) );
    filtered += 1 * m_vals->getScalar( getId( nX, nY, nZ, x    , y - 1, z - 1 ) );

    filtered += 1 * m_vals->getScalar( getId( nX, nY, nZ, x - 1, y - 1, z ) );
    filtered += 2 * m_vals->getScalar( getId( nX, nY, nZ, x    , y - 1, z ) );
    filtered += 1 * m_vals->getScalar( getId( nX, nY, nZ, x + 1, y - 1, z ) );
    filtered += 2 * m_vals->getScalar( getId( nX, nY, nZ, x - 1, y    , z ) );
    filtered += 4 * m_vals->getScalar( getId( nX, nY, nZ, x    , y    , z ) );
    filtered += 2 * m_vals->getScalar( getId( nX, nY, nZ, x + 1, y    , z ) );
    filtered += 1 * m_vals->getScalar( getId( nX, nY, nZ, x - 1, y + 1, z ) );
    filtered += 2 * m_vals->getScalar( getId( nX, nY, nZ, x    , y + 1, z ) );
    filtered += 1 * m_vals->getScalar( getId( nX, nY, nZ, x + 1, y + 1, z ) );

    filtered += 2 * m_vals->getScalar( getId( nX, nY, nZ, x    , y    , z     ) );
    filtered += 1 * m_vals->getScalar( getId( nX, nY, nZ, x + 1, y    , z     ) );
    filtered += 1 * m_vals->getScalar( getId( nX, nY, nZ, x - 1, y    , z     ) );
    filtered += 1 * m_vals->getScalar( getId( nX, nY, nZ, x    , y + 1, z     ) );
    filtered += 1 * m_vals->getScalar( getId( nX, nY, nZ, x    , y - 1, z     ) );
    filtered /= 28.;

    return filtered;
}

void WMGaussFiltering::moduleMain()
{
    // use the m_input "data changed" flag
    m_moduleState.add( m_input->getDataChangedCondition() );

    // signal ready state
    ready();

    // loop until the module container requests the module to quit
    while ( !m_shutdownFlag() )
    {
        sleep( 3 ); // TODO(wiebel): remove this
        // acquire data from the input connector
        m_dataSet = m_input->getData();
        if ( !m_dataSet )
        {
            // ok, the output has not yet sent data
            // NOTE: see comment at the end of this while loop for m_moduleState
            debugLog() << "Waiting for data ...";
            m_moduleState.wait();
            continue;
        }
        assert( m_dataSet );

        m_vals =  boost::shared_dynamic_cast< WValueSet< int16_t > >( ( *m_dataSet ).getValueSet() );
        assert( m_vals );
        boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( m_dataSet->getGrid() );
        assert( grid );
        size_t nX = grid->getNbCoordsX();
        size_t nY = grid->getNbCoordsY();
        size_t nZ = grid->getNbCoordsZ();
        std::vector< float > newVals( nX * nY * nZ, 0. );
        for( size_t z = 1; z < nZ - 1; z++ )
        {
            for( size_t y = 1; y < nY - 1; y++ )
            {
                for( size_t x = 1; x < nX - 1; x++ )
                {
                    newVals[getId( nX, nY, nZ, x, y, z )] = filterSimple( nX, nY, nZ, x, y, z );
                }
            }
        }

        boost::shared_ptr< WValueSet< float > > values;
        values = boost::shared_ptr< WValueSet< float > >( new WValueSet< float >( 0, 1, newVals, W_DT_FLOAT ) );

        m_dataSet = boost::shared_ptr< WDataSetSingle >( new WDataSetSingle( values, m_dataSet->getGrid() ) );
        m_output->updateData( m_dataSet );

        // this waits for m_moduleState to fire. By default, this is only the m_shutdownFlag condition.
        // NOTE: you can add your own conditions to m_moduleState using m_moduleState.add( ... )
        m_moduleState.wait();
    }
}

void WMGaussFiltering::connectors()
{
    // initialize connectors
    m_input = boost::shared_ptr< WModuleInputData < WDataSetSingle  > >(
        new WModuleInputData< WDataSetSingle >( shared_from_this(),
                                                               "in", "The dataset to filter" )
        );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_input );

    // initialize connectors
    m_output= boost::shared_ptr< WModuleOutputData< WDataSetSingle > >( new WModuleOutputData< WDataSetSingle >(
                shared_from_this(), "out", "The filtered data set." )
            );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_output );

    // call WModules initialization
    WModule::connectors();
}

void WMGaussFiltering::properties()
{
    ( m_properties->addInt( "Filter Size", 1 ) )->connect( boost::bind( &WMGaussFiltering::slotPropertyChanged, this, _1 ) );
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
