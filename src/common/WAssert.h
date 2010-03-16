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

#ifndef WASSERT_H
#define WASSERT_H

#include <string>

#include "WException.h"

/**
 * This class provides an interface siilar to assert,
 * but allows us to either throw an exception or a usual
 * assertion failure.
 * \warning At the moment we only support exceptions in very simple way. Assertions are future work.
 */
class WAssert
{
public:
    /**
     * OpenWalnut assertion.
     * \param value evaluated value of the assertion expression
     */
    inline explicit WAssert( bool value );
    /**
     * OpenWalnut assertion with info message.
     * \param value evaluated value of the assertion expression
     * \param msg Message describing the assertion.
     */
    inline explicit WAssert( bool value, std::string msg );
protected:
private:
};

WAssert::WAssert( bool value )
{
    if( !value )
    {
        throw WException( "WAssert failed. Please have a look at the backtrace for more information." );
    }
}

WAssert::WAssert( bool value, std::string msg )
{
    if( !value )
    {
        throw WException( "WAssert failed. Message: " + msg );
    }
}


#endif  // WASSERT_H
