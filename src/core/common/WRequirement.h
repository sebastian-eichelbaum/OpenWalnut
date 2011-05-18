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

#ifndef WREQUIREMENT_H
#define WREQUIREMENT_H

#include <string>

#include "WExportCommon.h"

/**
 * Base class for all possible kinds of requirements. Derive your own kind of requirement from this class. This allows easy checking of required
 * features on a target system. Of course this is only useful for things checkable during runtime. You should avoid writing classes for checking
 * weather some library is installed or not. This is checked by the linker on the target system.
 */
class OWCOMMON_EXPORT WRequirement // NOLINT
{
public:

    /**
     * Default constructor.
     */
    WRequirement();

    /**
     * Destructor.
     */
    virtual ~WRequirement();

    /**
     * Checks if the requirement is fulfilled on the system. Implement this for your specific case.
     *
     * \return true if the specific requirement is fulfilled.
     */
    virtual bool isComplied() const = 0;

    /**
     * Return a nice description of the requirement.
     *
     * \return the description.
     */
    virtual std::string getDescription() const = 0;

protected:

private:
};

#endif  // WREQUIREMENT_H

