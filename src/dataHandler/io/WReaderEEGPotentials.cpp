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

#include <fstream>
#include <string>

#include "WReaderEEGPotentials.h"

WReaderEEGPotentials::WReaderEEGPotentials( std::string fname )
    : WReader( fname )
{
    // initialize members
}

WReaderEEGPotentials::~WReaderEEGPotentials()
{
    // cleanup
}

boost::shared_ptr< WDataSetScalar > WReaderEEGPotentials::read()
{
    /*
    try
    {
        std::ifstream ifs = std::ifstream();
        ifs.open( m_fname.c_str(), std::ifstream::in | std::ifstream::binary );
        if( ifs->bad() )
        {
            throw WDHIOFailure( "File could not be opened for read." );
        }
    }
    catch( WDHException e )
    {
        wlog::error( "WReaderFiberVTK" ) << "Abort loading VTK fiber file: " << m_fname << ", due to: " << e.what();
    }

    boost::shared_ptr< WDataSetFibers > fibers = boost::shared_ptr< WDataSetFibers >( new WDataSetFibers( m_points,
                                                                                                          m_fiberStartIndices,
                                                                                                          m_fiberLengths,
                                                                                                          m_pointFiberMapping ) );
    fibers->setFileName( m_fname );

    m_ifs->close();
    return fibers;*/
}

