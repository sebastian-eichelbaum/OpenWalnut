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

#include "../../dataHandler/WDataTexture3D_2.h"
#include "../../graphicsEngine/WGEColormapping.h"
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

    m_replace = m_properties->addProperty( "Keep position",
                                           "If true, new texture on the input connector get placed in the list where the old one was.", true );

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

            // replace texture instead of removing it?
            if ( dataSet && dataSet->isTexture() && m_lastDataSet && m_replace->get( true ) )
            {
                debugLog() << "Replacing texture \"" << m_lastDataSet->getTexture2()->name()->get() << "\" with \"" <<
                                                        dataSet->getTexture2()->name()->get() << "\".";
                m_properties->removeProperty( m_lastDataSet->getTexture2()->getProperties() );
                m_infoProperties->removeProperty( m_lastDataSet->getTexture2()->getInformationProperties() );
                m_properties->addProperty( dataSet->getTexture2()->getProperties() );
                m_infoProperties->addProperty( dataSet->getTexture2()->getInformationProperties() );
                WGEColormapping::replaceTexture( m_lastDataSet->getTexture2(), dataSet->getTexture2() );
                m_lastDataSet = dataSet;
            }
            else
            {
                // de-register last input
                if ( m_lastDataSet )
                {
                    debugLog() << "Removing previous texture \"" << m_lastDataSet->getTexture2()->name()->get() << "\".";
                    m_properties->removeProperty( m_lastDataSet->getTexture2()->getProperties() );
                    m_infoProperties->removeProperty( m_lastDataSet->getTexture2()->getInformationProperties() );
                    WGEColormapping::deregisterTexture( m_lastDataSet->getTexture2() );
                    m_lastDataSet.reset();
                }

                // register only valid data
                if( dataSet && dataSet->isTexture() )
                {
                    m_lastDataSet = dataSet;

                    // register new
                    debugLog() << "Registering new texture \"" << m_lastDataSet->getTexture2()->name()->get() << "\".";
                    m_properties->addProperty( m_lastDataSet->getTexture2()->getProperties() );
                    m_infoProperties->addProperty( m_lastDataSet->getTexture2()->getInformationProperties() );
                    WGEColormapping::registerTexture( m_lastDataSet->getTexture2() );
                }
            }
        }
    }

    // remove if module is removed
    if ( m_lastDataSet )
    {
        debugLog() << "Removing previous texture \"" << m_lastDataSet->getTexture2()->name()->get() << "\".";
        WGEColormapping::deregisterTexture( m_lastDataSet->getTexture2() );
        // NOTE: the props get removed automatically
    }
}

void WMTextureMapper::activate()
{
    // deactivate the output if wanted
    if ( m_lastDataSet )
    {
        m_lastDataSet->getTexture2()->active()->set( m_active->get( true ) );
    }

    // Always call WModule's activate!
    WModule::activate();
}

