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

#include "../../kernel/WKernel.h"
#include "../../graphicsEngine/WGERequirement.h"
#include "WMReadSphericalHarmonics.xpm"

#include "WMReadSphericalHarmonics.h"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMReadSphericalHarmonics )

WMReadSphericalHarmonics::WMReadSphericalHarmonics():
    WModule()
{
}

WMReadSphericalHarmonics::~WMReadSphericalHarmonics()
{
}

boost::shared_ptr< WModule > WMReadSphericalHarmonics::factory() const
{
    return boost::shared_ptr< WModule >( new WMReadSphericalHarmonics() );
}

const char** WMReadSphericalHarmonics::getXPMIcon() const
{
    return WMReadSphericalHarmonics_xpm;
}
const std::string WMReadSphericalHarmonics::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "Read Spherical Harmonics";
}

const std::string WMReadSphericalHarmonics::getDescription() const
{
    return "This modules loads data from vectors in NIfTI files as SphericalHarmonicsDatasets.";
}

void WMReadSphericalHarmonics::connectors()
{
    m_output= boost::shared_ptr< WModuleOutputData< WDataSetSphericalHarmonics > >( new WModuleOutputData< WDataSetSphericalHarmonics >(
                shared_from_this(), "Spherical Harmonics", "A loaded spherical harmonics dataset." )
            );
    addConnector( m_output );

    WModule::connectors();
}

void WMReadSphericalHarmonics::properties()
{
    // Put the code for your properties here. See "src/modules/template/" for an extensively documented example.

    WModule::properties();
}

void WMReadSphericalHarmonics::requirements()
{
    m_requirements.push_back( new WGERequirement() );
}

void WMReadSphericalHarmonics::moduleMain()
{
    // Put the code for your module's main functionality here.
    // See "src/modules/template/" for an extensively documented example.
}
