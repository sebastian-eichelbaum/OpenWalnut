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

#include "core/common/WPathHelper.h"
#include "core/dataHandler/WDataHandlerEnums.h"
#include "core/graphicsEngine/WGERequirement.h"
#include "core/kernel/WKernel.h"

#include "io/WReaderNIfTI.h"

#include "WMReadSphericalHarmonics.h"
#include "WMReadSphericalHarmonics.xpm"

WMReadSphericalHarmonics::WMReadSphericalHarmonics():
    WModule()
{
}

WMReadSphericalHarmonics::~WMReadSphericalHarmonics()
{
}

boost::shared_ptr< WModule > WMReadSphericalHarmonics::factory() const
{
    return boost::shared_ptr< WModule >( new WMReadSphericalHarmonics() );
}

const char** WMReadSphericalHarmonics::getXPMIcon() const
{
    return WMReadSphericalHarmonics_xpm;
}
const std::string WMReadSphericalHarmonics::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "Read Spherical Harmonics";
}

const std::string WMReadSphericalHarmonics::getDescription() const
{
    return "This modules loads data from vectors in NIfTI files as SphericalHarmonicsDatasets.";
}

void WMReadSphericalHarmonics::connectors()
{
    m_output= boost::shared_ptr< WModuleOutputData< WDataSetSphericalHarmonics > >( new WModuleOutputData< WDataSetSphericalHarmonics >(
                shared_from_this(), "Spherical Harmonics", "A loaded spherical harmonics dataset." )
            );
    addConnector( m_output );

    WModule::connectors();
}

void WMReadSphericalHarmonics::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );
    m_dataFile = m_properties->addProperty( "NIfTI file", "", WPathHelper::getAppPath() );
    m_readTriggerProp = m_properties->addProperty( "Do read",  "Press!", WPVBaseTypes::PV_TRIGGER_READY, m_propCondition );
    WPropertyHelper::PC_PATHEXISTS::addTo( m_dataFile );

    WModule::properties();
}

void WMReadSphericalHarmonics::requirements()
{
    m_requirements.push_back( new WGERequirement() );
}

void WMReadSphericalHarmonics::moduleMain()
{
    m_moduleState.add( m_propCondition );
    ready();
    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        if( m_shutdownFlag() )
        {
            break;
        }
        std::string fileName = m_dataFile->get().string();

        boost::shared_ptr< WProgress > progress;
        progress = boost::shared_ptr< WProgress >( new WProgress( "Reading ...", 2 ) );
        m_progress->addSubProgress( progress );

        WReaderNIfTI niiLoader( fileName );
        boost::shared_ptr< WDataSet > data;
        data = niiLoader.load( W_DATASET_SPHERICALHARMONICS );

        ++*progress;

        if( data )
        {
            m_data = boost::dynamic_pointer_cast< WDataSetSphericalHarmonics >( data );
            if( m_data )
            {
                m_runtimeName->set( string_utils::tokenize( fileName, "/" ).back() );
                m_output->updateData( m_data );
            }
        }
        m_readTriggerProp->set( WPVBaseTypes::PV_TRIGGER_READY, false );

        progress->finish();
    }
}
