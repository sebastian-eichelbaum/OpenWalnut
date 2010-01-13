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


void WMGaussFiltering::moduleMain()
{
    // use the m_input "data changed" flag
    m_moduleState.add( m_input->getDataChangedCondition() );

    // signal ready state
    ready();

    // loop until the module container requests the module to quit
    while ( !m_shutdownFlag() )
    {
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
