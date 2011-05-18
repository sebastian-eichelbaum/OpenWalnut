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

#ifndef WPROTOTYPED_H
#define WPROTOTYPED_H

#include <string>

#include <boost/shared_ptr.hpp>

#include "WExportCommon.h"

/**
 * Interface class for the concept "Prototype". The requirements are a zero-parameter constructor.
 */
class OWCOMMON_EXPORT WPrototyped // NOLINT
{
public:

    /**
     * Default constructor. Creates a instance of the class. This not necessarily mean that the instance is fully usable. This is
     * required for type checking and inheritance checking.
     */
    WPrototyped();

    /**
     * Destructor.
     */
    virtual ~WPrototyped();

    /**
     * Gets the name of this prototype.
     *
     * \return the name.
     */
    virtual const std::string getName() const = 0;

    /**
     * Gets the description for this prototype.
     *
     * \return the description
     */
    virtual const std::string getDescription() const = 0;

    /**
     * Checks whether the actual prototype has the specified runtime type.
     *
     * \return true if you can safely cast this instance to the specified type.
     */
    template < typename T > bool isA();

protected:
private:
};

template < typename T >
bool WPrototyped::isA()
{
    return dynamic_cast< T* >( this );
}

#endif  // WPROTOTYPED_H

