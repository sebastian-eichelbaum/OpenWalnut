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

#include "WLinearAlgebraFunctions.h"

#include "WMatrix.h"
#include "WVector3D.h"

namespace wmath
{
WVector3D multMatrixWithVector3D( WMatrix<double> mat, WVector3D vec )
{
    WVector3D result;
    result[0] = mat( 0, 0 ) * vec[0] + mat( 0, 1 ) * vec[1] + mat( 0, 2 ) * vec[2];
    result[1] = mat( 1, 0 ) * vec[0] + mat( 1, 1 ) * vec[1] + mat( 1, 2 ) * vec[2];
    result[2] = mat( 2, 0 ) * vec[0] + mat( 2, 1 ) * vec[1] + mat( 2, 2 ) * vec[2];
    return result;
}

WMatrix<double> invertMatrix3x3( WMatrix<double> mat )
{
    double det = mat( 0, 0 ) * mat( 1, 1 ) * mat( 2, 2 ) +
                mat( 0, 1 ) * mat( 1, 2 ) * mat( 2, 0 ) +
                mat( 0, 2 ) * mat( 1, 0 ) * mat( 2, 1 ) -
                mat( 0, 2 ) * mat( 1, 1 ) * mat( 2, 0 ) -
                mat( 0, 1 ) * mat( 1, 0 ) * mat( 2, 2 ) -
                mat( 0, 0 ) * mat( 1, 2 ) * mat( 2, 1 );

    WMatrix<double> r( 3, 3 );

    r( 0, 0 ) = ( mat( 1, 1 ) * mat( 2, 2 ) - mat(  1, 2 ) * mat( 2, 1 ) ) / det;
    r( 1, 0 ) = ( mat( 1, 2 ) * mat( 2, 0 ) - mat(  1, 0 ) * mat( 2, 2 ) ) / det;
    r( 2, 0 ) = ( mat( 1, 0 ) * mat( 2, 1 ) - mat(  1, 1 ) * mat( 2, 0 ) ) / det;
    r( 0, 1 ) = ( mat( 0, 2 ) * mat( 2, 1 ) - mat(  0, 1 ) * mat( 2, 2 ) ) / det;
    r( 1, 1 ) = ( mat( 0, 0 ) * mat( 2, 2 ) - mat(  0, 2 ) * mat( 2, 0 ) ) / det;
    r( 2, 1 ) = ( mat( 0, 1 ) * mat( 2, 0 ) - mat(  0, 0 ) * mat( 2, 1 ) ) / det;
    r( 0, 2 ) = ( mat( 0, 1 ) * mat( 1, 2 ) - mat(  0, 2 ) * mat( 1, 1 ) ) / det;
    r( 1, 2 ) = ( mat( 0, 2 ) * mat( 1, 0 ) - mat(  0, 0 ) * mat( 1, 2 ) ) / det;
    r( 2, 2 ) = ( mat( 0, 0 ) * mat( 1, 1 ) - mat(  0, 1 ) * mat( 1, 0 ) ) / det;

    return r;
}
}
