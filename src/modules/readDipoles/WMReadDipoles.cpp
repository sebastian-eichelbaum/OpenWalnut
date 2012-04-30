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

#include "core/kernel/WKernel.h"
#include "modules/readDipoles/WMReadDipoles.xpm"

#include "WMReadDipoles.h"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMReadDipoles )

WMReadDipoles::WMReadDipoles():
    WModule()
{
}

WMReadDipoles::~WMReadDipoles()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMReadDipoles::factory() const
{
    return boost::shared_ptr< WModule >( new WMReadDipoles() );
}

const char** WMReadDipoles::getXPMIcon() const
{
    return WMReadDipoles_xpm; // Please put a real icon here.
}
const std::string WMReadDipoles::getName() const
{
    return "Read Dipoles";
}

const std::string WMReadDipoles::getDescription() const
{
    return "Reading \".dip\" files containing position and additional information on dipoles reconstructed from EEG..";
}

void WMReadDipoles::connectors()
{
    // Put the code for your connectors here. See "src/modules/template/" for an extensively documented example.

    WModule::connectors();
}

void WMReadDipoles::properties()
{
    // Put the code for your properties here. See "src/modules/template/" for an extensively documented example.
    WModule::properties();
}

void WMReadDipoles::requirements()
{
    // Put the code for your requirements here. See "src/modules/template/" for an extensively documented example.
}

void WMReadDipoles::moduleMain()
{
}
