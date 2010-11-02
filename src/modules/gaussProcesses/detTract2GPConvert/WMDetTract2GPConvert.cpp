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

#include "../../../kernel/WKernel.h"
#include "WMDetTract2GPConvert.xpm"
#include "WMDetTract2GPConvert.h"

W_LOADABLE_MODULE( WMDetTract2GPConvert )

WMDetTract2GPConvert::WMDetTract2GPConvert():
    WModule()
{
}

WMDetTract2GPConvert::~WMDetTract2GPConvert()
{
}

boost::shared_ptr< WModule > WMDetTract2GPConvert::factory() const
{
    return boost::shared_ptr< WModule >( new WMDetTract2GPConvert() );
}

const char** WMDetTract2GPConvert::getXPMIcon() const
{
    return WMDetTract2GPConvert_xpm;
}
const std::string WMDetTract2GPConvert::getName() const
{
    return "Deterministic Tract to Gaussian Process Converter";
}

const std::string WMDetTract2GPConvert::getDescription() const
{
    return "Converts deterministic tracts to gaussian processes as described in the paper of Wassermann: "
           "http://dx.doi.org/10.1016/j.neuroimage.2010.01.004";
}

void WMDetTract2GPConvert::connectors()
{
    m_tractIC = WModuleInputData< WDataSetFibers >::createAndAdd( shared_from_this(), "tractInput", "The deterministic tracts" );
    m_tensorIC = WModuleInputData< WDataSetDTI >::createAndAdd( shared_from_this(), "tensorInput", "The 2nd order symmetric diffusion tensors" );
    m_gpOC = WModuleOutputData< WDataSetGP >::createAndAdd( shared_from_this(), "gpOutput", "The Gaussian processes" );

    WModule::connectors();
}

void WMDetTract2GPConvert::properties()
{
    WModule::properties();
}

void WMDetTract2GPConvert::moduleMain()
{
}
