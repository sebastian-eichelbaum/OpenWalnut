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

#include "WMeshReaderInterface.h"

WMeshReaderInterface::~WMeshReaderInterface()
{
    // cleanup
}

std::string WMeshReaderInterface::getLine( boost::shared_ptr< std::ifstream > ifs, const std::string& desc ) const
{
    std::string line;
    try
    {
        // we use '\n' as line termination under every platform so our files (which are most likely to be generated on Unix systems)
        // can be read from all platforms not having those line termination symbols like e.g. windows ('\r\n').
        std::getline( *ifs, line, '\n' );
    }
    catch( const std::ios_base::failure &e )
    {
        throw WDHIOFailure( std::string( "IO error while " + desc + " of mesh file: , " + e.what() ) );
    }
    if( !ifs->good() )
    {
        throw WDHParseError( std::string( "Unexpected end of mesh file." ) );
    }
    return line;
}

