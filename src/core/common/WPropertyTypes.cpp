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

#include "WPropertyTypes.h"

std::ostream& WPVBaseTypes::operator<<( std::ostream& out, const WPVBaseTypes::PV_TRIGGER& c )
{
    // print it as nice string
    switch( c )
    {
        case PV_TRIGGER_TRIGGERED:
            out << "PV_TRIGGER_TRIGGERED";
            break;
        default:
            out << "PV_TRIGGER_READY";
    }

    return out;
}

std::istream& WPVBaseTypes::operator>>( std::istream& in, WPVBaseTypes::PV_TRIGGER& c )
{
    std::string s;
    in >> s;

    // interpret string
    if( s == "PV_TRIGGER_TRIGGERED" )
    {
        c = PV_TRIGGER_TRIGGERED;
    }
    else
    {
        c = PV_TRIGGER_READY;
    }

    return in;
}

