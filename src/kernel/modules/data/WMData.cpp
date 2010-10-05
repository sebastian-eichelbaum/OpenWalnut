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

    // use this callback for the other properties
    WPropertyBase::PropertyChangeNotifierType propertyCallback = boost::bind( &WMData::propertyChanged, this, _1 );


    m_groupTex = m_properties->addPropertyGroup( "Texture Properties",  "Properties only related to the texture representation." );
    m_groupTexManip = m_properties->addPropertyGroup( "Texture Manipulation",  "Properties only related to the texture manipulation." );

    // several other properties
    m_interpolation = m_groupTex->addProperty( "Interpolation",
                                                  "If active, the boundaries of single voxels"
                                                  " will not be visible in colormaps. The transition between"
                                                  " them will be smooth by using interpolation then.",
                                                  true,
                                                  propertyCallback );
    m_threshold = m_groupTex->addProperty( "Threshold", "Values below this threshold will not be "
                                              "shown in colormaps.", 0., propertyCallback );
    m_opacity = m_groupTex->addProperty( "Opacity %", "The opacity of this data in colormaps combining"
                                            " values from several data sets.", 100, propertyCallback );
    m_opacity->setMax( 100 );
    m_opacity->setMin( 0 );

    m_colorMapSelectionsList = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_colorMapSelectionsList->addItem( "Grayscale", "" );
    m_colorMapSelectionsList->addItem( "Rainbow", "" );
    m_colorMapSelectionsList->addItem( "Hot iron", "" );
    m_colorMapSelectionsList->addItem( "Red-Yellow", "" );
    m_colorMapSelectionsList->addItem( "Atlas", "" );
    m_colorMapSelectionsList->addItem( "Blue-Green-Purple", "" );

    m_colorMapSelection = m_groupTex->addProperty( "Colormap",  "Colormap type.", m_colorMapSelectionsList->getSelectorFirst(), propertyCallback );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_colorMapSelection );

    m_matrixSelectionsList = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_matrixSelectionsList->addItem( "No matrix", "" );
    m_matrixSelectionsList->addItem( "qform", "" );
    m_matrixSelectionsList->addItem( "sform", "" );

    m_matrixSelection = m_groupTexManip->addProperty( "Transformation matrix",  "matrix",
            m_matrixSelectionsList->getSelectorFirst(), propertyCallback );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_matrixSelection );

    m_translationX = m_groupTexManip->addProperty( "X translation", "", 0, propertyCallback );
    m_translationX->setMax( 300 );
    m_translationX->setMin( -300 );
    m_translationY = m_groupTexManip->addProperty( "Y translation", "", 0, propertyCallback );
    m_translationY->setMax( 300 );
    m_translationY->setMin( -300 );
    m_translationZ = m_groupTexManip->addProperty( "Z translation", "", 0, propertyCallback );
    m_translationZ->setMax( 300 );
    m_translationZ->setMin( -300 );

    m_stretchX = m_groupTexManip->addProperty( "Voxel size X", "", 1.0, propertyCallback );
    m_stretchX->setMax( 10. );
    m_stretchX->setMin( -10. );
    m_stretchY = m_groupTexManip->addProperty( "Voxel size Y", "", 1.0, propertyCallback );
    m_stretchY->setMax( 10. );
    m_stretchY->setMin( -10. );
    m_stretchZ = m_groupTexManip->addProperty( "Voxel size Z", "", 1.0, propertyCallback );
    m_stretchZ->setMax( 10. );
    m_stretchZ->setMin( -10. );

    m_rotationX = m_groupTexManip->addProperty( "X rotation", "", 0, propertyCallback );
    m_rotationX->setMax( 180 );
    m_rotationX->setMin( -180 );
    m_rotationY = m_groupTexManip->addProperty( "Y rotation", "", 0, propertyCallback );
    m_rotationY->setMax( 180 );
    m_rotationY->setMin( -180 );
    m_rotationZ = m_groupTexManip->addProperty( "Z rotation", "", 0, propertyCallback );
    m_rotationZ->setMax( 180 );
    m_rotationZ->setMin( -180 );
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
        else if ( property == m_translationX || property == m_translationY || property == m_translationZ )
        {
            boost::shared_ptr< WGridRegular3D > grid = m_dataSet->getTexture()->getGrid();
            wmath::WPosition pos( m_translationX->get(), m_translationY->get(), m_translationZ->get() );
            grid->translate( pos );
            WDataHandler::getDefaultSubject()->getChangeCondition()->notify();
            m_output->triggerUpdate();
        }
        else if ( property == m_stretchX || property == m_stretchY || property == m_stretchZ )
        {
            boost::shared_ptr< WGridRegular3D > grid = m_dataSet->getTexture()->getGrid();
            wmath::WPosition str( m_stretchX->get(), m_stretchY->get(), m_stretchZ->get() );
            grid->stretch( str );
            WDataHandler::getDefaultSubject()->getChangeCondition()->notify();
            m_output->triggerUpdate();
        }
        else if ( property == m_rotationX || property == m_rotationY || property == m_rotationZ )
        {
            float pi = 3.14159265;
            float rotx = static_cast<float>( m_rotationX->get() ) / 180. * pi;
            float roty = static_cast<float>( m_rotationY->get() ) / 180. * pi;
            float rotz = static_cast<float>( m_rotationZ->get() ) / 180. * pi;

            boost::shared_ptr< WGridRegular3D > grid = m_dataSet->getTexture()->getGrid();
            wmath::WPosition rot( rotx, roty, rotz );
            grid->rotate( rot );
            WDataHandler::getDefaultSubject()->getChangeCondition()->notify();
            m_output->triggerUpdate();
        }
        else if ( property == m_matrixSelection )
        {
            boost::shared_ptr< WGridRegular3D > grid = m_dataSet->getTexture()->getGrid();
            grid->setActiveMatrix( m_matrixSelection->get( true ).getItemIndexOfSelected( 0 ) );
            WDataHandler::getDefaultSubject()->getChangeCondition()->notify();
            m_output->triggerUpdate();
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

    if( suffix == ".fib"
        || suffix == ".cnt"
        || suffix == ".asc"
        || suffix == ".edf" )
    {
        // hide other properties since they make no sense fo these data set types.
        m_groupTex->setHidden();
        m_groupTexManip->setHidden();
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

        WReaderNIfTI niiLoader( fileName );
        m_dataSet = niiLoader.load();

        if( !boost::shared_dynamic_cast< WDataSetTimeSeries >( m_dataSet ) )
        {
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
            boost::shared_ptr< WGridRegular3D > grid = m_dataSet->getTexture()->getGrid();
            m_matrixSelection->set( m_matrixSelectionsList->getSelector( grid->getActiveMatrix() ) );
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
        m_properties->addProperty( m_dataSet->getTexture()->getProperties() );
        m_infoProperties->addProperty( m_dataSet->getTexture()->getInformationProperties() );
    }

    // i am interested in the active property ( manually subscribe signal )
    m_active->getCondition()->subscribeSignal( boost::bind( &WMData::propertyChanged, this, m_active ) );

    // register at datahandler
    WDataHandler::registerDataSet( m_dataSet ); // this will get obsolete soon
    if ( m_dataSet->isTexture() )
    {
        WGEColormapping::registerTexture( m_dataSet->getTexture2() );
    }

    // notify
    m_output->updateData( m_dataSet );
    ready();

    // go to idle mode
    waitForStop();  // WThreadedRunner offers this for us. It uses boost::condition to avoid wasting CPU cycles with while loops.

    // remove dataset from datahandler
    WDataHandler::deregisterDataSet( m_dataSet );
}

