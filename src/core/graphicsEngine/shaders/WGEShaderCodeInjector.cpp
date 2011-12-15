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

#include <algorithm>
#include <ostream>
#include <sstream>
#include <string>

#include <boost/regex.hpp>

#include "../../common/WLogger.h"
#include "WGEShaderCodeInjector.h"

WGEShaderCodeInjector::WGEShaderCodeInjector( std::string keyword ):
    m_keyword( std::string( "%" ) + keyword + std::string( "%" ) )
{
    // initialize members
}

WGEShaderCodeInjector::~WGEShaderCodeInjector()
{
    // cleanup
}

std::string WGEShaderCodeInjector::process( const std::string& /*file*/, const std::string& code ) const
{
    if( !getActive() )
    {
        return code;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // replace keyword
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // go through each line again
    std::string line;
    std::stringstream completeCode( code );
    std::ostringstream newCode;
    while( std::getline( completeCode, line ) )
    {
        size_t found = line.find( m_keyword );
        if( found != std::string::npos )
        {
            line.replace( found, m_keyword.length(), m_code );
        }
        newCode << line << std::endl;
    }

    return newCode.str();
}

void WGEShaderCodeInjector::setCode( std::string code )
{
    m_code = code;
    updated();
}
