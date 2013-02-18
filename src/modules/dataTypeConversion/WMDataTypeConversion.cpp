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

#include "core/common/WAssert.h"
#include "core/common/WStringUtils.h"
#include "core/common/WProgress.h"
#include "core/dataHandler/WGridRegular3D.h"
#include "core/kernel/WKernel.h"
#include "core/common/math/linearAlgebra/WLinearAlgebra.h"
#include "WMDataTypeConversion.h"
#include "WMDataTypeConversion.xpm"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMDataTypeConversion )

WMDataTypeConversion::WMDataTypeConversion() :
    WModule()
{
    // WARNING: initializing connectors inside the constructor will lead to an exception.
    // Implement WModule::initializeConnectors instead.
}

WMDataTypeConversion::~WMDataTypeConversion()
{
    // cleanup
    removeConnectors();
}

boost::shared_ptr< WModule > WMDataTypeConversion::factory() const
{
    return boost::shared_ptr< WModule >( new WMDataTypeConversion() );
}

const char** WMDataTypeConversion::getXPMIcon() const
{
    return datatypeconversion_xpm;
}

const std::string WMDataTypeConversion::getName() const
{
    return "Data Type Conversion (to float)";
}

const std::string WMDataTypeConversion::getDescription() const
{
    return "This module is intended for development use only. Modules for general use should not depend on it.<br><br> "
        "Allows one to change the data type of the provided WDataSetSingle to another type. "
        "E.g., double to float. At the moment only conversion to float is supported.";
}

void WMDataTypeConversion::moduleMain()
{
    // use the m_input "data changed" flag
    m_moduleState.add( m_input->getDataChangedCondition() );

    // signal ready state
    ready();

    // loop until the module container requests the module to quit
    while( !m_shutdownFlag() )
    {
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
        WAssert( m_dataSet, "No data set found." );

        switch( (*m_dataSet).getValueSet()->getDataType() )
        {
            case W_DT_UNSIGNED_CHAR:
            {
                boost::shared_ptr<WValueSet<unsigned char> > vals;
                vals = boost::dynamic_pointer_cast<WValueSet<unsigned char> >( ( *m_dataSet ).getValueSet() );
                WAssert( vals, "Data type and data type indicator must fit." );
                convertDataSet( vals );
                break;
            }
            case W_DT_INT16:
            {
                boost::shared_ptr<WValueSet<int16_t> > vals;
                vals = boost::dynamic_pointer_cast<WValueSet<int16_t> >( ( *m_dataSet ).getValueSet() );
                WAssert( vals, "Data type and data type indicator must fit." );
                convertDataSet( vals );
            }
            case W_DT_SIGNED_INT:
            {
                boost::shared_ptr<WValueSet<int32_t> > vals;
                vals = boost::dynamic_pointer_cast<WValueSet<int32_t> >( ( *m_dataSet ).getValueSet() );
                WAssert( vals, "Data type and data type indicator must fit." );
                convertDataSet( vals );
                break;
            }
            case W_DT_FLOAT:
            {
                boost::shared_ptr< WValueSet< float > > vals;
                vals = boost::dynamic_pointer_cast< WValueSet< float > >( ( *m_dataSet ).getValueSet() );
                WAssert( vals, "Data type and data type indicator must fit." );
                convertDataSet( vals );
                break;
            }
            case W_DT_DOUBLE:
            {
                boost::shared_ptr< WValueSet< double > > vals;
                vals = boost::dynamic_pointer_cast< WValueSet< double > >( ( *m_dataSet ).getValueSet() );
                WAssert( vals, "Data type and data type indicator must fit." );
                convertDataSet( vals );
                break;
            }
            default:
                WAssert( false, "Unknow data type in Data Type Conversion module." );
        }

        // this waits for m_moduleState to fire. By default, this is only the m_shutdownFlag condition.
        // NOTE: you can add your own conditions to m_moduleState using m_moduleState.add( ... )
        m_moduleState.wait();
    }
}

void WMDataTypeConversion::connectors()
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

void WMDataTypeConversion::properties()
{
    WModule::properties();
}


template< typename T > void WMDataTypeConversion::convertDataSet( boost::shared_ptr< WValueSet< T > > vals )
{
    boost::shared_ptr< std::vector< float > > newVals = boost::shared_ptr< std::vector< float > >( new std::vector< float >( vals->size() ) );
    for( size_t i = 0; i < newVals->size(); ++i )
    {
        ( *newVals )[i] = static_cast< float >( vals->getScalar( i ) );
    }

    boost::shared_ptr< WValueSet< float > > valueSet;
    valueSet = boost::shared_ptr< WValueSet< float > >( new WValueSet< float > ( 0, 1, newVals, W_DT_FLOAT ) );

    m_dataSet = boost::shared_ptr<WDataSetSingle>( new WDataSetSingle( valueSet, m_dataSet->getGrid() ) );
    m_output->updateData( m_dataSet );
}
