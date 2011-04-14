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

#ifndef WMATRIX4X4_H
#define WMATRIX4X4_H

#include <string>
#include <vector>

#include <boost/lexical_cast.hpp>

#include <osg/io_utils>
#include <osg/Matrix>

#include "../WAssert.h"
#include "../WStringUtils.h"

#include "../WDefines.h"

/**
 * Use osg 4x4 matrices as WMatrix4x4
 */
OW_API_DEPRECATED typedef osg::Matrixd WMatrix4x4;

/**
 * Write a 4x4 matrix in string representation.
 *
 * \param c  the matrix
 *
 * \return the matrix as string
 */
/*inline std::string toString( const WMatrix4x4& c )
{
    std::ostringstream out;
    for ( size_t row = 0; row < 4; ++row )
    {
        for ( size_t col = 0; col < 4; ++col )
        {
            out << c( row, col ) << ";";
        }
    }
    return out.str();
}
*/
/**
 * Read a 4x4 matrix in string representation from the given string.
 *
 * \param str the string to parse
 *
 * \return the matrix
 */
/*inline WMatrix4x4 fromString( std::string str )
{
    WMatrix4x4 c;
    std::vector< std::string > tokens;
    tokens = string_utils::tokenize( str, ";" );
    WAssert( tokens.size() >= 16, "There weren't 16 values for a 4x4 Matrix" );

    size_t idx = 0;
    for ( size_t row = 0; row < 4; ++row )
    {
        for ( size_t col = 0; col < 4; ++col )
        {
            c( row, col ) = boost::lexical_cast< double >( tokens[ idx ] );
            idx++;
        }
    }

    return c;
}*/

#endif  // WMATRIX4X4_H
