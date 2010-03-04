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

#ifndef WLINEARALGEBRAFUNCTIONS_H
#define WLINEARALGEBRAFUNCTIONS_H

namespace wmath
{
    class WVector3D;
    template< typename > class WMatrix;

    /**
     * helper routine to multiply a 3x3 matrix with a vector
     *
     * \param mat 3x3 matrix
     * \param vec vector
     */
    WVector3D multMatrixWithVector3D( WMatrix<double> mat, WVector3D vec );

    /**
     * helper routine to invert a 3x3 matrix
     *
     * \param mat 3x3 matrix
     *
     * \return inverted 3x3 matrix
     */
    WMatrix<double> invertMatrix3x3( WMatrix<double> mat );
}

#endif  // WLINEARALGEBRAFUNCTIONS_H
