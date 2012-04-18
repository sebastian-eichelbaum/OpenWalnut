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

#include <string>

#include "core/common/WAssert.h"
#include "core/common/WProgress.h"
#include "core/dataHandler/WGridRegular3D.h"
#include "core/kernel/WKernel.h"

#include "WMDataCreator.xpm"
#include "WMDataCreator.h"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMDataCreator )

WMDataCreator::WMDataCreator() :
    WModule()
{
    // WARNING: initializing connectors inside the constructor will lead to an exception.
    // Implement WModule::initializeConnectors instead.
}

WMDataCreator::~WMDataCreator()
{
    // cleanup
    removeConnectors();
}

boost::shared_ptr< WModule > WMDataCreator::factory() const
{
    return boost::shared_ptr< WModule >( new WMDataCreator() );
}

const char** WMDataCreator::getXPMIcon() const
{
    return datacreator_xpm;
}

const std::string WMDataCreator::getName() const
{
    return "Data Creator";
}

const std::string WMDataCreator::getDescription() const
{
    return "Allows the user to create data sets by providing a bunch of data creation schemes.";
}

void WMDataCreator::connectors()
{
    // initialize connectors
    m_output = WModuleOutputData< WDataSet >::createAndAdd( shared_from_this(), "out", "The data that has been created" );

    // call WModules initialization
    WModule::connectors();
}

void WMDataCreator::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    WModule::properties();
}

void WMDataCreator::moduleMain()
{
    // let the main loop awake if the data changes or the properties changed.
    m_moduleState.setResetable( true, true );
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

    }
}

