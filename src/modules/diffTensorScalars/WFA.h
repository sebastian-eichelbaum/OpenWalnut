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

#ifndef WFA_H
#define WFA_H

#include "core/common/math/linearAlgebra/WVectorFixed.h"
#include "core/common/math/WTensorSym.h"
#include "core/common/WObjectNDIP.h"

#include "WDataSetDTIToScalar_I.h"

/**
 * Computes the FA (Fractional Anisotropy, see: https://en.wikipedia.org/wiki/Fractional_anisotropy) of a given Tensor.
 * \f[
 *   \hat{\lambda} = \frac{1}{3} \sum \lambda_i
 * \f]
 *
 * \f[
 *   FA = \sqrt{ \frac{3}{2}
 *               \frac{ \left(\lambda_1 - \hat{\lambda} \right)^2 + \left(\lambda_2 - \hat{\lambda} \right)^2 +
 *                      \left(\lambda_3 - \hat{\lambda} \right)^2 }
 *                    {\lambda_1^2 + \lambda_2^2 + \lambda_3^2 }
 *             }
 * \f]
 */
class WFA : public WObjectNDIP< WDataSetDTIToScalar_I >
{
public:
    /**
     * Creates an object to perform the computation.
     */
    WFA();

protected:
    /**
     * Actual FA computation takes place inhere.
     *
     * \param evals With the three given Eigenvalues, we may compute the FA. See the wikipedia article above for the formula.
     * \param tensor Although not needed for FA computation, the API requires us to use the signature.
     *
     * \return The FA of the tensor.
     */
    virtual double tensorToScalar( const WVector3d& evals, const WTensorSym< 2, 3, float >& tensor );
};

#endif  // WFA_H
