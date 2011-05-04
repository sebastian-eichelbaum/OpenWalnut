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

// Use filesystem version 2 for compatibility with newer boost versions.
#ifndef BOOST_FILESYSTEM_VERSION
    #define BOOST_FILESYSTEM_VERSION 2
#endif
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>

#include "../../common/WAssert.h"
#include "../../common/WIOTools.h"
#include "../WDataSetFiberVector.h"
#include "../exceptions/WDHIOFailure.h"
#include "WWriterFiberVTK.h"

WWriterFiberVTK::WWriterFiberVTK( const boost::filesystem::path& path, bool overwrite )
    : WWriter( path.file_string(), overwrite )
{
}
void WWriterFiberVTK::writeFibs( boost::shared_ptr< const WDataSetFibers > fiberDS ) const
{
    writeFibs( boost::shared_ptr< WDataSetFiberVector >( new WDataSetFiberVector( fiberDS ) ) );
}

void WWriterFiberVTK::writeFibs( boost::shared_ptr< const WDataSetFiberVector > fiberDS ) const
{
    using std::fstream;
    fstream out( m_fname.c_str(), fstream::out | fstream::in | fstream::trunc );
    if( !out || out.bad() )
    {
        throw WDHIOFailure( std::string( "Invalid file, or permission: " + m_fname ) );
    }
    // We use '\n' as line delimiter so also files written under windows (having '\r\n' as delimtier) may be read anywhere
    char lineDelimiter = '\n';

    out << "# vtk DataFile Version 3.0" << lineDelimiter;
    out << "Fibers from OpenWalnut" << lineDelimiter;
    out << "BINARY" << lineDelimiter;
    out << "DATASET POLYDATA" << lineDelimiter;
    unsigned int numPoints = 0;
    unsigned int numLines = fiberDS->size();
    for( size_t i = 0; i < fiberDS->size(); ++i )
    {
        numPoints += (*fiberDS)[i].size();
    }
    out << "POINTS " << numPoints << " float" << lineDelimiter;
    unsigned int *rawLineData = new unsigned int[numPoints + numLines];
    float *rawPointData = new float[numPoints * 3];

    unsigned int pntPosOffset = 0;
    unsigned int lnsPosOffset = 0;
    for( size_t i = 0; i < fiberDS->size(); ++i )
    {
        const WFiber &fib = (*fiberDS)[i];
        rawLineData[lnsPosOffset++] = static_cast< unsigned int >( fib.size() );
        for( size_t j = 0; j < fib.size(); ++j )
        {
            const WPosition &point = fib[j];
            WAssert( pntPosOffset % 3 == 0, "(pOff % 3) was not equal to 0" );
            WAssert( pntPosOffset / 3 < numPoints, "pntPosOffset is to large." );
            rawLineData[lnsPosOffset++] = static_cast< unsigned int >( pntPosOffset / 3 );
            rawPointData[pntPosOffset++] = static_cast< float >( point[0] );
            rawPointData[pntPosOffset++] = static_cast< float >( point[1] );
            rawPointData[pntPosOffset++] = static_cast< float >( point[2] );
            WAssert( pntPosOffset < ( ( numPoints * 3 ) + 1 ), "pOff < #pts" );
        }
    }
    switchByteOrderOfArray< float >( rawPointData, numPoints * 3 );
    switchByteOrderOfArray< unsigned int >( rawLineData, numLines + numPoints );
    out.write( reinterpret_cast< char* >( rawPointData ), sizeof( float ) * numPoints * 3 );
    out << lineDelimiter;
    out << "LINES " << numLines << " " << numPoints + numLines << lineDelimiter;
    out.write( reinterpret_cast< char* >( rawLineData ), sizeof( unsigned int ) * ( numPoints + numLines ) );
    out << lineDelimiter;
    out.close();
}
