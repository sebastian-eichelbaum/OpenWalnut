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
#include "core/dataHandler/WDataSetScalar.h"
#include "core/dataHandler/WDataHandlerEnums.h"
#include "core/kernel/WKernel.h"
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
    return "Data Type Conversion";
}

const std::string WMDataTypeConversion::getDescription() const
{
    return "This module is intended for development use only. Modules for general use should not depend on it.<br><br> "
        "Allows one to change the data type of the provided WDataSetSingle to another type. "
        "E.g., double to float.";
}

/**
 * Visitor for discriminating the type of the first valueset.
 */
template< class TargetType >
class VisitorVSet: public boost::static_visitor< boost::shared_ptr< WValueSetBase > >
{
public:
    /**
     * Create visitor instance and convert it to the given input type
     */
    VisitorVSet():
        boost::static_visitor< result_type >()
    {
    }

    /**
     * Called by boost::varying during static visiting. Creates new, converted valueset
     *
     * \tparam T the real integral type of the first value set.
     * \param vals the first valueset currently visited.
     *
     * \return the result from the operation with this and the second value set
     */
    template < typename T >
    result_type operator()( const WValueSet< T >* const& vals ) const             // NOLINT
    {
        boost::shared_ptr< std::vector< TargetType > > newVals( new std::vector< TargetType >( vals->size() ) );
        for( size_t i = 0; i < newVals->size(); ++i )
        {
            ( *newVals )[i] = static_cast< TargetType >( vals->getScalar( i ) );
        }

        boost::shared_ptr< WValueSet< TargetType > > valueSet(
            new WValueSet< TargetType >( 0, 1, newVals, DataType< TargetType >::type )
        );

        return valueSet;
    }
};

void WMDataTypeConversion::moduleMain()
{
    // use the m_input "data changed" flag
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    // signal ready state
    ready();

    // loop until the module container requests the module to quit
    while( !m_shutdownFlag() )
    {
        // this waits for m_moduleState to fire. By default, this is only the m_shutdownFlag condition.
        // NOTE: you can add your own conditions to m_moduleState using m_moduleState.add( ... )
        m_moduleState.wait();

        // acquire data from the input connector
        m_dataSet = m_input->getData();
        if( !m_dataSet )
        {
            // ok, the output has not yet sent data
            // NOTE: see comment at the end of this while loop for m_moduleState
            debugLog() << "Waiting for data ...";
            continue;
        }

        std::string dataTypeName = m_dataTypeSelection->get().at( 0 )->getName();
        boost::shared_ptr< WValueSetBase > valueSet;

        // different types
        if( dataTypeName == "UINT8" )
        {
            VisitorVSet< uint8_t > visitor;
            valueSet = m_dataSet->getValueSet()->applyFunction( visitor );
        }
        else if( dataTypeName == "UINT16")
        {
            VisitorVSet< uint16_t > visitor;
            valueSet = m_dataSet->getValueSet()->applyFunction( visitor );
        }
        else if( dataTypeName == "UINT32")
        {
            VisitorVSet< uint32_t > visitor;
            valueSet = m_dataSet->getValueSet()->applyFunction( visitor );
        }
        else if( dataTypeName == "UINT64")
        {
            VisitorVSet< uint64_t > visitor;
            valueSet = m_dataSet->getValueSet()->applyFunction( visitor );
        }
        else if( dataTypeName == "INT8" )
        {
            VisitorVSet< int8_t > visitor;
            valueSet = m_dataSet->getValueSet()->applyFunction( visitor );
        }
        else if( dataTypeName == "INT16")
        {
            VisitorVSet< int16_t > visitor;
            valueSet = m_dataSet->getValueSet()->applyFunction( visitor );
        }
        else if( dataTypeName == "INT32")
        {
            VisitorVSet< int32_t > visitor;
            valueSet = m_dataSet->getValueSet()->applyFunction( visitor );
        }
        else if( dataTypeName == "INT64")
        {
            VisitorVSet< int64_t > visitor;
            valueSet = m_dataSet->getValueSet()->applyFunction( visitor );
        }
        else if( dataTypeName == "FLOAT" )
        {
            VisitorVSet< float > visitor;
            valueSet = m_dataSet->getValueSet()->applyFunction( visitor );
        }
        else if( dataTypeName == "DOUBLE" )
        {
            VisitorVSet< double > visitor;
            valueSet = m_dataSet->getValueSet()->applyFunction( visitor );
        }
        else if( dataTypeName == "FLOAT128" )
        {
            VisitorVSet< long double > visitor;
            valueSet = m_dataSet->getValueSet()->applyFunction( visitor );
        }
        else
        {
            throw WException( "Not supported data type while reading raw data." );
        }

        // we have the valueset -> create dataset
        m_dataSet = boost::shared_ptr<WDataSetScalar>( new WDataSetScalar( valueSet, m_dataSet->getGrid() ) );
        m_output->updateData( m_dataSet );
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
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_dataTypeSelectionsList = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_dataTypeSelectionsList->addItem( "DOUBLE", "" );
    m_dataTypeSelectionsList->addItem( "FLOAT128", "" );
    m_dataTypeSelectionsList->addItem( "FLOAT", "" );
    m_dataTypeSelectionsList->addItem( "UINT8", "" );
    m_dataTypeSelectionsList->addItem( "UINT16", "" );
    m_dataTypeSelectionsList->addItem( "UINT32", "" );
    m_dataTypeSelectionsList->addItem( "UINT64", "" );
    m_dataTypeSelectionsList->addItem( "INT8", "" );
    m_dataTypeSelectionsList->addItem( "INT16", "" );
    m_dataTypeSelectionsList->addItem( "INT32", "" );
    m_dataTypeSelectionsList->addItem( "INT64", "" );

    m_dataTypeSelection = m_properties->addProperty( "Data type",  "Data type.", m_dataTypeSelectionsList->getSelectorFirst(), m_propCondition );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_dataTypeSelection );

    WModule::properties();
}

