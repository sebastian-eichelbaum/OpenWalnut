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
#include <utility>

#include <cmath>

#include "core/common/WProgress.h"
#include "core/common/WAssert.h"
#include "core/kernel/WKernel.h"
#include "WMApplyMask.h"
#include "WMApplyMask.xpm"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMApplyMask )

WMApplyMask::WMApplyMask() :
    WModule()
{
    // WARNING: initializing connectors inside the constructor will lead to an exception.
    // Implement WModule::initializeConnectors instead.
}

WMApplyMask::~WMApplyMask()
{
    // cleanup
    removeConnectors();
}

boost::shared_ptr< WModule > WMApplyMask::factory() const
{
    return boost::shared_ptr< WModule >( new WMApplyMask() );
}

const char** WMApplyMask::getXPMIcon() const
{
    return apply_mask_xpm;
}

const std::string WMApplyMask::getName() const
{
    return "Apply Mask";
}

const std::string WMApplyMask::getDescription() const
{
    return "Applies a mask to a data set, i.e. sets all voxels to zero which are zero in the mask.";
}

void WMApplyMask::moduleMain()
{
    // use the m_input "data changed" flag
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_dataInput->getDataChangedCondition() );
    m_moduleState.add( m_maskInput->getDataChangedCondition() );

    // signal ready state
    ready();

    // loop until the module container requests the module to quit
    while( !m_shutdownFlag() )
    {
        // acquire data from the input connector
        m_dataSet = m_dataInput->getData();
        m_mask = m_maskInput->getData();
        if( !m_dataSet || !m_mask )
        {
            // ok, the output has not yet sent data
            // NOTE: see comment at the end of this while loop for m_moduleState
            debugLog() << "Waiting for data ...";
            m_moduleState.wait();
            continue;
        }
        dataType type = m_dataSet->getValueSet()->getDataType();
        switch( type )
        {
            case W_DT_UNSIGNED_CHAR:
            {
                boost::shared_ptr< WValueSet< unsigned char > > vals;
                vals = boost::dynamic_pointer_cast< WValueSet< unsigned char > >( ( *m_dataSet ).getValueSet() );
                WAssert( vals, "Data type and data type indicator must fit." );
                applyMask( vals, type );
                break;
            }
            case W_DT_INT16:
            {
                boost::shared_ptr< WValueSet< int16_t > > vals;
                vals = boost::dynamic_pointer_cast< WValueSet< int16_t > >( ( *m_dataSet ).getValueSet() );
                WAssert( vals, "Data type and data type indicator must fit." );
                applyMask( vals, type );
                break;
            }
            case W_DT_SIGNED_INT:
            {
                boost::shared_ptr< WValueSet< int32_t > > vals;
                vals = boost::dynamic_pointer_cast< WValueSet< int32_t > >( ( *m_dataSet ).getValueSet() );
                WAssert( vals, "Data type and data type indicator must fit." );
                applyMask( vals, type );
                break;
            }
            case W_DT_FLOAT:
            {
                boost::shared_ptr< WValueSet< float > > vals;
                vals = boost::dynamic_pointer_cast< WValueSet< float > >( ( *m_dataSet ).getValueSet() );
                WAssert( vals, "Data type and data type indicator must fit." );
                applyMask( vals, type );
                break;
            }
            case W_DT_DOUBLE:
            {
                boost::shared_ptr< WValueSet< double > > vals;
                vals = boost::dynamic_pointer_cast< WValueSet< double > >( ( *m_dataSet ).getValueSet() );
                WAssert( vals, "Data type and data type indicator must fit." );
                applyMask( vals, type );
                break;
            }
            default:
                throw WException( std::string( "Data type of value set not supported by this module." ) );
        }

        // this waits for m_moduleState to fire. By default, this is only the m_shutdownFlag condition.
        // NOTE: you can add your own conditions to m_moduleState using m_moduleState.add( ... )
        m_moduleState.wait();
    }
}

void WMApplyMask::connectors()
{
    // initialize connectors
    m_dataInput = boost::shared_ptr< WModuleInputData< WDataSetScalar > >( new WModuleInputData< WDataSetScalar >(
                    shared_from_this(), "dataSet", "The dataset to apply the mask to." ) );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_dataInput );

    // initialize connectors
    m_maskInput = boost::shared_ptr< WModuleInputData< WDataSetScalar > >(
            new WModuleInputData< WDataSetScalar >( shared_from_this(), "mask",
                    "The mask applied to the data." ) );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_maskInput );

    // initialize connectors
    m_output = boost::shared_ptr< WModuleOutputData< WDataSetScalar > >(
            new WModuleOutputData< WDataSetScalar >( shared_from_this(), "out",
                    "The filtered data set." ) );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_output );

    // call WModules initialization
    WModule::connectors();
}

void WMApplyMask::properties()
{
    WModule::properties();
}

template< typename T > void WMApplyMask::applyMask( boost::shared_ptr< WValueSet< T > > valSet, dataType type )
{
    boost::shared_ptr< WValueSetBase > maskBase = m_mask->getValueSet();
    boost::shared_ptr< WValueSet< float > > mask = boost::dynamic_pointer_cast< WValueSet< float > >( maskBase );

    if( !mask )
    {
        throw WException( std::string( "Mask is not of type float." ) );
    }

    boost::shared_ptr< WProgress > progress( new WProgress( "Apply Mask", valSet->size() ) );
    m_progress->addSubProgress( progress );

    boost::shared_ptr< std::vector< T > > newVals( new std::vector< T >( valSet->size() ) );
    for( size_t i = 0; i < valSet->size(); ++i )
    {
        ++*progress;
        if( mask->getScalar( i ) == 0 )
        {
            ( *newVals )[i] = 0;
        }
        else
        {
            ( *newVals )[i] = valSet->getScalar( i );
        }
    }
    progress->finish();

    boost::shared_ptr< WValueSet< T > > valueSet;
    valueSet = boost::shared_ptr< WValueSet< T > >( new WValueSet< T >( 0, 1, newVals, type ) );

    m_dataSetOut = boost::shared_ptr< WDataSetScalar >( new WDataSetScalar( valueSet, m_dataSet->getGrid() ) );
    m_output->updateData( m_dataSetOut );
}
