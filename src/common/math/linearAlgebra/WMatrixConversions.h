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

#ifndef WMATRIXCONVERSIONS_H
#define WMATRIXCONVERSIONS_H

#include <osg/Matrix>

/**
 * Converts a given WMatrix4d_2 to an osg matrix.
 *
 * \param m the matrix to convert
 *
 * \return the converted matrix
 */
inline osg::Matrixd toOsgMatrixd( WMatrix4d_2 m )
{
    osg::Matrixd m2;
    for ( size_t row = 0; row < 4; ++row )
    {
        for ( size_t col = 0; col < 4; ++col )
        {
            m2( row, col ) = m( row, col );
        }
    }
    return m2;
}

#endif  // WMATRIXCONVERSIONS_H
