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

#ifndef WMATRIXEIGEN_H
#define WMATRIXEIGEN_H

#define EIGEN_DONT_ALIGN_STATICALLY

#include <Eigen/Core>
#include <Eigen/LU>

/**
 * A double matrix of dynamic size. Heap-allocated.
 * If you want to access coefficients using the operator( size_t, size_t ), the first parameter is still the row index, starting with 0.
 *
 * \see http://eigen.tuxfamily.org/dox/classEigen_1_1Matrix.html
 * \see http://eigen.tuxfamily.org/dox/classEigen_1_1MatrixBase.html
 */
typedef Eigen::MatrixXd WMatrix_2;

/**
 * A complex double matrix of dynamic size. Heap-allocated.
 * If you want to access coefficients using the operator( size_t, size_t ), the first parameter is still the row index, starting with 0.
 *
 * \see http://eigen.tuxfamily.org/dox/classEigen_1_1Matrix.html
 * \see http://eigen.tuxfamily.org/dox/classEigen_1_1MatrixBase.html
 */
typedef Eigen::MatrixXcd WMatrixComplex_2;

#endif  // WMATRIXEIGEN_H

