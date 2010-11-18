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

#include "../../common/math/WPosition.h"
#include "../../common/math/WVector3D.h"
#include "../../common/WAssert.h"
#include "../../common/WProgress.h"
#include "../../common/WStringUtils.h"
#include "../../common/WTypeTraits.h"
#include "../../dataHandler/WGridRegular3D.h"
#include "../../kernel/WKernel.h"
#include "WMDataOperators.xpm"
#include "WMDataOperators.h"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMDataOperators )

WMDataOperators::WMDataOperators() :
    WModule()
{
    // initialize
}

WMDataOperators::~WMDataOperators()
{
    // cleanup
    removeConnectors();
}

boost::shared_ptr< WModule > WMDataOperators::factory() const
{
    return boost::shared_ptr< WModule >( new WMDataOperators() );
}

const char** WMDataOperators::getXPMIcon() const
{
    return WMDataOperators_xpm;
}

const std::string WMDataOperators::getName() const
{
    return "Data Operators";
}

const std::string WMDataOperators::getDescription() const
{
    return "Applies an selected operator on both datasets on a per-voxel basis. Until now, it assumes that both grids are the same.";
}

void WMDataOperators::connectors()
{
    m_inputA = WModuleInputData< WDataSetScalar >::createAndAdd( shared_from_this(), "operandA", "First operand of operation( A, B )." );
    m_inputB = WModuleInputData< WDataSetScalar >::createAndAdd( shared_from_this(), "operandB", "Second operand of operation( A, B )." );

    m_output = WModuleOutputData< WDataSetScalar >::createAndAdd( shared_from_this(), "result", "Result of voxel-wise operation( A, B )." );

    // call WModules initialization
    WModule::connectors();
}

void WMDataOperators::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    WModule::properties();
}

template < typename ValueTypeA, typename ValueTypeB >
boost::shared_ptr< WValueSet< WTypeTraits::TypePromotion< ValueTypeA, ValueTypeB > > >
operation( boost::shared_ptr< WValueSetBase > valueSetA, boost::shared_ptr< WValueSetBase > valueSetB )
{

}

void WMDataOperators::moduleMain()
{
    // let the main loop awake if the data changes or the properties changed.
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_inputA->getDataChangedCondition() );
    m_moduleState.add( m_inputB->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    // signal ready state
    ready();

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
        if( m_inputA->handledUpdate() || m_inputB->handledUpdate() )
        {
            boost::shared_ptr< WDataSetScalar > newDataSetA = m_inputA->getData();
            boost::shared_ptr< WDataSetScalar > newDataSetB = m_inputB->getData();

            // valid data?
            if( newDataSetA && newDataSetB )
            {
                debugLog() << "Processing ...";
                /*switch( (*m_dataSet).getValueSet()->getDataType() )
                {
                    case W_DT_UNSIGNED_CHAR:
                    {
                        boost::shared_ptr<WValueSet<unsigned char> > vals;
                        vals = boost::shared_dynamic_cast<WValueSet<unsigned char> >( ( *m_dataSet ).getValueSet() );
                        WAssert( vals, "Data type and data type indicator must fit." );
                        newValueSet = iterativeFilterField( vals, iterations );
                        break;
                    }
                    case W_DT_INT16:
                    {
                        boost::shared_ptr<WValueSet<int16_t> > vals;
                        vals = boost::shared_dynamic_cast<WValueSet<int16_t> >( ( *m_dataSet ).getValueSet() );
                        WAssert( vals, "Data type and data type indicator must fit." );
                        newValueSet = iterativeFilterField( vals, iterations );
                        break;
                    }
                    case W_DT_SIGNED_INT:
                    {
                        boost::shared_ptr<WValueSet<int32_t> > vals;
                        vals = boost::shared_dynamic_cast<WValueSet<int32_t> >( ( *m_dataSet ).getValueSet() );
                        WAssert( vals, "Data type and data type indicator must fit." );
                        newValueSet = iterativeFilterField( vals, iterations );
                        break;
                    }
                    case W_DT_FLOAT:
                    {
                        boost::shared_ptr<WValueSet<float> > vals;
                        vals = boost::shared_dynamic_cast<WValueSet<float> >( ( *m_dataSet ).getValueSet() );
                        WAssert( vals, "Data type and data type indicator must fit." );
                        newValueSet = iterativeFilterField( vals, iterations );
                        break;
                    }
                    case W_DT_DOUBLE:
                    {
                        boost::shared_ptr<WValueSet<double> > vals;
                        vals = boost::shared_dynamic_cast<WValueSet<double> >( ( *m_dataSet ).getValueSet() );
                        WAssert( vals, "Data type and data type indicator must fit." );
                        newValueSet = iterativeFilterField( vals, iterations );
                        break;
                    }
                    default:
                        WAssert( false, "Unknown data type in Gauss Filtering module" );
                }*/

                boost::shared_ptr< WValueSetBase > newValueSet;
                m_output->updateData( boost::shared_ptr<WDataSetScalar>( new WDataSetScalar( newValueSet, newDataSetA->getGrid() ) ) );

            }
        }
    }
}

