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

#include "../../dataHandler/WDataHandler.h"
#include "../../dataHandler/WDataTexture3D.h"
#include "../../kernel/WKernel.h"

#include "WMTextureMapper.xpm"
#include "WMTextureMapper.h"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMTextureMapper )

WMTextureMapper::WMTextureMapper() :
    WModule()
{
    // initialize
}

WMTextureMapper::~WMTextureMapper()
{
    // cleanup
    removeConnectors();
}

boost::shared_ptr< WModule > WMTextureMapper::factory() const
{
    return boost::shared_ptr< WModule >( new WMTextureMapper() );
}

const char** WMTextureMapper::getXPMIcon() const
{
    return WMTextureMapper_xpm;
}

const std::string WMTextureMapper::getName() const
{
    return "Texture Mapper";
}

const std::string WMTextureMapper::getDescription() const
{
    return "Can use the input as a texture that can be mapped to the navslices and so on.";
}

void WMTextureMapper::connectors()
{
    m_input = WModuleInputData< WDataSetSingle >::createAndAdd( shared_from_this(), "input", "Input to apply as texture." );

    // call WModules initialization
    WModule::connectors();
}

void WMTextureMapper::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    WPropertyBase::PropertyChangeNotifierType propertyCallback = boost::bind( &WMTextureMapper::propertyChanged, this, _1 );
    m_groupTex = m_properties->addPropertyGroup( "Texture Properties",  "Properties only related to the texture representation." );

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

    WModule::properties();
}

void WMTextureMapper::moduleMain()
{
    // let the main loop awake if the data changes or the properties changed.
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    // signal ready state
    ready();

    // loop until the module container requests the module to quit
    while( !m_shutdownFlag() )
    {
        // Now, the moduleState variable comes into play. The module can wait for the condition, which gets fired whenever the input receives data
        // or an property changes. The main loop now waits until something happens.
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        // woke up since the module is requested to finish
        if( m_shutdownFlag() )
        {
            break;
        }

        // has the data changed?
        if( m_input->handledUpdate() )
        {
            boost::shared_ptr< WDataSetSingle > dataSet = m_input->getData();

            // de-register at datahandler
            if ( m_lastDataSet )
            {
                debugLog() << "Removing previous texture.";
                WDataHandler::deregisterDataSet( m_lastDataSet );
            }

            // register only valid data
            if( dataSet )
            {
                m_lastDataSet = dataSet;

                // register new
                if ( m_lastDataSet->isTexture() )
                {
                    debugLog() << "Registering new texture";
                    WDataHandler::registerDataSet( m_lastDataSet );
                }
                else
                {
                    warnLog() << "Connected dataset is not usable as a texture.";
                }
            }
        }
    }

    // remove if module is removed
    if ( m_lastDataSet )
    {
        WDataHandler::deregisterDataSet( m_lastDataSet );
    }
}

void WMTextureMapper::activate()
{
    // deactivate the output if wanted
    if ( m_lastDataSet )
    {
        m_lastDataSet->getTexture()->setGloballyActive( m_active->get( true ) );
    }

    // Always call WModule's activate!
    WModule::activate();
}

void WMTextureMapper::propertyChanged( boost::shared_ptr< WPropertyBase > property )
{
    if ( !m_lastDataSet )
    {
        return;
    }

    if ( property == m_threshold )
    {
        m_lastDataSet->getTexture()->setThreshold( m_threshold->get() );
    }
    else if ( property == m_opacity )
    {
        m_lastDataSet->getTexture()->setOpacity( m_opacity->get() );
    }
    else if ( property == m_active )
    {
        m_lastDataSet->getTexture()->setGloballyActive( m_active->get() );
    }
    else if ( property == m_interpolation )
    {
        m_lastDataSet->getTexture()->setInterpolation( m_interpolation->get() );
    }
    else if ( property == m_colorMapSelection )
    {
        m_lastDataSet->getTexture()->setSelectedColormap( m_colorMapSelection->get( true ).getItemIndexOfSelected( 0 ) );
    }
}

