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

#include <list>
#include <stdexcept>
#include <string>

#include <boost/algorithm/string.hpp>

#include "WException.h"

WException::WException( const std::string& msg ): exception()
{
    // initialize members
    m_Msg = msg;
}

WException::~WException() throw()
{
    // cleanup
}

const char* WException::what() const throw()
{
    return m_Msg.c_str();
}

std::string WException::getTrace() const
{
    std::string result( what() );
    result += "\n\n";
    std::list< std::string >::const_iterator citer;
    for( citer = m_trace.begin(); citer != m_trace.end(); ++citer )
        result += "trace: " + *citer + "\n";
    boost::trim( result );
    return result;
}

