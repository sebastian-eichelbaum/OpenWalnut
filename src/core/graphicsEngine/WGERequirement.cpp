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

#include "WGraphicsEngine.h"

#include "WGERequirement.h"

WGERequirement::WGERequirement():
    WRequirement()
{
    // initialize members
}

WGERequirement::~WGERequirement()
{
    // cleanup
}

bool WGERequirement::isComplied() const
{
    // simply using isRunning() is not sufficient. It is possible that a module starts before the GE has completed startup.
    // We use the wait function. It returns immediately if the engine already runs. It returns false if engine is not running and will not be
    // running.
    return WGraphicsEngine::waitForStartupComplete();
}

std::string WGERequirement::getDescription() const
{
    return "Module needs an running graphics engine.";
}

