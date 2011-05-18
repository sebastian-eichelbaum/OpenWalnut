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
#include <vector>

#include <boost/shared_ptr.hpp>

#include "../../common/WIOTools.h"
#include "../exceptions/WDHIOFailure.h"
#include "WWriterMatrixSymVTK.h"

WWriterMatrixSymVTK::WWriterMatrixSymVTK( std::string fname, bool overwrite )
    : WWriter( fname, overwrite )
{
}

void WWriterMatrixSymVTK::writeTable( const std::vector< double > &table, size_t dim ) const
{
    using std::fstream;
    fstream out( m_fname.c_str(), fstream::out | fstream::in | fstream::trunc );
    if( !out || out.bad() )
    {
        throw WDHIOFailure( std::string( "Invalid file, or permission: " + m_fname ) );
    }
    out << "# vtk DataFile Version 3.0" << std::endl;
    out << "WMatrixSym from OpenWalnut" << std::endl;
    out << "BINARY" << std::endl;

    out << "FIELD WMatrixSym 1" << std::endl;
    unsigned int numDistances = table.size() + 1;
    out << "ELEMENTS " << numDistances << " 1 float" << std::endl;
    float *data = new float[numDistances];

    for( size_t i = 0; i < table.size() ; ++i )
    {
        data[i] = static_cast< float >( table[i] );
    }
    data[ numDistances - 1 ] = static_cast< float >( dim );

    switchByteOrderOfArray< float >( data, numDistances );
    out.write( reinterpret_cast< char* >( data ), sizeof( float ) * numDistances );
    out << std::endl;
    out.close();
}
