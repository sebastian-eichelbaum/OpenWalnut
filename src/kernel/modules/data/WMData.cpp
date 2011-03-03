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
#include "../../../dataHandler/WDataSetVector.h"
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
#include "../../../kernel/WModuleOutputData.h"
#include "WMData.h"
#include "data.xpm"

WMData::WMData():
    WModule(),
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
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    // properties

    m_dataName = m_infoProperties->addProperty( "Filename", "The filename of the dataset.", std::string( "" ) );
    m_dataType = m_infoProperties->addProperty( "Data type", "The type of the the single data values.", std::string( "" ) );

    // use this callback for the other properties
    WPropertyBase::PropertyChangeNotifierType propertyCallback = boost::bind( &WMData::propertyChanged, this, _1 );

    // { TODO(ebaum): this is deprecated and will be replaced by WGEColormapping
    m_groupTex = m_properties->addPropertyGroup( "Texture Properties ",  "Properties only related to the texture representation." );

    // several other properties
    m_interpolation = m_groupTex->addProperty( "Interpolation",
                                                  "If active, the boundaries of single voxels"
                                                  " will not be visible in colormaps. The transition between"
                                                  " them will be smooth by using interpolation then.",
                                                  true,
                                                  propertyCallback );
    m_threshold = m_groupTex->addProperty( "Threshold", "Values below this threshold will not be "
                                              "shown in colormaps.", 0.0, propertyCallback );
    m_threshold->setMax( 1.0 );
    m_threshold->setMin( 0.0 );

    m_opacity = m_groupTex->addProperty( "Opacity %", "The opacity of this data in colormaps combining"
                                            " values from several data sets.", 100, propertyCallback );
    m_opacity->setMax( 100 );
    m_opacity->setMin( 0 );

    m_colorMapSelectionsList = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_colorMapSelectionsList->addItem( "Grayscale", "" );
    m_colorMapSelectionsList->addItem( "Rainbow", "" );
    m_colorMapSelectionsList->addItem( "Hot iron", "" );
    m_colorMapSelectionsList->addItem( "Negative to positive", "" );
    m_colorMapSelectionsList->addItem( "Atlas", "" );
    m_colorMapSelectionsList->addItem( "Blue-Green-Purple", "" );
    m_colorMapSelectionsList->addItem( "Vector", "" );

    m_colorMapSelection = m_groupTex->addProperty( "Colormap",  "Colormap type.", m_colorMapSelectionsList->getSelectorFirst(), propertyCallback );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_colorMapSelection );

    m_matrixSelectionsList = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_matrixSelectionsList->addItem( "No matrix", "" );
    m_matrixSelectionsList->addItem( "sform", "" );
    m_matrixSelectionsList->addItem( "qform", "" );

    m_matrixSelection = m_properties->addProperty( "Transformation matrix",  "matrix",
            m_matrixSelectionsList->getSelectorFirst(), m_propCondition );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_matrixSelection );
    // }
}

void WMData::propertyChanged( boost::shared_ptr< WPropertyBase > property )
{
    if( m_isTexture )
    {
        // { TODO(ebaum): this is deprecated and will be replaced by WGEColormapping
        if ( property == m_threshold )
        {
            m_dataSet->getTexture()->setThreshold( m_threshold->get() );
        }
        else if ( property == m_opacity )
        {
            m_dataSet->getTexture()->setOpacity( m_opacity->get() );
        }
        else if ( property == m_interpolation )
        {
            m_dataSet->getTexture()->setInterpolation( m_interpolation->get() );
        }
        else if ( property == m_colorMapSelection )
        {
            m_dataSet->getTexture()->setSelectedColormap( m_colorMapSelection->get( true ).getItemIndexOfSelected( 0 ) );
        }
        // }
        if ( property == m_active )
        {
            // forward to texture
            m_dataSet->getTexture2()->active()->set( m_active->get( true ) );
            // { TODO(ebaum): this is deprecated and will be replaced by WGEColormapping
            m_dataSet->getTexture()->setGloballyActive( m_active->get() );
            // }
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

    m_transformNoMatrix.makeIdentity();
    m_transformSForm.makeIdentity();
    m_transformQForm.makeIdentity();

    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_propCondition );

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

    // { TODO(ebaum): this is deprecated and will be replaced by WGEColormapping
    if( suffix == ".fib"
        || suffix == ".cnt"
        || suffix == ".asc"
        || suffix == ".edf" )
    {
        // hide other properties since they make no sense fo these data set types.
        m_groupTex->setHidden();
    }
    // }

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
            switch( (*dss).getValueSet()->getDataType() )
            {
                case W_DT_UNSIGNED_CHAR:
                case W_DT_INT16:
                case W_DT_SIGNED_INT:
                    // { TODO(ebaum): this is deprecated and will be replaced by WGEColormapping
                    m_colorMapSelection->set( m_colorMapSelectionsList->getSelector( 0 ) );
                    // }
                    m_dataSet->getTexture2()->colormap()->set(
                        m_dataSet->getTexture2()->colormap()->get().newSelector( WItemSelector::IndexList( 1, 0 ) )
                    );
                    break;
                case W_DT_FLOAT:
                case W_DT_DOUBLE:
                    if( boost::shared_dynamic_cast< WDataSetVector >( m_dataSet ) )
                    {
                        // { TODO(ebaum): this is deprecated and will be replaced by WGEColormapping
                        m_colorMapSelection->set( m_colorMapSelectionsList->getSelector( 6 ) );
                        m_interpolation->set( false );
                        // }
                        m_dataSet->getTexture2()->colormap()->set(
                            m_dataSet->getTexture2()->colormap()->get().newSelector( WItemSelector::IndexList( 1, 6 ) )
                        );
                        m_dataSet->getTexture2()->interpolation()->set( false );
                    }
                    else
                    {
                        // { TODO(ebaum): this is deprecated and will be replaced by WGEColormapping
                        m_colorMapSelection->set( m_colorMapSelectionsList->getSelector( 5 ) );
                        // }
                        m_dataSet->getTexture2()->colormap()->set(
                            m_dataSet->getTexture2()->colormap()->get().newSelector( WItemSelector::IndexList( 1, 5 ) )
                        );
                    }
                    break;
                default:
                    WAssert( false, "Unknow data type in Data module" );
            }
        }

        // { TODO(ebaum): this is deprecated and will be replaced by WGEColormapping
        if( boost::shared_dynamic_cast< WDataSetScalar >( m_dataSet ) )
        {
            m_threshold->setMin( boost::shared_dynamic_cast< WDataSetScalar >( m_dataSet )->getMin() );
            m_threshold->setMax( boost::shared_dynamic_cast< WDataSetScalar >( m_dataSet )->getMax() );
            m_threshold->set( boost::shared_dynamic_cast< WDataSetScalar >( m_dataSet )->getMin() );
        }
        // }
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

    // I am interested in the active property ( manually subscribe signal )
    m_active->getCondition()->subscribeSignal( boost::bind( &WMData::propertyChanged, this, m_active ) );

    // textures also provide properties
    if ( m_dataSet->isTexture() )
    {
        WGEColormapping::registerTexture( m_dataSet->getTexture2(), m_dataName->get() );
        // m_properties->addProperty( m_dataSet->getTexture2()->getProperties() );
        m_infoProperties->addProperty( m_dataSet->getTexture2()->getInformationProperties() );
    }

    // { TODO(ebaum): this is deprecated and will be replaced by WGEColormapping
    // register at datahandler
    WDataHandler::registerDataSet( m_dataSet ); // this will get obsolete soon
    // }

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
                // { TODO(ebaum): this is deprecated and will be replaced by WGEColormapping
                WDataHandler::deregisterDataSet( m_dataSet );
                // }
                if ( m_dataSet->isTexture() )
                {
                    // m_properties->removeProperty( m_dataSet->getTexture2()->getProperties() );
                    m_infoProperties->removeProperty( m_dataSet->getTexture2()->getInformationProperties() );
                    WGEColormapping::deregisterTexture( m_dataSet->getTexture2() );
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
                    // textures also provide properties
                    // if ( m_dataSet->isTexture() )
                    {
                        WGEColormapping::registerTexture( m_dataSet->getTexture2(), m_dataName->get() );
                        // m_properties->addProperty( m_dataSet->getTexture2()->getProperties() );
                        m_infoProperties->addProperty( m_dataSet->getTexture2()->getInformationProperties() );
                    }

                    // { TODO(ebaum): this is deprecated and will be replaced by WGEColormapping
                    // register at datahandler
                    WDataHandler::registerDataSet( m_dataSet ); // this will get obsolete soon
                    // }
                }
            }
        }
    }

    // remove dataset from datahandler
    // { TODO(ebaum): this is deprecated and will be replaced by WGEColormapping
    WDataHandler::deregisterDataSet( m_dataSet );
    // }
    if ( m_dataSet->isTexture() )
    {
        // m_properties->removeProperty( m_dataSet->getTexture2()->getProperties() );
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

