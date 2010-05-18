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

#include "../../common/WAssert.h"
#include "../../common/WIOTools.h"
#include "../../common/WPropertyHelper.h"
#include "../../dataHandler/WDataSet.h"
#include "../../dataHandler/WDataSetSingle.h"
#include "../../dataHandler/WDataSetScalar.h"
#include "../../dataHandler/WSubject.h"
#include "../../dataHandler/WDataHandler.h"
#include "../../dataHandler/WDataTexture3D.h"
#include "../../dataHandler/WEEG2.h"
#include "../../dataHandler/exceptions/WDHException.h"
#include "../../dataHandler/io/WLoaderBiosig.h"
#include "../../dataHandler/io/WLoaderEEGASCII.h"
#include "../../dataHandler/io/WLoaderLibeep.h"
#include "../../dataHandler/io/WLoaderNIfTI.h"
#include "../../dataHandler/io/WPagerEEGLibeep.h"
#include "../../dataHandler/io/WReaderELC.h"
#include "../../dataHandler/io/WReaderFiberVTK.h"
#include "WMData.h"
#include "data.xpm"

WMData::WMData():
    WModule(),
    m_fileNameSet( false ),
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

const char** WMData::getXPMIcon() const
{
    return data_xpm;
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

void WMData::setFilename( boost::filesystem::path fname )
{
    if ( !m_fileNameSet )
    {
        m_fileNameSet = true;
        m_fileName = fname;
    }
}

boost::filesystem::path WMData::getFilename() const
{
    return m_fileName;
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

    m_dataName = m_infoProperties->addProperty( "Name", "The name of the dataset.", std::string( "" ) );

    // use this callback for the other properties
    WPropertyBase::PropertyChangeNotifierType propertyCallback = boost::bind( &WMData::propertyChanged, this, _1 );

    // several other properties
    m_interpolation = m_properties->addProperty( "Interpolation",
                                                  "If active, the boundaries of single voxels"
                                                  " will not be visible in colormaps. The transition between"
                                                  " them will be smooth by using interpolation then.",
                                                  true,
                                                  propertyCallback );
    m_threshold = m_properties->addProperty( "Threshold", "Values below this threshold will not be "
                                              "shown in colormaps.", 0., propertyCallback );
    m_opacity = m_properties->addProperty( "Opacity %", "The opacity of this data in colormaps combining"
                                            " values from several data sets.", 100, propertyCallback );
    m_opacity->setMax( 100 );
    m_opacity->setMin( 0 );

    m_colorMapSelectionsList = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_colorMapSelectionsList->addItem( "Grayscale", "" );
    m_colorMapSelectionsList->addItem( "Rainbow", "" );
    m_colorMapSelectionsList->addItem( "Hot iron", "" );
    m_colorMapSelectionsList->addItem( "Red-Yellow", "" );
    m_colorMapSelectionsList->addItem( "Blue-Lightblue", "" );
    m_colorMapSelectionsList->addItem( "Blue-Green-Purple", "" );

    m_colorMapSelection = m_properties->addProperty( "Colormap",  "Colormap type.", m_colorMapSelectionsList->getSelectorFirst(), propertyCallback );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_colorMapSelection );
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
        else if ( property == m_interpolation )
        {
            m_dataSet->getTexture()->setInterpolation( m_interpolation->get() );
        }
        else if ( property == m_colorMapSelection )
        {
            m_dataSet->getTexture()->setSelectedColormap( m_colorMapSelection->get( true ).getItemIndexOfSelected( 0 ) );
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
    WAssert( m_fileNameSet, "No filename specified." );

    using wiotools::getSuffix;
    std::string fileName = m_fileName.string();

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
        m_interpolation->setHidden();
        m_threshold->setHidden();
        m_opacity->setHidden();
        m_active->setHidden();
        m_colorMapSelection->setHidden();
    }

    if( suffix == ".nii"
        || ( suffix == ".gz" && ::nifti_compiled_with_zlib() ) )
    {
        if( suffix == ".gz" )  // it may be a NIfTI file too
        {
            boost::filesystem::path p( fileName );
            p.replace_extension( "" );
            suffix = getSuffix( p.string() );
            WAssert( suffix == ".nii", "Currently only nii files may be gzipped." );
        }

        m_isTexture = true;

        WLoaderNIfTI niiLoader( fileName );
        m_dataSet = niiLoader.load();

        if( boost::shared_dynamic_cast< WDataSetScalar >( m_dataSet ) )
        {
            m_threshold->setMin( boost::shared_dynamic_cast< WDataSetScalar >( m_dataSet )->getMin() );
            m_threshold->setMax( boost::shared_dynamic_cast< WDataSetScalar >( m_dataSet )->getMax() );
            m_threshold->set( boost::shared_dynamic_cast< WDataSetScalar >( m_dataSet )->getMin() );
        }

        boost::shared_ptr< WDataSetSingle > dss;
        dss =  boost::shared_dynamic_cast< WDataSetSingle >( m_dataSet );
        if( dss )
        {
            switch( (*dss).getValueSet()->getDataType() )
            {
                case W_DT_UNSIGNED_CHAR:
                case W_DT_INT16:
                case W_DT_SIGNED_INT:
                    m_colorMapSelection->set( m_colorMapSelectionsList->getSelector( 0 ) );
                    break;
                case W_DT_FLOAT:
                case W_DT_DOUBLE:
                    m_colorMapSelection->set( m_colorMapSelectionsList->getSelector( 5 ) );
                    break;
                default:
                    WAssert( false, "Unknow data type in Data module" );
            }
        }
        else
        {
            WAssert( false, "WDataSetSingle needed at this position." );
        }
    }
    else if( suffix == ".edf" )
    {
        WLoaderBiosig biosigLoader( fileName );
        m_dataSet = biosigLoader.load();
    }
    else if( suffix == ".asc" )
    {
        WLoaderEEGASCII eegAsciiLoader( fileName );
        m_dataSet = eegAsciiLoader.load();
    }
    else if( suffix == ".cnt" )
    {
        boost::shared_ptr< WPagerEEG > pager( new WPagerEEGLibeep( fileName ) );

        std::string elcFileName = fileName;
        elcFileName.resize( elcFileName.size() - 3 ); // drop suffix
        elcFileName += "elc"; // add new suffix
        WReaderELC elcReader( elcFileName );
        boost::shared_ptr< WEEGPositionsLibrary > eegPositionsLibrary = elcReader.read();

        m_dataSet = boost::shared_ptr< WEEG2 >( new WEEG2( pager, eegPositionsLibrary ) );
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

