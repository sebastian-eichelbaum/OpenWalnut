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

#ifndef WTENSORFUNCTIONS_H
#define WTENSORFUNCTIONS_H

#include <vector>

#include "WTensorSym.h"

namespace wmath
{
/**
 * Compute all eigenvalues as well as the corresponding eigenvectors of a
 * symmetric real Matrix.
 *
 * \pre Data_T must be castable to double.
 *
 * \param[in] mat A real symmetric matrix.
 * \param[out] eigenValues A pointer to a vector of eigenvalues.
 * \param[out] eigenVectors A pointer to a vector of eigenvectors.
 */
template< typename Data_T >
void jacobiEigenvector3D( WTensorSym< 2, 3, Data_T > const& mat,
                          std::vector< Data_T >* eigenValues,
                          std::vector< WTensor< 1, 3, Data_T > >* eigenVectors );

} // namespace wmath

#endif  // WTENSORFUNCTIONS_H
