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

#include <vector>

#include "../WAssert.h"
#include "../WLimits.h"

#include "WLinearAlgebraFunctions.h"
#include "WMatrix.h"
#include "linearAlgebra/WLinearAlgebra.h"

WVector3d multMatrixWithVector3D( WMatrix<double> mat, WVector3d vec )
{
    WVector3d result;
    result[0] = mat( 0, 0 ) * vec[0] + mat( 0, 1 ) * vec[1] + mat( 0, 2 ) * vec[2];
    result[1] = mat( 1, 0 ) * vec[0] + mat( 1, 1 ) * vec[1] + mat( 1, 2 ) * vec[2];
    result[2] = mat( 2, 0 ) * vec[0] + mat( 2, 1 ) * vec[1] + mat( 2, 2 ) * vec[2];
    return result;
}

WVector3d transformVector3DWithMatrix4D( WMatrix<double> mat, WVector3d vec )
{
    WAssert( mat.getNbRows() == 4 && mat.getNbCols() == 4, "Matrix has wrong size." );
    std::vector< double > resultVec4D( 4 );
    resultVec4D[0] = mat( 0, 0 ) * vec[0] + mat( 0, 1 ) * vec[1] + mat( 0, 2 ) * vec[2] /* + mat( 0, 3 ) * 0 */;
    resultVec4D[1] = mat( 1, 0 ) * vec[0] + mat( 1, 1 ) * vec[1] + mat( 1, 2 ) * vec[2] /* + mat( 1, 3 ) * 0 */;
    resultVec4D[2] = mat( 2, 0 ) * vec[0] + mat( 2, 1 ) * vec[1] + mat( 2, 2 ) * vec[2] /* + mat( 2, 3 ) * 0 */;
    resultVec4D[3] = mat( 3, 0 ) * vec[0] + mat( 3, 1 ) * vec[1] + mat( 3, 2 ) * vec[2] /* + mat( 3, 3 ) * 0 */;

    WVector3d result;
    result[0] = resultVec4D[0] / resultVec4D[3];
    result[1] = resultVec4D[1] / resultVec4D[3];
    result[2] = resultVec4D[2] / resultVec4D[3];
    return result;
}

WPosition transformPosition3DWithMatrix4D( WMatrix<double> mat, WPosition vec )
{
    WAssert( mat.getNbRows() == 4 && mat.getNbCols() == 4, "Matrix has wrong size." );
    std::vector< double > resultVec4D( 4 );
    resultVec4D[0] = mat( 0, 0 ) * vec[0] + mat( 0, 1 ) * vec[1] + mat( 0, 2 ) * vec[2] + mat( 0, 3 ) * 1;
    resultVec4D[1] = mat( 1, 0 ) * vec[0] + mat( 1, 1 ) * vec[1] + mat( 1, 2 ) * vec[2] + mat( 1, 3 ) * 1;
    resultVec4D[2] = mat( 2, 0 ) * vec[0] + mat( 2, 1 ) * vec[1] + mat( 2, 2 ) * vec[2] + mat( 2, 3 ) * 1;
    resultVec4D[3] = mat( 3, 0 ) * vec[0] + mat( 3, 1 ) * vec[1] + mat( 3, 2 ) * vec[2] + mat( 3, 3 ) * 1;

    WPosition result;
    result[0] = resultVec4D[0] / resultVec4D[3];
    result[1] = resultVec4D[1] / resultVec4D[3];
    result[2] = resultVec4D[2] / resultVec4D[3];
    return result;
}

WMatrix<double> invertMatrix3x3( WMatrix<double> mat )
{
    WAssert( mat.getNbRows(), "Zero rows found." );
    WAssert( mat.getNbCols(), "Zero columns found." );
    double det = mat( 0, 0 ) * mat( 1, 1 ) * mat( 2, 2 ) +
                mat( 0, 1 ) * mat( 1, 2 ) * mat( 2, 0 ) +
                mat( 0, 2 ) * mat( 1, 0 ) * mat( 2, 1 ) -
                mat( 0, 2 ) * mat( 1, 1 ) * mat( 2, 0 ) -
                mat( 0, 1 ) * mat( 1, 0 ) * mat( 2, 2 ) -
                mat( 0, 0 ) * mat( 1, 2 ) * mat( 2, 1 );

    WAssert( det != 0, "Determinant is zero. This matrix can not be inverted." );

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

WMatrix<double> invertMatrix4x4( WMatrix<double> mat )
{
    WAssert( mat.getNbRows(), "Zero rows found." );
    WAssert( mat.getNbCols(), "Zero columns found." );
    double det =
        mat( 0, 0 ) * mat( 1, 1 ) * mat( 2, 2 ) * mat( 3, 3 ) +
        mat( 0, 0 ) * mat( 1, 2 ) * mat( 2, 3 ) * mat( 3, 1 ) +
        mat( 0, 0 ) * mat( 1, 3 ) * mat( 2, 1 ) * mat( 3, 2 ) +

        mat( 0, 1 ) * mat( 1, 0 ) * mat( 2, 3 ) * mat( 3, 2 ) +
        mat( 0, 1 ) * mat( 1, 2 ) * mat( 2, 0 ) * mat( 3, 3 ) +
        mat( 0, 1 ) * mat( 1, 3 ) * mat( 2, 2 ) * mat( 3, 0 ) +

        mat( 0, 2 ) * mat( 1, 0 ) * mat( 2, 1 ) * mat( 3, 3 ) +
        mat( 0, 2 ) * mat( 1, 1 ) * mat( 2, 3 ) * mat( 3, 0 ) +
        mat( 0, 2 ) * mat( 1, 3 ) * mat( 2, 0 ) * mat( 3, 1 ) +

        mat( 0, 3 ) * mat( 1, 0 ) * mat( 2, 2 ) * mat( 3, 1 ) +
        mat( 0, 3 ) * mat( 1, 1 ) * mat( 2, 0 ) * mat( 3, 2 ) +
        mat( 0, 3 ) * mat( 1, 2 ) * mat( 2, 1 ) * mat( 3, 0 ) -

        mat( 0, 0 ) * mat( 1, 1 ) * mat( 2, 3 ) * mat( 3, 2 ) -
        mat( 0, 0 ) * mat( 1, 2 ) * mat( 2, 1 ) * mat( 3, 3 ) -
        mat( 0, 0 ) * mat( 1, 3 ) * mat( 2, 2 ) * mat( 3, 1 ) -

        mat( 0, 1 ) * mat( 1, 0 ) * mat( 2, 2 ) * mat( 3, 3 ) -
        mat( 0, 1 ) * mat( 1, 2 ) * mat( 2, 3 ) * mat( 3, 0 ) -
        mat( 0, 1 ) * mat( 1, 3 ) * mat( 2, 0 ) * mat( 3, 2 ) -

        mat( 0, 2 ) * mat( 1, 0 ) * mat( 2, 3 ) * mat( 3, 1 ) -
        mat( 0, 2 ) * mat( 1, 1 ) * mat( 2, 0 ) * mat( 3, 3 ) -
        mat( 0, 2 ) * mat( 1, 3 ) * mat( 2, 1 ) * mat( 3, 0 ) -

        mat( 0, 3 ) * mat( 1, 0 ) * mat( 2, 1 ) * mat( 3, 2 ) -
        mat( 0, 3 ) * mat( 1, 1 ) * mat( 2, 2 ) * mat( 3, 0 ) -
        mat( 0, 3 ) * mat( 1, 2 ) * mat( 2, 0 ) * mat( 3, 1 );

    WMatrix<double> result( 4, 4 );

    result( 0, 0 ) =
        mat( 1, 1 ) * mat( 2, 2 ) * mat( 3, 3 ) +
        mat( 1, 2 ) * mat( 2, 3 ) * mat( 3, 1 ) +
        mat( 1, 3 ) * mat( 2, 1 ) * mat( 3, 2 ) -
        mat( 1, 1 ) * mat( 2, 3 ) * mat( 3, 2 ) -
        mat( 1, 2 ) * mat( 2, 1 ) * mat( 3, 3 ) -
        mat( 1, 3 ) * mat( 2, 2 ) * mat( 3, 1 );

    result( 0, 1 ) =
        mat( 0, 1 ) * mat( 2, 3 ) * mat( 3, 2 ) +
        mat( 0, 2 ) * mat( 2, 1 ) * mat( 3, 3 ) +
        mat( 0, 3 ) * mat( 2, 2 ) * mat( 3, 1 ) -
        mat( 0, 1 ) * mat( 2, 2 ) * mat( 3, 3 ) -
        mat( 0, 2 ) * mat( 2, 3 ) * mat( 3, 1 ) -
        mat( 0, 3 ) * mat( 2, 1 ) * mat( 3, 2 );

    result( 0, 2 ) =
        mat( 0, 1 ) * mat( 1, 2 ) * mat( 3, 3 ) +
        mat( 0, 2 ) * mat( 1, 3 ) * mat( 3, 1 ) +
        mat( 0, 3 ) * mat( 1, 1 ) * mat( 3, 2 ) -
        mat( 0, 1 ) * mat( 1, 3 ) * mat( 3, 2 ) -
        mat( 0, 2 ) * mat( 1, 1 ) * mat( 3, 3 ) -
        mat( 0, 3 ) * mat( 1, 2 ) * mat( 3, 1 );

    result( 0, 3 ) =
        mat( 0, 1 ) * mat( 1, 3 ) * mat( 2, 2 ) +
        mat( 0, 2 ) * mat( 1, 1 ) * mat( 2, 3 ) +
        mat( 0, 3 ) * mat( 1, 2 ) * mat( 2, 1 ) -
        mat( 0, 1 ) * mat( 1, 2 ) * mat( 2, 3 ) -
        mat( 0, 2 ) * mat( 1, 3 ) * mat( 2, 1 ) -
        mat( 0, 3 ) * mat( 1, 1 ) * mat( 2, 2 );

    result( 1, 0 ) =
        mat( 1, 0 ) * mat( 2, 3 ) * mat( 3, 2 ) +
        mat( 1, 2 ) * mat( 2, 0 ) * mat( 3, 3 ) +
        mat( 1, 3 ) * mat( 2, 2 ) * mat( 3, 0 ) -
        mat( 1, 0 ) * mat( 2, 2 ) * mat( 3, 3 ) -
        mat( 1, 2 ) * mat( 2, 3 ) * mat( 3, 0 ) -
        mat( 1, 3 ) * mat( 2, 0 ) * mat( 3, 2 );

    result( 1, 1 ) =
        mat( 0, 0 ) * mat( 2, 2 ) * mat( 3, 3 ) +
        mat( 0, 2 ) * mat( 2, 3 ) * mat( 3, 0 ) +
        mat( 0, 3 ) * mat( 2, 0 ) * mat( 3, 2 ) -
        mat( 0, 0 ) * mat( 2, 3 ) * mat( 3, 2 ) -
        mat( 0, 2 ) * mat( 2, 0 ) * mat( 3, 3 ) -
        mat( 0, 3 ) * mat( 2, 2 ) * mat( 3, 0 );

    result( 1, 2 ) =
        mat( 0, 0 ) * mat( 1, 3 ) * mat( 3, 2 ) +
        mat( 0, 2 ) * mat( 1, 0 ) * mat( 3, 3 ) +
        mat( 0, 3 ) * mat( 1, 2 ) * mat( 3, 0 ) -
        mat( 0, 0 ) * mat( 1, 2 ) * mat( 3, 3 ) -
        mat( 0, 2 ) * mat( 1, 3 ) * mat( 3, 0 ) -
        mat( 0, 3 ) * mat( 1, 0 ) * mat( 3, 2 );

    result( 1, 3 ) =
        mat( 0, 0 ) * mat( 1, 2 ) * mat( 2, 3 ) +
        mat( 0, 2 ) * mat( 1, 3 ) * mat( 2, 0 ) +
        mat( 0, 3 ) * mat( 1, 0 ) * mat( 2, 2 ) -
        mat( 0, 0 ) * mat( 1, 3 ) * mat( 2, 2 ) -
        mat( 0, 2 ) * mat( 1, 0 ) * mat( 2, 3 ) -
        mat( 0, 3 ) * mat( 1, 2 ) * mat( 2, 0 );

    result( 2, 0 ) =
        mat( 1, 0 ) * mat( 2, 1 ) * mat( 3, 3 ) +
        mat( 1, 1 ) * mat( 2, 3 ) * mat( 3, 0 ) +
        mat( 1, 3 ) * mat( 2, 0 ) * mat( 3, 1 ) -
        mat( 1, 0 ) * mat( 2, 3 ) * mat( 3, 1 ) -
        mat( 1, 1 ) * mat( 2, 0 ) * mat( 3, 3 ) -
        mat( 1, 3 ) * mat( 2, 1 ) * mat( 3, 0 );

    result( 2, 1 ) =
        mat( 0, 0 ) * mat( 2, 3 ) * mat( 3, 1 ) +
        mat( 0, 1 ) * mat( 2, 0 ) * mat( 3, 3 ) +
        mat( 0, 3 ) * mat( 2, 1 ) * mat( 3, 0 ) -
        mat( 0, 0 ) * mat( 2, 1 ) * mat( 3, 3 ) -
        mat( 0, 1 ) * mat( 2, 3 ) * mat( 3, 0 ) -
        mat( 0, 3 ) * mat( 2, 0 ) * mat( 3, 1 );

    result( 2, 2 ) =
        mat( 0, 0 ) * mat( 1, 1 ) * mat( 3, 3 ) +
        mat( 0, 1 ) * mat( 1, 3 ) * mat( 3, 0 ) +
        mat( 0, 3 ) * mat( 1, 0 ) * mat( 3, 1 ) -
        mat( 0, 0 ) * mat( 1, 3 ) * mat( 3, 1 ) -
        mat( 0, 1 ) * mat( 1, 0 ) * mat( 3, 3 ) -
        mat( 0, 3 ) * mat( 1, 1 ) * mat( 3, 0 );

    result( 2, 3 ) =
        mat( 0, 0 ) * mat( 1, 3 ) * mat( 2, 1 ) +
        mat( 0, 1 ) * mat( 1, 0 ) * mat( 2, 3 ) +
        mat( 0, 3 ) * mat( 1, 1 ) * mat( 2, 0 ) -
        mat( 0, 0 ) * mat( 1, 1 ) * mat( 2, 3 ) -
        mat( 0, 1 ) * mat( 1, 3 ) * mat( 2, 0 ) -
        mat( 0, 3 ) * mat( 1, 0 ) * mat( 2, 1 );

    result( 3, 0 ) =
        mat( 1, 0 ) * mat( 2, 2 ) * mat( 3, 1 ) +
        mat( 1, 1 ) * mat( 2, 0 ) * mat( 3, 2 ) +
        mat( 1, 2 ) * mat( 2, 1 ) * mat( 3, 0 ) -
        mat( 1, 0 ) * mat( 2, 1 ) * mat( 3, 2 ) -
        mat( 1, 1 ) * mat( 2, 2 ) * mat( 3, 0 ) -
        mat( 1, 2 ) * mat( 2, 0 ) * mat( 3, 1 );

    result( 3, 1 ) =
        mat( 0, 0 ) * mat( 2, 1 ) * mat( 3, 2 ) +
        mat( 0, 1 ) * mat( 2, 2 ) * mat( 3, 0 ) +
        mat( 0, 2 ) * mat( 2, 0 ) * mat( 3, 1 ) -
        mat( 0, 0 ) * mat( 2, 2 ) * mat( 3, 1 ) -
        mat( 0, 1 ) * mat( 2, 0 ) * mat( 3, 2 ) -
        mat( 0, 2 ) * mat( 2, 1 ) * mat( 3, 0 );

    result( 3, 2 ) =
        mat( 0, 0 ) * mat( 1, 2 ) * mat( 3, 1 ) +
        mat( 0, 1 ) * mat( 1, 0 ) * mat( 3, 2 ) +
        mat( 0, 2 ) * mat( 1, 1 ) * mat( 3, 0 ) -
        mat( 0, 0 ) * mat( 1, 1 ) * mat( 3, 2 ) -
        mat( 0, 1 ) * mat( 1, 2 ) * mat( 3, 0 ) -
        mat( 0, 2 ) * mat( 1, 0 ) * mat( 3, 1 );

    result( 3, 3 ) =
        mat( 0, 0 ) * mat( 1, 1 ) * mat( 2, 2 ) +
        mat( 0, 1 ) * mat( 1, 2 ) * mat( 2, 0 ) +
        mat( 0, 2 ) * mat( 1, 0 ) * mat( 2, 1 ) -
        mat( 0, 0 ) * mat( 1, 2 ) * mat( 2, 1 ) -
        mat( 0, 1 ) * mat( 1, 0 ) * mat( 2, 2 ) -
        mat( 0, 2 ) * mat( 1, 1 ) * mat( 2, 0 );

    WAssert( det != 0, "Determinat is zero. This matrix can not be inverted." );

    double detInv = 1. / det;
    for( size_t r = 0; r < 4; ++r)
    {
        for( size_t c = 0; c < 4; ++c )
        {
            result( c, r ) *= detInv;
        }
    }

    return result;
}

bool linearIndependent( const WVector3d& u, const WVector3d& v )
{
    WVector3d cp = cross( u, v );
    if( std::fabs( cp[0] ) < wlimits::DBL_EPS && std::fabs( cp[1] ) < wlimits::DBL_EPS && std::fabs( cp[2] ) < wlimits::DBL_EPS )
    {
        return false;
    }
    return true;
}
