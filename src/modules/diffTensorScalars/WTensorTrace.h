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

#ifndef WTENSORTRACE_H
#define WTENSORTRACE_H

#include "core/common/math/linearAlgebra/WVectorFixed.h"
#include "core/common/math/WTensorSym.h"
#include "core/common/WObjectNDIP.h"

#include "WDataSetDTIToScalar_I.h"

/**
 * Computes the trace (sum of diagonal elements) of the tensor.
 * \f[
 *   trace(T) = \sum_{i=1}^n T(i,i)
 * \f]
 */
class WTensorTrace : public WObjectNDIP< WDataSetDTIToScalar_I >
{
public:
    /**
     * Constructs new TensorTrace object.
     */
    WTensorTrace();

protected:
    /**
     * Actual computation is done inhere.
     *
     * \param evals Although not needed here, for reasons of simplicity the API is covering eigenvalues as well.
     * \param tensor The tensor for which the trace should be computed.
     *
     * \return Trace of the tensor
     */
    virtual double tensorToScalar( const WVector3d& evals, const WTensorSym< 2, 3, float >& tensor );
};

#endif  // WTENSORTRACE_H
