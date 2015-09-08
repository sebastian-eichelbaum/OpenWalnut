//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2015 OpenWalnut Community, Nemtics, BSV@Uni-Leipzig
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

#include "core/kernel/WModuleFactory.h"
#include "WMDiffTensorScalars.h"

#include "WTensorTrace.h"
#include "WFA.h"
#include "WRA.h"
#include "WRD.h"
#include "WMD.h"
#include "WAD.h"

W_LOADABLE_MODULE( WMDiffTensorScalars )

WMDiffTensorScalars::WMDiffTensorScalars()
    : WModuleContainer( "Diffusion Tensor Scalars", "Computes various scalar measures for a given diffusion tensor field (2nd order)." ),
      m_strategy( "Scalar indices derived from tensors.", "Select one algorithm" )
{
    m_strategy.addStrategy( WTensorTrace::SPtr( new WFA() ) );
    m_strategy.addStrategy( WTensorTrace::SPtr( new WRA() ) );
    m_strategy.addStrategy( WTensorTrace::SPtr( new WAD() ) );
    m_strategy.addStrategy( WTensorTrace::SPtr( new WMD() ) );
    m_strategy.addStrategy( WTensorTrace::SPtr( new WRD() ) );
    m_strategy.addStrategy( WTensorTrace::SPtr( new WTensorTrace() ) );
}

WMDiffTensorScalars::~WMDiffTensorScalars()
{
}

boost::shared_ptr< WModule > WMDiffTensorScalars::factory() const
{
    return boost::shared_ptr< WModule >( new WMDiffTensorScalars() );
}

void WMDiffTensorScalars::connectors()
{
    m_tensorsIC = WModuleInputForwardData< WDataSetDTI >::createAndAdd( shared_from_this(), "tensorInput",
            "DataSetDTI storing the tensor field" );

    // math: I do not know how to hide this connector in network editor, as it is for internal use only
    m_evalsIC = WModuleInputData< WDataSetVector >::createAndAdd( shared_from_this(), "eigenValues",
            "EigenValues from the internal ÉigenSystem module" );

    m_scalarOC = WModuleOutputData< WDataSetScalar >::createAndAdd( shared_from_this(), "scalars",
        "The computed scalar dataset derived from the given tensor dataset" );

    WModule::connectors();
}

void WMDiffTensorScalars::properties()
{
    m_properties->addProperty( m_strategy.getProperties() );

    WModule::properties();
}

void WMDiffTensorScalars::moduleMain()
{
    initSubModules();

    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_strategy.getProperties()->getUpdateCondition() );

    // math: for several measures we need the eigenvalues from our submodule. Once they are computed
    // we need to proceed. Hence we need the dataChanged condition in our module state. But beware
    // there is also the tensor data input giving us events for the module main loop.
    m_moduleState.add( m_eigenSystem->getOutputConnector( "evalsOutput" )->getDataChangedCondition() );

    ready();

    while( !m_shutdownFlag() )
    {
        debugLog() << "Waiting...";

        m_moduleState.wait();

        boost::shared_ptr< WModuleOutputConnector > evalConnector = m_eigenSystem->getOutputConnector( "evalsOutput" );
        typedef WModuleOutputData< WDataSetVector > OCType;
        boost::shared_ptr< OCType > evalsOC = boost::dynamic_pointer_cast< OCType >( evalConnector );
        if( evalsOC )
        {
            m_evals = evalsOC->getData();
            m_tensors = m_tensorsIC->getData();
        }

        if( !m_tensors || !m_evals )
        {
            continue;
        }

        // Strategy anwenden
        debugLog() << "Start computing scalars...";

        WProgress::SPtr progress( new WProgress( "Creating Dataset", m_evals->getGrid()->size() ) );
        m_progress->addSubProgress( progress );

        m_scalarOC->updateData( m_strategy()->operator()( progress, m_shutdownFlag, m_tensors, m_evals ) );

        progress->finish();
        m_progress->removeSubProgress( progress );

        debugLog() << "Created scalar data done";

        if( m_shutdownFlag() )
        {
            break;
        }
    }
}

void WMDiffTensorScalars::initSubModules()
{
    // instantiation
    m_eigenSystem = WModuleFactory::getModuleFactory()->create( WModuleFactory::getModuleFactory()->getPrototypeByName(
            "Eigen System" ) ); // NOLINT
    add( m_eigenSystem );
    m_eigenSystem->isReady().wait();

    // wiring
    debugLog() << "Start wiring";
    m_tensorsIC->forward( m_eigenSystem->getInputConnector( "tensorInput" ) );
    debugLog() << "Wiring done";

    // add proporties of submodules
    m_properties->addPropertyGroup( "EigenSystem", "Props" )->addProperty( m_eigenSystem->getProperties() );
}
