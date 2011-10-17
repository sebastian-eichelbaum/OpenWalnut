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

#include <boost/lexical_cast.hpp>

#include "core/dataHandler/WDataSetScalar.h"
#include "core/kernel/WModuleFactory.h"
#include "core/kernel/WModuleInputForwardData.h"
#include "WMProbTractDisplay.h"
#include "WMProbTractDisplay.xpm"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMProbTractDisplay )

WMProbTractDisplay::WMProbTractDisplay():
    WModuleContainer()
{
}

WMProbTractDisplay::~WMProbTractDisplay()
{
}

boost::shared_ptr< WModule > WMProbTractDisplay::factory() const
{
    return boost::shared_ptr< WModule >( new WMProbTractDisplay() );
}

const char** WMProbTractDisplay::getXPMIcon() const
{
    return probtractdisplay_xpm;
}

const std::string WMProbTractDisplay::getName() const
{
    return "Probabilistic Tract Rendering with multi transparent iso surfaces.";
}

const std::string WMProbTractDisplay::getDescription() const
{
    return "This module display probabilistic DTI tractograms with iso surfaces.";
}

void WMProbTractDisplay::connectors()
{
    m_input = WModuleInputForwardData < WDataSetScalar >::createAndAdd( shared_from_this(),
                                                                        "probTractInput",
                                                                        "The probabilistic tractogram as scalar dataset." );
    WModule::connectors();
}

void WMProbTractDisplay::properties()
{
    WModule::properties();
}

void WMProbTractDisplay::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    initSubModules();
    ready();
    debugLog() << "Module is now ready.";
    while( !m_shutdownFlag() )
    {
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        if( m_shutdownFlag() )
        {
            break;
        }
    }
}

void WMProbTractDisplay::initSubModules()
{
    static WColor colorInit[] = { defaultColor::GREEN, defaultColor::BLUE, defaultColor::RED }; // NOLINT curly braces
    std::vector< WColor > preDefinedColors( colorInit, colorInit + sizeof colorInit / sizeof colorInit[ 0 ] );

    static int init[] = { 100, 30, 0 }; // NOLINT
    std::vector< int > preDefinedOpacities( init, init + sizeof init / sizeof init[ 0 ] );

    WAssert( preDefinedOpacities.size() == preDefinedColors.size(), "preDefinedColors.size() many iso surfaces => so many colors are needed!" );

    // The reason for only having three iso surfaces is just simplicity, you may change it as you need, or even make it dynamically if you want
    for( size_t i = 0; i < preDefinedOpacities.size(); ++i )
    {
        m_isoSurfaces.push_back( WModuleFactory::getModuleFactory()->create( WModuleFactory::getModuleFactory()->getPrototypeByName( "Isosurface" ) ) ); // NOLINT
        add( m_isoSurfaces.back() );
        m_isoSurfaces.back()->isReady().wait();

        // forward data
        m_input->forward( m_isoSurfaces.back()->getInputConnector( "values" ) );

        // forward properties of interest, the reason for grouping is: property container cannot hold not-unique names
        std::string isoSurfaceNum = boost::lexical_cast< std::string >( i );
        WPropGroup m_group = m_properties->addPropertyGroup( "Isosurface " + isoSurfaceNum,  "Property group for isosurface " + isoSurfaceNum );
        WPropDouble p0 = m_isoSurfaces.back()->getProperties()->getProperty( "Iso value" )->toPropDouble();
        p0->setMax( 1000.0 );
        p0->set( 1000.0 );
        m_group->addProperty( p0 );
        WPropInt p1 = m_isoSurfaces.back()->getProperties()->getProperty( "Opacity %" )->toPropInt();
        p1->set( preDefinedOpacities[i] );
        m_group->addProperty( p1 );
        WPropColor p2 = m_isoSurfaces.back()->getProperties()->getProperty( "Surface color" )->toPropColor();
        p2->set( preDefinedColors[i] );
        m_group->addProperty( p2 );
    }
}
