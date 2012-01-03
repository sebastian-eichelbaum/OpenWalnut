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

#include "core/common/WAssert.h"
#include "core/common/WIOTools.h"
#include "core/common/WPropertyHelper.h"
#include "core/dataHandler/WDataSet.h"
#include "core/dataHandler/WDataSetSingle.h"
#include "core/dataHandler/WDataSetScalar.h"
#include "core/dataHandler/WDataSetTimeSeries.h"
#include "core/dataHandler/WDataSetVector.h"
#include "core/dataHandler/WSubject.h"
#include "core/dataHandler/WDataHandler.h"
#include "core/dataHandler/WDataTexture3D.h"
#include "core/dataHandler/WEEG2.h"
#include "core/dataHandler/exceptions/WDHException.h"
#include "core/graphicsEngine/WGEColormapping.h"
#include "core/kernel/WModuleOutputData.h"

#include "io/WReaderBiosig.h"
#include "io/WReaderEEGASCII.h"
#include "io/WReaderNIfTI.h"
#include "io/WReaderELC.h"
#include "io/WReaderFiberVTK.h"
#include "io/WReaderLibeep.h"
#include "io/WPagerEEGLibeep.h"

#include "WMData.h"
#include "WMData.xpm"

WMData::WMData():
    WDataModule(),
    m_fileNameSet( false ),
    m_isTexture(),
    m_transformNoMatrix( 4, 4 ),
    m_transformSForm( 4, 4 ),
    m_transformQForm( 4, 4 )
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
    return WMData_xpm;
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
    if( !m_fileNameSet )
    {
        m_fileNameSet = true;
        m_fileName = fname;
    }
}

boost::filesystem::path WMData::getFilename() const
{
    return m_fileName;
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
    WDataModule::connectors();
}

void WMData::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    // properties
    m_dataName = m_infoProperties->addProperty( "Filename", "The filename of the dataset.", std::string( "" ) );
    m_dataType = m_infoProperties->addProperty( "Data type", "The type of the the single data values.", std::string( "" ) );

    m_matrixSelectionsList = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_matrixSelectionsList->addItem( "No matrix", "" );
    m_matrixSelectionsList->addItem( "sform", "" );
    m_matrixSelectionsList->addItem( "qform", "" );

    m_matrixSelection = m_properties->addProperty( "Transformation matrix",  "matrix",
            m_matrixSelectionsList->getSelectorFirst(), m_propCondition );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_matrixSelection );

    // use this callback for the other properties
    WPropertyBase::PropertyChangeNotifierType propertyCallback = boost::bind( &WMData::propertyChanged, this, _1 );
}

void WMData::propertyChanged( boost::shared_ptr< WPropertyBase > property )
{
    if( m_isTexture )
    {
        if( property == m_active )
        {
            // forward to texture
            m_dataSet->getTexture()->active()->set( m_active->get( true ) );
        }
    }
    else
    {
        if( property == m_active )
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

void WMData::moduleMain()
{
    WAssert( m_fileNameSet, "No filename specified." );

    m_transformNoMatrix.makeIdentity();
    m_transformSForm.makeIdentity();
    m_transformQForm.makeIdentity();

    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_propCondition );

    std::string fileName = m_fileName.string();

    debugLog() << "Loading data from \"" << fileName << "\".";
    m_dataName->set( fileName );

    // remove the path up to the file name and set it as a convenient name for this module instance
    if( fileName != "" )
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

        WReaderNIfTI niiLoader( fileName );
        m_dataSet = niiLoader.load();
        m_transformNoMatrix = niiLoader.getStandardTransform();
        m_transformSForm = niiLoader.getSFormTransform();
        m_transformQForm = niiLoader.getQFormTransform();

        m_isTexture = m_dataSet->isTexture();

        boost::shared_ptr< WDataSetSingle > dss = boost::shared_dynamic_cast< WDataSetSingle >( m_dataSet );
        if( dss )
        {
            m_dataType->set( getDataTypeString( dss ) );
            if( dss->isTexture() )
            {
                switch( (*dss).getValueSet()->getDataType() )
                {
                    case W_DT_UNSIGNED_CHAR:
                    case W_DT_INT16:
                    case W_DT_UINT16:
                    case W_DT_UINT32:
                    case W_DT_SIGNED_INT:
                        m_dataSet->getTexture()->colormap()->set(
                            m_dataSet->getTexture()->colormap()->get().newSelector( WItemSelector::IndexList( 1, 0 ) )
                        );
                        break;
                    case W_DT_FLOAT:
                    case W_DT_DOUBLE:
                        if( boost::shared_dynamic_cast< WDataSetVector >( m_dataSet ) )
                        {
                            m_dataSet->getTexture()->colormap()->set(
                                m_dataSet->getTexture()->colormap()->get().newSelector( WItemSelector::IndexList( 1, 6 ) )
                            );
                            m_dataSet->getTexture()->interpolation()->set( false );
                        }
                        else
                        {
                            m_dataSet->getTexture()->colormap()->set(
                                m_dataSet->getTexture()->colormap()->get().newSelector( WItemSelector::IndexList( 1, 5 ) )
                            );
                        }
                        break;
                    default:
                        WAssert( false, "Could not load \"" + fileName + "\". Reason: unknown data type in Data module" );
                }
            }
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
        throw WDHException( std::string( "Could not load \"" + fileName + "\". Reason: unknown file type: \"" + suffix + "\"" ) );
    }

    debugLog() << "Loading data done.";

    // register the dataset properties
    m_properties->addProperty( m_dataSet->getProperties() );
    m_infoProperties->addProperty( m_dataSet->getInformationProperties() );

    // I am interested in the active property ( manually subscribe signal )
    m_active->getCondition()->subscribeSignal( boost::bind( &WMData::propertyChanged, this, m_active ) );

    // textures also provide properties
    if( m_dataSet->isTexture() )
    {
        if( !getSuppressColormaps() )
        {
            WGEColormapping::registerTexture( m_dataSet->getTexture(), m_runtimeName->get() );
        }
        m_properties->addProperty( m_dataSet->getTexture()->getProperties() );
        m_infoProperties->addProperty( m_dataSet->getTexture()->getInformationProperties() );
    }

    // notify
    m_output->updateData( m_dataSet );
    ready();

    WDataSetSingle::SPtr dataSetAsSingle = boost::shared_dynamic_cast< WDataSetSingle >( m_dataSet );

    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();
        if( m_shutdownFlag() )
        {
            break;
        }

        // change transform matrix (only if we have a dataset single which contains the grid)
        if( m_matrixSelection->changed() && dataSetAsSingle )
        {
            if( m_dataSet && m_isTexture )
            {
                // remove dataset from datahandler
                if( m_dataSet->isTexture() )
                {
                    m_properties->removeProperty( m_dataSet->getTexture()->getProperties() );
                    m_infoProperties->removeProperty( m_dataSet->getTexture()->getInformationProperties() );
                    WGEColormapping::deregisterTexture( m_dataSet->getTexture() );
                }
            }

            // a new grid
            boost::shared_ptr< WGrid > newGrid;
            boost::shared_ptr< WGridRegular3D > oldGrid = boost::shared_dynamic_cast< WGridRegular3D >( dataSetAsSingle->getGrid() );

            switch( m_matrixSelection->get( true ).getItemIndexOfSelected( 0 ) )
            {
            case 0:
                newGrid = boost::shared_ptr< WGrid >( new WGridRegular3D( oldGrid->getNbCoordsX(), oldGrid->getNbCoordsY(), oldGrid->getNbCoordsZ(),
                                                                          WGridTransformOrtho( m_transformNoMatrix ) ) );
                break;
            case 1:
                newGrid = boost::shared_ptr< WGrid >( new WGridRegular3D( oldGrid->getNbCoordsX(), oldGrid->getNbCoordsY(), oldGrid->getNbCoordsZ(),
                                                                          WGridTransformOrtho( m_transformSForm ) ) );
                break;
            case 2:
                newGrid = boost::shared_ptr< WGrid >( new WGridRegular3D( oldGrid->getNbCoordsX(), oldGrid->getNbCoordsY(), oldGrid->getNbCoordsZ(),
                                                                          WGridTransformOrtho( m_transformQForm ) ) );
                break;
            }

            m_dataSet = dataSetAsSingle->clone( newGrid );

            // the clone() may have returned a zero-pointer, only update if it hasn't
            // this may happen if the clone() operation has not been implemented in the derived dataset class
            if( m_dataSet )
            {
                m_output->updateData( m_dataSet );

                if( m_isTexture )
                {
                    if( !getSuppressColormaps() )
                    {
                        WGEColormapping::registerTexture( m_dataSet->getTexture(), m_runtimeName->get() );
                    }
                    m_properties->addProperty( m_dataSet->getTexture()->getProperties() );
                    m_infoProperties->addProperty( m_dataSet->getTexture()->getInformationProperties() );
                }
            }
        }
    }

    // remove dataset from datahandler
    if( m_dataSet->isTexture() )
    {
        m_properties->removeProperty( m_dataSet->getTexture()->getProperties() );
        m_infoProperties->removeProperty( m_dataSet->getTexture()->getInformationProperties() );
        WGEColormapping::deregisterTexture( m_dataSet->getTexture() );
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

