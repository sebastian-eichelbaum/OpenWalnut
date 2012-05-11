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

#ifndef WLINEARALGEBRA_H
#define WLINEARALGEBRA_H

#include "WMatrixEigen.h"

#include "WMatrixFixed.h"
#include "WVectorFixed.h"
#include "WPosition.h"

/**
 * The new dynamic vector type. It is a heap-allocated double vector with dynamic size.
 *
 * \see http://eigen.tuxfamily.org/dox/classEigen_1_1Matrix.html
 * \see http://eigen.tuxfamily.org/dox/classEigen_1_1MatrixBase.html
 */
typedef Eigen::Matrix< double, Eigen::Dynamic, 1 > WVector_2;

/**
 * A complex double vector of dynamic size. Heap-allocated.
 *
 * \see http://eigen.tuxfamily.org/dox/classEigen_1_1Matrix.html
 * \see http://eigen.tuxfamily.org/dox/classEigen_1_1MatrixBase.html
 */
typedef Eigen::VectorXcd WVectorComplex_2;


#endif  // WLINEARALGEBRA_H
