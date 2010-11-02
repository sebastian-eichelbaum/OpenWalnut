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

#include "../../../common/WAssert.h"
#include "../../../common/WIOTools.h"
#include "../../../common/WPropertyHelper.h"
#include "../../../dataHandler/WDataSet.h"
#include "../../../dataHandler/WDataSetSingle.h"
#include "../../../dataHandler/WDataSetScalar.h"
#include "../../../dataHandler/WDataSetTimeSeries.h"
#include "../../../dataHandler/WSubject.h"
#include "../../../dataHandler/WDataHandler.h"
#include "../../../dataHandler/WDataTexture3D.h"
#include "../../../dataHandler/WDataTexture3D_2.h"
#include "../../../dataHandler/WEEG2.h"
#include "../../../dataHandler/exceptions/WDHException.h"
#include "../../../dataHandler/io/WReaderBiosig.h"
#include "../../../dataHandler/io/WReaderEEGASCII.h"
#include "../../../dataHandler/io/WReaderLibeep.h"
#include "../../../dataHandler/io/WReaderNIfTI.h"
#include "../../../dataHandler/io/WPagerEEGLibeep.h"
#include "../../../dataHandler/io/WReaderELC.h"
#include "../../../dataHandler/io/WReaderFiberVTK.h"
#include "../../../graphicsEngine/WGEColormapping.h"
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
    m_dataName = m_infoProperties->addProperty( "Filename", "The filename of the dataset.", std::string( "" ) );
    m_dataType = m_infoProperties->addProperty( "Data type", "The type of the the single data values.", std::string( "" ) );

    // use this callback for the other properties
    WPropertyBase::PropertyChangeNotifierType propertyCallback = boost::bind( &WMData::propertyChanged, this, _1 );
}

void WMData::propertyChanged( boost::shared_ptr< WPropertyBase > property )
{
    if( m_isTexture )
    {
        if ( property == m_active )
        {
            // forward to texture
            m_dataSet->getTexture2()->active()->set( m_active->get( true ) );
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

    // remove the path up to the file name and set it as a convenient name for this module instance
    if ( fileName != "" )
    {
        m_runtimeName->set( string_utils::tokenize( fileName, "/" ).back() );
    }

    // load it now
    std::string suffix = getSuffix( fileName );

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

        WReaderNIfTI niiLoader( fileName );
        m_dataSet = niiLoader.load();

        boost::shared_ptr< WDataSetSingle > dss;
        dss =  boost::shared_dynamic_cast< WDataSetSingle >( m_dataSet );
        if( dss )
        {
            m_dataType->set( getDataTypeString( dss ) );
        }

    }
    else if( suffix == ".edf" )
    {
        WReaderBiosig biosigLoader( fileName );
        m_dataSet = biosigLoader.load();
    }
    else if( suffix == ".asc" )
    {
        WReaderEEGASCII eegAsciiLoader( fileName );
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
        throw WDHException( std::string( "Unknown file type: '" + suffix + "'" ) );
    }

    debugLog() << "Loading data done.";

    // register the dataset properties
    m_properties->addProperty( m_dataSet->getProperties() );
    m_infoProperties->addProperty( m_dataSet->getInformationProperties() );

    // textures also provide properties
    if ( m_dataSet->isTexture() )
    {
        WGEColormapping::registerTexture( m_dataSet->getTexture2(), m_dataName->get() );
        m_properties->addProperty( m_dataSet->getTexture2()->getProperties() );
        m_infoProperties->addProperty( m_dataSet->getTexture2()->getInformationProperties() );
    }

    // i am interested in the active property ( manually subscribe signal )
    m_active->getCondition()->subscribeSignal( boost::bind( &WMData::propertyChanged, this, m_active ) );

    // notify
    m_output->updateData( m_dataSet );
    ready();

    // go to idle mode
    waitForStop();  // WThreadedRunner offers this for us. It uses boost::condition to avoid wasting CPU cycles with while loops.

    // remove dataset from datahandler
    if ( m_dataSet->isTexture() )
    {
        m_properties->removeProperty( m_dataSet->getTexture2()->getProperties() );
        m_infoProperties->removeProperty( m_dataSet->getTexture2()->getInformationProperties() );
        WGEColormapping::deregisterTexture( m_dataSet->getTexture2() );
    }
}

// TODO(wiebel): move this to some central place.
std::string WMData::getDataTypeString( boost::shared_ptr< WDataSetSingle > dss )
{
    std::string result;
    switch( (*dss).getValueSet()->getDataType() )
    {
        case W_DT_NONE:
            result = "none";
            break;
        case W_DT_BINARY:
            result = "binary (1 bit)";
            break;
        case W_DT_UNSIGNED_CHAR:
            result = "unsigned char (8 bits)";
            break;
        case W_DT_SIGNED_SHORT:
            result = "signed short (16 bits)";
            break;
        case W_DT_SIGNED_INT:
            result = "signed int (32 bits)";
            break;
        case W_DT_FLOAT:
            result = "float (32 bits)";
            break;
        case W_DT_COMPLEX:
            result = "complex";
            break;
        case W_DT_DOUBLE:
            result = "double (64 bits)";
            break;
        case W_DT_RGB:
            result = "RGB triple (24 bits)";
            break;
        case W_DT_ALL:
            result = "ALL (not very useful)";
            break;
        case W_DT_INT8:
            result = "signed char (8 bits)";
            break;
        case W_DT_UINT16:
            result = "unsigned short (16 bits)";
            break;
        case W_DT_UINT32 :
            result = "unsigned int (32 bits)";
            break;
        case W_DT_INT64:
            result = "int64";
            break;
        case W_DT_UINT64:
            result = "unsigned long long (64 bits)";
            break;
        case W_DT_FLOAT128:
            result = "float (128 bits)";
            break;
        case W_DT_COMPLEX128:
            result = "double pair (128 bits)";
            break;
        case W_DT_COMPLEX256:
            result = " long double pair (256 bits)";
            break;
        case W_DT_RGBA32:
            result = "4 byte RGBA (32 bits)";
            break;
        default:
            WAssert( false, "Unknow data type in getDataTypeString" );
    }
    return result;
}

