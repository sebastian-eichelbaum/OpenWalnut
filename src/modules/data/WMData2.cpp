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

#include "WMData2.h"

WMData2::WMData2():
    WModule()
{
    // initialize members
}

WMData2::~WMData2()
{
    // cleanup
}

boost::shared_ptr< WModule > WMData2::factory() const
{
    return boost::shared_ptr< WModule >( new WMData2() );
}

const std::string WMData2::getName() const
{
    return "Data Module";
}

const std::string WMData2::getDescription() const
{
    return "This module encapsulates data.";
}

void WMData2::connectors()
{
    // initialize connectors
    m_output= boost::shared_ptr< WModuleOutputData< boost::shared_ptr< WDataSet > > >(
            new WModuleOutputData< boost::shared_ptr< WDataSet > >( shared_from_this(),
                "out1", "A loaded dataset." )
            );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_output );

    // call WModules initialization
    WModule::connectors();
}

void WMData2::properties()
{
    // not yet implemented
}

void WMData2::notifyConnectionEstablished( boost::shared_ptr<WModuleConnector> /*here*/,
                                           boost::shared_ptr<WModuleConnector> /*there*/ )
{
    // not yet implemented
}

void WMData2::notifyConnectionClosed( boost::shared_ptr<WModuleConnector> /*here*/, boost::shared_ptr<WModuleConnector> /*there*/ )
{
    // not yet implemented
}

void WMData2::notifyDataChange( boost::shared_ptr<WModuleConnector> /*input*/,
                                boost::shared_ptr<WModuleConnector> /*output*/ )
{
    // not yet implemented
}

void WMData2::threadMain()
{
    // not yet implemented
}

