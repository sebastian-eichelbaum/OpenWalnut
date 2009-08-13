//---------------------------------------------------------------------------
//
// Project: OpenWalnut
//
// Copyright 2009 SomeCopyrightowner
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

#ifndef WTESTMODULE_H
#define WTESTMODULE_H

#include <string>

#include "WModule.h"

/**
 * \par Description:
 * Simple module for testing some WKernel functionality.
 */
class WTestModule: public WModule
{
public:

    /**
     * \par Description
     * Default constructor.
     */
    WTestModule();

    /**
     * \par Description
     * Destructor.
     */
    virtual ~WTestModule();

    /**
     * \par Description
     * Copy constructor
     * \param other Reference on object to copy.
     */
    WTestModule( const WTestModule& other );

    /**
     * \par Description
     * Gives back the name of this module.
     * \return the module's name.
     */
     virtual std::string getName() const;

    /**
     * \par Description
     * Gives bach a description of this module.
     * \return description to module.
     */
    virtual std::string getDescription() const;

protected:

private:
};

#endif  // WTESTMODULE_H

