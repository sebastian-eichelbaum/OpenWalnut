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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This is a tutorial on how to re-use other modules and how to combine them into a new module, a so called container-module.
//
// If you want to learn how to program a module, refer to WMTemplate.cpp. It is an extensive tutorial on all the details.
// In this tutorial, we assume you already know how to write modules. For other examples, refer to the README file.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <string>

#include "core/kernel/WKernel.h"

#include "WMTemplateContainers.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// All the basic setup ...
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WMTemplateContainers::WMTemplateContainers()
    : WModule()
{
}

WMTemplateContainers::~WMTemplateContainers()
{
}

boost::shared_ptr< WModule > WMTemplateContainers::factory() const
{
    return boost::shared_ptr< WModule >( new WMTemplateContainers() );
}

const std::string WMTemplateContainers::getName() const
{
    return "Template Container";
}

const std::string WMTemplateContainers::getDescription() const
{
    return "Show the possibilities of combining and re-using existing modules in container modules.";
}

void WMTemplateContainers::connectors()
{
    // We do not need any connectors. Have a look at WMTemplate.cpp if you want to know what this means.
    WModule::connectors();
}

void WMTemplateContainers::properties()
{
    /*
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    // We create some dummy preferences here to use in our widgets:
    m_triggerProp = m_properties->addProperty( "Do it now!",               "Trigger Button Text.", WPVBaseTypes::PV_TRIGGER_READY );
    m_boolProp = m_properties->addProperty( "Enable feature",           "Description.", true );
    m_properties->addProperty( "Number of shape rows",     "Number of shape rows.", 10 );
    m_properties->addProperty( "CLONE!Number of shape rows",
                                "A property which gets modified if \"Number of shape rows\" gets modified.", 10 );
    m_properties->addProperty( "Shape radii",              "Shape radii.", 20.0 );
    m_properties->addProperty( "A string",                 "Something.", std::string( "hello" ) );
    m_properties->addProperty( "A filename",               "Description.", WPathHelper::getAppPath() );
    m_properties->addProperty( "A color",                  "Description.", WColor( 1.0, 0.0, 0.0, 1.0 ) );
    m_properties->addProperty( "Somewhere",                "Description.", WPosition( 0.0, 0.0, 0.0 ) );
*/
    WModule::properties();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ATTENTION: now it gets interesting ...
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void WMTemplateContainers::requirements()
{
}

void WMTemplateContainers::moduleMain()
{
    m_moduleState.setResetable( true, true );
    //m_moduleState.add( m_propCondition );

    // As done above, we
    // Now, we can mark the module ready.
    ready();

    // Now the remaining module code. In our case, this is empty.
    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();
        if( m_shutdownFlag() )
        {
            break;
        }
    }

    // Never miss to clean up. If you miss this step, the shared_ptr might get deleted -> GUI crash
    debugLog() << "Shutting down ...";
}

