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

#include "WMDistanceMap.h"

#include "../../kernel/WKernel.h"

WMDistanceMap::WMDistanceMap():
    WModule()
{
    // WARNING: initializing connectors inside the constructor will lead to an exception.
    // Implement WModule::initializeConnectors instead.
}
WMDistanceMap::~WMDistanceMap()
{
    // cleanup
    removeConnectors();
}

boost::shared_ptr< WModule > WMDistanceMap::factory() const
{
    return boost::shared_ptr< WModule >( new WMDistanceMap() );
}

const std::string WMDistanceMap::getName() const
{
    return "Distance Map";
}

const std::string WMDistanceMap::getDescription() const
{
    return "This description has to be improved when the module is completed."
" By now lets say the following: Computes a smoothed version of the dataset"
" and a distance map on it. Finally it renders this distance map using MarchinCubes";
}



void WMDistanceMap::moduleMain()
{
    // TODO(wiebel): MC fix this hack when possible by using an input connector.
    while ( !WKernel::getRunningKernel() )
    {
        sleep( 1 );
    }
    while ( !WKernel::getRunningKernel()->getDataHandler() )
    {
        sleep( 1 );
    }
    while ( !WKernel::getRunningKernel()->getDataHandler()->getNumberOfSubjects() )
    {
        sleep( 1 );
    }
}

void WMDistanceMap::connectors()
{
    // initialize connectors

    m_input = boost::shared_ptr<WModuleInputData< boost::shared_ptr< WDataSet > > >(
        new WModuleInputData< boost::shared_ptr< WDataSet > >( shared_from_this(),
                                                               "in", "Dataset to compute isosurface for." )
        );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_input );

    // call WModules initialization
    WModule::connectors();
}

void WMDistanceMap::properties()
{
//     ( m_properties->addDouble( "isoValue", 80 ) )->connect( boost::bind( &WMMarchingCubes::slotPropertyChanged, this, _1 ) );
}
