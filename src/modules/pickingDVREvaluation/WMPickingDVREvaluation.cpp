//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2017 OpenWalnut Community
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

#include "WMPickingDVREvaluation.h"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMPickingDVREvaluation )

WMPickingDVREvaluation::WMPickingDVREvaluation():
    WModule()
{
}

WMPickingDVREvaluation::~WMPickingDVREvaluation()
{
}

boost::shared_ptr< WModule > WMPickingDVREvaluation::factory() const
{
    return boost::shared_ptr< WModule >( new WMPickingDVREvaluation() );
}

const std::string WMPickingDVREvaluation::getName() const
{
    return "Picking in DVR Evaluation";
}
 
const std::string WMPickingDVREvaluation::getDescription() const
{
    return "Evaluate different picking techniques with regard to certain metrics.";
}

void WMPickingDVREvaluation::connectors()
{
    WModule::connectors();
}

void WMPickingDVREvaluation::properties()
{
    WModule::properties();
}

void WMPickingDVREvaluation::requirements()
{
}

void WMPickingDVREvaluation::moduleMain()
{
}
