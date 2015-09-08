//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2015 OpenWalnut Community, Nemtics, BSV@Uni-Leipzig
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

#ifndef WAD_H
#define WAD_H

#include "core/common/math/linearAlgebra/WVectorFixed.h"
#include "core/common/math/WTensorSym.h"
#include "core/common/WObjectNDIP.h"

#include "WDataSetDTIToScalar_I.h"

/**
 * Computes the AD (Axional Diffusivity) of a given Tensor.
 * \f[
 *   AD = \lambda_1
 * \f]
 */
class WAD : public WObjectNDIP< WDataSetDTIToScalar_I >
{
public:
    /**
     * Creates an object to perform the computation.
     */
    WAD();

protected:
    /**
     * Actual AD computation takes place inhere.
     *
     * \param evals With the three given Eigenvalues, we may compute the AD. See the wikipedia article above for the formula.
     * \param tensor Although not needed for AD computation, the API requires us to use the signature.
     *
     * \return The AD of the tensor.
     */
    virtual double tensorToScalar( const WVector3d& evals, const WTensorSym< 2, 3, float >& tensor );
};

#endif  // WAD_H
