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

#ifndef WMODULEREQUIREMENTNOTMET_H
#define WMODULEREQUIREMENTNOTMET_H

#include <string>

#include "WModuleException.h"

#include "../WExportKernel.h"

class WRequirement;

/**
 * Thrown whenever a module should be run but its requirements are not completely met.
 */
class OWKERNEL_EXPORT WModuleRequirementNotMet: public WModuleException
{
public:
    /**
     * Default constructor. Creates an instance and sets the message according to the requirement specified here.
     *
     * \param requirement the requirement that was not met.
     */
    explicit WModuleRequirementNotMet( const WRequirement* requirement );

    /**
     * Destructor.
     */
    virtual ~WModuleRequirementNotMet() throw();

protected:
private:
};

#endif  // WMODULEREQUIREMENTNOTMET_H

