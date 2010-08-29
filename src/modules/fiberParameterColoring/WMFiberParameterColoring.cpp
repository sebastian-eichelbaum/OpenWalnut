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

#include <vector>
#include <string>

#include "../../kernel/WKernel.h"
#include "../../common/WPropertyHelper.h"
#include "../../dataHandler/WDataHandler.h"
#include "../../dataHandler/WDataTexture3D.h"

#include "WMFiberParameterColoring.h"
#include "WMFiberParameterColoring.xpm"

// This line is needed by the module loader to actually find your module. You need to add this to your module too. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMFiberParameterColoring )

WMFiberParameterColoring::WMFiberParameterColoring():
    WModule()
{
}

WMFiberParameterColoring::~WMFiberParameterColoring()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMFiberParameterColoring::factory() const
{
    return boost::shared_ptr< WModule >( new WMFiberParameterColoring() );
}

const char** WMFiberParameterColoring::getXPMIcon() const
{
    return WMFiberParameterColoring_xpm;
}

const std::string WMFiberParameterColoring::getName() const
{
    return "Fiber Parameter Coloring";
}

const std::string WMFiberParameterColoring::getDescription() const
{
    return "This module can color fibers according to different parameters and schemes.";
}

void WMFiberParameterColoring::connectors()
{
    // The input fiber dataset
    m_fiberInput = boost::shared_ptr< WModuleInputData < WDataSetFibers > >(
        new WModuleInputData< WDataSetFibers >( shared_from_this(), "fibers", "The fiber dataset to color" )
    );

    // As properties, every connector needs to be added to the list of connectors.
    addConnector( m_fiberInput );

    // the selected fibers go to this output
    m_fiberOutput = boost::shared_ptr< WModuleOutputData < WDataSetFibers > >(
        new WModuleOutputData< WDataSetFibers >( shared_from_this(),
                                        "out", "The fiber dataset colored by this module." )
    );

    // As above: make it known.
    addConnector( m_fiberOutput );

    // call WModule's initialization
    WModule::connectors();
}

void WMFiberParameterColoring::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    // call WModule's initialization
    WModule::properties();
}

void WMFiberParameterColoring::moduleMain()
{
    // get notified about data changes
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_fiberInput->getDataChangedCondition() );
    // Remember the condition provided to some properties in properties()? The condition can now be used with this condition set.
    m_moduleState.add( m_propCondition );

    ready();

    // main loop
    while ( !m_shutdownFlag() )
    {
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        // woke up since the module is requested to finish?
        if ( m_shutdownFlag() )
        {
            break;
        }

        // To query whether an input was updated, simply ask the input:
        bool dataUpdated = m_fiberInput->updated();
        boost::shared_ptr< WDataSetFibers > dataSet = m_fiberInput->getData();
        bool dataValid = ( dataSet );
        if ( !dataValid && !dataUpdated )
        {
            continue;
        }

        // update coloring
        dataSet->getVertices()->size();

        debugLog() << "Done";
    }
}

