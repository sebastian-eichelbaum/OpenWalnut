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

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMWriteTracts )

WMWriteTracts::WMWriteTracts():
    WModule()
{
}

WMWriteTracts::~WMWriteTracts()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMWriteTracts::factory() const
{
    // See "src/modules/template/" for an extensively documented example.
    return boost::shared_ptr< WModule >( new WMWriteTracts() );
}

const char** WMWriteTracts::getXPMIcon() const
{
    return writeTracts_xpm;
}

const std::string WMWriteTracts::getName() const
{
    return "Write Tracts";
}

const std::string WMWriteTracts::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    // See "src/modules/template/" for an extensively documented example.
    return "Someone should add some documentation here. "
    "Probably the best person would be the modules's creator, i.e. \"wiebel\"";
}

void WMWriteTracts::connectors()
{
    typedef WModuleInputData< const WFiberCluster > InputType; // just an alias
    m_input = boost::shared_ptr< InputType >( new InputType( shared_from_this(), "tractInput", "A loaded dataset with grid." ) );
    addConnector( m_input );

    // call WModules initialization
    WModule::connectors();
}

void WMWriteTracts::properties()
{
    m_savePath         = m_properties->addProperty( "Save Path", "Where to save the result", boost::filesystem::path( "/no/such/file" ) );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_savePath );

    WModule::properties();
}

void WMWriteTracts::moduleMain()
{
    m_moduleState.add( m_input->getDataChangedCondition() );

    // signal ready state
    ready();

    // loop until the module container requests the module to quit
    while( !m_shutdownFlag() )
    {
        if( !m_input->getData() )
        {
            // ok, the output has not yet sent data
            // NOTE: see comment at the end of this while loop for m_moduleState
            debugLog() << "Waiting for data ...";
            m_moduleState.wait();
            continue;
        }
        WWriterFiberVTK w( m_savePath->get(), true );
        w.writeFibs( m_input->getData()->getDataSetReference() );

        // this waits for m_moduleState to fire. By default, this is only the m_shutdownFlag condition.
        // NOTE: you can add your own conditions to m_moduleState using m_moduleState.add( ... )
        m_moduleState.wait();
    }
}
