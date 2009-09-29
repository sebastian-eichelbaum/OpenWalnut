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

#ifndef WTESTMODULE_H
#define WTESTMODULE_H

#include <string>

#include "../../kernel/WModule.h"

/**
 * Simple module for testing some WKernel functionality.
 * \ingroup kernel
 */
class WTestModule: public WModule
{
public:

    /**
     * Default constructor.
     */
    WTestModule();

    /**
     * Destructor.
     */
    virtual ~WTestModule();

    /**
     * Copy constructor
     * \param other Reference on object to copy.
     */
    WTestModule( const WTestModule& other );

    /**
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Gives back a description of this module.
     * \return description to module.
     */
    virtual const std::string getDescription() const;

protected:

    /**
     * Entry point after loading the module. Runs in separate thread.
     */
    virtual void threadMain();

private:
};

#endif  // WTESTMODULE_H

