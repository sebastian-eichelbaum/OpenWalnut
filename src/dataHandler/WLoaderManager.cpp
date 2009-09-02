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

#include <iostream>
#include <string>
#include <boost/thread.hpp>

#include "WLoaderManager.h"
#include "WDataSet.h"
#include "io/WLoaderNIfTI.h"
#include "io/WLoaderBiosig.h"

std::string getSuffix( std::string name )
{
    size_t position = name.find_last_of( '.' );
    if( position == std::string::npos )
        return "";
    else
        return name.substr( position + 1 );
}

void WLoaderManager::load( std::string fileName, boost::shared_ptr< WDataHandler > dataHandler )
{
    std::string suffix = getSuffix( fileName );

    if( suffix == "nii" || suffix == "gz" )
    {
        WLoaderNIfTI niiLoader( fileName, dataHandler );
        boost::thread loaderThread( niiLoader );
    }
    else if( suffix == "edf" )
    {
        WLoaderBiosig biosigLoader( fileName, dataHandler );
        boost::thread loaderThread( biosigLoader );
    }
    else if( suffix == "vtk" )
    {
        // This is a dummy implementation.
        // You need to provide a real implementation here if you want to load vtk.
        std::cout << "VTK not implemented yet" << std::endl;
        assert( 0 );
    }
    else
    {
        std::cout << "Unknown file type: \"" << suffix << "\"" << std::endl;
        assert( 0 );
    }
}



