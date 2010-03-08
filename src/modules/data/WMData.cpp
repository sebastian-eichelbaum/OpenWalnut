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

#include "../../common/WIOTools.h"
#include "../../dataHandler/WDataSet.h"
#include "../../dataHandler/WDataSetSingle.h"
#include "../../dataHandler/WSubject.h"
#include "../../dataHandler/WDataHandler.h"
#include "../../dataHandler/WDataTexture3D.h"
#include "../../dataHandler/exceptions/WDHException.h"
#ifndef _MSC_VER
#include "../../dataHandler/io/WLoaderBiosig.h"
#endif
#include "../../dataHandler/io/WLoaderEEGASCII.h"
#include "../../dataHandler/io/WLoaderLibeep.h"
#include "../../dataHandler/io/WLoaderNIfTI.h"
#include "../../dataHandler/io/WReaderFiberVTK.h"
#include "WMData.h"

WMData::WMData():
    WModule(),
    m_isTexture()
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

MODULE_TYPE WMData::getType() const
{
    return MODULE_DATA;
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
    m_filename = m_properties2->addProperty( "filename", "The file to load.", WKernel::getAppPathObject(), true );
    m_dataName = m_properties2->addProperty( "Name", "The name of the dataset.", std::string( "" ) );

    // use this callback for the other properties
    WPropertyBase::PropertyChangeNotifierType propertyCallback = boost::bind( &WMData::propertyChanged, this, _1 );

    // several other properties
    m_interpolation = m_properties2->addProperty( "Interpolation", "Is interpolation active?", true, propertyCallback );
    m_threshold = m_properties2->addProperty( "Threshold", "The value threshold.", 0, propertyCallback );
    m_opacity = m_properties2->addProperty( "Opacity %", "The opacity of this data on other surfaces.", 100, propertyCallback );
    m_opacity->setMax( 100 );
    m_opacity->setMin( 0 );
}

void WMData::propertyChanged( boost::shared_ptr< WPropertyBase > property )
{
    if( m_isTexture )
    {
        if ( property == m_threshold )
        {
            m_dataSet->getTexture()->setThreshold( m_threshold->get() );
        }
        else if ( property == m_opacity )
        {
            m_dataSet->getTexture()->setOpacity( m_opacity->get() );
        }
        else if ( property == m_active )
        {
            m_dataSet->getTexture()->setGloballyActive( m_active->get() );
        }
    }
    else
    {
        if ( property == m_active )
        {
            if( m_active->get() )
            {
                m_output->updateData( m_dataSet );
            }
            else
            {
                m_output->updateData( boost::shared_ptr< WDataSet >() );
            }
        }
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

void WMData::notifyStop()
{
    // not used here. It gets called whenever the module should stop running.
}

void WMData::moduleMain()
{
    using wiotools::getSuffix;
    std::string fileName = m_filename->get().string();

    debugLog() << "Loading data from \"" << fileName << "\".";
    m_dataName->set( fileName );

    // load it now
    std::string suffix = getSuffix( fileName );

    if( suffix == ".fib"
        || suffix == ".cnt"
        || suffix == ".asc"
        || suffix == ".edf" )
    {
        // hide other properties since they make no sense fo these data set types.
        m_filename->setHidden();
        m_interpolation->setHidden();
        m_threshold->setHidden();
        m_opacity->setHidden();
        m_active->setHidden();
    }

    if( suffix == ".nii"
//#ifndef _MSC_VER
        || suffix == ".gz"
//#endif
        )
    {
        if( suffix == ".gz" )  // it may be a NIfTI file too
        {
            boost::filesystem::path p( fileName );
            p.replace_extension( "" );
            suffix = getSuffix( p.string() );
            assert( suffix == ".nii" && "currently only nii files may be gzipped" );
        }

        m_isTexture = true;

        WLoaderNIfTI niiLoader( fileName );
        m_dataSet = niiLoader.load();
    }
#ifndef _MSC_VER
    else if( suffix == ".edf" )
    {
        WLoaderBiosig biosigLoader( fileName );
        m_dataSet = biosigLoader.load();
    }
#endif
    else if( suffix == ".asc" )
    {
        WLoaderEEGASCII eegAsciiLoader( fileName );
        m_dataSet = eegAsciiLoader.load();
    }
    else if( suffix == ".cnt" )
    {
        WLoaderLibeep libeepLoader( fileName );
        m_dataSet = libeepLoader.load();
    }
    else if( suffix == ".fib" )
    {
        WReaderFiberVTK fibReader( fileName );
        m_dataSet = fibReader.read();
    }
    else
    {
        throw WDHException( "Unknown file type: '" + suffix + "'" );
    }

    debugLog() << "Loading data done.";

    // i am interested in the active property ( manually subscribe signal )
    m_active->getCondition()->subscribeSignal( boost::bind( &WMData::propertyChanged, this, m_active ) );

    // register at datahandler
    WDataHandler::registerDataSet( m_dataSet );

    // notify
    m_output->updateData( m_dataSet );
    ready();

    // go to idle mode
    waitForStop();  // WThreadedRunner offers this for us. It uses boost::condition to avoid wasting CPU cycles with while loops.

    // remove dataset from datahandler
    WDataHandler::deregisterDataSet( m_dataSet );
}

