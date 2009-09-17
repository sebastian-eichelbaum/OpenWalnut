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

#include <iostream>
#include <string>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>

#include "WLoaderManager.h"
#include "WDataSet.h"
#include "io/WLoaderNIfTI.h"
#include "io/WLoaderBiosig.h"
#include "io/WLoaderEEGASCII.h"

std::string getSuffix( std::string name )
{
    boost::filesystem::path p( name );
    return p.extension();
}

void WLoaderManager::load( std::string fileName, boost::shared_ptr< WDataHandler > dataHandler )
{
    std::string suffix = getSuffix( fileName );

    if( suffix == ".nii" || suffix == ".gz" )
    {
        if( suffix == ".gz" )  // it may be a NIfTI file too
        {
            boost::filesystem::path p( fileName );
            p.replace_extension( "" );
            suffix = getSuffix( p.string() );
            assert( suffix == ".nii" && "currently only nii files may be gzipped" );
        }
        WLoaderNIfTI niiLoader( fileName, dataHandler );
        boost::thread loaderThread( niiLoader );
    }
    else if( suffix == ".edf" )
    {
        WLoaderBiosig biosigLoader( fileName, dataHandler );
        boost::thread loaderThread( biosigLoader );
    }
    else if( suffix == ".asc" )
    {
        WLoaderEEGASCII eegAsciiLoader( fileName, dataHandler );
        boost::thread loaderThread( eegAsciiLoader );
    }
    else if( suffix == ".vtk" )
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



