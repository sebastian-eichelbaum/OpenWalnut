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
#include "../../common/WStringUtils.h"

#include "WGEShaderVersionPreprocessor.h"

WGEShaderVersionPreprocessor::WGEShaderVersionPreprocessor()
{
    // initialize members
}

WGEShaderVersionPreprocessor::~WGEShaderVersionPreprocessor()
{
    // cleanup
}

std::string WGEShaderVersionPreprocessor::process( const std::string& file, const std::string& code ) const
{
    if( !getActive() )
    {
        return code;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Eliminate all #version statements and put it to the beginning.
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // this is an expression for the #version statement
    static const boost::regex versionRegexp( "^[ ]*#[ ]*version[ ]+([123456789][0123456789][0123456789]).*$" );

    // go through each line again
    std::string line;
    boost::smatch matches;          // the list of matches
    bool foundVersion = false;
    unsigned int version = 120;     // our default version
    std::stringstream completeCode( code );
    std::ostringstream cleanedCode;
    while( std::getline( completeCode, line ) )
    {
        if( boost::regex_match( line, matches, versionRegexp ) ) // look for the #version statement
        {
            unsigned int versionNum = string_utils::fromString< unsigned int >( matches[1] );
            version = std::max( versionNum, version );
            foundVersion = true;
            continue;
        }

        cleanedCode << line << std::endl;
    }

    // no version statement found, assume 1.2
    if( !foundVersion )
    {
        wlog::warn( "WGEShader (" + file + ")" ) << "No version statements in unrolled shader file \"" << file << "\" found. Using default: "
                                                 << version << ".";
    }

    // the ATI compiler needs the version statement to be the first statement in the shader
    std::stringstream vs;
    vs << "#version " << version << std::endl << cleanedCode.str();
    return vs.str();
}

