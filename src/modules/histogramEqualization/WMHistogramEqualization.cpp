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

#include "../../kernel/WKernel.h"
#include "../../common/WPropertyHelper.h"

#include "WMHistogramEqualization.h"
#include "WMHistogramEqualization.xpm"

// This line is needed by the module loader to actually find your module. You need to add this to your module too. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMHistogramEqualization )

WMHistogramEqualization::WMHistogramEqualization():
    WModule()
{
}

WMHistogramEqualization::~WMHistogramEqualization()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMHistogramEqualization::factory() const
{
    return boost::shared_ptr< WModule >( new WMHistogramEqualization() );
}

const char** WMHistogramEqualization::getXPMIcon() const
{
    return WMHistogramEqualization_xpm;
}

const std::string WMHistogramEqualization::getName() const
{
    return "Histogram Equalization";
}

const std::string WMHistogramEqualization::getDescription() const
{
    return "This module takes an arbitrary data set and equalizes its histogram. This increases contrast in several regions of the data.";
}

void WMHistogramEqualization::connectors()
{
    // call WModules initialization
    WModule::connectors();
}

void WMHistogramEqualization::properties()
{
    // call WModules initialization
    WModule::properties();
}

void WMHistogramEqualization::moduleMain()
{
    ready();

    waitForStop();
}

