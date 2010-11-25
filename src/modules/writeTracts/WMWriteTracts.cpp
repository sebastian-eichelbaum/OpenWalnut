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

#include "../../common/WPropertyHelper.h"
#include "../../dataHandler/io/WWriterFiberVTK.h"
#include "../../kernel/WKernel.h"
#include "WMWriteTracts.h"
#include "WMWriteTracts.xpm"

W_LOADABLE_MODULE( WMWriteTracts )

WMWriteTracts::WMWriteTracts():
    WModule()
{
}

WMWriteTracts::~WMWriteTracts()
{
}

boost::shared_ptr< WModule > WMWriteTracts::factory() const
{
    return boost::shared_ptr< WModule >( new WMWriteTracts() );
}

const char** WMWriteTracts::getXPMIcon() const
{
    return WMWriteTracts_xpm;
}

const std::string WMWriteTracts::getName() const
{
    return "Write Tracts";
}

const std::string WMWriteTracts::getDescription() const
{
    return "Writes tracts either from a cluster or from a WDataSetFibers to a file";
}

void WMWriteTracts::connectors()
{
    m_clusterIC = WModuleInputData< const WFiberCluster >::createAndAdd( shared_from_this(), "clusterInput", "A the tracts behind the WFiberCluster" ); // NOLINT line length
    m_tractIC = WModuleInputData< const WDataSetFibers >::createAndAdd( shared_from_this(), "tractInput", "A dataset of tracts" );

    WModule::connectors();
}

void WMWriteTracts::properties()
{
    m_savePath = m_properties->addProperty( "Save Path", "Where to save the result", boost::filesystem::path( "/no/such/file" ) );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_savePath );
    m_run      = m_properties->addProperty( "Save", "Start saving", WPVBaseTypes::PV_TRIGGER_READY );

    WModule::properties();
}

void WMWriteTracts::moduleMain()
{
    m_moduleState.add( m_clusterIC->getDataChangedCondition() );
    m_moduleState.add( m_tractIC->getDataChangedCondition() );
    m_moduleState.add( m_run->getCondition() );

    ready();

    while( !m_shutdownFlag() )
    {
        debugLog() << "Waiting for data ...";
        m_moduleState.wait();

        if( !m_clusterIC->getData() && !m_tractIC->getData() )
        {
            continue;
        }

        if( m_run->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            WWriterFiberVTK w( m_savePath->get(), true );
            if( m_clusterIC->getData() )
            {
                w.writeFibs( m_clusterIC->getData()->getDataSetReference() );
            }
            else if( m_tractIC->getData() )
            {
                w.writeFibs( m_tractIC->getData() );
            }
        }
    }
}
