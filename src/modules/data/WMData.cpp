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
#include <vector>

#include "../../dataHandler/WDataSet.h"
#include "../../dataHandler/WDataSetSingle.h"
#include "../../dataHandler/WDataTexture3D.h"
#include "../../dataHandler/exceptions/WDHException.h"
#ifndef _WIN32
#include "../../dataHandler/io/WLoaderBiosig.h"
#endif
#include "../../dataHandler/io/WLoaderEEGASCII.h"
#include "../../dataHandler/io/WLoaderFibers.h"
#include "../../dataHandler/io/WLoaderNIfTI.h"
#include "../../utils/WIOTools.h"
#include "WMData.h"

WMData::WMData():
    WModule()
{
    // initialize members
}

WMData::~WMData()
{
    // cleanup
}

boost::shared_ptr< WModule > WMData::factory() const
{
    return boost::shared_ptr< WModule >( new WMData() );
}

const std::string WMData::getName() const
{
    return "Data Module";
}

const std::string WMData::getDescription() const
{
    return "This module encapsulates data.";
}

boost::shared_ptr< WDataSet > WMData::getDataSet()
{
    return m_dataSet;
}

void WMData::connectors()
{
    // initialize connectors
    m_output= boost::shared_ptr< WModuleOutputData< WDataSet > >( new WModuleOutputData< WDataSet >(
                shared_from_this(), "out", "A loaded dataset." )
            );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_output );

    // call WModules initialization
    WModule::connectors();
}

void WMData::properties()
{
    // properties

    // filename of file to load and handle
    m_properties->addString( "filename", "", true );
    m_properties->addString( "name" );
    ( m_properties->addBool( "active", true, true ) )->connect( boost::bind( &WMData::slotPropertyChanged, this, _1 ) );
    ( m_properties->addBool( "interpolation", true ) )->connect( boost::bind( &WMData::slotPropertyChanged, this, _1 ) );
    ( m_properties->addInt( "threshold", 0 ) )->connect( boost::bind( &WMData::slotPropertyChanged, this, _1 ) );
    ( m_properties->addInt( "alpha", 100 ) )->connect( boost::bind( &WMData::slotPropertyChanged, this, _1 ) );
    m_properties->setMax( "alpha", 100 );
}

void WMData::slotPropertyChanged( std::string propertyName )
{
    if ( propertyName == "threshold" )
    {
        m_dataSet->getTexture()->setThreshold( m_properties->getValue<float>( "threshold" ) );
    }
    if ( propertyName == "alpha" )
    {
        m_dataSet->getTexture()->setAlpha( m_properties->getValue<float>( "alpha" ) / 100.0 );
    }
}

void WMData::notifyConnectionEstablished( boost::shared_ptr<WModuleConnector> here,
                                           boost::shared_ptr<WModuleConnector> there )
{
    WLogger::getLogger()->addLogMessage( "Connected \"" + here->getCanonicalName() + "\" to \"" + there->getCanonicalName() + "\".",
            getName(), LL_DEBUG );
}

void WMData::notifyConnectionClosed( boost::shared_ptr<WModuleConnector> here, boost::shared_ptr<WModuleConnector> there )
{
    WLogger::getLogger()->addLogMessage( "Disconnected \"" + here->getCanonicalName() + "\" from \"" + there->getCanonicalName() + "\".",
            getName(), LL_DEBUG );
}

void WMData::notifyDataChange( boost::shared_ptr<WModuleConnector> /*input*/, boost::shared_ptr<WModuleConnector> /*output*/ )
{
    // not used here, since data modules can not receive an input.
}

void WMData::notifyStop()
{
    // not used here. It gets called whenever the module should stop running.
}

void WMData::moduleMain()
{
    using wiotools::getSuffix;
    std::string fileName = m_properties->getValue< std::string >( "filename" );

    debugLog() << "Loading data from \"" << fileName << "\".";
    m_properties->setValue( "name", fileName );

    // load it now
    std::string suffix = getSuffix( fileName );

    if( suffix == ".nii" || suffix == ".gz" )
    {
        if( suffix == ".gz" )  // it may be a NIfTI file too
        {
            boost::filesystem::path p( fileName );
            p.replace_extension( "" );
            suffix = getSuffix( p.string() );
            assert( suffix == ".nii" && "currently only nii files may be gzipped" );
        }
        WLoaderNIfTI niiLoader( fileName, WKernel::getRunningKernel()->getDataHandler() );
        m_dataSet = niiLoader.load();
    }
#ifndef _WIN32
    else if( suffix == ".edf" )
    {
        WLoaderBiosig biosigLoader( fileName, WKernel::getRunningKernel()->getDataHandler() );
        m_dataSet = biosigLoader.load();
    }
#endif
    else if( suffix == ".asc" )
    {
        WLoaderEEGASCII eegAsciiLoader( fileName, WKernel::getRunningKernel()->getDataHandler() );
        m_dataSet = eegAsciiLoader.load();
    }
    else if( suffix == ".vtk" )
    {
        // This is a dummy implementation.
        // You need to provide a real implementation here if you want to load vtk.
        errorLog() << "VTK not implemented yet";
        assert( 0 );
    }
    else if( suffix == ".fib" )
    {
        WLoaderFibers fibLoader( fileName, WKernel::getRunningKernel()->getDataHandler() );
        m_dataSet = fibLoader.load();

        // hide other properties since they make no sense at all
        m_properties->hideProperty( "filename" ); // File name is got via m_dataSet->getFileName()
        m_properties->hideProperty( "name" );
        m_properties->hideProperty( "active" );
        m_properties->hideProperty( "interpolation" );
        m_properties->hideProperty( "threshold" );
        m_properties->hideProperty( "alpha" );
    }
    else
    {
        throw WDHException( "Unknown file type: '" + suffix + "'" );
    }

    debugLog() << "Loading data done.";
    // notify
    m_output->updateData( m_dataSet );
    ready();

    // go to idle mode
    waitForStop();  // WThreadedRunner offers this for us. It uses boost::condition to avoid wasting CPU cycles with while loops.
}
