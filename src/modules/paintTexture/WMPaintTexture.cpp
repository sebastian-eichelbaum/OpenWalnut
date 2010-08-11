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
#include <sstream>
#include <vector>

#include "../../kernel/WKernel.h"
#include "../../dataHandler/WDataHandler.h"
#include "../../dataHandler/WDataTexture3D.h"
#include "../../common/WPropertyHelper.h"


#include "paintTexture.xpm" // Please put a real icon here.

#include "WMPaintTexture.h"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMPaintTexture )

WMPaintTexture::WMPaintTexture():
    WModule()
{
}

WMPaintTexture::~WMPaintTexture()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMPaintTexture::factory() const
{
    return boost::shared_ptr< WModule >( new WMPaintTexture() );
}

const char** WMPaintTexture::getXPMIcon() const
{
    return paintTexture_xpm; // Please put a real icon here.
}
const std::string WMPaintTexture::getName() const
{
    return "PaintTexture";
}

const std::string WMPaintTexture::getDescription() const
{
    return "This module allows painting areas in a 3D texture";
}

void WMPaintTexture::connectors()
{
    // the input dataset is just used as source for resolurtion and transformation matrix
    m_input = boost::shared_ptr< WModuleInputData < WDataSetSingle  > >(
        new WModuleInputData< WDataSetSingle >( shared_from_this(), "in", "The input dataset." ) );
    addConnector( m_input );

    m_output = boost::shared_ptr< WModuleOutputData < WDataSetScalar  > >(
        new WModuleOutputData< WDataSetScalar >( shared_from_this(), "out", "The extracted image." ) );
    addConnector( m_output );

    WModule::connectors();
}

void WMPaintTexture::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_painting = m_properties->addProperty( "Paint", "If active, left click in the scene with pressed ctrl key"
                                                      " will paint something.", false, m_propCondition );

    m_pencilSelectionsList = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_pencilSelectionsList->addItem( "1x1", "" );
    m_pencilSelectionsList->addItem( "3x3", "" );
    m_pencilSelectionsList->addItem( "5x5", "" );
    m_pencilSelection = m_properties->addProperty( "Pencil",  "Pencil type.", m_pencilSelectionsList->getSelectorFirst() );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_pencilSelection );

    m_paintIndex = m_properties->addProperty( "Paint index", "Index we paint into the output texture", 1 );
    m_paintIndex->setMin( 0 );
    m_paintIndex->setMax( 255 );

    // these are taken from WMData
    m_interpolation = m_properties->addProperty( "Interpolation",
                                                  "If active, the boundaries of single voxels"
                                                  " will not be visible in colormaps. The transition between"
                                                  " them will be smooth by using interpolation then.",
                                                  false,
                                                  m_propCondition );
    m_opacity = m_properties->addProperty( "Opacity %", "The opacity of this data in colormaps combining"
                                            " values from several data sets.", 100, m_propCondition );
    m_opacity->setMax( 100 );
    m_opacity->setMin( 0 );

    m_colorMapSelectionsList = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_colorMapSelectionsList->addItem( "Grayscale", "" );
    m_colorMapSelectionsList->addItem( "Rainbow", "" );
    m_colorMapSelectionsList->addItem( "Hot iron", "" );
    m_colorMapSelectionsList->addItem( "Red-Yellow", "" );
    m_colorMapSelectionsList->addItem( "Atlas", "" );
    m_colorMapSelectionsList->addItem( "Blue-Green-Purple", "" );

    m_colorMapSelection = m_properties->addProperty( "Colormap",  "Colormap type.", m_colorMapSelectionsList->getSelector( 4 ), m_propCondition );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_colorMapSelection );

    m_queueAdded = m_properties->addProperty( "Something paint", "", false, m_propCondition );
    m_queueAdded->setHidden();
}

void WMPaintTexture::moduleMain()
{
    WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getPickHandler()->getPickSignal()->connect(
            boost::bind( &WMPaintTexture::queuePaint, this, _1 ) );

    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    ready();

    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        if( m_shutdownFlag() )
        {
            break;
        }

        boost::shared_ptr< WDataSetSingle > newDataSet = m_input->getData();
        bool dataChanged = ( m_dataSet != newDataSet );
        bool dataValid   = ( newDataSet );

        if( dataValid )
        {
            if( dataChanged )
            {
                m_dataSet = newDataSet;
                WAssert( m_dataSet, "" );
                WAssert( m_dataSet->getValueSet(), "" );

                if( m_outData )
                {
                    WDataHandler::deregisterDataSet( m_outData );
                }

                m_outData = createNewOutTexture();

                if( m_outData )
                {
                    setOutputProps();
                    m_outData->setFileName( std::string( "painted" ) );
                    WDataHandler::registerDataSet( m_outData );
                }

                m_output->updateData( m_outData );
            }
            else
            {
                if( m_outData )
                {
                    setOutputProps();
                }
            }
            if ( m_painting->changed() )
            {
                if ( m_painting->get( true ) )
                {
                    WKernel::getRunningKernel()->getSelectionManager()->setPaintMode( PAINTMODE_PAINT );
                }
                else
                {
                    WKernel::getRunningKernel()->getSelectionManager()->setPaintMode( PAINTMODE_NONE );
                }
            }
        }
        else // case !dataValid
        {
            if( m_outData )
            {
                WDataHandler::deregisterDataSet( m_outData );
            }
            m_outData = boost::shared_ptr< WDataSetScalar >();
            m_output->updateData( m_outData );
        }
        if ( m_queueAdded->changed() && m_queueAdded->get( true ) )
        {
            boost::shared_ptr< WDataSetScalar > old;
            if( m_outData )
            {
                old = m_outData;
            }
            m_outData = doPaint();

            WDataHandler::registerDataSet( m_outData );
            WDataHandler::deregisterDataSet( old );
            m_output->updateData( m_outData );
        }
    }

    debugLog() << "Shutting down...";

    if( m_outData )
    {
        WDataHandler::deregisterDataSet( m_outData );
    }

    debugLog() << "Finished! Good Bye!";
}

void WMPaintTexture::activate()
{
    if( m_outData )
    {
        m_outData->getTexture()->setGloballyActive( m_active->get() );
    }
    WModule::activate();
}

void WMPaintTexture::setOutputProps()
{
    if( m_outData )
    {
        m_outData->getTexture()->setThreshold( 0 );
        m_outData->getTexture()->setOpacity( m_opacity->get() );
        m_outData->getTexture()->setInterpolation( m_interpolation->get() );
        m_outData->getTexture()->setSelectedColormap( m_colorMapSelection->get( true ).getItemIndexOfSelected( 0 ) );
    }
}

boost::shared_ptr< WDataSetScalar > WMPaintTexture::createNewOutTexture()
{
    WAssert( m_dataSet, "" );
    WAssert( m_dataSet->getValueSet(), "" );
    WAssert( m_dataSet->getGrid(), "" );

    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( m_dataSet->getGrid() );
    WAssert( grid, "" );

    m_values.resize( m_dataSet->getGrid()->size(), 0 );
    m_values[0] = 255;

    boost::shared_ptr< WValueSet< unsigned char > > vs =
        boost::shared_ptr< WValueSet< unsigned char > >( new WValueSet< unsigned char >( 0, 1, m_values, W_DT_UINT8 ) );

    return boost::shared_ptr< WDataSetScalar >( new WDataSetScalar( vs, grid ) );
}

boost::shared_ptr< WDataSetScalar > WMPaintTexture::doPaint()
{
    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( m_outData->getGrid() );

    while ( !m_paintQueue.empty() )
    {
        wmath::WPosition paintPosition = m_paintQueue.front();
        m_paintQueue.pop();

        int voxelNum = grid->getVoxelNum( paintPosition );
        if ( voxelNum != -1 )
        {
            switch ( m_pencilSelection->get( true ).getItemIndexOfSelected( 0 ) )
            {
                case 0:
                    m_values[ voxelNum ] = m_paintIndex->get();
                    break;
                case 1:
                {
                    m_values[ voxelNum ] = m_paintIndex->get();
                    std::vector< size_t > ids = grid->getNeighbours27( voxelNum );
                    for ( size_t i = 0; i < ids.size(); ++i )
                    {
                        m_values[ ids[i] ] = m_paintIndex->get();
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }
    m_queueAdded->set( false );

    boost::shared_ptr< WValueSet< unsigned char > > vs =
                        boost::shared_ptr< WValueSet< unsigned char > >( new WValueSet< unsigned char >( 0, 1, m_values, W_DT_UINT8 ) );

    return boost::shared_ptr< WDataSetScalar >( new WDataSetScalar( vs, grid ) );
}

void WMPaintTexture::queuePaint( WPickInfo pickInfo )
{
    if ( !m_painting->get() || ( pickInfo.getMouseButton() != WPickInfo::MOUSE_LEFT ) )
    {
        return;
    }
    m_paintQueue.push( pickInfo.getPickPosition() );
    m_queueAdded->set( true );
}

void WMPaintTexture::createTexture()
{
    osg::ref_ptr< osg::Image > ima = new osg::Image;
    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( m_dataSet->getGrid() );

    ima->allocateImage( grid->getNbCoordsX(), grid->getNbCoordsY(), grid->getNbCoordsZ(), GL_LUMINANCE, GL_UNSIGNED_BYTE );

    unsigned char* data = ima->data();

    for ( unsigned int i = 0; i < grid->getNbCoordsX() * grid->getNbCoordsY() * grid->getNbCoordsZ(); ++i )
    {
        data[i] = m_values[i];
    }

    m_texture = osg::ref_ptr<osg::Texture3D>( new osg::Texture3D );
    m_texture->setFilter( osg::Texture3D::MIN_FILTER, osg::Texture3D::LINEAR );
    m_texture->setFilter( osg::Texture3D::MAG_FILTER, osg::Texture3D::LINEAR );
    m_texture->setWrap( osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_BORDER );
    m_texture->setWrap( osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_BORDER );
    m_texture->setWrap( osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_BORDER );
    m_texture->setImage( ima );
    m_texture->setResizeNonPowerOfTwoHint( false );
}
