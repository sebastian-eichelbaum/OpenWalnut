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

#include <cassert>
#include <fstream>
#include <string>

#include <boost/shared_ptr.hpp>

#include "WWriterVTK.h"
#include "WIOTools.hpp"
#include "../WDataSetFibers.h"
#include "../exceptions/WDHIOFailure.h"

WWriterVTK::WWriterVTK( std::string fname, bool overwrite )
    : m_overwrite( overwrite )
{
    setFileName( fname );
}

void WWriterVTK::setFileName( std::string fname )
{
    m_fname = fname;
    if( !m_overwrite && wiotools::fileExists( m_fname ) )
    {
        throw WDHIOFailure( "File '" + m_fname + "' already exists, skip writing" );
    }
}

void WWriterVTK::writeFibs( boost::shared_ptr< const WDataSetFibers > fiberDS ) const
{
    using std::fstream;
    fstream out( m_fname.c_str(), fstream::out | fstream::in | fstream::trunc );
    if( !out || out.bad() )
    {
        throw WDHIOFailure( "Invalid file, or permission: " + m_fname );
    }
    out << "# vtk DataFile Version 3.0" << std::endl;
    out << "Fibers from OpenWalnut" << std::endl;
    out << "BINARY" << std::endl;
    out << "DATASET POLYDATA" << std::endl;
    unsigned int numPoints = 0;
    unsigned int numLines = fiberDS->size();
    for( size_t i = 0; i < fiberDS->size(); ++i )
    {
        numPoints += (*fiberDS)[i].size();
    }
    out << "POINTS " << numPoints << " float" << std::endl;
    unsigned int *rawLineData = new unsigned int[numPoints + numLines];
    float *rawPointData = new float[numPoints * 3];

    unsigned int pntPosOffset = 0;
    unsigned int lnsPosOffset = 0;
    for( size_t i = 0; i < fiberDS->size(); ++i )
    {
        const wmath::WFiber &fib = (*fiberDS)[i];
        rawLineData[lnsPosOffset++] = static_cast< unsigned int >( fib.size() );
        for( size_t j = 0; j < fib.size(); ++j )
        {
            const wmath::WPosition &point = fib[j];
            assert( pntPosOffset % 3 == 0  && "(pOff % 3) was not equal to 0" );
            assert( pntPosOffset / 3 < numPoints );
            rawLineData[lnsPosOffset++] = static_cast< unsigned int >( pntPosOffset / 3 );
            rawPointData[pntPosOffset++] = static_cast< float >( point[0] );
            rawPointData[pntPosOffset++] = static_cast< float >( point[1] );
            rawPointData[pntPosOffset++] = static_cast< float >( point[2] );
            assert( pntPosOffset < ( ( numPoints * 3 ) + 1 ) && "pOff < #pts" );
        }
    }
    wiotools::switchByteOrderOfArray< float >( rawPointData, numPoints * 3 );
    wiotools::switchByteOrderOfArray< unsigned int >( rawLineData, numLines + numPoints );
    out.write( reinterpret_cast< char* >( rawPointData ), sizeof( float ) * numPoints * 3 );
    out << std::endl;
    out << "LINES " << numLines << " " << numPoints + numLines << std::endl;
    out.write( reinterpret_cast< char* >( rawLineData ), sizeof( unsigned int ) * ( numPoints + numLines ) );
    out << std::endl;
    out.close();
}
