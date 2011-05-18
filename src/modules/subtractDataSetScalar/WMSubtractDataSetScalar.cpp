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

#include <string>

#include "core/kernel/WKernel.h"
#include "WMSubtractDataSetScalar.xpm"

#include "WMSubtractDataSetScalar.h"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMSubtractDataSetScalar )

WMSubtractDataSetScalar::WMSubtractDataSetScalar():
    WModule()
{
}

WMSubtractDataSetScalar::~WMSubtractDataSetScalar()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMSubtractDataSetScalar::factory() const
{
    return boost::shared_ptr< WModule >( new WMSubtractDataSetScalar() );
}

const char** WMSubtractDataSetScalar::getXPMIcon() const
{
    return WMSubtractDataSetScalar_xpm;
}

const std::string WMSubtractDataSetScalar::getName() const
{
    return "SubtractDataSetScalar";
}

const std::string WMSubtractDataSetScalar::getDescription() const
{
    return "Performs voxel-wise subtraction of values.";
}

void WMSubtractDataSetScalar::connectors()
{
    m_input1 = boost::shared_ptr< WModuleInputData< WDataSetScalar > >(
                            new WModuleInputData< WDataSetScalar >( shared_from_this(),
                                "in1", "The first input dataset." )
            );

    m_input2 = boost::shared_ptr< WModuleInputData< WDataSetScalar > >(
                            new WModuleInputData< WDataSetScalar >( shared_from_this(),
                                "in2", "The second input dataset." )
            );

    m_output = boost::shared_ptr< WModuleOutputData< WDataSetScalar > >(
                            new WModuleOutputData< WDataSetScalar >( shared_from_this(),
                                "out", "The first minus the second dataset." )
            );

    addConnector( m_input1 );
    addConnector( m_input2 );
    addConnector( m_output );

    // call WModules initialization
    WModule::connectors();
}

void WMSubtractDataSetScalar::properties()
{
}

void WMSubtractDataSetScalar::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input1->getDataChangedCondition() );
    m_moduleState.add( m_input2->getDataChangedCondition() );

    ready();

    while ( !m_shutdownFlag() )
    {
        debugLog() << "Waiting.";
        m_moduleState.wait();

        boost::shared_ptr< WDataSetScalar > first = m_input1->getData();
        boost::shared_ptr< WDataSetScalar > second = m_input2->getData();

        bool dataChanged = ( m_first != first ) || ( m_second != second );

        if( dataChanged && first && second )
        {
            m_first = first;
            m_second = second;

            subtract();
            m_output->updateData( m_result );
        }
    }
}

void WMSubtractDataSetScalar::subtract()
{
    if( m_first->getValueSet()->getDataType() != m_second->getValueSet()->getDataType() ||
        m_first->getGrid()->size() != m_second->getGrid()->size() )
    {
        m_result = boost::shared_ptr< WDataSetScalar >();
        return;
    }

    switch( m_first->getValueSet()->getDataType() )
    {
    case W_DT_INT16:
        subtractTyped< int16_t >();
        break;
    default:
        throw WException( std::string( "Unknown datatype." ) );
    }
}
